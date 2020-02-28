
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

cbuffer ExternalData : register(b0) 
{
	Light dirLight;
	Light pointLight;
	Light dirLight3;

	float3 cameraPosition;
	float shininess;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float4 color		: COLOR;
	float3 worldPos		: POSITION;
};

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
	
	// 1. Get the dot product between the perfect reflection of the light
	//    and the vector to the camera, as that tells us how "close" we 
	//    are to seeing the perfect reflection.  
	// 2. Saturate to ensure it doesn't go negative on the back of the object
	// 3. Raise to a power to create a very quick "falloff" (so the highlight
	//    appears smaller if the object is shinier)
	return pow(saturate(dot(refl, dirToCamera)), exp);
}

float4 CalculateLight(float3 normal, float4 surfaceColor, float3 worldPos, Light light) 
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
		spec = SpecularPhong(normal, light.direction, toCamera, 64.0f);
	}

	float3 finalLight = ( (diffuse + ( shininess * spec ) ) * light.diffuseIntensity * light.diffuseColor) + (light.ambientColor * light.ambientIntensity);

	// @todo: think if you actually want to consider the surfaceColor here with the specular
	return float4(finalLight * surfaceColor, 1);
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float4 light1 = CalculateLight(input.normal, input.color, input.worldPos, dirLight);
	float4 light2 = CalculateLight(input.normal, input.color, input.worldPos, pointLight);
	//return light2;
	float4 light3 = CalculateLight(input.normal, input.color, input.worldPos, dirLight3);
	return (light1 + light2 + light3);
}