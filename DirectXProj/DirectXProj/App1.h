// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TessellatedSphereMesh.h"
#include "Shaders.h"

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
	void firstPass();
	void downSample();
	void verticalBlur();
	void horizontalBlur();
	void depthPass(Light* light, RenderTexture* rTex);
	void upSample();
	void finalPass();
	void gui();

private:
	TextureShader* textureShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	TessellationShader* tessellationShader;
	TessellationDepthShader* tessellationDepthShader;
	DisplacementShader* displacementShader;
	DisplacementDepthShader* displacementDepthShader;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* verticalBlurShader;

	PlaneMesh* mesh;
	OrthoMesh* leftOrthoMesh;
	OrthoMesh* rightOrthoMesh;
	OrthoMesh* screenOrthoMesh;
	TessellatedSphereMesh* waterTessellatedSphereMesh;
	TessellatedSphereMesh* earthTessellatedSphereMesh;
	TessellatedSphereMesh* reflectiveTessellatedSphereMesh;

	Light* lights[MAX_LIGHTS];	

	RenderTexture* shadowMap;
	RenderTexture* shadowMap2;

	RenderTexture* renderTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* downSampleTexture;
	RenderTexture* upSampleTexture;

	// Variables
	float tessellationFactor;
	float displacementHeight;

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