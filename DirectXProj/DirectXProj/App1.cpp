// App1.cpp
// Project showcasing Vertex Manipulation, Tessellation, Post Processing
// Lighting and Shadows, Geometry Shader
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Set up meshes
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waterTessellatedSphereMesh = new TessellatedSphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.0f, 40.0f);
	earthTessellatedSphereMesh = new TessellatedSphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.0f, 40.0f);
	reflectiveTessellatedSphereMesh = new TessellatedSphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.0f, 40.0f);

	// Set up orthoMeshes
	leftOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth / 2.7, screenHeight / 2.7);
	rightOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 2.7, screenHeight / 2.7);
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
	reflectionShader = new ReflectionShader(renderer->getDevice(), hwnd);

	// Set shadow map width/height
	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;

	// 4096 breaks my graphics debugger on win7???

	// Set scene width/height for lights
	float sceneWidth = 100;
	float sceneHeight = 100;

	// This is your shadow map
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap2 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	normalSceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	depthOfFieldTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	cameraDepthTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	reflectionTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	lights[0] = new Light;
	lights[0]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	lights[0]->setDiffuseColour(0.8f, 0.0f, 0.0f, 1.0f);
	lights[0]->setDirection(0.7f, -0.7f, 0.0f);
	lights[0]->setPosition(-10.f, 0.f, 0.f);
	lights[0]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	lights[1] = new Light;
	lights[1]->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	lights[1]->setDiffuseColour(0.0f, 0.0f, 0.8f, 1.0f);
	lights[1]->setDirection(-0.7f, -0.7f, 0.0f);
	lights[1]->setPosition(10.f, 0.f, 0.f);
	lights[1]->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	displacementHeight = 0;

	// Set default tessellation factor
	tessellationFactor = 2.0f;

	wavVar.elapsedTime = 0.0f;
	wavVar.height = 0.0f;
	wavVar.frequency = 0.0f;
	wavVar.speed = 1.0f;

	depthOfFieldRange = 1.0f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

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
	// Perform depth pass
	depthPass(lights[0], shadowMap);
	depthPass(lights[1], shadowMap2);
	// Perform depth pass on camera
	cameraDepthPass();
	// Reflection Pass
	reflectionPass();
	// Render scene to render texture
	firstPass();
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
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	tessellationDepthShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("height"), tessellationFactor, displacementHeight, camera->getPosition());
	displacementDepthShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Render reflective tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	reflectiveTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("height"), tessellationFactor, displacementHeight, camera->getPosition());
	displacementDepthShader->render(renderer->getDeviceContext(), reflectiveTessellatedSphereMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

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
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	tessellationDepthShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());
	
	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), tessellationFactor, displacementHeight, camera->getPosition());
	displacementDepthShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Render reflective tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	reflectiveTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), tessellationFactor, displacementHeight, camera->getPosition());
	displacementDepthShader->render(renderer->getDeviceContext(), reflectiveTessellatedSphereMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::renderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;
	float radians;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	// For planar reflection invert the Y position of the camera.
	position.x = camera->getPosition().x;
	position.y = -camera->getPosition().y + (height * 2.0f);
	position.z = camera->getPosition().z;

	XMFLOAT3 rot;
	XMStoreFloat3(&rot, camera->getRotation());

	// Calculate the rotation in radians.
	radians = rot.y * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + camera->getPosition().x;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + camera->getPosition().z;

	// Create the view matrix from the three vectors.
	reflectionViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&lookAt), XMLoadFloat3(&up));
}

void App1::reflectionPass()
{
	// Set the render target to be the render to texture and clear it
	reflectionTexture->setRenderTarget(renderer->getDeviceContext());
	reflectionTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Clear the scene. (default blue colour)
	//renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// Generate the reflection matrix
	renderReflection(5.0f);

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	/*mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, reflectionViewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());*/

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, reflectionViewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition(), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	tessellationShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, reflectionViewMatrix, projectionMatrix, textureMgr->getTexture("height"), lights, tessellationFactor, displacementHeight, camera->getPosition());
	displacementShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	reflectiveTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, reflectionViewMatrix, projectionMatrix, textureMgr->getTexture("height"), lights, tessellationFactor, displacementHeight, camera->getPosition());
	displacementShader->render(renderer->getDeviceContext(), reflectiveTessellatedSphereMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::firstPass()
{
	// Set the render target to be the render to texture and clear it
	normalSceneTexture->setRenderTarget(renderer->getDeviceContext());
	normalSceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Clear the scene. (default blue colour)
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	/*reflectionShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		reflectionTexture->getShaderResourceView(), textureMgr->getTexture("brick"), reflectionViewMatrix);
	reflectionShader->render(renderer->getDeviceContext(), mesh->getIndexCount());*/

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render water tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	waterTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition(), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	tessellationShader->render(renderer->getDeviceContext(), waterTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, -5.0f);
	earthTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), lights, tessellationFactor, displacementHeight, camera->getPosition());
	displacementShader->render(renderer->getDeviceContext(), earthTessellatedSphereMesh->getIndexCount());

	// Render earth tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 5.0f);
	reflectiveTessellatedSphereMesh->sendData(renderer->getDeviceContext());
	displacementShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), lights, tessellationFactor, displacementHeight, camera->getPosition());
	displacementShader->render(renderer->getDeviceContext(), reflectiveTessellatedSphereMesh->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

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
	depthOfFieldShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, normalSceneTexture->getShaderResourceView(), verticalBlurTexture->getShaderResourceView(), cameraDepthTexture->getShaderResourceView(), depthOfFieldRange, SCREEN_NEAR, SCREEN_DEPTH);
	depthOfFieldShader->render(renderer->getDeviceContext(), screenOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

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

	// Render top left orthomesh
	leftOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, shadowMap->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), leftOrthoMesh->getIndexCount());

	// Render top right orthomesh
	rightOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, shadowMap2->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), rightOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	gui();
	renderer->endScene();
}

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
		ImGui::SliderFloat("Wave Height", &wavVar.height, 0.0f, 5.0f);
		ImGui::SliderFloat("Wave Frequency", &wavVar.frequency, 0.0f, 15.0f);
		ImGui::SliderFloat("Wave Speed", &wavVar.speed, 0.0f, 5.0f);
	}
	
	if (ImGui::CollapsingHeader("Displacement", 0))
	{
		ImGui::SliderFloat("Displacement Height", &displacementHeight, 0.0f, 2.0f);
	}

	if (ImGui::CollapsingHeader("Depth Of Field", 0))
	{
		ImGui::SliderFloat("Range", &depthOfFieldRange, 0.0f, 2.0f);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

