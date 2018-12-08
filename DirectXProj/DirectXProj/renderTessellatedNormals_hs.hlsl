// Render tessellated normals hull shader
// Prepares control points for tessellation

cbuffer TessellationBuffer : register(b0)
{
	float tessellationFactor;
	float3 padding;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float dist : TEXCOORD1;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float patchDist = (inputPatch[0].dist + inputPatch[1].dist + inputPatch[2].dist + inputPatch[3].dist) / inputPatch.Length;

	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessellationFactor / patchDist;
	output.edges[1] = tessellationFactor / patchDist;
	output.edges[2] = tessellationFactor / patchDist;
	output.edges[3] = tessellationFactor / patchDist;

	// Set the tessellation factor for tessallating inside the triangle.
	output.inside[0] = tessellationFactor / patchDist;
	output.inside[1] = tessellationFactor / patchDist;

	return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;

	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;

	// Set the texture coordinates for this control point as the output texture coordinate.
	output.tex = patch[pointId].tex;

	// Set the normal for this control point as the output normal.
	output.normal = patch[pointId].normal;

	return output;
}