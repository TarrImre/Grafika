#version 430 compatibility

out vec4 myColor;

void main(void)
{
	gl_Position = gl_Vertex;
	myColor = vec4(0.0, 0.0, 1.0, 1.0);
}