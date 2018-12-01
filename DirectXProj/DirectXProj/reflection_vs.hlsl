// Reflection vertex shader

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ReflectionBuffer : register(b1)
{
	matrix reflectionMatrix;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPos : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;
	matrix reflectProjWorld;

	// Change pos vector to be 4 for proper matrix calcs
	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	// Create reflection matrix
	reflectProjWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjWorld = mul(worldMatrix, reflectProjWorld);

	// Calc input pos against reflectProjWorld
	output.reflectionPos = mul(input.position, reflectProjWorld);
    //output.reflectionPos.w = 1.0f;

	return output;
}
