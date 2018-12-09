// TessellatedSphereMesh
// A sphere mesh that can be tessellated.

#pragma once

#include "DXF.h"

using namespace DirectX;

class TessellatedSphereMesh : public BaseMesh
{
public:
	TessellatedSphereMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius_, float segments_);
	~TessellatedSphereMesh();

	void sendData(ID3D11DeviceContext* deviceContext, D3D11_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
protected:
	void initBuffers(ID3D11Device* device);

	// Variables
	float radius;
	float segments;
};

