#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 1) in vec4 vertexPosition_modelspace;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in float vertexSize;

out vec4 vs_vertexColor;
void main(){
	gl_Position =  vertexPosition_modelspace;
	gl_PointSize = vertexSize;

	// UV of the vertex. No special space for this one.
	vs_vertexColor = vertexColor;
}

