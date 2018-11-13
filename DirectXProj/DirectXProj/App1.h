// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "TessellatedSphereMesh.h"

#define MAX_LIGHTS 2

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass(Light* light, RenderTexture* rTex);
	void finalPass();
	void gui();

private:
	TextureShader* textureShader;
	PlaneMesh* mesh;
	CubeMesh* cubeMesh;
	SphereMesh* sphereMesh;
	OrthoMesh* leftOrthoMesh;
	OrthoMesh* rightOrthoMesh;
	TessellatedSphereMesh* tessellatedSphereMesh;

	Light* lights[MAX_LIGHTS];
	Model* model;
	ShadowShader* shadowShader;
	DepthShader* depthShader;

	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;
	float rotate;
};

#endif