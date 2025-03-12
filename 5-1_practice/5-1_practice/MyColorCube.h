#ifndef _MY_COLOR_CUBE_
#define _MY_COLOR_CUBE_

#include <vgl.h>
#include <vec.h>

class MyColorCube
{
public:
	int NUM_VERTEX; 
	vec4* points;
	vec4* colors;
	GLuint vao;
	GLuint vbo;

	MyColorCube() {
		NUM_VERTEX = 6 * 2 * 3;
		points = new vec4[NUM_VERTEX];
		colors = new vec4[NUM_VERTEX];
	}

	void init() {
			// 1. create data in CPU
		generateCube();

		// 2. send the data to GPU
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec4)*NUM_VERTEX, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * NUM_VERTEX, points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * NUM_VERTEX, sizeof(vec4) * NUM_VERTEX, colors);
	}
	void draw(GLuint program)
	{
		glBindVertexArray(vao);
		connectShader(program);
		glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX);
	}
	void connectShader(GLuint program)
	{
		GLuint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4) * NUM_VERTEX));

	}
	void setRectangle(int a, int b, int c, int d, vec4 vertex_pos[], vec4 vertex_color[])
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
		setRectangle(0, 3, 2, 1, vertex_pos, vertex_color);
		setRectangle(0, 4, 7, 3, vertex_pos, vertex_color);
		setRectangle(3, 7, 6, 2, vertex_pos, vertex_color);
		setRectangle(2, 6, 5, 1, vertex_pos, vertex_color);
		setRectangle(1, 5, 4, 0, vertex_pos, vertex_color);
		setRectangle(4, 5, 6, 7, vertex_pos, vertex_color);
	}
};

#endif




