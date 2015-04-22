#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 1) in vec4 vertexPosition_modelspace;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in float vertexSize;
layout(location = 4) in vec4 vertexNormal;
layout(location = 5) in vec2 vertexTextureCoord;

out vec4 vs_vertexColor;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 Light1Direction_cameraspace;
out vec3 Light2Direction_cameraspace;
out vec2 TexCoord0;

uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 Light1Position_worldspace;
uniform vec3 Light2Position_worldspace;

void main(){
	gl_PointSize = vertexSize;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vertexPosition_modelspace;
	
	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (ModelMatrix * vertexPosition_modelspace).xyz;
	
	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( ViewMatrix * ModelMatrix * vertexPosition_modelspace).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 Light1Position_cameraspace = ( ViewMatrix * vec4(Light1Position_worldspace,1)).xyz;
	vec3 Light2Position_cameraspace = ( ViewMatrix * vec4(Light2Position_worldspace,1)).xyz;
	Light1Direction_cameraspace = Light1Position_cameraspace + EyeDirection_cameraspace;
	Light2Direction_cameraspace = Light2Position_cameraspace + EyeDirection_cameraspace;
	
	// Normal of the the vertex, in camera space
	Normal_cameraspace = ( ViewMatrix * ModelMatrix * vertexNormal).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// UV of the vertex. No special space for this one.
	vs_vertexColor = vertexColor;

	TexCoord0 = vertexTextureCoord;
}

