#ifndef _MY_SPHERE_H_
#define _MY_SPHERE_H_
#include <vgl.h>
#include <vec.h>

struct MySphereVertex {
	vec4 position;
	vec4 color;
};

class MySphere {
public:
	int m_numLaSlice, m_numLoSlice;		// latitude(À§µµ=°¡·Î´Ü¸éÀÇ °¹¼ö), longitude(°æµµ=¼¼·Î´Ü¸é)
	int m_numVertices;
	GLuint m_vao;
	GLuint m_vbo;
	bool m_bInit;

	MySphere() {
		m_numLaSlice = 0;
		m_numLoSlice = 0;
		m_numVertices = 0;
		m_bInit = false;
	}
	void init(int laSlice, int loSlice, float radius = 1.0f) {

		m_numLaSlice = laSlice;
		m_numLoSlice = loSlice;
		m_numVertices = loSlice * 2 * 3				// À§¾Æ·¡ ¶Ñ²±ÀÇ »ï°¢Çü°¹¼ö
			+ (laSlice - 2) * loSlice * 2 * 3;	// ¶Ñ²±À» Á¦¿ÜÇÑ ´Ü¸éÀÇ °¹¼ö(laSlice-2)

		MySphereVertex* vertices = new MySphereVertex[m_numVertices];
		float r = radius;
		float dphi = 3.141592 / laSlice;
		float dtheta = 2 * 3.141592 / loSlice;

		int cur = 0;
		for (int j = 0; j < laSlice; j++)
		{
			float y1 = r * cos(dphi * j);
			float r1 = r * sin(dphi * j);

			float y2 = r * cos(dphi * (j + 1));
			float r2 = r * sin(dphi * (j + 1));

			for (int i = 0; i < loSlice; i++)
			{
				float x1 = cos(dtheta * i);
				float z1 = sin(dtheta * i);

				float x2 = cos(dtheta * (i + 1));
				float z2 = sin(dtheta * (i + 1));

				vec4 a = vec4(r1 * x1, y1, r1 * z1, 1);
				vec4 b = vec4(r1 * x2, y1, r1 * z2, 1);
				vec4 c = vec4(r2 * x1, y2, r2 * z1, 1);
				vec4 d = vec4(r2 * x2, y2, r2 * z2, 1);
				
				vec4 color = vec4(0.5, 0.5, 0.5, 1);
				if ((i + j) % 2 == 1) color = vec4(0.8, 0.8, 0.8, 1);

				if (j != 0)
				{
					vertices[cur].position = a;	vertices[cur].color = color; cur++;
					vertices[cur].position = b;	vertices[cur].color = color; cur++;
					vertices[cur].position = c;	vertices[cur].color = color; cur++;
				}
				if (j != laSlice - 1)
				{
					vertices[cur].position = c;	vertices[cur].color = color; cur++;
					vertices[cur].position = b;	vertices[cur].color = color; cur++;
					vertices[cur].position = d;	vertices[cur].color = color; cur++;
				}
			}
		}

		if (m_bInit == false)
		{
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		}
		else
		{
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(MySphereVertex) * m_numVertices, vertices,
			GL_STATIC_DRAW);

		delete[] vertices;
		m_bInit = true;
	}
	void connectShaderWithProgram(GLuint prog) {
		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE,
			sizeof(MySphereVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE,
			sizeof(MySphereVertex), BUFFER_OFFSET(sizeof(vec4)));
	}
	void draw(GLuint prog) {
		glBindVertexArray(m_vao);
		glUseProgram(prog);
		connectShaderWithProgram(prog);
		glDrawArrays(GL_TRIANGLES, 0, m_numVertices);

	}
};

#endif


