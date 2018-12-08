// Displacement Shader
// Takes in a displacement map and alters a given shape accordingly.

#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

#define MAX_LIGHTS 3

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
		XMFLOAT4 ambient[3];
		XMFLOAT4 diffuse[3];
		XMFLOAT4 direction[3];
		XMFLOAT4 position;
		float spotlightAngle;
		XMFLOAT3 padding;
	};

	struct HeightBufferType
	{
		float height;
		XMFLOAT3 padding;
	};

	struct MatrixBufferType2
	{
		XMMATRIX lightView[3];
		XMMATRIX lightProjection[3];
	};

	struct AttenuationBufferType
	{
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
		float pad;
	};

public:
	DisplacementShader(ID3D11Device* device, HWND hwnd);
	~DisplacementShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* heightTex, Light* lights[MAX_LIGHTS], float tessFactor, float height, XMFLOAT3 camPos, float spotAngle, float constFactor, float linFactor, float quadFactor);

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
	ID3D11Buffer* attenuationBuffer;
};

