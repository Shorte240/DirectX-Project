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
	float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;
	float3 vertNorm : TEXCOOR5;
	float2 uv : TEXCOORD6;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 colour : COLOR;
	float4 lightViewPos[3] : TEXCOORD1;
	float3 worldPosition : TEXCOORD4;
	float3 vertNorm : TEXCOOR5;
	float3 vertNorm2 : TEXCOOR6;
	float2 uv : TEXCOORD7;
};

// Input primitives
// Point, Line, Triangle

// Output types
// PointStream, LineStream, TriangleStream

[maxvertexcount(4)]
void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	float3 vertexNormal;

	/*Begin Function CalculateSurfaceNormal(Input Triangle) Returns Vector

		Set Vector U to(Triangle.p2 minus Triangle.p1)
		Set Vector V to(Triangle.p3 minus Triangle.p1)

		Set Normal.x to(multiply U.y by V.z) minus(multiply U.z by V.y)
		Set Normal.y to(multiply U.z by V.x) minus(multiply U.x by V.z)
		Set Normal.z to(multiply U.x by V.y) minus(multiply U.y by V.x)

		Returning Normal

	End Function*/

	/*Begin Function CalculateSurfaceNormal(Input Polygon) Returns Vector

		Set Vertex Normal to(0, 0, 0)

		Begin Cycle for Index in[0, Polygon.vertexNumber)

		Set Vertex Current to Polygon.verts[Index]
		Set Vertex Next    to Polygon.verts[(Index plus 1) mod Polygon.vertexNumber]

		Set Normal.x to Sum of Normal.x and(multiply(Current.y minus Next.y) by(Current.z plus Next.z))
		Set Normal.y to Sum of Normal.y and(multiply(Current.z minus Next.z) by(Current.x plus Next.x))
		Set Normal.z to Sum of Normal.z and(multiply(Current.x minus Next.x) by(Current.y plus Next.y))

		End Cycle

		Returning Normalize(Normal)

	End Function*/

	/*float3 u = (input[1].position - input[0].position);
	float3 v = (input[2].position - input[0].position);

	vertexNormal.x = ((mul(u.y, v.z)) - (mul(u.z, v.y)));
	vertexNormal.y = ((mul(u.z, v.x)) - (mul(u.x, v.z)));
	vertexNormal.z = ((mul(u.x, v.y)) - (mul(u.y, v.x)));*/

	//normalize(vertexNormal);

	// Determine the normal of the new vertex.
	for (int i = 0; i < 3; i++)
	{
		// -- SLIGHTY NEWER --//
		/*float4 posCur = input[i].position;
		float4 posNext = input[((i + 1) % 3)].position;

		vertexNormal.x += (mul((posCur.y - posNext.y), (posCur.z + posNext.z)));
		vertexNormal.y += (mul((posCur.z - posNext.z), (posCur.x + posNext.x)));
		vertexNormal.z += (mul((posCur.x - posNext.x), (posCur.y + posNext.y)));*/

		// -- OLD --//
		float3 n1 = lerp(input[0].normal, input[1].normal, input[i].uv.y);
		float3 n2 = lerp(input[0].normal, input[2].normal, input[i].uv.y);
		float3 n3 = lerp(input[2].normal, input[1].normal, input[i].uv.y);
		vertexNormal += lerp(n1, n2, input[i].uv.x);
	}

	//normalize(vertexNormal);

	for (int i = 0; i < 3; i++)
	{
		output.position = input[i].position;
		output.normal = input[i].normal;
		/*output.normal = mul(vertexNormal, (float3x3) worldMatrix);
		output.normal = normalize(output.normal);*/
		output.tex = input[i].tex;
		output.colour = input[i].colour;
		output.lightViewPos[0] = input[i].lightViewPos[0];
		output.lightViewPos[1] = input[i].lightViewPos[1];
		output.lightViewPos[2] = input[i].lightViewPos[2];
		output.worldPosition = input[i].worldPosition;
		output.vertNorm = input[i].vertNorm;
		output.vertNorm2 = vertexNormal;
		output.uv = input[i].uv;
		triStream.Append(output);
	}
	triStream.RestartStrip();

	// Recalculate the normals
	///
	///
	///
}