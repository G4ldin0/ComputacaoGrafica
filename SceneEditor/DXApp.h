#pragma once

#include <App.h>
#include <Mesh.h>
#include <Geometry.h>
#include <DirectXMath.h>
#include <Object.h>
using namespace DirectX;


struct ObjConstants {
	XMFLOAT4X4 w =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	XMFLOAT4X4 v =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	XMFLOAT4X4 p =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	bool selected = false;

};

class DXApp : public App
{
	ID3D12RootSignature* rootSignature = nullptr;

	ID3D12PipelineState* pipelineStateUI = nullptr;
	ID3D12PipelineState* pipelineStateSceneWireframe = nullptr;
	ID3D12PipelineState* pipelineStateSceneSolid = nullptr;

	D3D12_VIEWPORT viewports[4];

	Object * geometry = nullptr;
	Mesh * ui = nullptr;


	ObjConstants cb;

	float delta;

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
};

