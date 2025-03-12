#version 330

in vec4 Color;
in vec4 Pos;
in vec3 Normal;

out vec4 fColor;

void main()
{
	vec4 Lpos = vec4(5,5,0,1);			// light position in camera coord.
	vec4 I = vec4(1,1,1,1);
	vec4 ka = vec4(0.1,0.1,0.1,1);	// ambient reflection coeff. 
	vec4 kd = vec4(0.5,0.2,0.2,1);	// diffuse reflection coeff.
	vec4 ks = vec4(0.5,0.5,0.5,1);
	float sh = 100;
	
	vec4 vPos = vec4(0,0,0,1);

	vec3 L3 = normalize((Lpos - Pos).xyz);
	vec3 N3 = normalize(Normal);
	vec3 R3 = 2*dot(L3, N3)*N3 - L3;	// L3 + R3 = 2*dot(L3, N3)*N3
	vec3 V3 = normalize((vPos - Pos).xyz);
	
	vec4 amb = ka*I;
	vec4 diff = kd*I*max(dot(N3, L3),0);
	vec4 spec = ks*I*pow(max(dot(R3, V3),0), sh);

	fColor = amb + diff + spec;
}
