// Tessellation pixel shader
// Output colour passed to stage.

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
};

float4 main(InputType input) : SV_TARGET
{
	return input.colour;
}