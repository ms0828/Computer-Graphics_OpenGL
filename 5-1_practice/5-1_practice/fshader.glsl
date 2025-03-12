#version 330

in  vec4 color;
in  vec4 position;

out vec4 fColor;

void main()
{
	fColor = color;

	/*
	if(abs(position.x) >0.45 &&
	   abs(position.y) >0.45) discard;

	if(abs(position.x) >0.45 &&
	   abs(position.z) >0.45) discard;

	if(abs(position.y) >0.45 &&
	   abs(position.z) >0.45) discard;
	*/
}