#ifndef _MY_PLANE_H_
#define _MY_PLANE_H_

#include <vgl.h>
#include <vec.h>

struct MyPlaneVertex
{
	vec4 position;
	vec4 color;
};

class MyPlane
{
public:
	int m_Division;
	int m_NumVertex;
	float m_Width;
	float m_Height;
	GLuint m_vao;
	GLuint m_vbo;

	MyPlane()
	{
		m_Division = 30;
		m_Width = 1.4f;
		m_Height = 1.4f;
	}

	void Init(int n)
	{
		if (n < 2)
			n = 2;
		m_Division = n;
		m_NumVertex = m_Division * m_Division * 2 * 3;

		MyPlaneVertex* vertices = new MyPlaneVertex[m_NumVertex];

		int cur = 0;
		for (int i = 0; i < m_Division; i++)
		{
			for (int j = 0; j < m_Division; j++)
			{
				float x_start = -m_Width / 2;
				float y_start = m_Height / 2;
				float x_width = m_Width / m_Division;
				float y_width = m_Height / m_Division;
				
				vec4 a = vec4(x_start + j * x_width, y_start - i * y_width, 0, 1);
				vec4 b = vec4(x_start + j * x_width + x_width, y_start - i * y_width, 0, 1);
				vec4 c = vec4(x_start + j * x_width + x_width, y_start - i * y_width - y_width, 0, 1);
				vec4 d = vec4(x_start + j * x_width, y_start - i * y_width - y_width, 0, 1);

				vec4 color;
				if ((i + j) % 2 == 0)
				{
					color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
				}
				else
				{
					color = vec4(0.6f, 0.6f, 0.6f, 1.0f);
				}

				vertices[cur].position = a;	vertices[cur].color = color; cur++;
				vertices[cur].position = b;	vertices[cur].color = color; cur++;
				vertices[cur].position = c;	vertices[cur].color = color; cur++;

				vertices[cur].position = a;	vertices[cur].color = color; cur++;
				vertices[cur].position = c;	vertices[cur].color = color; cur++;
				vertices[cur].position = d;	vertices[cur].color = color; cur++;
			}
		}

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(MyPlaneVertex) * m_NumVertex, vertices, GL_STATIC_DRAW);

		delete[] vertices;
	}

	void connectShader(GLuint prog)
	{
		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyPlaneVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyPlaneVertex), BUFFER_OFFSET(sizeof(vec4)));
	}

	void draw(GLuint program)
	{
		glBindVertexArray(m_vao);
		glUseProgram(program);
		connectShader(program);
		glDrawArrays(GL_TRIANGLES, 0, m_NumVertex);
	}

	void increase() {
		int num = m_Division + 1;
		Init(num);
	}
	void decrease() {
		int num = m_Division - 1;
		Init(num);
	}
};


#endif