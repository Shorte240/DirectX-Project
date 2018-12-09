// Horizontal Blur Shader.h
// Loads horizontal blur shaders (vs and ps)
// Passes screen width to shaders, for sample coordinate calculation
// It is the one used in the labs
// But it is used for a depth of field post processing effect
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class HorizontalBlurShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT3 padding;
	};

public:

	HorizontalBlurShader(ID3D11Device* device, HWND hwnd);
	~HorizontalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float width);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	// Matrix buffers
	ID3D11Buffer * matrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;

	// Variable buffers
	ID3D11Buffer* screenSizeBuffer;
};