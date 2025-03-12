#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include "MyColorCube.h"

GLuint program;
MyColorCube cube;

void myInit()
{
	cube.init();
	
	// load shaders
	program = InitShader("vshader.glsl", "fshader.glsl");
}

float myTime = 0;

void display()
{
	glUseProgram(program);

	printf("Time = %f \n", myTime);
	GLuint uTime = glGetUniformLocation(program, "uTime");
	glUniform1f(uTime, myTime);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX);
	glFlush();
}

void idle()
{
	myTime += 0.0333f;
	Sleep(33);				// 33 milisecond
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutCreateWindow("A Color Cube");

	glewExperimental = true;
	glewInit();

	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}