// RenderTessellatedNormals shader.h
// Used to render the normals of tessellated shapes.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

#define MAX_LIGHTS 3

class RenderTessellatedNormalsShader : public BaseShader
{
private:
	struct TessellationBufferType
	{
		float tessellationFactor;
		XMFLOAT3 padding;
	};

	struct TimeBufferType
	{
		float time;
		float height;
		float frequency;
		float speed;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float pad;
	};

public:

	RenderTessellatedNormalsShader(ID3D11Device* device, HWND hwnd);
	~RenderTessellatedNormalsShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, float tessFactor, XMFLOAT4 waveVariables, XMFLOAT3 camPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
};

