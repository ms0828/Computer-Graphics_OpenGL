#include <vgl.h>
#include <InitShader.h>
#include <sstream>
#include <fstream>
#include "MyCube.h"
#include <vec.h>
#include <mat.h>
#include <vector>
#include <stdio.h>
#include <unordered_map>
using namespace std;

struct ModelVertex
{
	vec4 position;
	vec4 color;
	vec3 surfaceNormal;
	int vertexNum;
	vec3 vertexNormal;
};

MyCube cube;
GLuint vao;
GLuint buffer;

GLuint phongShader;
GLuint defaultShader;
GLuint uMatHandle;
GLuint uColorHandle;
mat4 pvMat = mat4(1.0f);

unordered_map<int, vector<vec3>> SurfaceNormalsFromVertexNum;
vector<vec4> vertex;
vector<vec4> triangle;
int numOfVertex = 0;
int index = 0;
ModelVertex* ModelVertices;

int winWidth = 500;
int winHeight = 500;

float centerX;
float centerY;
float centerZ;

float scaleValue = 1;

// Control
float g_Time = 0;
float rotateSpeed = 90.0f;
float turnX = 0.0f;
float turnY = 0.0f;
float turnZ = 0.0f;
bool isRotate = false;
bool isXRotation = false;
bool isYRotation = false;
bool isZRotation = false;
float isFlat = 0;

// 조명의 위치 및 색상
vec4 lightPosition = vec4(3, 3, 0, 1);
vec4 lightIntensity = vec4(1, 1, 1, 1);

// 계수
vec4 ka = vec4(0.3, 0.3, 0.3, 1);
vec4 kd = vec4(0.7, 0.3, 0.3, 1);
vec4 ks = vec4(0.3, 0.3, 0.3, 1);
float shiness = 3.0;

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	mat4 V(1.0f);
	vec3 n = at - eye;
	n = normalize(n);

	float a = dot(up, n);
	vec3 v = up - a * n;
	v = normalize(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);
	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Rc[i][j] = Rw[j][i];

	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);
	V = Rc * Tc;
	return V;
}

mat4 myPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	mat4 P(1.0f);

	float rad = fovy * 3.141592 / 180.0f;

	float sz = 1 / zFar;
	float h = zFar * tan(rad / 2);

	float sy = 1 / h;
	float w = h * aspectRatio;
	float sx = 1 / w;

	mat4 S = Scale(sx, sy, sz);
	mat4 M(1.0f);

	float c = -zNear / zFar;
	M[2][2] = 1 / (c + 1);
	M[2][3] = -c / (c + 1);
	M[3][2] = -1;
	M[3][3] = 0;

	P = M * S;
	return P;
}

vec3 getSurfaceNormal(vec4 p0, vec4 p1, vec4 p2)
{
	vec4 a4 = p1 - p0;
	vec4 b4 = p2 - p0;
	vec3 a = vec3(a4.x, a4.y, a4.z);
	vec3 b = vec3(b4.x, b4.y, b4.z);

	vec3 normal = normalize(cross(a, b));
	return normal;
}

void setVertexNormal()
{
	vec3 sumSurfaceNormal = vec3(0, 0, 0);
	vec3 vertexNormal = vec3(0, 0, 0);

	for (int i = 0; i < index; i++)
	{
		sumSurfaceNormal = vec3(0, 0, 0);
		for (int j = 0; j < SurfaceNormalsFromVertexNum[ModelVertices[i].vertexNum].size(); j++)
		{
			sumSurfaceNormal += SurfaceNormalsFromVertexNum[ModelVertices[i].vertexNum][j];
		}
		vertexNormal = normalize(sumSurfaceNormal);
		ModelVertices[i].vertexNormal = vertexNormal;
	}
}

void setTriangle(int a, int b, int c)
{
	vec3 surfaceNormal = getSurfaceNormal(vertex[a], vertex[b], vertex[c]);

	ModelVertices[index].vertexNum = a; ModelVertices[index].position = vertex[a]; ModelVertices[index].color = vec4(0.5, 0.5, 0.5, 1); ModelVertices[index].surfaceNormal = surfaceNormal; index++;
	ModelVertices[index].vertexNum = b; ModelVertices[index].position = vertex[b]; ModelVertices[index].color = vec4(0.5, 0.5, 0.5, 1); ModelVertices[index].surfaceNormal = surfaceNormal; index++;
	ModelVertices[index].vertexNum = c; ModelVertices[index].position = vertex[c]; ModelVertices[index].color = vec4(0.5, 0.5, 0.5, 1); ModelVertices[index].surfaceNormal = surfaceNormal; index++;

	SurfaceNormalsFromVertexNum[a].push_back(surfaceNormal);
	SurfaceNormalsFromVertexNum[b].push_back(surfaceNormal);
	SurfaceNormalsFromVertexNum[c].push_back(surfaceNormal);
}

