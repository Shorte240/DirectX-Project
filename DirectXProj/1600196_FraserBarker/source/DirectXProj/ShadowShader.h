// Shadow Shader.h
// Multiple light shader setup
#ifndef _SHADOWSHADER_H_
#define _SHADOWSHADER_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;

#define MAX_LIGHTS 3

class ShadowShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[3];
		XMMATRIX lightProjection[3];
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

	struct AttenuationBufferType
	{
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
		float pad;
	};

public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView*depthMap, ID3D11ShaderResourceView*depthMap2, ID3D11ShaderResourceView*depthMap3, Light* lights[MAX_LIGHTS], float spotAngle, float constFactor, float linFactor, float quadFactor);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	// Matrix buffers
	ID3D11Buffer* matrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;

	// Variable buffers
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* attenuationBuffer;
};

#endif