// Depth of Field pixel shader

Texture2D normalSceneTexture : register(t0);
Texture2D blurSceneTexture : register(t1);
Texture2D depthSceneTexture : register(t2);
SamplerState SampleType : register(s0);

cbuffer DepthBuffer : register(b0)
{
	float range;
	float nearVal;
	float farVal;
	float padding;
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
	float depthTexel = depthSceneTexture.Sample(SampleType, input.tex).r;

	float centreDepthTexel = depthSceneTexture.Sample(SampleType, float2(0.5f,0.5f)).r;

	// Invert the depth texel
	depthTexel = 1 - depthTexel;
	centreDepthTexel = 1 - centreDepthTexel;

	// Scale to world from 0 - 1 range
	centreDepthTexel *= (farVal - nearVal);
	depthTexel *= (farVal - nearVal);

	// Calculate blur factor
	float blurFactor = saturate(abs(depthTexel - centreDepthTexel) / range);

	// Return lerp
	return lerp(normalScene, blurScene, blurFactor);
}

