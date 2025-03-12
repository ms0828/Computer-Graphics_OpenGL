#version 330

uniform float uTime;
uniform bool bWave;
in vec4 vPosition;
in vec4 vColor;

out vec4 position;
out vec4 color;

void main()
{
	float ang = uTime * 30 / 180.0f * 3.141592f;
	float distance = sqrt(vPosition.x * vPosition.x + vPosition.y * vPosition.y);
	float maxDistance = 0.7f;
	float distanceRatio = 0.0f;
	if(distance <= maxDistance)
	{
		distanceRatio = (maxDistance - distance) / maxDistance;
	}

	float frequency = 25.0f;
	float waveSpeed = 15.0f;
	float waveScale = 0.3f * distanceRatio;
	
	float height = 0.0f;
	if(bWave)
	{
		height = sin((distance * frequency) - ang * waveSpeed) * waveScale;
	}


	// x-rotation matrix
	mat4 m1 = mat4(1.0f);
	float xRotation = 45.0f;
	float xRotationAngle = xRotation / 180 * 3.141592; 
	m1[1][1] = cos(xRotationAngle);
	m1[2][1] = -sin(xRotationAngle);
	m1[1][2] = sin(xRotationAngle);
	m1[2][2] = cos(xRotationAngle);


	// z-rotation matrix
    mat4 m2 = mat4(1.0f);
    m2[0][0] = cos(-ang);
    m2[0][1] = -sin(-ang);
    m2[1][0] = sin(-ang);
    m2[1][1] = cos(-ang);

	vec4 newPosition = vPosition;
	newPosition.z += height;

	gl_Position = m1 * m2 * newPosition;

	color = vColor;
	position = newPosition;
}