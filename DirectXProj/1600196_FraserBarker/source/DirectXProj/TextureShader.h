// Texture Shader.h
// Same as the one used in the labs
// Used to apply render textures to orthomeshes.

#pragma once

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class TextureShader : public BaseShader
{
public:
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	// Matrix Buffers
	ID3D11Buffer * matrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;
};

