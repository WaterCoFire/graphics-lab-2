// Minimal fragment shader

#version 410 core // Personal modification 400 core to 410 core

in vec4 fcolour;
out vec4 outputColor;
void main()
{
	outputColor = fcolour;
}