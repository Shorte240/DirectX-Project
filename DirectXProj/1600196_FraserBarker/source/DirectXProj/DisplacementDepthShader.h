// Tessellation Depth Shader.h
// Tessellation depth setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class DisplacementDepthShader : public BaseShader
{
private:
	struct TessellationBufferType
	{
		float tessellationFactor;
		XMFLOAT3 padding;
	};

	struct TimeBufferType
	{
		float height;
		XMFLOAT3 padding;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float pad;
	};
public:
	DisplacementDepthShader(ID3D11Device* device, HWND hwnd);
	~DisplacementDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float tessFactor, float height, XMFLOAT3 camPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	// Matrix buffers
	ID3D11Buffer * matrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;

	// Variable buffers
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
};

