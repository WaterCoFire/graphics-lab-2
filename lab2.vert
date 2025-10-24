// Vertex shader with a model transformation uniform

#version 410 core // Personal modification 400 core to 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
out vec4 fcolour;
uniform mat4 model;

uniform mat4 view; // Personal modification for different views

uniform uint colormode; // Personal modification for color mode switching

uniform mat4 projection; // Personal modification - projection matrix for perspective effects

void main()
{
	gl_Position = projection * view * model * position; // Personal modification: "projection" and "view" at the beginning

	// Personal modification below
	// For color mode switching
	if (colormode == 0u) {
	    fcolour = colour;
	} else {
	    fcolour = position * 2.0 + vec4(0.5, 0.5, 0.5, 1.0);
	}
}