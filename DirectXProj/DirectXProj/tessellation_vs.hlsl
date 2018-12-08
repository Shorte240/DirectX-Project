// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

cbuffer CameraBuffer : register(b0)
{
	float3 camPos;
	float pad;
};

//cbuffer MatrixBuffer : register(b1)
//{
//	matrix worldMatrix;
//	matrix lightViewMatrix[3];
//	matrix lightProjectionMatrix[3];
//};

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
	float dist : PSIZE;
	/*float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;*/
};

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	// Pass the tex coord to the hull shader.
	output.tex = input.tex;

	// Pass the normal to the domain shader.
	output.normal = input.normal;

	// Get the distance between the camera and vertex position.
	output.dist = distance(camPos, output.position);

	// Calculate the position of the vertice as viewed by the light source.
	/*for (int i = 0; i < 3; i++)
	{
		output.lightViewPos[i] = mul(input.position, worldMatrix);
		output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
		output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
		output.lightViewPos[i].w = 1.0f;
	}

	output.worldPosition = mul(input.position, worldMatrix).xyz;*/

	return output;
}
