#pragma once

#include <App.h>
#include <Mesh.h>
#include <DirectXMath.h>
using namespace DirectX;

struct ObjectConstants {
	XMFLOAT4X4 world =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	XMFLOAT4X4 ViewProj =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	//XMFLOAT3 cameraPos = {0.0f, 0.0f, -10.0f};
	XMFLOAT4 lightDirection = { 0.0f, 0.0f, 1.0f, 0.0f};

	XMFLOAT4 lightAmbient = {0.5f, 0.0f, 0.0f, 1.0f};

	float strenght = 1.0f;
	XMFLOAT3 offset;
	XMFLOAT4 cameraPos;
};

class DXApp : public App
{
	ID3D12PipelineState* pipelineState = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	ID3D12DescriptorHeap* constantBufferHeap = nullptr;
	ID3D12Resource* constantBufferUpload = nullptr;
	BYTE* constantBufferData = nullptr;


	Mesh * geometry = nullptr;

	XMFLOAT4X4 World = {};
	XMFLOAT4X4 View = {};
	XMFLOAT4X4 Proj = {};

	XMFLOAT3 worldPos = {};

	float mouseX = 0;
	float mouseY = 0;

	float theta = 0;
	float phi = 0;
	float radius = 0;

	float lightTheta = 0;
	float lightPhi = 0;
	float lightStrenght = 0.5f;

	// Herdado por meio de App
	void Init() override;
	void Update() override;
	void Draw();
	void Finalize() override;
	void OnPause() override;

	// Configura assinatura raiz da GPU
	void BuildRootSignature();

	// Configura estados de função fixa e indexa Shaders do Pipeline
	void BuildPipelineState();

	void BuildGeometry();

	void BuildConstantBuffers();
};

