// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TessellatedSphereMesh.h"
#include "Shaders.h"
#include "D3D.h"

#define MAX_LIGHTS 3

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
	void finalPass();
	void gui();
	void updateLightSettings();
	void renderTessellatedNormals();
	void initialiseLights();
	void releaseShaders();
	void releaseMeshes();
	void releaseTextures();

private:
	// Shaders
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
	RenderTessellatedNormalsShader* renderTessNormalsShader;
	RenderDisplacementNormalsShader* renderDispNormalsShader;

	// Meshes
	PlaneMesh* floorMesh;
	OrthoMesh* topLeftOrthoMesh;
	OrthoMesh* topRightOrthoMesh;
	OrthoMesh* bottomLeftOrthoMesh;
	OrthoMesh* bottomRightOrthoMesh;
	OrthoMesh* screenOrthoMesh;
	TessellatedSphereMesh* waterTessellatedSphereMesh;
	TessellatedSphereMesh* earthTessellatedSphereMesh;

	// Lights
	Light* lights[MAX_LIGHTS];	

	// Render Textures
	RenderTexture* leftDirectionalLightShadowMap;
	RenderTexture* rightDirectionalLightShadowMap;
	RenderTexture* spotLightShadowMap;
	RenderTexture* normalSceneTexture;
	RenderTexture* renderNormalsTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* downSampleTexture;
	RenderTexture* upSampleTexture;
	RenderTexture* depthOfFieldTexture;
	RenderTexture* cameraDepthTexture;

	// Variables
	float tessellationFactor;
	float displacementHeight;
	float depthOfFieldRange;
	bool renderTopLeftOrthoMesh;
	bool renderTopRightOrthoMesh;
	bool renderBottomLeftOrthoMesh;
	bool renderBottomRightOrthoMesh;

	// Lights ambient colour
	float leftDirectionalAmbientColour[4];
	float rightDirectionalAmbientColour[4];
	float spotAmbientColour[4];

	// Lights diffuse colour
	float leftDirectionalDiffuseColour[4];
	float rightDirectionalDiffuseColour[4];
	float spotDiffuseColour[4];

	// Lights direction
	float leftDirectionalDirection[3];
	float rightDirectionalDirection[3];
	float spotDirection[4];

	// Lights position
	float leftDirectionalPosition[3];
	float rightDirectionalPosition[3];
	float spotPosition[3];

	// Spot light angle
	float spotLightAngle;
	float constantFactor;
	float linearFactor;
	float quadraticFactor;

	// Scene variables
	float sceneWidth;
	float sceneHeight;

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