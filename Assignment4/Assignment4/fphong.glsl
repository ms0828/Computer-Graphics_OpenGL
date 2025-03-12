#version 330

in  vec4 color;

in vec3 N3;
in vec3 L3;
in vec3 V3;

out vec4 fColor;

uniform vec4 uLightIntensity;
uniform vec4 uKa;
uniform vec4 uKd;
uniform vec4 uKs;
uniform float uShiness;

void main()
{
	vec3 N = normalize(N3);
	vec3 L = normalize(L3);
	vec3 V = normalize(V3);
	vec3 R = normalize(2 * dot(L, N) * N - L);
	float d =  max(0,dot(N, L));
	float s = pow(max(dot(V,R),0), uShiness);

	vec4 amb = uKa * uLightIntensity;
	vec4 dif = uKd * uLightIntensity * d;
	vec4 spec = uKs * uLightIntensity* s; 

	fColor = dif + spec + amb;
}