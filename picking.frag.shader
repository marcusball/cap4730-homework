#version 330 core

in vec4 vs_vertexColor;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
//uniform vec4 PickingColor;

void main(){

	color = vs_vertexColor;

}