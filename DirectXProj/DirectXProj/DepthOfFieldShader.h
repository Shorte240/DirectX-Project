// Depth Of Field Shader.h
// Using the vertical and horizontal blur shaders
// Creates a depth of field post processing effect on the scene.

#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class DepthOfFieldShader : public BaseShader
{
private:
	struct DepthBufferType
	{
		float range;
		float nearVal;
		float farVal;
		float padding;
	};
public:
	DepthOfFieldShader(ID3D11Device* device, HWND hwnd);
	~DepthOfFieldShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* normalSceneTex, ID3D11ShaderResourceView* blurSceneTex, ID3D11ShaderResourceView* depthSceneTex, float range, float nearV, float farV);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	// Matrix buffers
	ID3D11Buffer * matrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;

	// Variable buffers
	ID3D11Buffer* depthBuffer;
};

