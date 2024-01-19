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
private:
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* pipelineState = nullptr;

	Mesh* geometry = nullptr;

	XMFLOAT4 cor;

	static const uint vMax = 1024;
	uint vIndex;
	uint count;

	Vertex* vertices;


public:
	void Init();
	void Update();
	void Display();
	void Finalize();
	void OnPause() override;

	void Debug(const char* t);
	void UpdateBuffer();

	void BuildGeometry();
	void BuildRootSignature();
	void BuildPipelineState();

};