#pragma once

#include <App.h>
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


	XMFLOAT4X4 World = {};
	XMFLOAT4X4 View = {};
	XMFLOAT4X4 Proj = {};

	XMFLOAT3 worldPos = {};

	Mesh* geometry = nullptr;

	float theta = 0;
	float phi = 0;
	float radius = 0;

	float mouseX = 0;
	float mouseY = 0;

	bool lines = false;
	bool rotateMode = false;
	float rotateAngle = 0.0f;
	float timer = 0.0f;

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

	void BuildConstantBuffers();

	void BuildGeometry();
};

