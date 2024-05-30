#pragma once

#include <App.h>
#include <Timer.h>
#include <Error.h>
#include <Geometry.h>
#include <Mesh.h>
#include <DirectXMath.h>
using namespace DirectX;

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

	ID3D12DescriptorHeap* constantBufferHeap = nullptr;
	ID3D12Resource* constantBufferUpload = nullptr;
	BYTE* constantBufferData = nullptr;

	Mesh * geometry = nullptr;


	XMFLOAT4X4 World = {};
	XMFLOAT4X4 View = {};
	XMFLOAT4X4 Proj = {};

	float mouseX = 0.0f;
	float mouseY = 0.0f;

	float theta = 0.0f;
	float phi = 0.0f;
	float radius = 0.0f;

	bool rotateMode = false;
	Timer timer;

	// Herdado por meio de App
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void OnPause() override;

	// Configura assinatura raiz da GPU
	void BuildRootSignature();

	// Configura estados de função fixa e indexa Shaders do Pipeline
	void BuildPipelineState();

	void BuildGeometry();

	void BuildConstantBuffers();
};

