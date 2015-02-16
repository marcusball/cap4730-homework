#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in uint pointId;
layout(location = 1) in vec4 vertexPosition_modelspace;

out vec4 vs_vertexColor;
void main(){
	gl_Position = vertexPosition_modelspace;

	float colorId = pointId / 255.0;

	vs_vertexColor = vec4(colorId, 0.0, 0.0, 1.0);	// set color based on the ID mark
}


