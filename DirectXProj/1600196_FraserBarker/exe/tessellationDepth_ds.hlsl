// Tessellation domain shader
// Re-calculates the vertex position, texture coordinates and normals for the given mesh
// Passes position and depth position to the pixel shader

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
	float time;
	float height;
	float frequency;
	float speed;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD1;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	OutputType output;

	// Temp variables to calculate specific components.
	float3 vertexPosition, vertexNormal;
	float2 vertexTexCoords;

	// Determine the position of the new vertex.
	float3 v1 = lerp(patch[0].position, patch[1].position, uvCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvCoord.y);
	vertexPosition = lerp(v1, v2, uvCoord.x);

	// Determine the texCoord of the new vertex.
	float2 t1 = lerp(patch[0].tex, patch[1].tex, uvCoord.y);
	float2 t2 = lerp(patch[3].tex, patch[2].tex, uvCoord.y);
	vertexTexCoords = lerp(t1, t2, uvCoord.x);

	// Determine the normal of the new vertex.
	float3 n1 = lerp(patch[0].normal, patch[1].normal, uvCoord.y);
	float3 n2 = lerp(patch[3].normal, patch[2].normal, uvCoord.y);
	vertexNormal = lerp(n1, n2, uvCoord.x);

	// Offset position based on the normals using a sine wave
	vertexPosition += vertexNormal * (height * (sin(((vertexNormal * frequency) + (time * speed)))));

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;

	return output;
}

