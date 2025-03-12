#version 330

uniform float uTime;

in vec4 vPosition;			// x,y,z,1		-> vec3 ->vec4
in vec4 vColor;

out vec4 color;
out vec4 position;

void main()
{
	float ang = uTime*90/180.0f*3.141592f;

	mat4 m1 = mat4(1.0f);			//[1 0 0 0]
	// x-rotation					//[0 c-s 0]
									//[0 s c 0]
									//[0 0 0 1]
	m1[1][1] = cos(ang);
	m1[2][1] = -sin(ang);
	m1[1][2] = sin(ang);
	m1[2][2] = cos(ang);


	mat4 m2 = mat4(1.0f);			//[ c 0 s 0]
	// y-rotation					//[ 0 1 0 0]
									//[-s 0 c 0]
									//[ 0 0 0 1]
	m2[0][0] = cos(ang);
	m2[2][0] = sin(ang);
	m2[0][2] = -sin(ang);
	m2[2][2] = cos(ang);


	gl_Position = m2*m1*vPosition;

	gl_Position.w = 1.0f;
	color = vColor;
	position = vPosition;
}