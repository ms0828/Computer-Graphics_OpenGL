#version 330

uniform float uTime;

in vec4 vPosition;			// x,y,z,1		-> vec3 ->vec4
in vec4 vColor;

out vec4 color;

void main()
{
	float ang = uTime*90/180.0f*3.141592f;

	mat4 m = mat4(1.0f);			//[1 0 0 0]
	// y-rotation					//[0 1 0 0]
									//[0 0 1 0]
									//[0 0 0 1]
	m[0][0] = cos(ang);
	m[2][0] = sin(ang);
	m[0][2] = -sin(ang);
	m[2][2] = cos(ang);


	gl_Position = m*vPosition;

	gl_Position.w = 1.0f;
	color = vColor;
}