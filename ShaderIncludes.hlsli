#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__ 

// STRUCTS

struct Light 
{
	float3 ambientColor;
	float ambientIntensity;

	float3 diffuseColor;
	float diffuseIntensity;

	float3 direction;
	// pad 1

	float3 position;
	uint type;
};

struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float4 color		: COLOR;        // RGBA color
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
};

struct V2P_NormalMap
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float4 color		: COLOR;        // RGBA color
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float3 tangent		: TANGENT;
};

// HELPER FUNCTIONS

float3 NormalizedDirToLight(float3 lightDir)
{
	return normalize(-1 * lightDir);
}

float NdotL(float3 normal, float3 nDirToLight)
{
	return saturate(dot(normal, nDirToLight));
}

float Diffuse(float3 normal, float3 lightDir) 
{
	float3 nDirToLight = NormalizedDirToLight(lightDir);
	return NdotL(normal, nDirToLight);
}

float SpecularPhong(float3 normal, float3 lightDir, float3 dirToCamera, float exp) 
{
	// Calculate light reflection vector
	float3 refl = reflect(lightDir, normal);

	return pow(saturate(dot(refl, dirToCamera)), exp);
}

float4 CalculateLight(float3 normal, float4 surfaceColor, float3 worldPos, Light light, float3 cameraPosition, float shininess) 
{
	// directional diffuse
	float diffuse = 0;

	// specular of the the material (how shiny it is)
	float spec = 0;

	// vector from world space pixel pos to camera world world space position
	float3 toCamera = normalize(cameraPosition - worldPos);

	// point light diffuse & spec
	if(light.type == 1) 
	{
		float3 pointLightDirection = normalize(worldPos - light.position);
		diffuse = Diffuse(normal, pointLightDirection);
		spec = SpecularPhong(normal, pointLightDirection, toCamera, 64.0f);
	}
	// dir light diffuse & spec
	else 
	{
		diffuse = Diffuse(normal, light.direction);
		// make sure to normalize the light direction, otherwise the highlights are off
		spec = SpecularPhong(normal, normalize(light.direction), toCamera, 64.0f);
	}
	spec *= any(diffuse);
	float3 finalLight = ( (diffuse + ( shininess * spec ) ) * light.diffuseIntensity * light.diffuseColor) + (light.ambientColor * light.ambientIntensity);

	// @todo: think if you actually want to consider the surfaceColor here with the specular
	// @todo: specular lights will overlap each other, consider abstracting each light's specular and multiplying them all together
	return float4(finalLight * (float3)surfaceColor, 1);
}


#endif
