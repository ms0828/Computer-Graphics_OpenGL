#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include "MyColorCube.h"
#include "MyColorCylinder.h"
#include "MySphere.h"
GLuint program;
MyColorCube cube;
MyColorCylinder cylinder;
MySphere sphere;
void myInit()
{
	//cube.init();
	cylinder.init(8);
	//sphere.init(10, 10, 1.0f);

	program = InitShader("vshader.glsl", "fshader.glsl");
}

float myTime = 0;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);

	GLuint uTime = glGetUniformLocation(program, "uTime");
	glUniform1f(uTime, myTime);

	//cube.draw(program);
	cylinder.draw(program);
	//sphere.draw(program);
	glFlush();
}

bool bPlay = true;

void idle()
{
	if(bPlay)
		myTime += 0.0333f;
	Sleep(33);				// 33 milisecond
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		printf("1 is increasing polygon!\n");
		cylinder.increase();
		break;
	case '2':
		printf("2 is decreasing polygon!\n");
		cylinder.decrease();
		break;
	case ' ':
		printf("stop or play!\n");
		bPlay = !bPlay;
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("A Color Cube");

	glewExperimental = true;
	glewInit();

	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}