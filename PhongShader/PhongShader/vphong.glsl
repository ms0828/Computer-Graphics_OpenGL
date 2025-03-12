#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec4 vNormal;

out vec4 Color;
out vec4 Pos;
out vec3 Normal;

uniform mat4 uProjMat;
uniform mat4 uModelMat;

void main()
{
	gl_Position  = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1,1,-1,1);	// z축 방향이 반대임
		   	
	Pos = uModelMat*vPosition;		

	vec4 N = uModelMat * vec4(vNormal.xyz,0);
	Normal = normalize(N.xyz);

	Color = vColor;
}
