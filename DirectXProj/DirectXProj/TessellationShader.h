// Tessellation shader.h
// Tessellation setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class TessellationShader : public BaseShader
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

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float tessFactor, XMFLOAT4 waveVariables, XMFLOAT3 camPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
};
