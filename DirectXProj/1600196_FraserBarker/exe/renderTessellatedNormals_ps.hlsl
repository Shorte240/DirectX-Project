// Render tessellated normals pixel shader
// Returns red colour to show the normals

struct InputType
{
	float4 position : SV_POSITION;
};

float4 main(InputType input) : SV_TARGET
{
	return float4(1.0f,0.0f,0.0f,1.0f);
}