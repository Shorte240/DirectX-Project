// App1.cpp
// Project showcasing Vertex Manipulation, Tessellation, Post Processing
// Lighting and Shadows and use of the Geometry Shader
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Set up meshes
	floorMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waterTessellatedSphereMesh = new TessellatedSphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.0f, 40.0f);
	earthTessellatedSphereMesh = new TessellatedSphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.0f, 40.0f);

	// Set up orthoMeshes
	topLeftOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth / 2.7, screenHeight / 2.7);
	bottomLeftOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth / 2.7, -screenHeight / 2.7);
	topRightOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 2.7, screenHeight / 2.7);
	bottomRightOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 2.7, -screenHeight / 2.7);
	screenOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size

	// Load in textures
	textureMgr->loadTexture("brick", L"res/brick1.dds");
	textureMgr->loadTexture("water", L"res/water.jpg");
	textureMgr->loadTexture("height", L"res/earth.png");

	// Set up shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	tessellationDepthShader = new TessellationDepthShader(renderer->getDevice(), hwnd);
	displacementShader = new DisplacementShader(renderer->getDevice(), hwnd);
	displacementDepthShader = new DisplacementDepthShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	depthOfFieldShader = new DepthOfFieldShader(renderer->getDevice(), hwnd);
	renderTessNormalsShader = new RenderTessellatedNormalsShader(renderer->getDevice(), hwnd);
	renderDispNormalsShader = new RenderDisplacementNormalsShader(renderer->getDevice(), hwnd);

	// Set shadow map width/height
	// 4096 breaks graphics debugger on win7.
	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;

	// Set scene width/height for lights
	sceneWidth = 100;
	sceneHeight = 100;

	// This is your shadow map
	leftDirectionalLightShadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	rightDirectionalLightShadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	spotLightShadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	normalSceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	renderNormalsTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	depthOfFieldTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	cameraDepthTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Give all the lights initial ambient, diffuse, direction and position variables if appropriate
	initialiseLights();

	// Height maps initial displacement height
	displacementHeight = 0;

	// Set default tessellation factor
	tessellationFactor = 2.0f;

	// Wave variables initial values
	wavVar.elapsedTime = 0.0f;
	wavVar.height = 0.0f;
	wavVar.frequency = 0.0f;
	wavVar.speed = 1.0f;

	// Depth of field initial range
	depthOfFieldRange = 1.0f;
	depthOfFieldOffset = 0.0f;

	// Bool initial values
	renderTopLeftOrthoMesh = true;
	renderTopRightOrthoMesh = true;
	renderBottomLeftOrthoMesh = true;
	renderBottomRightOrthoMesh = true;

	// Spot lights initial angle value
	spotLightAngle = 45.f;
	constantFactor = 0.5f;
	linearFactor = 0.125f;
	quadraticFactor = 0.0f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the shaders.
	releaseShaders();

	// Release the meshes.
	releaseMeshes();

	// Release the textures.
	releaseTextures();
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// Set the light settings
	updateLightSettings();

	// Render scene to render texture
	firstPass();

	// Perform depth passes for each light
	depthPass(lights[0], leftDirectionalLightShadowMap);
	depthPass(lights[1], rightDirectionalLightShadowMap);
	depthPass(lights[2], spotLightShadowMap);

	// Perform depth pass on camera
	cameraDepthPass();
	
	// Render tessellated shapes normals
	renderTessellatedNormals();

	// Disable wireframe
	renderer->setWireframeMode(false);

	// Down sample
	downSample();

	// Apply horizontal blur stage
	horizontalBlur();

	// Apply vertical blur to the horizontal blur stage
	verticalBlur();

	// Apply depth of field shader
	depthOfFieldPass();

	// Up sample
	upSample();
	
	// Render the whole scene
	finalPass();

	return true;
}

