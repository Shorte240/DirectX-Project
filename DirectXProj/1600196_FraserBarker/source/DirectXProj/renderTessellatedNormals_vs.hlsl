// Render tessellated normals vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data

cbuffer CameraBuffer : register(b0)
{
	float3 camPos;
	float pad;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float dist : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	// Pass the texture coordinates into the hull shader.
	output.tex = input.tex;

	// Pass the normal to the domain shader.
	output.normal = input.normal;

	// Get the distance between the camera and vertex position.
	output.dist = distance(camPos, input.position);

	return output;
}