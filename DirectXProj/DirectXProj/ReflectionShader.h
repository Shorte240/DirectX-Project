// Reflection shader class

#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class ReflectionShader : public BaseShader
{
private:
	struct ReflectionBufferType
	{
		XMMATRIX reflectionMatrix;
	};
public:
	ReflectionShader(ID3D11Device* device, HWND hwnd);
	~ReflectionShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, const XMMATRIX &reflectionMatrix);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer * reflectionBuffer;
};

