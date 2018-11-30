// Displacement Shader
// Takes in a displacement map and alters a given shape accordingly.

#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

#define MAX_LIGHTS 2

class DisplacementShader : public BaseShader
{
private:
	struct TessellationBufferType
	{
		float tessellationFactor;
		XMFLOAT3 padding;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float pad;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[2];
		XMFLOAT4 diffuse[2];
		XMFLOAT4 direction[2];
	};

	struct HeightBufferType
	{
		float height;
		XMFLOAT3 padding;
	};

	struct MatrixBufferType2
	{
		XMMATRIX world;
		XMMATRIX lightView[2];
		XMMATRIX lightProjection[2];
	};
public:
	DisplacementShader(ID3D11Device* device, HWND hwnd);
	~DisplacementShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightTex, Light* lights[MAX_LIGHTS], float tessFactor, XMFLOAT4 waveVariables, XMFLOAT3 camPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer* matrixBuffer2;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* heightBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* cameraBuffer;
};

