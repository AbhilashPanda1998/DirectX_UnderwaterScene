﻿#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace ACW;

using namespace DirectX;
using namespace Windows::Foundation;

Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_indexCount(0),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	mContext = m_deviceResources->GetD3DDeviceContext();
}

void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	//Perspective matrix
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		0.01f,
		1000.0f
	);

	//Orientation matrix
	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	//Projection matrix
	XMStoreFloat4x4(
		&m_constantBufferDataCamera.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	//Set view matrix
	lookAt = XMMatrixLookAtLH(eye, at, up);
	XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));

	XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
	XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);
	XMStoreFloat4(&m_constantBufferDataCamera.upDir, up);

	//Set light pos and colour
	DirectX::XMVECTOR lightPos = { -10, 100, -10, 1 };
	DirectX::XMVECTOR lightColour = { 1, 1, 1, 1 };
	XMStoreFloat4(&mConstantBufferDataLight.lightPos, lightPos);
	XMStoreFloat4(&mConstantBufferDataLight.lightColour, lightColour);
}

void Sample3DSceneRenderer::Update(DX::StepTimer const& timer, const std::vector<bool>& pInput)
{
	float dt = timer.GetElapsedSeconds();
	mConstantBufferDataTime.time = timer.GetTotalSeconds();

	//Forward,left,right,back
	if (pInput[0] == true)
	{
		XMFLOAT4 translation(0, 0, 1.0f * dt, 1);
		eye += XMLoadFloat4(&translation);
		at += XMLoadFloat4(&translation);
		XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
		XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);

		lookAt = XMMatrixLookAtLH(eye, at, up);
		XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));
	}
	if (pInput[1] == true)
	{
		XMFLOAT4 translation(-1.0f * dt, 0, 0, 1);
		eye += XMLoadFloat4(&translation);
		at += XMLoadFloat4(&translation);
		XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
		XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);

		lookAt = XMMatrixLookAtLH(eye, at, up);
		XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));
	}
	if (pInput[2] == true)
	{
		XMFLOAT4 translation(0, 0, -1.0f * dt, 1);
		eye += XMLoadFloat4(&translation);
		at += XMLoadFloat4(&translation);
		XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
		XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);

		lookAt = XMMatrixLookAtLH(eye, at, up);
		XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));
	}
	if (pInput[3] == true)
	{
		XMFLOAT4 translation(1.0f * dt, 0, 0, 1);
		eye += XMLoadFloat4(&translation);
		at += XMLoadFloat4(&translation);
		XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
		XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);

		lookAt = XMMatrixLookAtLH(eye, at, up);
		XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));
	}

	//Up/Down
	if (pInput[4] == true)
	{
		XMFLOAT4 translation(0.0f, 1.0f * dt, 0, 1);
		eye += XMLoadFloat4(&translation);
		at += XMLoadFloat4(&translation);
		XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
		XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);

		lookAt = XMMatrixLookAtLH(eye, at, up);
		XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));
	}
	if (pInput[5] == true)
	{
		XMFLOAT4 translation(0.0f, -1.0f * dt, 0, 1);
		eye += XMLoadFloat4(&translation);
		at += XMLoadFloat4(&translation);
		XMStoreFloat4(&m_constantBufferDataCamera.eye, eye);
		XMStoreFloat4(&m_constantBufferDataCamera.lookAt, at);

		lookAt = XMMatrixLookAtLH(eye, at, up);
		XMStoreFloat4x4(&m_constantBufferDataCamera.view, XMMatrixTranspose(lookAt));
	}

	//Rotation
	if (pInput[6] == true)
	{

	}
	if (pInput[7] == true)
	{

	}
	if (pInput[8] == true)
	{

	}
	if (pInput[9] == true)
	{

	}

	XMStoreFloat4x4(&m_constantBufferDataCamera.fmodel, XMMatrixTranspose(XMMatrixRotationY(20)));

}