void calculateModelScale()
{
	float maxX = -999999, maxY = -999999, maxZ = -999999;
	float minX = 999999, minY = 999999, minZ = 999999;
	float s = vertex.size();

	for (int i = 1; i < s; i++)
	{
		if (vertex[i].x > maxX)
			maxX = vertex[i].x;
		if (vertex[i].y > maxY)
			maxY = vertex[i].y;
		if (vertex[i].z > maxZ)
			maxZ = vertex[i].z;

		if (vertex[i].x < minX)
			minX = vertex[i].x;
		if (vertex[i].y < minY)
			minY = vertex[i].y;
		if (vertex[i].z < minZ)
			minZ = vertex[i].z;
	}

	centerX = (minX + maxX) / 2.0f;
	centerY = (minY + maxY) / 2.0f;
	centerZ = (minZ + maxZ) / 2.0f;

	// 물체의 각 축 크기 계산
	float sizeX = maxX - minX;
	float sizeY = maxY - minY;
	float sizeZ = maxZ - minZ;

	// 물체를 감싸는 바운더리의 대각선 길이
	float maxDimension = sqrt(sizeX * sizeX + sizeY * sizeY + sizeZ * sizeZ);

	// 물체를 화면에 일정 크기로 맞추기 위한 스케일 계산
	float desiredSize = 1.0f;
	scaleValue = desiredSize / maxDimension;
}

void modelInit()
{
	for (int i = 0; i < triangle.size(); i++)
	{
		setTriangle(triangle[i].x, triangle[i].y, triangle[i].z);
	}
	setVertexNormal();
	calculateModelScale();
}

void saveModelInfoArr(string line)
{
	if (!(line[0] == 'f' || line[0] == 'v'))
		return;

	istringstream strStream(line);
	string buffer;
	vector<string> bufferArr;

	if (line[0] == 'v')
	{
		while (getline(strStream, buffer, ' '))
		{
			bufferArr.push_back(buffer);
		}
		vec3 v;
		v.x = stof(bufferArr[1]);
		v.y = stof(bufferArr[2]);
		v.z = stof(bufferArr[3]);
		vertex.push_back(v);
	}
	if (line[0] == 'f')
	{
		while (getline(strStream, buffer, ' '))
		{
			bufferArr.push_back(buffer);
		}
		vec3 f;
		f.x = stof(bufferArr[1]);
		f.y = stof(bufferArr[2]);
		f.z = stof(bufferArr[3]);
		triangle.push_back(f);
	}
}

void readFileAndSaveModelInfo()
{
	ifstream openFile;
	string fileName;

	while (1)
	{
		cout << "Input FIle Path: ";
		cin >> fileName;
		openFile.open(fileName);
		if (openFile)
		{
			break;
		}
		cout << "File not Found!\n";
		continue;
	}

	string line;
	if (openFile.is_open())
	{
		while (getline(openFile, line))
		{
			saveModelInfoArr(line);
		}
		openFile.close();
	}
}

void myInit()
{
	cube.Init();
	modelInit();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ModelVertex) * numOfVertex, ModelVertices, GL_STATIC_DRAW);

	defaultShader = InitShader("vshader.glsl", "fshader.glsl");
	phongShader = InitShader("vphong.glsl", "fphong.glsl");
}

