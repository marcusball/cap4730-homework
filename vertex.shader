#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;

out vec4 vs_vertexColor;
void main(){
	gl_Position =  vertexPosition_modelspace;

	// UV of the vertex. No special space for this one.
	vs_vertexColor = vertexColor;
}

