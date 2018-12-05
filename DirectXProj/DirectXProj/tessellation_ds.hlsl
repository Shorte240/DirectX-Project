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
	float3 colour : COLOR;
	float4 lightViewPos[3] : TEXCOORD1;
};

struct OutputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 colour : COLOR;
	float4 lightViewPos[3] : TEXCOORD1;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition, vertexNormal;
    float2 vertexTexCoords;
	OutputType output;

	// Determine the position of the new vertex.
	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    // Determine the texCoord of the new vertex.
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    vertexTexCoords = lerp(t1, t2, uvwCoord.x);

	// Determine the normal of the new vertex.
	float3 n1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 n2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	vertexNormal = lerp(n1, n2, uvwCoord.x);

	// Offset position based on sine wave
	//vertexPosition.y = sin(vertexPosition.x + time);
    //vertexPosition.x = vertexPosition.x + (height * vertexNormal * (sin(((vertexPosition.z * frequency) + (time * speed)))));
    //vertexPosition.y = vertexPosition.y + (height * (sin(((vertexPosition.x * frequency) + (time * speed)))));
    //vertexPosition.z = vertexPosition.z + (height * vertexNormal * (sin(((vertexPosition.y * frequency) + (time * speed)))));

    vertexPosition += vertexNormal * (height * (sin(((vertexNormal * frequency) + (time * speed)))));

	// Modify the normals
	//vertexNormal.x = 1 - cos(vertexPosition.x + time);
	//vertexNormal.y = 1 - abs(cos(vertexPosition.x + time));

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

	// Send the input color into the pixel shader.
	output.colour = patch[0].colour;

	// WHERE ITS MOST LIKELY TO NOT WORK
	for (int i = 0; i < 3; i++)
	{
		output.lightViewPos[i] = patch[i].lightViewPos[i];
	}

	return output;
}