void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	//Update buffer data
	UpdateBuffers();

	//Setup cube vertices and indices
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mContext->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	mContext->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	//Set triangle list topology
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set input layout
	mContext->IASetInputLayout(m_inputLayout.Get());

	//Set default rasteriser state
	mContext->RSSetState(mDefaultRasteriser.Get());

	//Set blend state
	mContext->OMSetBlendState(mNoBlend.Get(), nullptr, 0xffffffff);

	//Set depth stencil
	mContext->OMSetDepthStencilState(mDepthLessThanEqualAll.Get(), 0);

	////Draw ray casted effects
	//RenderSpheres();
	//RenderImplicitShapes();
	//RenderImplicitPrimitives();
	//RenderCorals1();

	RenderSnakes();

	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	RenderTerrain();
	RenderWater();
	
	RenderPlants();
	RenderCoral();
}

void ACW::Sample3DSceneRenderer::RenderSpheres()
{
	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderSpheres.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderSpheres.Get(),
		nullptr,
		0
	);

	// Attach our geometry shader.
	mContext->GSSetShader(
		nullptr,
		nullptr,
		0
	);

	// Draw the objects.
	mContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::RenderImplicitShapes()
{
	// Attach our vertex shader.
	mContext->VSSetShader(
		m_vertexShaderImplicitShapes.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		m_pixelShaderImplicitShapes.Get(),
		nullptr,
		0
	);

	//Draw the objects.
	mContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::RenderImplicitPrimitives()
{
	// Attach our vertex shader.
	mContext->VSSetShader(
		m_vertexShaderImplicitPrimitives.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		m_pixelShaderImplicitPrimitives.Get(),
		nullptr,
		0
	);

	//Draw the objects.
	mContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreateTerrain()
{
	//Terrain shaders
	auto loadVSTaskTerrain = DX::ReadDataAsync(L"TerrainVertex.cso");
	auto loadPSTaskTerrain = DX::ReadDataAsync(L"TerrainPixel.cso");
	auto loadDSTaskTerrain = DX::ReadDataAsync(L"TerrainDomain.cso");
	auto loadHSTaskTerrain = DX::ReadDataAsync(L"TerrainHull.cso");

	//After the vertex shader file is loaded, create the shader
	auto TerrainVSTask = loadVSTaskTerrain.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderTerrain
			)
		);
		});

	//After the pixel shader file is loaded, create the shader
	auto TerrainPSTask = loadPSTaskTerrain.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderTerrain
			)
		);
		});

	//After the domain shader file is loaded, create the shader
	auto TerrainDSTask = loadDSTaskTerrain.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mDomainShaderTerrain
			)
		);
		});

	//After the hull shader file is loaded, create the shader
	auto TerrainHSTask = loadHSTaskTerrain.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mHullShaderTerrain
			)
		);
		});
}

