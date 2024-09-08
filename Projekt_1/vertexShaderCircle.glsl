#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;


uniform float offsetY;
uniform float offsetX;

void main()
{
	float y = aPosition.y + offsetY;
    float x = aPosition.x + offsetX;
	gl_Position = vec4(aPosition.x + offsetX, aPosition.y + offsetY, aPosition.z, 1.0);
	// be�ll�tja az ourColor-t a vertex adatokb�l kapott bemeneti sz�nre.
    ourColor = aColor;
} 