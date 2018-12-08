// Vertical blur shader handler
// Loads vertical blur shaders (vs and ps)
// Passes screen height to shaders, for sample coordinate calculation
// It is the one used in the labs
// But it is used for a depth of field post processing effect
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class VerticalBlurShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenHeight;
		XMFLOAT3 padding;
	};

public:

	VerticalBlurShader(ID3D11Device* device, HWND hwnd);
	~VerticalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float width);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	// Matrix buffer
	ID3D11Buffer * matrixBuffer;

	// Sampler states
	ID3D11SamplerState* sampleState;

	// Variable buffers
	ID3D11Buffer* screenSizeBuffer;
};