// depthPass, does a depth pass on all objects on the scene,
// from each lights point of view
void App1::depthPass(Light* light, RenderTexture* rTex)
{
	// Set the render target to be the render to texture.
	rTex->setRenderTarget(renderer->getDeviceContext());
	rTex->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	floorMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), floorMesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	tessellationDepthShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("height"), tessellationFactor, displacementHeight, camera->getPosition());
	displacementDepthShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

// Camera depth pass does a depth pass from the cameras view on all objects in the scene,
// this is then used in the depth of field shader
void App1::cameraDepthPass()
{
	// Set the render target to be the render to texture.
	cameraDepthTexture->setRenderTarget(renderer->getDeviceContext());
	cameraDepthTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	floorMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), floorMesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	tessellationDepthShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());
	
	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), tessellationFactor, displacementHeight, camera->getPosition());
	displacementDepthShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

// firstPass renders the regular scene to a texture
void App1::firstPass()
{
	// Set the render target to be the render to texture and clear it
	normalSceneTexture->setRenderTarget(renderer->getDeviceContext());
	normalSceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Update the camera
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	floorMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), leftDirectionalLightShadowMap->getShaderResourceView(), rightDirectionalLightShadowMap->getShaderResourceView(), spotLightShadowMap->getShaderResourceView(), lights, spotLightAngle, constantFactor, linearFactor, quadraticFactor);
	shadowShader->render(renderer->getDeviceContext(), floorMesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition(), leftDirectionalLightShadowMap->getShaderResourceView(), rightDirectionalLightShadowMap->getShaderResourceView(), spotLightShadowMap->getShaderResourceView(), lights, spotLightAngle, constantFactor, linearFactor, quadraticFactor);
	tessellationShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), lights, tessellationFactor, displacementHeight, camera->getPosition(), spotLightAngle, constantFactor, linearFactor, quadraticFactor);
	displacementShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// Down samples the normal scene texture to make blurring faster
void App1::downSample()
{
	// Set the render target to be the render to texture and clear it
	downSampleTexture->setRenderTarget(renderer->getDeviceContext());
	downSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, normalSceneTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// Applies a horizontal blur to the down sampled scene texture
void App1::horizontalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, downSampleTexture->getShaderResourceView(), screenSizeX);
	horizontalBlurShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// Applies a vertical blur to the horizontal blur texture
void App1::verticalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY);
	verticalBlurShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// depthOfFieldPass, uses the vertical and horizontal blur shaders to create a depth of field
// post processing effect.
void App1::depthOfFieldPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	depthOfFieldTexture->setRenderTarget(renderer->getDeviceContext());
	depthOfFieldTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	depthOfFieldShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, normalSceneTexture->getShaderResourceView(), verticalBlurTexture->getShaderResourceView(), cameraDepthTexture->getShaderResourceView(), depthOfFieldRange, SCREEN_NEAR, SCREEN_DEPTH, depthOfFieldOffset);
	depthOfFieldShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// upSample, up samples the depth of field texture
void App1::upSample()
{
	// Set the render target to be the render to texture and clear it
	upSampleTexture->setRenderTarget(renderer->getDeviceContext());
	upSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, depthOfFieldTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// Final shader pass, used to render to the screen
void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	// Render the scene to the orthomesh covering the screen
	screenOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upSampleTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());

	if (renderTopLeftOrthoMesh)
	{
		// Render top left orthomesh
		topLeftOrthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, leftDirectionalLightShadowMap->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), topLeftOrthoMesh->getIndexCount());
	}

	if (renderBottomLeftOrthoMesh)
	{
		// Render bottom left orthomesh
		bottomLeftOrthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, spotLightShadowMap->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), bottomLeftOrthoMesh->getIndexCount());
	}

	if (renderTopRightOrthoMesh)
	{
		// Render top right orthomesh
		topRightOrthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, rightDirectionalLightShadowMap->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), topRightOrthoMesh->getIndexCount());
	}

	if (renderBottomRightOrthoMesh)
	{
		// Render top right orthomesh
		bottomRightOrthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderNormalsTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), bottomRightOrthoMesh->getIndexCount());
	}

	renderer->setZBuffer(true);

	gui();
	renderer->endScene();
}

