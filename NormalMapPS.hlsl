#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	Light dirLight;
	Light pointLight;
	Light dirLight3;

	float3 cameraPosition;
	float shininess;
}

Texture2D diffuseTexture:		register(t0);
Texture2D normalMap:			register(t1);

SamplerState samplerOptions:	register(s0);

float4 main( V2P_NormalMap input ) : SV_TARGET
{
	float3 unpackedNormal = normalMap.Sample(samplerOptions, input.uv).rgb * 2 - 1;
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	float3 N = input.normal;  // Must be normalized
	float3 T = input.tangent; // Must be normalized
	T = normalize(T - N * dot(T, N)); // Gram-Schmidt orthogonalization
	float3 B = cross(T, N); // bi - tangent 
	float3x3 TBN = float3x3(T, B, N);

	// order of the multiplication matters
	input.normal = mul(unpackedNormal, TBN); 
	//return float4(input.normal, 1.f);
	input.color = diffuseTexture.Sample(samplerOptions, input.uv) * input.color;

	float4 light1 = CalculateLight(input.normal, input.color, input.worldPos, dirLight, cameraPosition, shininess);
	float4 light2 = CalculateLight(input.normal, input.color, input.worldPos, pointLight, cameraPosition, shininess);
	float4 light3 = CalculateLight(input.normal, input.color, input.worldPos, dirLight3, cameraPosition, shininess);

	return (light1 + light2 + light3);
}