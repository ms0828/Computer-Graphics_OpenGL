#include <vgl.h>
#include <InitShader.h>
#include <vec.h>

// (x,y,z) --> (x,y,z,w=1) 
// (r,g,b) --> (r,g,b,a=1)

const int NUM_VERTEX = 6 * 2 * 3;
vec4 points[NUM_VERTEX];
vec4 colors[NUM_VERTEX];

GLuint program;

vec4 vertex_pos[] = {
	vec4(-0.5,-0.5, 0.5,1),
	vec4(0.5,-0.5, 0.5,1),
	vec4(0.5, 0.5, 0.5,1),
	vec4(-0.5, 0.5, 0.5,1),
	vec4(-0.5,-0.5,-0.5,1),
	vec4(0.5,-0.5,-0.5,1),
	vec4(0.5, 0.5,-0.5,1),
	vec4(-0.5, 0.5,-0.5,1)
};
vec4 vertex_color[] = {
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1),
	vec4(1, 1, 0, 1),
	vec4(0, 1, 1, 1),
	vec4(1, 0, 1, 1),
	vec4(1, 1, 1, 1),
	vec4(0, 0, 0, 1)
};

void setRectangle(int a, int b, int c, int d)
{
	static int vindex = 0;
	points[vindex] = vertex_pos[a];		colors[vindex] = vertex_color[a];  vindex++;
	points[vindex] = vertex_pos[b];		colors[vindex] = vertex_color[b];  vindex++;
	points[vindex] = vertex_pos[c];		colors[vindex] = vertex_color[c];  vindex++;

	points[vindex] = vertex_pos[c];		colors[vindex] = vertex_color[c];  vindex++;
	points[vindex] = vertex_pos[d];		colors[vindex] = vertex_color[d];  vindex++;
	points[vindex] = vertex_pos[a];		colors[vindex] = vertex_color[a];  vindex++;

}

void generateCube() {
	setRectangle(0, 3, 2, 1);
	setRectangle(0, 4, 7, 3);
	setRectangle(3, 7, 6, 2);
	setRectangle(2, 6, 5, 1);
	setRectangle(1, 5, 4, 0);
	setRectangle(4, 5, 6, 7);
}

void myInit()
{
	// 1. create data in CPU
	generateCube();

	// 2. send the data to GPU
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// 3. load shaders

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// 4. Connect my data with the shaders
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
}

float myTime = 0;

void display()
{
	GLuint uTime = glGetUniformLocation(program, "uTime");
	glUniform1f(uTime, myTime);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX);
	glEnable(GL_DEPTH_TEST);
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

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
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