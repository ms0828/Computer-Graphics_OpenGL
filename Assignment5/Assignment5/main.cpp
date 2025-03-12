#define _CRT_SECURE_NO_WARNINGS
#include <vgl.h>
#include <InitShader.h>
#include <sstream>
#include <fstream>
#include "MyObject.h"
#include "MySphere.h"
#include "Targa.h"
#include <vec.h>
#include <mat.h>
#include <vector>
#include <stdio.h>
using namespace std;

#define PI 3.141592 

GLuint vao;
GLuint buffer;
GLuint shader;

MySphere environment;
MySphere sphere;
MyObject bunny;

int winWidth = 800;
int winHeight = 500;

float g_Time = 0;
float g_aspect = 1;

bool isDragging = false;    // 왼쪽 마우스 드래그 상태
bool isRightDragging = false; // 오른쪽 마우스 드래그 상태
int prevMouseX, prevMouseY; // 이전 마우스 좌표
int prevRightMouseY; // 이전 오른쪽 마우스 좌표

float maxZoomDistance = 5.0f;
float minZoomDistance = -10.0f;
float zoomDistance = 1;
float zoomSensitive = 0.05f;

float yaw = 0.0f;           // Y축 (Yaw) 각도
float pitch = 0.0f;         // X축 (Pitch) 각도
vec3 eye = vec3(0, 0, 0);
vec3 at = vec3(0, 0, -1);
vec3 up = vec3(0, 1, 0);

bool isRotate= false;
float rotationAngle = 0.0f;
float fresnelPower = 9;
bool isDiffuseMap = false;
int modeIndex = 0;

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


bool myInit(string imageName)
{
	// ---- 환경
	environment.Init(40, 40);
	sphere.Init(40, 40);
	bunny.Init("bunny.obj");

	shader = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(shader);

	bool isSuccessFileOpen = false;
	// ---- 이미지 로드
	STGA image;
	for (int i = 0; i < 2; i++)
	{
		if (i == 0)		
		{
			string imageFullName = imageName + "_spheremap.tga";
			const char* imageFullNameCstr = imageFullName.c_str();
			isSuccessFileOpen = image.loadTGA(imageFullNameCstr);
		}
		else if (i == 1)	
		{
			string imageFullName = imageName + "_diffusemap.tga";
			const char* imageFullNameCstr = imageFullName.c_str();
			isSuccessFileOpen = image.loadTGA(imageFullNameCstr);
		}
		int w = image.width;
		int h = image.height;

		GLuint tex;
		glGenTextures(1, &tex);

		if (i == 0)		
			glActiveTexture(GL_TEXTURE0);
		else if (i == 1)	
			glActiveTexture(GL_TEXTURE1);

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	return isSuccessFileOpen;
}


float degreeToRadian(float degree)
{
	return degree * (PI / 180.0f);
}
vec3 calculateCameraDirection()
{
	vec3 direction;
	direction.x = cos(degreeToRadian(yaw)) * cos(degreeToRadian(pitch));
	direction.y = sin(degreeToRadian(pitch));
	direction.z = sin(degreeToRadian(yaw)) * cos(degreeToRadian(pitch));
	return normalize(direction);
}

vec3 calculateModelPosition(float distance)
{
	vec3 forward = normalize(at - eye);
	vec3 modelPos = eye + forward * distance;
	return modelPos;
}


void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//----------------환경-----------------
	// 카메라 방향 계산
	vec3 cameraDirection = calculateCameraDirection();
	at = eye + cameraDirection;
	
	mat4 ModelMat = Scale(vec3(20, 20, 20));
	mat4 ViewMat = myLookAt(eye + cameraDirection * zoomDistance, at + cameraDirection * zoomDistance, up);
	mat4 ProjMat = myPerspective(80, g_aspect, 0.01f, 100.0f);
	
	glUseProgram(shader);

	//---텍스처 설정
	GLuint uSphereMapHandle = glGetUniformLocation(shader, "uSphereMap");
	GLuint uDiffuseMapHandle = glGetUniformLocation(shader, "uDiffuseMap");
	glUniform1i(uSphereMapHandle, 0); // GL_TEXTURE0 연결
	glUniform1i(uDiffuseMapHandle, 1); // GL_TEXTURE1 연결

	GLuint uModelMat = glGetUniformLocation(shader, "uModelMat");
	GLuint uViewMat = glGetUniformLocation(shader, "uViewMat");
	GLuint uProjMat = glGetUniformLocation(shader, "uProjMat");
	GLuint uUseSphereMap = glGetUniformLocation(shader, "uUseSphereMap");

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat);
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat);
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniform1i(uUseSphereMap, 1);

	environment.Draw(shader);

	//----------------오브젝트-----------------
	float distance = 10.0f; // 카메라로부터 object의 거리
	mat4 RotationY = RotateY(rotationAngle);
	vec3 modelPos = calculateModelPosition(distance); // 카메라 앞에 있는 위치 계산
	mat4 ModelMat2 = Translate(modelPos) * RotationY * Scale(vec3(3, 3, 3));

	vec4 lPos = vec4(2, 2, 0, 1);
	vec4 li = vec4(0.8f, 0.8f, 0.8f, 1);
	vec4 kd = vec4(1, 1, 1, 1);

	GLuint uLIHandle = glGetUniformLocation(shader, "uLI");
	GLuint uLPosHandle = glGetUniformLocation(shader, "uLPos");
	GLuint uKdHandle = glGetUniformLocation(shader, "uKd");
	GLuint uEPosHandle = glGetUniformLocation(shader, "uEpos");
	GLuint uFresnelPowerHandle = glGetUniformLocation(shader, "uFresnelPower");
	GLuint uUseDiffuseMapHandle = glGetUniformLocation(shader, "uUseDiffuseMap");

	glUniform4f(uLIHandle, li.x, li.y, li.z, li.w);
	glUniform4f(uLPosHandle, lPos[0], lPos[1], lPos[2], lPos[3]);
	glUniform4f(uKdHandle, kd.x, kd.y, kd.z, kd.w);
	glUniform1f(uFresnelPowerHandle, fresnelPower);
	glUniform3f(uEPosHandle, eye.x, eye.y, eye.z);

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat2);
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat);
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat);

	glUniform1i(uUseSphereMap, 0); // 오브젝트는 텍스처 사용 X

	if (isDiffuseMap)
		glUniform1i(uUseDiffuseMapHandle, 1);
	else
		glUniform1i(uUseDiffuseMapHandle, 0);


	switch (modeIndex)
	{
	case 0:
		break;
	case 1:
		sphere.Draw(shader);
		break;
	case 2:
		bunny.Draw(shader);
		break;
	}

	glutSwapBuffers();
}

