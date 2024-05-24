#pragma once

#include <App.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <Mesh.h>
#include <Timer.h>
#include <Geometry.h>

using namespace DirectX;

//struct Vertex
//{
//	XMFLOAT4 Position;
//	XMFLOAT4 Color;
//};

struct ObjectConstants {
	XMFLOAT4X4 worldViewProj = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
};

class DXApp : public App
{
	ID3D12PipelineState* pipelineState = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	Mesh* geometry = nullptr;

	ID3D12DescriptorHeap* constantBufferHeap = nullptr;
	ID3D12Resource* constantBufferupload = nullptr;
	BYTE* constantBufferData = nullptr;

	XMFLOAT4X4 World = {};
	XMFLOAT4X4 View = {};
	XMFLOAT4X4 Proj = {};

	float theta = 0;
	float phi = 0;
	float radius = 0;

	float mouseX = 0;
	float mouseY = 0;

	bool lines = false;
	bool rotateMode = false;

	float rotateAngle = 0;
	float timer = 0;

	// Herdado por meio de App
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void OnPause() override;


	void BuildConstantBuffers();

	// Configura assinatura raiz da GPU
	void BuildRootSignature();

	// Configura estados de função fixa e indexa Shaders do Pipeline
	void BuildPipelineState();

	void BuildGeometry();
};

