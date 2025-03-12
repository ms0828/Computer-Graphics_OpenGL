#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"

MyCube cube;
MyPyramid pyramid;

GLuint program;
GLuint uMat;

mat4 g_Mat = mat4(1.0);

float g_Time = 0;
float g_aspect = 1.0f;			// aspect ratio(종횡비)
								// width/height > 1
float g_winWidth = 500;
float g_winHeight = 500;


void myInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}

mat4 myLookAt(vec3 eye, vec3 focus, vec3 up)
{
	vec3 n = focus - eye;
	n = normalize(n);
	vec3 u = dot(n, up)*n;
	vec3 v = up - u;
	v = normalize(v);
	vec3 w = cross(v, -n);
	w = normalize(w);

	mat4 Rw(1.0f);
	Rw[0][0] = w.x;	Rw[0][1] = v.x;	Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y;	Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z;	Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	Rc[0][0] = w.x;	Rc[0][1] = w.y;	Rc[0][2] = w.z;
	Rc[1][0] = v.x;	Rc[1][1] = v.y;	Rc[1][2] = v.z;
	Rc[2][0] =-n.x;	Rc[2][1] =-n.y;	Rc[2][2] =-n.z;

//	Rc = transpose(Rw);
	mat4 T = Translate(-eye.x, -eye.y, -eye.z);
	
	return Rc * T;
}


mat4 myOrtho(float left, float right, 
	         float bottom, float top,
	         float zNear, float zFar)
{
	float cx = (left + right) / 2;
	float cy = (bottom + top) / 2;
	float cz = -zNear;

	float sx =  2 / (right - left);
	float sy =  2 / (top - bottom);
	float sz = 1 / (zFar - zNear);

	mat4 T = Translate(-cx, -cy, -cz);
	mat4 S = Scale(sx, sy, sz);
	return S * T;
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	g_aspect = g_winWidth / (float)g_winHeight;
	glViewport(0, 0, g_winWidth, g_winHeight);

	GLuint uMat = glGetUniformLocation(program, "uMat");

	//quantize (양자화)
	//0~z : n등분해서 단계로 표현하는 것
	float ex = 2 * cos(2 * 3.141592 * g_Time / 2);
	float ez = 2 * sin(2 * 3.141592 * g_Time / 2);

	vec3 eye(ex, 2 * sin(g_Time) + 2, ez);
	vec3 focus(0, 0, 0);
	vec3 up(0, 1, 0);
	mat4 V = myLookAt(eye, focus, up);

	// aspect = width / height
	float xmax = 2;
	float ymax = xmax / g_aspect;
	mat4 P = myOrtho(-xmax, xmax, -ymax, ymax, 0, 10);

	mat4 M(1.0);
	glUniformMatrix4fv(uMat, 1, true, P*V*M);

	//pyramid.Draw(program);
	cube.Draw(program);
	glutSwapBuffers();
}

void myIdle()
{
	g_Time += 0.016f;
	Sleep(16);
	glutPostRedisplay();
}

void myReshape(int winW, int winH) {
	g_winWidth = winW;
	g_winHeight = winH;

}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(g_winWidth,g_winHeight);
	glutCreateWindow("Cube and Pyramid");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutIdleFunc(myIdle);
	glutReshapeFunc(myReshape);

	glutMainLoop();

	return 0;
}