void ACW::Sample3DSceneRenderer::RenderTerrain()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mContext->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	mContext->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderTerrain.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderTerrain.Get(),
		nullptr,
		0
	);

	// Attach our geometry shader.
	mContext->GSSetShader(
		nullptr,
		nullptr,
		0
	);

	//Attach our domain shader
	mContext->DSSetShader(
		mDomainShaderTerrain.Get(),
		nullptr,
		0
	);

	//Attach our hull shader
	mContext->HSSetShader(
		mHullShaderTerrain.Get(),
		nullptr,
		0
	);

	// Draw the objects.
	mContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreatePlants()
{
	//Plants shaders
	auto loadVSTaskPlants = DX::ReadDataAsync(L"PlantVertex.cso");
	auto loadPSTaskPlants = DX::ReadDataAsync(L"PlantPixel.cso");
	auto loadGSTaskPlants = DX::ReadDataAsync(L"PlantGeometry.cso");

	//After the vertex shader file is loaded, create the shader
	auto PlantsVSTask = loadVSTaskPlants.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderPlants
			)
		);
		});

	//Load plant texture from file
	auto hr = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"grass.dds", nullptr, mPlantTexture.GetAddressOf());

	//After the pixel shader file is loaded, create the shader
	auto PlantsPSTask = loadPSTaskPlants.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderPlants
			)
		);
		});

	//After the geometry shader file is loaded, create the shader
	auto PlantsGSTask = loadGSTaskPlants.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mGeometryShaderPlants
			)
		);
		});

	//Once the shaders using the plant vertices are loaded, load the plant vertices
	auto createPlantsTask = (PlantsVSTask && PlantsPSTask && PlantsGSTask).then([this]() {

		static std::vector<Vertex> plantVertices;
	static std::vector<unsigned short> plantIndices;

	for (int i = -10; i < 11; i++)
	{
		for (int j = -10; j < 11; j++)
		{
			plantVertices.emplace_back(Vertex{ XMFLOAT3(i, 0, j) });
		}
	}

	for (int i = 0; i < plantVertices.size(); i++)
	{
		plantIndices.push_back(i);
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(plantVertices[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex) * plantVertices.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&mPlantVertexBuffer
		)
	);

	mPlantIndex = plantIndices.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &(plantIndices[0]);
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * plantIndices.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&mPlantIndexBuffer
		)
	);
		});


	auto complete = (createPlantsTask).then([this]() {
		SetBuffers();
	m_loadingComplete = true;
		});
}

void ACW::Sample3DSceneRenderer::RenderPlants()
{
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Each vertex is one instance of the Vertex struct.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mContext->IASetVertexBuffers(
		0,
		1,
		mPlantVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	mContext->IASetIndexBuffer(
		mPlantIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderPlants.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderPlants.Get(),
		nullptr,
		0
	);

	mContext->PSSetShaderResources(0, 1, mPlantTexture.GetAddressOf());

	mContext->PSSetSamplers(0, 1, mSampler.GetAddressOf());

	//Set depth stencil
	mContext->OMSetDepthStencilState(mDepthLessThanEqual.Get(), 0);

	//Set blend state
	mContext->OMSetBlendState(mAlphaBlend.Get(), 0, 0xffffffff);

	// Attach our geometry shader.
	mContext->GSSetShader(
		mGeometryShaderPlants.Get(),
		nullptr,
		0
	);

	//Attach our domain shader
	mContext->DSSetShader(
		nullptr,
		nullptr,
		0
	);

	//Attach our hull shader
	mContext->HSSetShader(
		nullptr,
		nullptr,
		0
	);

	// Draw the objects.
	mContext->DrawIndexed(
		mPlantIndex,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreateSnakes()
{
	//Snakes shaders
	auto loadVSTaskSnakes = DX::ReadDataAsync(L"SnakeVertex.cso");
	auto loadPSTaskSnakes = DX::ReadDataAsync(L"SnakePixel.cso");
	auto loadGSTaskSnakes = DX::ReadDataAsync(L"SnakeGeometry.cso");
	auto loadVSTaskSnakes2 = DX::ReadDataAsync(L"SnakeVertex2.cso");
	auto loadPSTaskSnakes2 = DX::ReadDataAsync(L"SnakePixel2.cso");
	auto loadGSTaskSnakes2 = DX::ReadDataAsync(L"SnakeGeometry2.cso");

	//After the vertex shader file is loaded, create the shader
	auto SnakesVSTask = loadVSTaskSnakes.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderSnakes
			)
		);
		});

	//After the pixel shader file is loaded, create the shader
	auto SnakesPSTask = loadPSTaskSnakes.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderSnakes
			)
		);
		});

	//After the geometry shader file is loaded, create the shader
	auto SnakesGSTask = loadGSTaskSnakes.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mGeometryShaderSnakes
			)
		);
		});

	//After the vertex shader file is loaded, create the shader
	auto SnakesVSTask2 = loadVSTaskSnakes2.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderSnakes2
			)
		);
		});

	//After the pixel shader file is loaded, create the shader
	auto SnakesPSTask2 = loadPSTaskSnakes2.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderSnakes2
			)
		);
		});

	//After the geometry shader file is loaded, create the shader
	auto SnakesGSTask2 = loadGSTaskSnakes2.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mGeometryShaderSnakes2
			)
		);
		});


	//Once the shaders using the snake vertices are loaded, load the snake vertices
	auto createSnakesTask = (SnakesVSTask && SnakesPSTask && SnakesGSTask).then([this]() {

		static std::vector<Vertex> snakeVertices;
	static std::vector<unsigned short> snakeIndices;

	for (int i = 1; i < 10; i++)
	{
		snakeVertices.emplace_back(Vertex{ XMFLOAT3((float)i - 0.5, 0, 0) });
	}

	for (int i = 0; i < snakeVertices.size(); i++)
	{
		snakeIndices.push_back(i);
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(snakeVertices[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex) * snakeVertices.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&mSnakeVertexBuffer
		)
	);

	mSnakeIndex = snakeIndices.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &(snakeIndices[0]);
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * snakeIndices.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&mSnakeIndexBuffer
		)
	);
		});

	//Once the shaders using the snake vertices are loaded, load the snake vertices
	auto createSnakesTask2 = (SnakesVSTask2 && SnakesPSTask2 && SnakesGSTask2).then([this]() {

		static std::vector<Vertex> snakeVertices;
	static std::vector<unsigned short> snakeIndices;

	for (int i = 1; i < 10; i++)
	{
		snakeVertices.emplace_back(Vertex{ XMFLOAT3(0, 0, (float)i - 0.5) });
	}

	for (int i = 0; i < snakeVertices.size(); i++)
	{
		snakeIndices.push_back(i);
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &(snakeVertices[0]);
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(Vertex) * snakeVertices.size(), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&mSnakeVertexBuffer2
		)
	);

	mSnakeIndex = snakeIndices.size();

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &(snakeIndices[0]);
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * snakeIndices.size(), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&mSnakeIndexBuffer2
		)
	);
		});

	//Once all vertices are loaded, set buffers and set loading complete to true
	auto complete = (createSnakesTask && createSnakesTask2).then([this]() {
		SetBuffers();
	m_loadingComplete = true;
		});
}

