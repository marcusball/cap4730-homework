#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in uint pointId;
layout(location = 1) in vec4 vertexPosition_modelspace;
layout(location = 3) in float vertexSize;

out vec4 vs_vertexColor;

uniform mat4 MVP;

void main(){
	gl_Position = MVP * vertexPosition_modelspace;
	gl_PointSize = vertexSize;

	float colorId = pointId / 255.0;

	vs_vertexColor = vec4(colorId, 0.0, 0.0, 1.0);	// set color based on the ID mark
}


