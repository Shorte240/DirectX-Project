#include "DisplacementShader.h"

DisplacementShader::DisplacementShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	// Quad tessellation
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"displacement_ds.cso", L"tessellation_ps.cso");
}

DisplacementShader::~DisplacementShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (heightBuffer)
	{
		heightBuffer->Release();
		heightBuffer = 0;
	}

	if (tessellationBuffer)
	{
		tessellationBuffer->Release();
		tessellationBuffer = 0;
	}

	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	// Release the attenuation constant buffer.
	if (attenuationBuffer)
	{
		attenuationBuffer->Release();
		attenuationBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DisplacementShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC matrixBufferDesc2;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC heightBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC attenuationBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc2.ByteWidth = sizeof(MatrixBufferType2);
	matrixBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc2.MiscFlags = 0;
	matrixBufferDesc2.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc2, NULL, &matrixBuffer2);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup time buffer
	heightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightBufferDesc.ByteWidth = sizeof(HeightBufferType);
	heightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightBufferDesc.MiscFlags = 0;
	heightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightBufferDesc, NULL, &heightBuffer);

	// Setup the description of the tessellation buffer description.
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationBuffer);

	// Setup camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	// Setup attenuation buffer
	// Setup the description of the attenuation factor dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	attenuationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	attenuationBufferDesc.ByteWidth = sizeof(AttenuationBufferType);
	attenuationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attenuationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attenuationBufferDesc.MiscFlags = 0;
	attenuationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&attenuationBufferDesc, NULL, &attenuationBuffer);
}

void DisplacementShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void DisplacementShader::setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & worldMatrix, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, ID3D11ShaderResourceView* heightTex, Light* lights[MAX_LIGHTS], float tessFactor, float height, XMFLOAT3 camPos, float spotAngle, float constFactor, float linFactor, float quadFactor)
{
	HRESULT result, result2;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	// TO DO: INCLUDE ALL LIGHT VIEW AND ORTHO MATRICES
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(lights[0]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(lights[0]->getOrthoMatrix());
	XMMATRIX tLightViewMatrix2 = XMMatrixTranspose(lights[1]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix2 = XMMatrixTranspose(lights[1]->getOrthoMatrix());
	XMMATRIX tLightViewMatrix3 = XMMatrixTranspose(lights[2]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix3 = XMMatrixTranspose(lights[2]->getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	result2 = deviceContext->Map(matrixBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType2* dataPtr2 = (MatrixBufferType2*)mappedResource.pData;
	dataPtr2->world = tworld;// worldMatrix;
	dataPtr2->lightView[0] = tLightViewMatrix;
	dataPtr2->lightProjection[0] = tLightProjectionMatrix;
	dataPtr2->lightView[1] = tLightViewMatrix2;
	dataPtr2->lightProjection[1] = tLightProjectionMatrix2;
	dataPtr2->lightView[2] = tLightViewMatrix3;
	dataPtr2->lightProjection[2] = tLightProjectionMatrix3;
	deviceContext->Unmap(matrixBuffer2, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &matrixBuffer2);

	// Send camera position to vertex shader
	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->cameraPos = camPos;
	camPtr->pad = 1.0f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cameraBuffer);

	// Send tessellation data to hull shader
	TessellationBufferType* tesPtr;
	deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tesPtr = (TessellationBufferType*)mappedResource.pData;
	tesPtr->tessellationFactor = tessFactor;
	tesPtr->padding = XMFLOAT3(1.0f, 1.0f, 1.0f);
	deviceContext->Unmap(tessellationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	//Additional
	// Send light data to pixel shader
	HeightBufferType* heightPtr;
	deviceContext->Map(heightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	heightPtr = (HeightBufferType*)mappedResource.pData;
	heightPtr->height = height;
	heightPtr->padding = XMFLOAT3(1.0f, 1.0f, 1.0f);
	deviceContext->Unmap(heightBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &heightBuffer);

	//Additional
	// Send light data to pixel shader
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		lightPtr->ambient[i] = lights[i]->getAmbientColour();
		lightPtr->diffuse[i] = lights[i]->getDiffuseColour();
		lightPtr->direction[i] = XMFLOAT4(lights[i]->getDirection().x, lights[i]->getDirection().y, lights[i]->getDirection().z, 1.0f);
	}
	lightPtr->position = XMFLOAT4(lights[2]->getPosition().x, lights[2]->getPosition().y, lights[2]->getPosition().z, 1.0f);
	lightPtr->spotlightAngle = spotAngle;
	lightPtr->padding = XMFLOAT3(1.0f, 1.0f, 1.0f);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Send attenuation data to pixel shader
	AttenuationBufferType* attenPtr;
	deviceContext->Map(attenuationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	attenPtr = (AttenuationBufferType*)mappedResource.pData;
	attenPtr->constantFactor = constFactor;
	attenPtr->linearFactor = linFactor;
	attenPtr->quadraticFactor = quadFactor;
	attenPtr->pad = 0.0f;
	deviceContext->Unmap(attenuationBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &attenuationBuffer);

	// Set shader texture resource for height map in domain shader.
	deviceContext->DSSetShaderResources(0, 1, &heightTex);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &heightTex);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}