void ACW::Sample3DSceneRenderer::RenderSnakes()
{
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

#pragma region Snake 1
	// Each vertex is one instance of the Vertex struct.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mContext->IASetVertexBuffers(
		0,
		1,
		mSnakeVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	mContext->IASetIndexBuffer(
		mSnakeIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderSnakes.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderSnakes.Get(),
		nullptr,
		0
	);

	// Attach our geometry shader.
	mContext->GSSetShader(
		mGeometryShaderSnakes.Get(),
		nullptr,
		0
	);

	// Draw the objects.
	mContext->DrawIndexed(
		mSnakeIndex,
		0,
		0
	);
#pragma endregion

#pragma region Snake2
	// Each vertex is one instance of the Vertex struct.
	stride = sizeof(Vertex);
	offset = 0;
	mContext->IASetVertexBuffers(
		0,
		1,
		mSnakeVertexBuffer2.GetAddressOf(),
		&stride,
		&offset
	);

	mContext->IASetIndexBuffer(
		mSnakeIndexBuffer2.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderSnakes2.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderSnakes2.Get(),
		nullptr,
		0
	);

	// Attach our geometry shader.
	mContext->GSSetShader(
		mGeometryShaderSnakes2.Get(),
		nullptr,
		0
	);

	// Draw the objects.
	mContext->DrawIndexed(
		mSnakeIndex,
		0,
		0
	);
#pragma endregion
}