void DrawAxis()
{
	mat4 rotateMat = RotateZ(turnZ * rotateSpeed) * RotateY(turnY * rotateSpeed) * RotateX(turnX * rotateSpeed);

	mat4 xAxisModelMat = rotateMat * Translate(1, 0, 0) * Scale(2, 0.01, 0.01);
	glUniformMatrix4fv(uMatHandle, 1, GL_TRUE, pvMat * xAxisModelMat);
	glUniform4f(uColorHandle, 1, 0, 0, 1);
	cube.Draw(defaultShader);

	mat4 yAxisModelMat = rotateMat * Translate(0, 1, 0) * Scale(0.01, 2, 0.01);
	glUniformMatrix4fv(uMatHandle, 1, GL_TRUE, pvMat * yAxisModelMat);
	glUniform4f(uColorHandle, 0, 1, 0, 1);
	cube.Draw(defaultShader);

	mat4 zAxisModelMat = rotateMat * Translate(0, 0, 1) * Scale(0.01, 0.01, 2);
	glUniformMatrix4fv(uMatHandle, 1, GL_TRUE, pvMat * zAxisModelMat);
	glUniform4f(uColorHandle, 0, 0, 1, 1);
	cube.Draw(defaultShader);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	//---- 축 그리기 ----
	glUseProgram(defaultShader);
	float aspect = winWidth / (float)winHeight;
	float h = 1;
	mat4 projectionMat = myPerspective(30, aspect, 0.1, 100);
	mat4 viewMat = myLookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	pvMat = projectionMat * viewMat;
	uMatHandle = glGetUniformLocation(defaultShader, "uMat");
	uColorHandle = glGetUniformLocation(defaultShader, "uColor");
	DrawAxis();

	//---- 도형 그리기 ----
	glUseProgram(phongShader);
	GLuint uProjectionMatHandle = glGetUniformLocation(phongShader, "uProjectionMat");
	GLuint uModelAndViewMatHandle = glGetUniformLocation(phongShader, "uModelAndViewMat");

	mat4 rotateMat = RotateZ(turnZ * rotateSpeed) * RotateY(turnY * rotateSpeed) * RotateX(turnX * rotateSpeed);
	mat4 translateMat = Translate(-centerX, -centerY, -centerZ);
	mat4 scaleMat = Scale(scaleValue, scaleValue, scaleValue);
	mat4 modelMat = rotateMat * scaleMat * translateMat;

	glUniformMatrix4fv(uProjectionMatHandle, 1, GL_TRUE, projectionMat);
	glUniformMatrix4fv(uModelAndViewMatHandle, 1, GL_TRUE, viewMat * modelMat);

	GLuint uLightPositionHandle = glGetUniformLocation(phongShader, "uLightPosition");
	GLuint uLightIntensityHandle = glGetUniformLocation(phongShader, "uLightIntensity");
	GLuint uKaHandle = glGetUniformLocation(phongShader, "uKa");
	GLuint uKdHandle = glGetUniformLocation(phongShader, "uKd");
	GLuint uKsHandle = glGetUniformLocation(phongShader, "uKs");
	GLuint uShinessHandle = glGetUniformLocation(phongShader, "uShiness");
	GLuint uIsFlat = glGetUniformLocation(phongShader, "uIsFlat");

	glUniform4f(uLightPositionHandle, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
	glUniform4f(uLightIntensityHandle, lightIntensity.x, lightIntensity.y, lightIntensity.z, lightIntensity.w);
	glUniform4f(uKaHandle, ka.x, ka.y, ka.z, ka.w);
	glUniform4f(uKdHandle, kd.x, kd.y, kd.z, kd.w);
	glUniform4f(uKsHandle, ks.x, ks.y, ks.z, ks.w);
	glUniform1f(uShinessHandle, shiness);
	glUniform1f(uIsFlat, isFlat);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	GLuint vPosition = glGetAttribLocation(phongShader, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(phongShader, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(ModelVertex), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vSurfaceNormalHandle = glGetAttribLocation(phongShader, "vSurfaceNormal");
	glEnableVertexAttribArray(vSurfaceNormalHandle);
	glVertexAttribPointer(vSurfaceNormalHandle, 3, GL_FLOAT, GL_TRUE, sizeof(ModelVertex), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4)));

	GLuint vVertexNormalHandle = glGetAttribLocation(phongShader, "vVertexNormal");
	glEnableVertexAttribArray(vVertexNormalHandle);
	glVertexAttribPointer(vVertexNormalHandle, 3, GL_FLOAT, GL_TRUE, sizeof(ModelVertex), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4) + sizeof(vec3) + sizeof(GL_FLOAT)));

	glDrawArrays(GL_TRIANGLES, 0, numOfVertex);

	glutSwapBuffers();
}

void idle()
{
	if (isRotate)
	{
		if (isXRotation)
		{
			turnX += 0.02;
		}
		else if (isYRotation)
		{
			turnY += 0.02;
		}
		else if (isZRotation)
		{
			turnZ += 0.02;
		}
	}
	Sleep(16);
	glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

void keyboardFunc(unsigned char ch, int x, int y)
{
	if (ch == ' ')
		isRotate = !isRotate;
	else if (ch == '1')
	{
		cout << "Using Vertex Normal!" << endl;
		isFlat = 0;
	}
	else if (ch == '2')
	{
		cout << "Using Surface Normal!" << endl;
		isFlat = 1;
	}
	else if (ch == '3')
	{
		cout << "Increasing Specular Effect!" << endl;
		if (ks.x < 1.0)
			ks += 0.1f;
	}
	else if (ch == '4')
	{
		cout << "Decreasing Specular Effect!" << endl;
		if (ks.x > 0.2)
			ks -= 0.1f;
	}
	else if (ch == '5')
	{
		cout << "Increasing Shiness!" << endl;
		if (shiness < 50)
			shiness++;
	}
	else if (ch == '6')
	{
		cout << "Decreasing Shiness!" << endl;
		if (shiness > 1)
			shiness--;
	}
	else if (ch == 'q')
	{
		exit(0);
	}
		
}

void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		isXRotation = true;
		isYRotation = false;
		isZRotation = false;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		isXRotation = false;
		isYRotation = true;
		isZRotation = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		isXRotation = false;
		isYRotation = false;
		isZRotation = true;
	}
	
}

int main(int argc, char** argv)
{
	vec4 t;
	vertex.push_back(t);

	readFileAndSaveModelInfo();
	numOfVertex = triangle.size() * 3;
	ModelVertices = (ModelVertex*)malloc(sizeof(ModelVertex) * numOfVertex);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Assignment4");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutMainLoop();

	return 0;
}