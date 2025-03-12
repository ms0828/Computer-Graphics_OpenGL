#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include <mat.h>
#include "MyPlane.h"

GLuint program;
MyPlane plane;
bool bPlay = false;
bool bIsWave = false;
float myTime = 0;

void myInit()
{
	plane.Init(25);
	program = InitShader("vshader.glsl", "fshader.glsl");
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);

	GLuint uTime = glGetUniformLocation(program, "uTime");
	glUniform1f(uTime, myTime);
	
	GLuint bWave = glGetUniformLocation(program, "bWave");
	glUniform1f(bWave, bIsWave);

	plane.draw(program);

	glFlush();
}

void idle()
{
	if (bPlay) myTime += 0.02f;
	Sleep(20);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1': plane.decrease();	break;
	case '2': plane.increase();	break;
	case ' ': bPlay = !bPlay;		break;
	case 'w':
		bIsWave = !bIsWave;
		break;
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Assignment1");

	glewExperimental = true;
	glewInit();

	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}