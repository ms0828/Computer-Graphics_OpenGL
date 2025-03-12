#version 330
in vec3 N3; 
in vec3 L3;
in vec3 V3; 
in vec2 T2;
in vec3 wN;
in vec3 wV;
in vec3 wP;

in vec3 pos;

out vec4 fColor;

uniform mat4 uModelMat; 
uniform mat4 uProjMat; 

uniform sampler2D uSphereMap;
uniform sampler2D uDiffuseMap;


uniform float uTime;

uniform vec4 uLI;
uniform vec4 uKd;

uniform float uFresnelPower;

uniform int uUseSphereMap;
uniform int uUseDiffuseMap;

void main()
{
	vec3 N = normalize(N3); 
	vec3 V = normalize(V3);
	vec3 L = normalize(L3);
	float d =  max(0,dot(N, L));

	vec3 viewDir = normalize(wP - wV);
	vec3 R = reflect(viewDir, normalize(wN));
	R = normalize(R);
	
	// 반사 벡터를 구 좌표계로 변환
	float u = 0.5 + atan(R.z, R.x) / (2.0 * 3.141592); // 수평 방향 (u 좌표)
	float v = 0.5 + asin(R.y) / 3.141592;             // 수직 방향 (v 좌표)
	vec2 sphereCoord = vec2(u,v);


	// ------------환경 구 매핑-------------
	if (uUseSphereMap == 1)
	{
		vec3 dir = pos;
		dir.y = -dir.y;
		
		float theta = atan(dir.z, dir.x);
		float phi = acos(dir.y);

		// 텍스처 좌표로 변환
		float u = (theta + 3.141592) / (2.0 * 3.141592); // 0 ~ 1
		float v = phi / 3.141592;                       

		vec4 texColor = texture(uSphereMap, vec2(u,v));
		fColor = texColor;
	}
	else 
	{
		vec4 dif;
		if(uUseDiffuseMap == 1)
		{
			vec3 normal = normalize(wN);
			vec4 offsetRight = vec4(0.1f, 0.1f, 0.1f, 0.1f);
			// 표면 노말 벡터를 구 좌표계로 변환
			float u = 0.5 + atan(normal.z, normal.x) / (2.0 * 3.141592); // 수평 방향 (u 좌표)
			float v = 0.5 + asin(normal.y) / 3.141592;             // 수직 방향 (v 좌표)
			vec2 sphereCoord = vec2(u,v);
			vec4 diffuse_color = texture(uDiffuseMap, sphereCoord);
			dif = offsetRight + diffuse_color;
		}
		else
		{
			dif = uKd * uLI * d;
		}
		
		
		float F = 0; 
		float ratio = F + (1.0 - F) * pow((1.0 + dot(viewDir, normalize(wN))), uFresnelPower);
		if(uFresnelPower >= 10.5f)
		{
			ratio = 0;
		}
		vec4 reflect_color = texture(uSphereMap, sphereCoord);

		fColor = mix(dif, reflect_color, ratio);
	}
	fColor.w = 1;	
}
