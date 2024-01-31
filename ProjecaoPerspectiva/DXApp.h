#pragma once

#include <App.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <Mesh.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT4 Position;
	XMFLOAT4 Color;
};

class DXApp : public App
{
	ID3D12PipelineState* pipelineState = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;
	Mesh* geometry = nullptr;

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
};

