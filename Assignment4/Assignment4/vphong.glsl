#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vSurfaceNormal;
in	vec3 vVertexNormal;

out vec3 N3;
out vec3 L3;
out vec3 V3;

uniform mat4 uProjectionMat;
uniform mat4 uModelAndViewMat;
uniform vec4 uLightPosition;
uniform float uIsFlat;

void main()
{	
	gl_Position  = uProjectionMat * uModelAndViewMat * vPosition;
	gl_Position *= vec4(1,1,-1,1);

	vec4 lPos = uLightPosition;
	vec4 vPos = uModelAndViewMat * vPosition;
	vec4 N;

	if(uIsFlat == 0)
	{
		N = uModelAndViewMat*vec4(vVertexNormal,0);
	}
	else if(uIsFlat == 1)
	{
		N = uModelAndViewMat*vec4(vSurfaceNormal,0);
	}
	
	vec4 L = lPos - vPos;
	N3 = normalize(N.xyz);
	L3 = normalize(L.xyz);
	V3 = normalize(vec3(0,0,0) - vPos.xyz);
}