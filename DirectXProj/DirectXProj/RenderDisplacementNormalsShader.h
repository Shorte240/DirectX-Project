// RenderDisplacementNormalsShader.h
// Used to render the normals of displacement based tessellated shapes.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class RenderDisplacementNormalsShader : public BaseShader
{
private:
	struct TessellationBufferType
	{
		float tessellationFactor;
		XMFLOAT3 padding;
	};

	struct HeightBufferType
	{
		float height;
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

	RenderDisplacementNormalsShader(ID3D11Device* device, HWND hwnd);
	~RenderDisplacementNormalsShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* heightTex, float height, float tessFactor, XMFLOAT4 waveVariables, XMFLOAT3 camPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* heightBuffer;
};