void ACW::Sample3DSceneRenderer::CreateWater()
{
	//Water shaders
	auto loadVSTaskWater = DX::ReadDataAsync(L"WaterVertex.cso");
	auto loadPSTaskWater = DX::ReadDataAsync(L"WaterPixel.cso");
	auto loadDSTaskWater = DX::ReadDataAsync(L"WaterDomain.cso");
	auto loadHSTaskWater = DX::ReadDataAsync(L"WaterHull.cso");

	//After the vertex shader file is loaded, create the shader
	auto WaterVSTask = loadVSTaskWater.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderWater
			)
		);
		});

	//After the pixel shader file is loaded, create the shader
	auto WaterPSTask = loadPSTaskWater.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderWater
			)
		);
		});

	//After the domain shader file is loaded, create the shader
	auto WaterDSTask = loadDSTaskWater.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mDomainShaderWater
			)
		);
		});

	//After the hull shader file is loaded, create the shader
	auto WaterHSTask = loadHSTaskWater.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mHullShaderWater
			)
		);
		});
}

void ACW::Sample3DSceneRenderer::RenderWater()
{
	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderWater.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderWater.Get(),
		nullptr,
		0
	);

	//Attach our domain shader
	mContext->DSSetShader(
		mDomainShaderWater.Get(),
		nullptr,
		0
	);

	//Attach our hull shader
	mContext->HSSetShader(
		mHullShaderWater.Get(),
		nullptr,
		0
	);

	// Draw the objects.
	mContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreateCoral()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"CoralVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"CoralPixelShader.cso");
	auto loadGSTask = DX::ReadDataAsync(L"CoralGeometryShader.cso");
	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_CoralvertexShader
			)
		);

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&fileData[0],
			fileData.size(),
			&m_inputLayout
		)
	);
		});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_CoralpixelShader
			)
		);

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_constantBufferCamera
		)
	);
		});



	auto createGSTask = loadGSTask.then([this](const std::vector<byte>&
		fileData) {
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateGeometryShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&m_CoralGeometryShader
				)
			);
		});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] =
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_CoralVertexBuffer
		)
	);

	// Load mesh indices. Each trio of indices represents
	// a triangle to be rendered on the screen.
	// For example: 0,2,1 means that the vertices with indexes
	// 0, 2 and 1 from the vertex buffer compose the 
	// first triangle of this mesh.
	static const unsigned short cubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};

	m_indexCount = ARRAYSIZE(cubeIndices);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_CoralIndexBuffer
		)
	);
		});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void ACW::Sample3DSceneRenderer::RenderCoral()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}
	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_constantBufferCamera.Get(),
		0,
		NULL,
		&m_constantBufferDataCamera,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_CoralVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_CoralIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_CoralvertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBufferCamera.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_CoralpixelShader.Get(),
		nullptr,
		0
	);

	//Attach our geometry shader
	context->GSSetShader(
		m_CoralGeometryShader.Get(),
		nullptr,
		0
	);

	context->GSSetConstantBuffers1(
		0,
		1,
		m_constantBufferCamera.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreateCorals1()
{
	auto loadVSTaskFractals = DX::ReadDataAsync(L"FractalsVertex.cso");
	auto loadPSTaskFractals = DX::ReadDataAsync(L"FractalsPixel.cso");

	//After the vertex shader file is loaded, create the shader
	auto FractalsVSTask = loadVSTaskFractals.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderFractals
			)
		);
		});

	//After the pixel shader file is loaded, create the shader
	auto FractalsPSTask = loadPSTaskFractals.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderFractals
			)
		);
		});


	//Once the shaders using the cube vertices are loaded, load the cube vertices
	auto createCubeTask = (/*ImplicitShapesPSTask && ImplicitShapesVSTask
		&& ImplicitPrimitivesPSTask && ImplicitPrimitivesVSTask
		&& TerrainVSTask && TerrainPSTask && TerrainDSTask && TerrainHSTask
		&& WaterVSTask && WaterPSTask && WaterDSTask && WaterHSTask
		&& SpheresVSTask && SpheresPSTask
		&& */FractalsVSTask && FractalsPSTask).then([this]() {

			static const Vertex cubeVertices[] =
			{
				{XMFLOAT3(-0.5f, -0.5f, -0.5f)},
				{XMFLOAT3(-0.5f, -0.5f,  0.5f)},
				{XMFLOAT3(-0.5f,  0.5f, -0.5f)},
				{XMFLOAT3(-0.5f,  0.5f,  0.5f)},
				{XMFLOAT3(0.5f, -0.5f, -0.5f)},
				{XMFLOAT3(0.5f, -0.5f,  0.5f)},
				{XMFLOAT3(0.5f,  0.5f, -0.5f)},
				{XMFLOAT3(0.5f,  0.5f,  0.5f)},
			};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
		)
	);

	static const unsigned short cubeIndices[] =
	{
		0,2,1,
		1,2,3,

		4,5,6,
		5,7,6,

		0,1,5,
		0,5,4,

		2,6,7,
		2,7,3,

		0,4,6,
		0,6,2,

		1,3,7,
		1,7,5,
	};

	m_indexCount = ARRAYSIZE(cubeIndices);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
		)
	);
			});

	auto complete = (createCubeTask).then([this]() {
		SetBuffers();
	m_loadingComplete = true;
		});
}

