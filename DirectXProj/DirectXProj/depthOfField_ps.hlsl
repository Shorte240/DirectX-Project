// Depth of Field pixel shader

Texture2D normalSceneTexture : register(t0);
Texture2D blurSceneTexture : register(t1);
Texture2D depthSceneTexture : register(t2);
SamplerState SampleType : register(s0);

cbuffer DepthBuffer : register(b0)
{
	float distance;
	float range;
	float nearVal;
	float farVal;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	// Get the normal scene texel
	float4 normalScene = normalSceneTexture.Sample(SampleType, input.tex);

	// Get the blurred scene texel
	float4 blurScene = blurSceneTexture.Sample(SampleType, input.tex);

	// Get the depth texel
	float fDepth = depthSceneTexture.Sample(SampleType, input.tex).r;

	// Invert the depth texel
	//fDepth = 1 - fDepth;

	// Calculate distance
	float fSceneZ = (-nearVal * farVal) / (fDepth - farVal);
	float blurFactor = saturate(abs(fSceneZ - distance) / range);

	// Return lerp
	return lerp(normalScene, blurScene, blurFactor);
}

