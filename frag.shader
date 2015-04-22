#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 Light1Direction_cameraspace;
in vec3 Light2Direction_cameraspace;
in vec2 TexCoord0;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 Light1Position_worldspace;
uniform vec3 Light2Position_worldspace;
uniform sampler2D gSampler;

void main(){
	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,0.8);
	float LightPower = 120.0;
	
	// Material properties
	vec3 DiffuseColor = vec3(1,1,1);
	vec3 AmbientColor = vec3(0.2,0.2,0.2) * DiffuseColor;
	vec3 SpecularColor = DiffuseColor * 0.1;
	float light1DiffuseIntensity = 0.5;
	float light2DiffuseIntensity = 0.5;
	float light1SpecularIntensity = 0.5;
	float light2SpecularIntensity = 0.5;
	float SpecularPower = 0.7;

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
	float diffuseFactor1 = dot( n,l1 );
	float diffuseFactor2 = dot( n,l2 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R1 = reflect(l1,n);
	vec3 R2 = reflect(l2,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float specularFactor1 = dot( E,-R1 );
	float specularFactor2 = dot( E,-R2 );
	
	//color = vs_vertexColor.rgb;
	
	vec3 colorSum = vec3(0,0,0);
	colorSum += AmbientColor;

	if(diffuseFactor1 > 0){
		vec3 diffuseColor1 = DiffuseColor * light1DiffuseIntensity * diffuseFactor1;
		colorSum = colorSum + diffuseColor1;
	}
	if(diffuseFactor2 > 0){
		vec3 diffuseColor2 = DiffuseColor * light2DiffuseIntensity * diffuseFactor2;
		colorSum = colorSum + diffuseColor2;
	}

	specularFactor1 = pow(specularFactor1, SpecularPower);
	specularFactor2 = pow(specularFactor2, SpecularPower);
	if(specularFactor1 > 0){
		vec3 specularColor1 = SpecularColor * light1SpecularIntensity * specularFactor1;
		colorSum += specularColor1;
	}
	if(specularFactor2 > 0){
		vec3 specularColor2 = SpecularColor * light2SpecularIntensity * specularFactor2;
		colorSum += specularColor2;
	}

	vec2 texc = TexCoord0.st;
	vec4 textureColor = texture(gSampler, texc);
	if(texc == vec2(0,0)){
		texc = vec2(0.5,0.5);
	}
	/*if(texc.x > 0.9){
		texc.x = 0;
	}
	if(texc.y > 0.9){
		texc.y = 0;
	}*/
	color = vec3(texc,1) * vs_vertexColor.xyz;
	//color = textureColor.rgb * vs_vertexColor.xyz * colorSum;
		// Ambient : simulates indirect lighting
		//MaterialAmbientColor +
		// Diffuse : "color" of the object
		//1.f * MaterialDiffuseColor * LightColor * LightPower * diffuseFactor1 / pow(distance1,2) +
		//1.f * MaterialDiffuseColor * LightColor * LightPower * diffuseFactor2 / pow(distance2,2) +
		// Specular : reflective highlight, like a mirror
		//1.f * MaterialSpecularColor * LightColor * LightPower * pow(diffuseFactor1,1) / pow(distance1,2) + 
		//1.f * MaterialSpecularColor * LightColor * LightPower * pow(diffuseFactor2,1) / pow(distance2,2);	
}