void ACW::Sample3DSceneRenderer::RenderCorals1()
{
	// Attach our vertex shader.
	mContext->VSSetShader(
		mVertexShaderFractals.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	mContext->PSSetShader(
		mPixelShaderFractals.Get(),
		nullptr,
		0
	);

	//Draw the objects.
	mContext->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreateBuffers()
{
	//Constant buffer for camera data
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_constantBufferCamera
		)
	);

	//Constant buffer for light data
	constantBufferDesc = CD3D11_BUFFER_DESC(sizeof(LightConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&mConstantBufferLight
		)
	);

	//Constant buffer for time data
	constantBufferDesc = CD3D11_BUFFER_DESC(sizeof(TimeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&mConstantBufferTime
		)
	);
}

void ACW::Sample3DSceneRenderer::SetBuffers()
{
	//Camera buffer for vertex shader
	mContext->VSSetConstantBuffers1(
		0,
		1,
		m_constantBufferCamera.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Camera buffer for pixel shader
	mContext->PSSetConstantBuffers1(
		0,
		1,
		m_constantBufferCamera.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Camera buffer for geometry shader
	mContext->GSSetConstantBuffers1(
		0,
		1,
		m_constantBufferCamera.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Camera buffer for domain shader
	mContext->DSSetConstantBuffers1(
		0,
		1,
		m_constantBufferCamera.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Light buffer for pixel shader
	mContext->PSSetConstantBuffers1(
		1,
		1,
		mConstantBufferLight.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Time buffer for Vertex shader
	mContext->VSSetConstantBuffers1(
		1,
		1,
		mConstantBufferTime.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Time buffer for Geometry shader
	mContext->GSSetConstantBuffers1(
		1,
		1,
		mConstantBufferTime.GetAddressOf(),
		nullptr,
		nullptr
	);

	//Time buffer for Domain shader
	mContext->DSSetConstantBuffers1(
		1,
		1,
		mConstantBufferTime.GetAddressOf(),
		nullptr,
		nullptr
	);
}

void ACW::Sample3DSceneRenderer::UpdateBuffers()
{
	//Update camera buffer
	mContext->UpdateSubresource1(
		m_constantBufferCamera.Get(),
		0,
		NULL,
		&m_constantBufferDataCamera,
		0,
		0,
		0
	);

	//Update light buffer
	mContext->UpdateSubresource1(
		mConstantBufferLight.Get(),
		0,
		NULL,
		&mConstantBufferDataLight,
		0,
		0,
		0
	);

	//Update time buffer
	mContext->UpdateSubresource1(
		mConstantBufferTime.Get(),
		0,
		NULL,
		&mConstantBufferDataTime,
		0,
		0,
		0
	);
}

void ACW::Sample3DSceneRenderer::CreateBlendStates()
{
	//Create blend state for alpha blending
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_deviceResources->GetD3DDevice()->CreateBlendState(&blendDesc, mAlphaBlend.GetAddressOf());

	//Blend state for no blending
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	m_deviceResources->GetD3DDevice()->CreateBlendState(&blendDesc, mNoBlend.GetAddressOf());
}

void ACW::Sample3DSceneRenderer::CreateDepthStencils()
{
	//Create depth stencil for quad drawing
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&dsDesc, mDepthLessThanEqual.GetAddressOf());

	//Create depth stencil for depth blending
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&dsDesc, mDepthLessThanEqualAll.GetAddressOf());
}

void ACW::Sample3DSceneRenderer::CreateRasteriserStates()
{
	//Wireframe rasteriser
	D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, mWireframeRasteriser.GetAddressOf());

	//Default rasteriser
	rasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, mDefaultRasteriser.GetAddressOf());
}

void ACW::Sample3DSceneRenderer::CreateSamplerState()
{
	//Create sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_deviceResources->GetD3DDevice()->CreateSamplerState(&sampDesc, mSampler.GetAddressOf());
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	//Create directX resources
	CreateBuffers();
	CreateBlendStates();
	CreateDepthStencils();
	CreateRasteriserStates();
	CreateSamplerState();

	//Load All Shader Files
	auto loadVSTask = DX::ReadDataAsync(L"ImplicitShapesVertex.cso");
	auto loadPSTask = DX::ReadDataAsync(L"ImplicitShapesPixel.cso");

	//Implicit primitives shaders
	auto loadVSTaskPrimitives = DX::ReadDataAsync(L"ImplicitPrimitivesVertex.cso");
	auto loadPSTaskPrimitives = DX::ReadDataAsync(L"ImplicitPrimitivesPixel.cso");

	//Shiny spheres shaders
	auto loadVSTaskSpheres = DX::ReadDataAsync(L"ShinySphereRayTracerVertex.cso");
	auto loadPSTaskSpheres = DX::ReadDataAsync(L"ShinySphereRayTracerPixel.cso");


#pragma region Implicit shapes

	//After the vertex shader file is loaded, create the shader and input layout.
	auto ImplicitShapesVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShaderImplicitShapes
			)
		);

	//Input layout
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&fileData[0],
			fileData.size(),
			&m_inputLayout
		)
	);
		});

	//After the pixel shader file is loaded, create the shader.
	auto ImplicitShapesPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShaderImplicitShapes
			)
		);
		});

#pragma endregion

#pragma region Implicit primitives

	//After the vertex shader file is loaded, create the shader.
	auto ImplicitPrimitivesVSTask = loadVSTaskPrimitives.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShaderImplicitPrimitives
			)
		);
		});

	//After the pixel shader file is loaded, create the shader.
	auto ImplicitPrimitivesPSTask = loadPSTaskPrimitives.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShaderImplicitPrimitives
			)
		);
		});

#pragma endregion

#pragma region Shiny Spheres
	//After the vertex shader file is loaded, create the shader
	auto SpheresVSTask = loadVSTaskSpheres.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mVertexShaderSpheres
			)
		);
		});

	//After the pixel shader file is loaded, create the shader
	auto SpheresPSTask = loadPSTaskSpheres.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&mPixelShaderSpheres
			)
		);
		});
#pragma endregion

	CreateCoral();
	//CreateCorals1();
	CreateTerrain();
	CreateWater();
	CreateSnakes();
	CreatePlants();
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShaderImplicitShapes.Reset();
	m_inputLayout.Reset();
	m_pixelShaderImplicitShapes.Reset();
	m_constantBufferCamera.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}