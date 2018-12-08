// Render tessellated normals pixel shader

struct InputType
{
	float4 position : SV_POSITION;
	//float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
	return float4(1.0f,0.0f,0.0f,1.0f);
}