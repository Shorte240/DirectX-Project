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
void main(triangle InputType input[3], inout LineStream<OutputType> lineStream)
{
	OutputType output;

	for (int i = 0; i < 3; i++)
	{
		output.position = input[i].position;
		lineStream.Append(output);

		output.position = input[i].position + (float4(input[i].normal, 1.0f) * length(input[i].normal));
		lineStream.Append(output);
	}
	lineStream.RestartStrip();
}