// Reflection pixel shader

Texture2D shaderTexture : register(t0);
Texture2D reflectionTexture : register(t1);
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPos : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
	float4 texColour;
	float2 reflectTexCoord;
	float4 reflectionColour;
	float4 colour;

	// Sample the texture pixel at this location
	texColour = shaderTexture.Sample(SampleType, input.tex);
    //texColour.w = 1.0f;

	// Calc projected reflection tex coord
	reflectTexCoord.x = ((input.reflectionPos.x / input.reflectionPos.w) / 2.0f) + 0.5f;
	reflectTexCoord.y = ((-input.reflectionPos.y / input.reflectionPos.w) / 2.0f) + 0.5f;

	// Sample texture pixel from reflection texture
    reflectionColour = reflectionTexture.Sample(SampleType, reflectTexCoord);
    //reflectionColour.w = 1.0f;

	// Do linear interpolation between two textures for blend effect
	colour = lerp(texColour, reflectionColour, 0.15f);

	return colour;
}

