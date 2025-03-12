#version 330

in  vec4 color;
in  vec4 position;

out vec4 fColor;

void main()
{
	vec4 yColor = vec4(1.0f, 0.5f, 0, 1.0f);
	vec4 bColor = vec4(0, 0.5f, 1.0f, 1.0f);

	vec4 newColor = color;
	if(position.z > 0) 
	{
		float colorRatio = abs(position.z) * 3.5f;
		newColor = colorRatio * bColor + (1 - colorRatio) * color;
	}
	else
	{
		float colorRatio = abs(position.z) * 3.5f;
		newColor = colorRatio * yColor + (1 - colorRatio) * color;
	}

	fColor = newColor;
}