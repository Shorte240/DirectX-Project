#include "TessellatedSphereMesh.h"

#define PI 3.14159265

TessellatedSphereMesh::TessellatedSphereMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius_, float segments_)
{
	initBuffers(device);
	radius = radius_;
	segments = segments_;
}

TessellatedSphereMesh::~TessellatedSphereMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

void TessellatedSphereMesh::initBuffers(ID3D11Device * device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = 4 * (segments * segments);
	indexCount = 4 * (segments * segments);

	VertexType* vertices = new VertexType[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	int r = radius;
	int d = 2 * r;

	// Load the vertex array with data.
	// Load the index array with data.
	for (int lon = 0; lon < segments; lon++)
	{
		float delta = ((lon*PI) / (segments));

		for (int lat = 0; lat < segments; lat++)
		{
			float theta = (lat * 2 * PI) / (segments);	// Angle of latitude
			float x1 = (r*cos(theta))*(sin(delta));
			float y1 = r * cos(delta);
			float z1 = (r*sin(theta))*(sin(delta));

			lon++;
			delta = (lon*PI) / (segments);
			float x2 = (r*cos(theta))*(sin(delta));
			float y2 = r * cos(delta);
			float z2 = (r*sin(theta))*(sin(delta));

			lat++;
			theta = (lat * 2 * PI) / (segments);
			float x3 = (r*cos(theta))*(sin(delta));
			float y3 = r * cos(delta);
			float z3 = (r*sin(theta))*(sin(delta));

			lon--;
			delta = (lon*PI) / (segments);
			float x4 = (r*cos(theta))*(sin(delta));
			float y4 = r * cos(delta);
			float z4 = (r*sin(theta))*(sin(delta));
			lat--;

			float u1 = 0 + (lat * (1 / (segments)));
			float v1 = 0 + (lon * (1 / (segments)));
			float u2 = u1;
			float v2 = v1 + (1 / (segments));
			float u3 = u2 + (1 / (segments));
			float v3 = v2;
			float u4 = u3;
			float v4 = v1;

			vertices[lon].normal = XMFLOAT3(x1, y1, z1);
			vertices[lon].texture = XMFLOAT2(u1, v1); // GREEN
			vertices[lon].position = XMFLOAT3(x1, y1, z1);  // Top left.
			indices[lon] = lon;  // Top left.

			vertices[lon + 1].normal = XMFLOAT3(x1, y1, z1);
			vertices[lon + 1].texture = XMFLOAT2(u1, v1); // GREEN
			vertices[lon + 1].position = XMFLOAT3(x1, y1, z1);  // Top left.
			indices[lon + 1] = (lon + 1);  // Bottom left.

			vertices[lon + 2].normal = XMFLOAT3(x1, y1, z1);
			vertices[lon + 2].texture = XMFLOAT2(u1, v1); // GREEN
			vertices[lon + 2].position = XMFLOAT3(x1, y1, z1);  // Top left.
			indices[lon + 2] = (lon + 2);  // Bottom right.

			vertices[lon + 3].normal = XMFLOAT3(x1, y1, z1);
			vertices[lon + 3].texture = XMFLOAT2(u1, v1); // GREEN
			vertices[lon + 3].position = XMFLOAT3(x1, y1, z1);  // Top left.
			indices[lon + 3] = (lon + 3);  // Top right.
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(VertexType) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	vertexData = { vertices, 0 , 0 };
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc = { sizeof(unsigned long) * indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	indexData = { indices, 0, 0 };
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

void TessellatedSphereMesh::sendData(ID3D11DeviceContext * deviceContext, D3D11_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType_Colour);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}
