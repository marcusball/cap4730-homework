#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 Light1Direction_cameraspace;
in vec3 Light2Direction_cameraspace;
//in vec2 TexCoord0;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 Light1Position_worldspace;
uniform vec3 Light2Position_worldspace;
uniform sampler2D gSampler;

void main(){
	
	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l1 = normalize( Light1Direction_cameraspace );
	vec3 l2 = normalize( Light2Direction_cameraspace );

	/* It's probably really bad that this program doesn't run
	 * if the above lines are removed or commented out, even
	 * though they are currently doing nothing at all. */

	color = vs_vertexColor.xyz;
}