void idle()
{
	Sleep(16);

	if (isRotate)
	{
		rotationAngle += 1.0f;
		if (rotationAngle >= 360.0f) 
			rotationAngle = 0.0f;
	}

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
	{
		isRotate= !isRotate;
	}
	else if (ch == '1')
	{
		if (fresnelPower > 0.5f)
			fresnelPower -= 0.5f;
		cout << "fresnel power = " << fresnelPower << "\n";
	}
	else if (ch == '2')
	{
		if (fresnelPower <= 10)
			fresnelPower += 0.5f;
		cout << "fresnel power = " << fresnelPower << "\n";
	}
	else if (ch == '3')
	{
		if (isDiffuseMap)
			cout << "Diffuse light Map Off" << endl;
		else
			cout << "Diffuse light Map On" << endl;

		isDiffuseMap = !isDiffuseMap;
	}
	else if (ch == 'q')
	{
		modeIndex = (modeIndex + 1) % 3;
	}
	glutPostRedisplay();
}


void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            isDragging = true;
            prevMouseX = x;
            prevMouseY = y;
        }
        else if (state == GLUT_UP)
        {
            isDragging = false;
        }
    }
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			isRightDragging = true;
			prevRightMouseY = y;
		}
		else if (state == GLUT_UP)
		{
			isRightDragging = false;
		}
	}
}

void motionFunc(int x, int y)
{
	if (isDragging)
	{
		// 마우스 이동 차이
		int deltaX = x - prevMouseX;
		int deltaY = y - prevMouseY;

		// 회전 각도 조절
		float sensitivity = 0.2f;
		yaw -= deltaX * sensitivity;    // 수평 회전
		pitch += deltaY * sensitivity;  // 수직 회전 (음수로 뒤집음)

		// Pitch 값 제한
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		prevMouseX = x;
		prevMouseY = y;
	}
	else if (isRightDragging) // 줌
	{
		int deltaY = y - prevRightMouseY;
		prevRightMouseY = y;

		zoomDistance += deltaY * zoomSensitive;
		if (zoomDistance >= maxZoomDistance)
			zoomDistance = maxZoomDistance;
		if (zoomDistance <= minZoomDistance)
			zoomDistance = minZoomDistance;

		glutPostRedisplay();
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	string fileName;
	cout << "Input Map File Name: ";
	cin >> fileName;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Assignment5");

	glewExperimental = true;
	glewInit();

	if(!myInit(fileName))
	{
		cout << "Map 이름이 올바르지 않습니다.\n";
		return 0;
	}
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshapeFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutMainLoop();

	return 0;
}