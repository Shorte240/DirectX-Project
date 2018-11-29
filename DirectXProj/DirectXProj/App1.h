// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "TessellatedSphereMesh.h"
#include "TessellationShader.h"
#include "TessellationDepthShader.h"

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
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	TessellationShader* tessellationShader;
	TessellationDepthShader* tessellationDepthShader;

	PlaneMesh* mesh;
	CubeMesh* cubeMesh;
	SphereMesh* sphereMesh;
	OrthoMesh* leftOrthoMesh;
	OrthoMesh* rightOrthoMesh;
	TessellatedSphereMesh* waterTessellatedSphereMesh;
	TessellatedSphereMesh* earthTessellatedSphereMesh;

	Light* lights[MAX_LIGHTS];
	Model* model;
	

	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;

	// Variables
	float tessellationFactor;
	float rotate;

	struct WaveVariables
	{
		float elapsedTime;
		float height;
		float frequency;
		float speed;
	};

	WaveVariables wavVar;
};

#endif