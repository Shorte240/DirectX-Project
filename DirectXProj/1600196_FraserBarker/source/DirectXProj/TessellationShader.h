// Tessellation Shader.h
// Tessellation setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

#define MAX_LIGHTS 3

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

	struct LightBufferType
	{
		XMFLOAT4 ambient[3];
		XMFLOAT4 diffuse[3];
		XMFLOAT4 direction[3];
		XMFLOAT4 position;
		float spotlightAngle;
		XMFLOAT3 padding;
	};

	struct LightMatrixBufferType
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

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float tessFactor, XMFLOAT4 waveVariables, XMFLOAT3 camPos, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* lights[MAX_LIGHTS], float spotAngle, float constFactor, float linFactor, float quadFactor);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	// Matrix buffers
	ID3D11Buffer * matrixBuffer;
	ID3D11Buffer* lightMatrixBuffer;

	// Sample states
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;

	// Variable buffers
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* attenuationBuffer;
};
