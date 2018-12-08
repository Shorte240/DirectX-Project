// Displacement domain shader
// After tessellation the domain shader processes the all the vertices

Texture2D heightTex : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer HeightBuffer : register(b1)
{
    float height;
    float3 padding;
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
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD1;
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
    // Sample the texture. Use colour to alter height of plane.
    float4 textureColour = heightTex.SampleLevel(sampler0, vertexTexCoords, 0, 0);
    vertexPosition += vertexNormal * (textureColour.r * height);

	// Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;

    return output;
}

