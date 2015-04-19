#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 Light1Direction_cameraspace;
in vec3 Light2Direction_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 Light1Position_worldspace;
uniform vec3 Light2Position_worldspace;

void main(){
	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,0.8);
	float LightPower = 120.0f;
	
	// Material properties
	vec3 MaterialDiffuseColor = vs_vertexColor.rgb;
	vec3 MaterialAmbientColor = vec3(0.2,0.2,0.2) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = MaterialDiffuseColor * 0.1;

	// Distance to the light
	float distance1 = length( Light1Position_worldspace - Position_worldspace );
	float distance2 = length( Light2Position_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l1 = normalize( Light1Direction_cameraspace );
	vec3 l2 = normalize( Light2Direction_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta1 = clamp( dot( n,l1 ), 0,1 );
	float cosTheta2 = clamp( dot( n,l2 ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R1 = reflect(-l1,n);
	vec3 R2 = reflect(-l2,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha1 = clamp( dot( E,R1 ), 0,1 );
	float cosAlpha2 = clamp( dot( E,R2 ), 0,1 );
	
	//color = vs_vertexColor.rgb;
	
	color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		0.5 * MaterialDiffuseColor * LightColor * LightPower * cosTheta1 / pow(distance1,2) +
		0.5 * MaterialDiffuseColor * LightColor * LightPower * cosTheta2 / pow(distance2,2) +
		// Specular : reflective highlight, like a mirror
		1.f * MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha1,5) / pow(distance1,2) + 
		1.f * MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha2,5) / pow(distance2,2);	
}