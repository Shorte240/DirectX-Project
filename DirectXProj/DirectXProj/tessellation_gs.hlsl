cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 colour : COLOR;
	float4 lightViewPos[2] : TEXCOORD1;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 colour : COLOR;
	float4 lightViewPos[2] : TEXCOORD1;
};

// Input primitives
// Point, Line, Triangle

// Output types
// PointStream, LineStream, TriangleStream

[maxvertexcount(4)]
void main(triangle InputType input[3], inout LineStream<OutputType> triStream)
{
	OutputType output;

	for (int i = 0; i < 3; i++)
	{
		output.position = input[i].position;
		output.normal = input[i].normal;
		output.tex = input[i].tex;
		output.colour = input[i].colour;
		output.lightViewPos[0] = input[i].lightViewPos[0];
		output.lightViewPos[1] = input[i].lightViewPos[1];
		triStream.Append(output);
	}
	triStream.RestartStrip();

	// Recalculate the normals
	///
	///
	///
}