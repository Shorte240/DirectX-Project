// Render tessellated normals geometry shader

struct InputType
{
	float4 position : POSITION;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	//float3 normal : NORMAL;
};

// Input primitives
// Point, Line, Triangle

// Output types
// PointStream, LineStream, TriangleStream

[maxvertexcount(2)]
void main(triangle InputType input[3], inout LineStream<OutputType> triStream)
{
	OutputType output;

	for (int i = 0; i < 3; i++)
	{
		output.position = input[i].position;
		triStream.Append(output);

		output.position = input[i].position + (float4(input[i].normal, 1.0f) * length(input[i].normal));
		triStream.Append(output);
	}
	triStream.RestartStrip();
}