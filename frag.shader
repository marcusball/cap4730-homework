#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;

// Ouput data
out vec3 color;

void main(){
	color = vs_vertexColor.xyz;

}