#pragma once

#include <App.h>
#include <Mesh.h>
#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class DXApp : public App
{
	ID3D12PipelineState* pipelineState = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	Mesh* geometry = nullptr;

	XMFLOAT4 cor;

	static const uint vMax = 1024;
	uint vIndex;
	uint count;

	Vertex* vertices;


	// Herdado por meio de App
	void Init() override;
	void Update() override;
	void Display();
	void Finalize() override;
	void OnPause() override;

	// Configura assinatura raiz da GPU
	void BuildRootSignature();

	// Configura estados de função fixa e indexa Shaders do Pipeline
	void BuildPipelineState();

	void BuildGeometry();

	void Debug(const char* t);
	void UpdateBuffer();
};

