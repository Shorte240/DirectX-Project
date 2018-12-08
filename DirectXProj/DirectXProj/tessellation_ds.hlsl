// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

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

cbuffer MatrixBuffer2 : register(b2)
{
	matrix lightViewMatrix[3];
	matrix lightProjectionMatrix[3];
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
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;
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

    // Calculate the tex coords.
    output.tex.x = vertexTexCoords.x;
    output.tex.y = vertexTexCoords.y;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(vertexNormal, (float3x3) worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the position of the vertice as viewed by the light source.
	for (int i = 0; i < 3; i++)
	{
		output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
		output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
		output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
	}

	// Calculate the world position of the vertice.
	output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;

	return output;
}

