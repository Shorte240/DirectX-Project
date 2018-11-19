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

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	cubeMesh = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	tessellatedSphereMesh = new TessellatedSphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 2.0f, 40.0f);

	leftOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, -screenWidth / 2.7, screenHeight / 2.7);
	rightOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 2.7, screenHeight / 2.7);

	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/teapot.obj");

	textureMgr->loadTexture("brick", L"res/brick1.dds");
	textureMgr->loadTexture("wood", L"res/wood.png");
	textureMgr->loadTexture("bunny", L"res/bunny.png");

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	tessellationDepthShader = new TessellationDepthShader(renderer->getDevice(), hwnd);

	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;

	// 4096 breaks my graphics debugger on win7???

	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMap = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap2 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);

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

	rotate = 0;

	// Set default tessellation factor
	tessellationFactor = 2.0f;

	wavVar.elapsedTime = 0.0f;
	wavVar.height = 0.0f;
	wavVar.frequency = 0.0f;
	wavVar.speed = 1.0f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	rotate += 0.005f;
	

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
	// Render scene
	finalPass();

	return true;
}

void App1::depthPass(Light* light, RenderTexture* rTex)
{
	// DO SEPARATE DEPTH PASSES AND SAVE TO SEPARATE TEXTURES

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

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, rotate, 0.0f);
	//worldMatrix = XMMatrixMultiply(rotationMatrix, XMMatrixMultiply(worldMatrix, scaleMatrix));
	//// Render model
	//model->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//XMMATRIX cubetranslateMatrix = XMMatrixTranslation(10.5f, 0.0f, 0.f);
	//XMMATRIX cuberotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, (rotate * 2.0f));
	//XMMATRIX cubetranslateMatrix2 = XMMatrixTranslation(0.f, 1.f, 3.75f);
	//worldMatrix = cubetranslateMatrix * cuberotationMatrix * cubetranslateMatrix2;
	//// Render cube
	//cubeMesh->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//depthShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	//// Render sphere
	//worldMatrix = renderer->getWorldMatrix();
	//XMMATRIX translateMatrix = XMMatrixTranslation(-10.5f, 0.0f, 0.f);
	//XMMATRIX rotationMatrix2 = XMMatrixRotationRollPitchYaw(0.0f, (rotate * 2.0f), 0.0f);
	//XMMATRIX translateMatrix2 = XMMatrixTranslation(0.f, 1.f, 3.75f);
	//worldMatrix = translateMatrix * rotationMatrix2 * translateMatrix2;
	//sphereMesh->sendData(renderer->getDeviceContext());
	//depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	tessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("brick"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	tessellationDepthShader->render(renderer->getDeviceContext(), tessellatedSphereMesh->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
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

	// Render model
	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, rotate, 0.0f);
	//worldMatrix = XMMatrixMultiply(rotationMatrix,XMMatrixMultiply(worldMatrix, scaleMatrix));
	//model->sendData(renderer->getDeviceContext());
	//shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	//shadowShader->render(renderer->getDeviceContext(), model->getIndexCount());

	//// Render cube
	//worldMatrix = renderer->getWorldMatrix();
	//XMMATRIX cubetranslateMatrix = XMMatrixTranslation(10.5f, 0.0f, 0.f);
	//XMMATRIX cuberotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, (rotate * 2.0f));
	//XMMATRIX cubetranslateMatrix2 = XMMatrixTranslation(0.f, 1.f, 3.75f);
	//worldMatrix = cubetranslateMatrix * cuberotationMatrix * cubetranslateMatrix2;
	//cubeMesh->sendData(renderer->getDeviceContext());
	//shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("bunny"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	//shadowShader->render(renderer->getDeviceContext(), cubeMesh->getIndexCount());

	//// Render sphere
	//worldMatrix = renderer->getWorldMatrix();
	//XMMATRIX translateMatrix = XMMatrixTranslation(-10.5f, 0.0f, 0.f);
	//XMMATRIX rotationMatrix2 = XMMatrixRotationRollPitchYaw(0.0f, (rotate * 2.0f), 0.0f);
	//XMMATRIX translateMatrix2 = XMMatrixTranslation(0.f, 1.f, 3.75f);
	//worldMatrix = translateMatrix * rotationMatrix2 * translateMatrix2;
	//sphereMesh->sendData(renderer->getDeviceContext());
	//shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("wood"), shadowMap->getShaderResourceView(), shadowMap2->getShaderResourceView(), lights);
	//shadowShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	// Get the elapsed time
	wavVar.elapsedTime += timer->getTime();

	// Render tessellated sphere
	worldMatrix = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	tessellatedSphereMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick"), tessellationFactor, XMFLOAT4(wavVar.elapsedTime, wavVar.height, wavVar.frequency, wavVar.speed), camera->getPosition());
	tessellationShader->render(renderer->getDeviceContext(), tessellatedSphereMesh->getIndexCount());

	renderer->setZBuffer(false);
	worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	// Render orthoMesh
	leftOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, shadowMap->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), leftOrthoMesh->getIndexCount());

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
	ImGui::SliderFloat("Tessellation Factor", &tessellationFactor, 1.0f, 64.0f);
	ImGui::SliderFloat("Wave Height", &wavVar.height, 0, 2.0f);
	ImGui::SliderFloat("Wave Frequency", &wavVar.frequency, 0, 15);
	ImGui::SliderFloat("Wave Speed", &wavVar.speed, 0, 5);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

