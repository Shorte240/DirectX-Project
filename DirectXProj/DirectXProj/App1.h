// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TessellatedSphereMesh.h"
#include "Shaders.h"
#include "D3D.h"

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
	void cameraDepthPass();
	void depthPass(Light* light, RenderTexture* rTex);
	void depthOfFieldPass();
	void upSample();
	void renderReflection(float height);
	void reflectionPass();
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
	DepthOfFieldShader* depthOfFieldShader;
	ReflectionShader* reflectionShader;

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

	RenderTexture* normalSceneTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* downSampleTexture;
	RenderTexture* upSampleTexture;
	RenderTexture* depthOfFieldTexture;
	RenderTexture* cameraDepthTexture;
	RenderTexture* reflectionTexture;

	// Variables
	float tessellationFactor;
	float displacementHeight;
	float depthOfFieldDistance;
	float depthOfFieldRange;
	XMMATRIX reflectionViewMatrix;

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