#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in uint pointId;
layout(location = 1) in vec4 vertexPosition_modelspace;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in float vertexSize;
layout(location = 4) in vec4 vertexNormal;
layout(location = 5) in vec2 vertexTextureCoord;

out vec4 vs_vertexColor;

uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform uint ObjectId;

void main(){
	gl_PointSize = vertexSize;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vertexPosition_modelspace;
	
	vec3 colorIdParts = vec3(0,0,0);
	colorIdParts.r = (pointId & 0xff);
	colorIdParts.g = ((pointId >> 8) & 0xff);
	colorIdParts.b = ((pointId >> 16) & 0xff);

	//vs_vertexColor = vec4(colorIdParts, 1.0);	// set color based on the ID mark
	//vs_vertexColor = vec4(69.0, 80085.0, 1337.0,0.0);
	vs_vertexColor = vec4(float(ObjectId), float(pointId),float(0),0);

	//TexCoord0 = vertexTextureCoord;
}

