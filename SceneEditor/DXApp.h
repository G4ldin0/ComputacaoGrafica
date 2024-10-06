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
	int selected = false;
};

struct Teste {
	XMFLOAT4X4 p =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };
};

class DXApp : public App
{
	ID3D12RootSignature* rootSignature = nullptr;

	ID3D12PipelineState* pipelineStateUI = nullptr;
	ID3D12PipelineState* pipelineStateSceneWireframe = nullptr;
	ID3D12PipelineState* pipelineStateSceneSolid = nullptr;

	Mesh * testeViewBuffer = nullptr;
	Mesh * testeProjBuffer = nullptr;

	Mesh * testeArquivo = nullptr;

	Mesh * ui = nullptr;

	D3D12_VIEWPORT viewports[4];

	Mesh * buffer = nullptr;
	vector<Object> scene;
	vector<Vertex> vertices;
	vector<uint> indices;
	//vector<vector<ObjConstants>> constantes;
	vector<vector<ObjConstants>> constantes = vector<vector<ObjConstants>>(4, vector<ObjConstants>());

	XMMATRIX viewMatrix[4];
	XMMATRIX projP;
	XMMATRIX projO;

	ObjConstants cb[4];

	float delta;

	uint select = 0;
	bool visualizacao = false;

	float theta = 0;
	float phi = 0;
	float radius = 0;
	float lastMousePosX = 0;
	float lastMousePosY = 0;


	// Herdado por meio de App
	void Init() override;
	void Update() override;
	void DisplayPadrao();
	void DisplayViews();
	void Finalize() override;
	void OnPause() override;

	// Configura assinatura raiz da GPU
	void BuildRootSignature();

	// Configura estados de função fixa e indexa Shaders do Pipeline
	void BuildPipelineState();

	void createObject(Geometry*);

	void LoadFile(string);

	void updateBuffer();

	void updateSelected(uint);
};