// Function to set up and render the ImGui client
void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	if (ImGui::CollapsingHeader("Tessellation", 0))
	{
		ImGui::SliderFloat("Tessellation Factor", &tessellationFactor, 1.0f, 64.0f);
		ImGui::SliderFloat("Wave Height", &wavVar.height, 0.0f, 0.5f);
		ImGui::SliderFloat("Wave Frequency", &wavVar.frequency, 0.0f, 5.0f);
		ImGui::SliderFloat("Wave Speed", &wavVar.speed, 0.0f, 5.0f);
	}
	
	if (ImGui::CollapsingHeader("Displacement", 0))
	{
		ImGui::SliderFloat("Displacement Height", &displacementHeight, 0.0f, 1.0f);
	}

	if (ImGui::CollapsingHeader("Depth Of Field", 0))
	{
		ImGui::SliderFloat("Offset", &depthOfFieldOffset, -1.0f, 1.0f);
		ImGui::SliderFloat("Range", &depthOfFieldRange, 0.0f, 2.0f);
	}

	if (ImGui::CollapsingHeader("Light Settings", 0))
	{
		if (ImGui::TreeNode("Left Directional"))
		{
			ImGui::DragFloat4("Ambient Colour", leftDirectionalAmbientColour, (1.0f / 255.0f), 0.0f, 1.f, "%.2f", 1.f);
			ImGui::ColorEdit4("Diffuse Colour", leftDirectionalDiffuseColour);
			if (leftDirectionalDirection[0] >= 0.01f && leftDirectionalDirection[0] <= 0.015f)
			{
				leftDirectionalDirection[0] -= 0.03f;
			}
			if (leftDirectionalDirection[0] <= -0.01f && leftDirectionalDirection[0] >= -0.015f)
			{
				leftDirectionalDirection[0] += 0.03f;
			}
			if (leftDirectionalDirection[0] != 0.009f || leftDirectionalDirection[0] != -0.009f)
			{
				ImGui::DragFloat3("Direction", leftDirectionalDirection, 0.01f, -1.f, 1.f, "%.3f", 1.f);
			}
			ImGui::DragFloat3("Position", leftDirectionalPosition, 0.5f, -100.f, 100.f, "%.2f", 1.f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Right Directional"))
		{
			ImGui::DragFloat4("Ambient Colour", rightDirectionalAmbientColour, (1.0f / 255.0f), 0.0f, 1.f, "%.2f", 1.f);
			ImGui::ColorEdit4("Diffuse Colour", rightDirectionalDiffuseColour);
			if (rightDirectionalDirection[0] >= 0.01f && rightDirectionalDirection[0] <= 0.015f)
			{
				rightDirectionalDirection[0] -= 0.03f;
			}
			if (rightDirectionalDirection[0] <= -0.01f && rightDirectionalDirection[0] >= -0.015f)
			{
				rightDirectionalDirection[0] += 0.03f;
			}
			if (rightDirectionalDirection[0] != 0.009f || rightDirectionalDirection[0] != -0.009f)
			{
				ImGui::DragFloat3("Direction", rightDirectionalDirection, 0.01f, -1.f, 1.f, "%.3f", 1.f);
			}
			ImGui::DragFloat3("Position", rightDirectionalPosition, 0.5f, -100.f, 100.f, "%.2f", 1.f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Spot Light"))
		{
			ImGui::DragFloat4("Ambient Colour", spotAmbientColour, (1.0f / 255.0f), 0.0f, 1.f, "%.2f", 1.f);
			ImGui::ColorEdit4("Diffuse Colour", spotDiffuseColour);
			if (spotDirection[0] >= 0.01f && spotDirection[0] <= 0.015f)
			{
				spotDirection[0] -= 0.03f;
			}
			if (spotDirection[0] <= -0.01f && spotDirection[0] >= -0.015f)
			{
				spotDirection[0] += 0.03f;
			}
			if (spotDirection[0] != 0.009f || spotDirection[0] != -0.009f)
			{
				ImGui::DragFloat3("Direction", spotDirection, 0.01f, -1.f, 1.f, "%.3f", 1.f);
			}
			ImGui::DragFloat3("Position", spotPosition, 0.5f, -100.f, 100.f, "%.2f", 1.f);
			if (ImGui::TreeNode("Advanced"))
			{
				ImGui::DragFloat("Angle", &spotLightAngle, 0.5f, 0.f, 90.f, "%.2f", 1.f);
				ImGui::DragFloat("Constant Factor", &constantFactor, 0.01f, 0.01f, 1.f, "%.2f", 1.f);
				ImGui::DragFloat("Linear Factor", &linearFactor, 0.01f, 0.01f, 1.f, "%.3f", 1.f);
				ImGui::DragFloat("Quadratic Factor", &quadraticFactor, 0.001f, 0.f, 0.5f, "%.2f", 1.f);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Ortho Meshes", 0))
	{
		ImGui::Checkbox("Render top left ortho mesh", &renderTopLeftOrthoMesh);
		ImGui::Checkbox("Render top right ortho mesh", &renderTopRightOrthoMesh);
		ImGui::Checkbox("Render bottom left ortho mesh", &renderBottomLeftOrthoMesh);
		ImGui::Checkbox("Render bottom right ortho mesh", &renderBottomRightOrthoMesh);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// Function to set the light values to the variables altered via ImGui
void App1::updateLightSettings()
{
	lights[0]->setAmbientColour(leftDirectionalAmbientColour[0], leftDirectionalAmbientColour[1], leftDirectionalAmbientColour[2], leftDirectionalAmbientColour[3]);
	lights[0]->setDiffuseColour(leftDirectionalDiffuseColour[0], leftDirectionalDiffuseColour[1], leftDirectionalDiffuseColour[2], leftDirectionalDiffuseColour[3]);
	lights[0]->setDirection(leftDirectionalDirection[0], leftDirectionalDirection[1], leftDirectionalDirection[2]);
	lights[0]->setPosition(leftDirectionalPosition[0], leftDirectionalPosition[1], leftDirectionalPosition[2]);

	lights[1]->setAmbientColour(rightDirectionalAmbientColour[0], rightDirectionalAmbientColour[1], rightDirectionalAmbientColour[2], rightDirectionalAmbientColour[3]);
	lights[1]->setDiffuseColour(rightDirectionalDiffuseColour[0], rightDirectionalDiffuseColour[1], rightDirectionalDiffuseColour[2], rightDirectionalDiffuseColour[3]);
	lights[1]->setDirection(rightDirectionalDirection[0], rightDirectionalDirection[1], rightDirectionalDirection[2]);
	lights[1]->setPosition(rightDirectionalPosition[0], rightDirectionalPosition[1], rightDirectionalPosition[2]);

	lights[2]->setAmbientColour(spotAmbientColour[0], spotAmbientColour[1], spotAmbientColour[2], spotAmbientColour[3]);
	lights[2]->setDiffuseColour(spotDiffuseColour[0], spotDiffuseColour[1], spotDiffuseColour[2], spotDiffuseColour[3]);
	lights[2]->setDirection(spotDirection[0], spotDirection[1], spotDirection[2]);
	lights[2]->setPosition(spotPosition[0], spotPosition[1], spotPosition[2]);
}

// Shader used to render the normals of the tessellated shapes.
void App1::renderTessellatedNormals()
{
	// Set the render target to be the render to texture and clear it
	renderNormalsTexture->setRenderTarget(renderer->getDeviceContext());
	renderNormalsTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition(), leftDirectionalLightShadowMap->getShaderResourceView(), rightDirectionalLightShadowMap->getShaderResourceView(), spotLightShadowMap->getShaderResourceView(), lights, spotLightAngle, constantFactor, linearFactor, quadraticFactor);
	tessellationShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), lights, tessellationFactor, displacementHeight, camera->getPosition(), spotLightAngle, constantFactor, linearFactor, quadraticFactor);
	displacementShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	renderTessNormalsShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	renderTessNormalsShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	renderDispNormalsShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), displacementHeight, tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	renderDispNormalsShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

// Gives all the lights used in the scene an initial value.
void App1::initialiseLights()
{
	// Light[0] is a direction light coming from the left
	lights[0] = new Light;
	// Light[1] is a direction light coming from the right
	lights[1] = new Light;
	// Light[2] is a spot light coming from above
	lights[2] = new Light;
	for (int i = 0; i < 3; i++)
	{
		leftDirectionalAmbientColour[i] = 0.3f;
		rightDirectionalAmbientColour[i] = 0.3f;
		spotAmbientColour[i] = 0.3f;
		spotDiffuseColour[i] = 1.0f;
	}
	// Set light .w/alpha to 1.0f
	leftDirectionalAmbientColour[3] = 1.0f;
	rightDirectionalAmbientColour[3] = 1.0f;
	spotAmbientColour[3] = 1.0f;
	// Set lights diffuse colour
	leftDirectionalDiffuseColour[0] = 0.8f;
	leftDirectionalDiffuseColour[3] = 1.0f;
	rightDirectionalDiffuseColour[2] = 0.8f;
	rightDirectionalDiffuseColour[3] = 1.0f;
	spotDiffuseColour[3] = 1.0f;
	// Set lights direction
	leftDirectionalDirection[0] = 0.7f;
	leftDirectionalDirection[1] = -0.7f;
	leftDirectionalDirection[2] = 0.0f;
	rightDirectionalDirection[0] = -0.7f;
	rightDirectionalDirection[1] = -0.7f;
	rightDirectionalDirection[2] = 0.0f;
	spotDirection[0] = 0.1f;
	// Up/Down matrices are broken
	spotDirection[1] = -1.0f;
	spotDirection[2] = 0.1f;
	// Set lights position
	leftDirectionalPosition[0] = -10.0f;
	leftDirectionalPosition[1] = 0.0f;
	leftDirectionalPosition[2] = 0.0f;
	spotPosition[0] = 0.0f;
	spotPosition[1] = 10.0f;
	spotPosition[2] = 10.0f;
	rightDirectionalPosition[0] = 10.0f;
	rightDirectionalPosition[1] = 0.0f;
	rightDirectionalPosition[2] = 0.0f;
	// Generate light ortho matrix
	lights[0]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);
	lights[1]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);
	lights[2]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);
}

// Releases/deletes all shaders used in the scene when the application closes down.
// Used in the deconstructor.
void App1::releaseShaders()
{
	// Release the texture shader.
	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	// Release the shadow shader.
	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}

	// Release the depth shader.
	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	// Release the tessellation shader.
	if (tessellationShader)
	{
		delete tessellationShader;
		tessellationShader = 0;
	}

	// Release the tessellaiton depth shader.
	if (tessellationDepthShader)
	{
		delete tessellationDepthShader;
		tessellationDepthShader = 0;
	}

	// Release the displacement shader.
	if (displacementShader)
	{
		delete displacementShader;
		displacementShader = 0;
	}

	// Release the displacement depth shader.
	if (displacementDepthShader)
	{
		delete displacementDepthShader;
		displacementDepthShader = 0;
	}

	// Release the horizontal blur shader.
	if (horizontalBlurShader)
	{
		delete horizontalBlurShader;
		horizontalBlurShader = 0;
	}

	// Release the vertical blur shader.
	if (verticalBlurShader)
	{
		delete verticalBlurShader;
		verticalBlurShader = 0;
	}

	// Release the depth of field shader.
	if (depthOfFieldShader)
	{
		delete depthOfFieldShader;
		depthOfFieldShader = 0;
	}

	// Release the render tessellated normals shader.
	if (renderTessNormalsShader)
	{
		delete renderTessNormalsShader;
		renderTessNormalsShader = 0;
	}

	// Release the render displacement normals shader.
	if (renderDispNormalsShader)
	{
		delete renderDispNormalsShader;
		renderDispNormalsShader = 0;
	}
}

// Releases/deletes all meshes used in the scene when the application closes down.
// Used in the deconstructor.
void App1::releaseMeshes()
{
	// Release the floor mesh.
	if (floorMesh)
	{
		delete floorMesh;
		floorMesh = 0;
	}

	// Release the top left ortho mesh.
	if (topLeftOrthoMesh)
	{
		delete topLeftOrthoMesh;
		topLeftOrthoMesh = 0;
	}

	// Release the top right ortho mesh.
	if (topRightOrthoMesh)
	{
		delete topRightOrthoMesh;
		topRightOrthoMesh = 0;
	}

	// Release the bottom left ortho mesh.
	if (bottomLeftOrthoMesh)
	{
		delete bottomLeftOrthoMesh;
		bottomLeftOrthoMesh = 0;
	}

	// Release the bottom right ortho mesh.
	if (bottomRightOrthoMesh)
	{
		delete bottomRightOrthoMesh;
		bottomRightOrthoMesh = 0;
	}

	// Release the screen ortho mesh.
	if (screenOrthoMesh)
	{
		delete screenOrthoMesh;
		screenOrthoMesh = 0;
	}

	// Release the water tessellated sphere mesh.
	if (waterTessellatedSphereMesh)
	{
		delete waterTessellatedSphereMesh;
		waterTessellatedSphereMesh = 0;
	}

	// Release the earth tessellated sphere mesh.
	if (earthTessellatedSphereMesh)
	{
		delete earthTessellatedSphereMesh;
		earthTessellatedSphereMesh = 0;
	}
}

// Releases/deletes all textures used in the scene when the application closes down.
// Used in the deconstructor.
void App1::releaseTextures()
{
	// Release the left directional shadow map texture.
	if (leftDirectionalLightShadowMap)
	{
		delete leftDirectionalLightShadowMap;
		leftDirectionalLightShadowMap = 0;
	}

	// Release the right directional shadow map texture.
	if (rightDirectionalLightShadowMap)
	{
		delete rightDirectionalLightShadowMap;
		rightDirectionalLightShadowMap = 0;
	}

	// Release the spot light shadow map texture.
	if (spotLightShadowMap)
	{
		delete spotLightShadowMap;
		spotLightShadowMap = 0;
	}

	// Release the normal scene texture.
	if (normalSceneTexture)
	{
		delete normalSceneTexture;
		normalSceneTexture = 0;
	}

	// Release the render normals texture.
	if (renderNormalsTexture)
	{
		delete renderNormalsTexture;
		renderNormalsTexture = 0;
	}

	// Release the horizontal blur texture.
	if (horizontalBlurTexture)
	{
		delete horizontalBlurTexture;
		horizontalBlurTexture = 0;
	}

	// Release the vertical blur texture.
	if (verticalBlurTexture)
	{
		delete verticalBlurTexture;
		verticalBlurTexture = 0;
	}

	// Release the down sampler texture.
	if (downSampleTexture)
	{
		delete downSampleTexture;
		downSampleTexture = 0;
	}

	// Release the up sampler texture.
	if (upSampleTexture)
	{
		delete upSampleTexture;
		upSampleTexture = 0;
	}

	// Release the depth of field texture.
	if (depthOfFieldTexture)
	{
		delete depthOfFieldTexture;
		depthOfFieldTexture = 0;
	}

	// Release the camera depth texture.
	if (cameraDepthTexture)
	{
		delete cameraDepthTexture;
		cameraDepthTexture = 0;
	}
}

