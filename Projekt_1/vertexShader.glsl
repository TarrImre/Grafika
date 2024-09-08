#version 430

layout (location = 0) in vec3 aPosition;

uniform float offsetY;
uniform float offsetX;


void main(void)
{
	gl_Position = vec4(aPosition.x + offsetX, aPosition.y + offsetY, aPosition.z, 1.0);
}