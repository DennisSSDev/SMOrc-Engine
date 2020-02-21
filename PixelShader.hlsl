
struct DirectionalLight 
{
	float3 ambientColor;
	//pad 1
	float3 diffuseColor;
	//pad 1
	float3 direction;
};

cbuffer ExternalLightData : register(b0) 
{
	DirectionalLight dirLight;
	DirectionalLight dirLight2;
	DirectionalLight dirLight3;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float4 color		: COLOR;
};

float3 NormalizedDirToLight(float3 lightDir)
{
	return normalize(-1 * lightDir);
}

float NdotL(float3 normal, float3 nDirToLight)
{
	return saturate(dot(normal, nDirToLight));
}

float3 CalculateLight(float3 normal, float4 surfaceColor, DirectionalLight light) 
{
	float3 nDirToLight = NormalizedDirToLight(light.direction);

	float lightAmount = NdotL(normal, nDirToLight);

	return ((lightAmount * light.diffuseColor * (float3)surfaceColor) + (light.ambientColor * (float3)surfaceColor));
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 light1 = CalculateLight(input.normal, input.color, dirLight);
	float3 light2 = CalculateLight(input.normal, input.color, dirLight2);
	float3 light3 = CalculateLight(input.normal, input.color, dirLight3);

	return float4(light1 + light2 + light3, 1);
}