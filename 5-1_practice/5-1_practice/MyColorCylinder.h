#ifndef _MY_COLOR_CYLINDER_H
#define _MY_COLOR__MY_COLOR_CYLINDER_H

#include <vgl.h>
#include <vec.h>

struct MyCylinderVertex
{
	vec4 position;
	vec4 color;
};


class MyColorCylinder
{
public:
	int m_NumPoly;			// number of division for a circle
	int m_NumVertex;		// m_NumPoly * 2 * 3
	
	GLuint m_vao;
	GLuint m_vbo;

	bool m_bInit;
	
	MyColorCylinder()
	{
		m_NumPoly = 0;
		m_NumVertex = 0;
		m_bInit = false;
	}

	void init(int numPoly, bool bCap = true)
	{
		if (numPoly < 3)
			numPoly = 3;
		if (m_bInit == true && numPoly == m_NumPoly)
			return;

		m_NumPoly = numPoly;
		m_NumVertex = m_NumPoly * 2 * 3;
		
		if (bCap)
		{
			m_NumVertex += m_NumPoly * 3 * 2;
		}

		MyCylinderVertex* vertices = new MyCylinderVertex[m_NumVertex];

		float r = 0.5;
		float dtheta = 2 * 3.141592 / m_NumPoly;
		
		int cur = 0;
		for (int i = 0; i < m_NumPoly; i++)
		{
			float ang1 = dtheta * i; 
			float ang2 = dtheta * (i + 1);
			vec4 a = vec4(r * cos(ang1), 0.5, r * sin(ang1), 1); // 오른쪽 상단
			vec4 b = vec4(r * cos(ang2), 0.5, r * sin(ang2), 1); // 왼쪽 상단
			vec4 c = vec4(r * cos(ang1), -0.5, r * sin(ang1), 1); // 오른쪽 하단
			vec4 d = vec4(r * cos(ang2), -0.5, r * sin(ang2), 1); // 왼쪽 하단
			
			vec4 o1 = vec4(0, 0.5, 0, 1);
			vec4 o2 = vec4(0, -0.5, 0, 1);
			vec4 cc = vec4(0, 1, 0, 1);
			float ra = (float)i / (m_NumPoly - 1);
			vec4 color = vec4(ra, 0, 1 - ra, 1);

			if (bCap)	// upper cap
			{
				vertices[cur].position = a;		vertices[cur].color = cc; cur++;
				vertices[cur].position = o1;	vertices[cur].color = cc; cur++;
				vertices[cur].position = b;		vertices[cur].color = cc; cur++;
			}

			vertices[cur].position = a; vertices[cur].color = color; cur++;
			vertices[cur].position = b; vertices[cur].color = color; cur++;
			vertices[cur].position = c; vertices[cur].color = color; cur++;

			vertices[cur].position = b; vertices[cur].color = color; cur++;
			vertices[cur].position = d; vertices[cur].color = color; cur++;
			vertices[cur].position = c; vertices[cur].color = color; cur++;


			if (bCap)	// lower cap
			{
				vertices[cur].position = c;		vertices[cur].color = cc; cur++;
				vertices[cur].position = d;	vertices[cur].color = cc; cur++;
				vertices[cur].position = o2;		vertices[cur].color = cc; cur++;
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

		// 같은 버퍼에 메모리를 보내면, 기존에 있던 메모리는 지워짐
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyCylinderVertex) * m_NumVertex, vertices, GL_STATIC_DRAW);

		delete[] vertices;

		m_bInit = true;
	}

	void ConnectShader(GLuint program)
	{
		GLuint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyCylinderVertex), BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyCylinderVertex), BUFFER_OFFSET(sizeof(vec4)));
	}
	void draw(GLuint program)
	{
		glBindVertexArray(m_vao);
		glUseProgram(program);

		ConnectShader(program);
		glDrawArrays(GL_TRIANGLES, 0, m_NumVertex);
	}
	
	void increase()
	{
		int num = m_NumPoly + 1;
		init(num);
	}

	void decrease()
	{
		int num = m_NumPoly - 1;
		init(num);
	}

};


#endif
