// Tessellation geometry shader
// Simple example of passing data through to pixel shader.

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;
};

// Input primitives
// Point, Line, Triangle

// Output types
// PointStream, LineStream, TriangleStream

[maxvertexcount(4)]
void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	// Pass all data to the pixel shader.
	for (int i = 0; i < 3; i++)
	{
		output.position = input[i].position;
		output.tex = input[i].tex;
		output.normal = input[i].normal;
		output.lightViewPos[0] = input[0].lightViewPos[0];
		output.lightViewPos[1] = input[1].lightViewPos[1];
		output.lightViewPos[2] = input[2].lightViewPos[2];
		output.worldPosition = input[i].worldPosition;
		triStream.Append(output);
	}

	triStream.RestartStrip();
}