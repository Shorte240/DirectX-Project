// Horizontal Blur pixel shader
// Blur pixels horizontally based on the 4 neighbours left or right
// of the given centre pixel

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenWidth;
	float3 padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float weight0, weight1, weight2, weight3, weight4;
	float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
	//--- Regular Weights ---//
	weight0 = 0.382928f;
	weight1 = 0.241732f;
	weight2 = 0.060598f;
	weight3 = 0.005977f;
	weight4 = 0.000229f;

	//--- Inverted Weights ---//
	/*weight0 = 0.000229f;
	weight1 = 0.005977f;
	weight2 = 0.060598f;
	weight3 = 0.241732f;
	weight4 = 0.382928f;*/

	//--- Ninth Weights ---//
	/*weight0 = 1.0f / 9.0f;
	weight1 = 1.0f / 9.0f;
	weight2 = 1.0f / 9.0f;
	weight3 = 1.0f / 9.0f;
	weight4 = 1.0f / 9.0f;*/

	//--- Random Weights ---//
	/*weight0 = 0.000229f;
	weight1 = 0.382928f;
	weight2 = 0.005977f;
	weight3 = 0.241732f;
	weight4 = 0.060598f;*/

	// Initialize the colour to black.
	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float texelSize = 1.0f / screenWidth;
	// Add the horizontal pixels to the colour by the specific weight of each.
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -4.0f, 0.0f)) * weight4;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -3.0f, 0.0f)) * weight3;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -2.0f, 0.0f)) * weight2;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -1.0f, 0.0f)) * weight1;
	colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 1.0f, 0.0f)) * weight1;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 2.0f, 0.0f)) * weight2;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 3.0f, 0.0f)) * weight3;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 4.0f, 0.0f)) * weight4;

	// Set the alpha channel to one.
	colour.a = 1.0f;

	return colour;
}
