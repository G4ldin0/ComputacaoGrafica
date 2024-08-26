#pragma once

#include <App.h>
#include "DXUT.h"
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;

// ------------------------------------------------------------------------------

struct Vertex
{
    XMFLOAT4 Pos;
    XMFLOAT4 Color;
};

// ------------------------------------------------------------------------------

class Cube : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    static const uint VertexCount = 36;
    Mesh* geometry1 = nullptr;
    Mesh* geometry2 = nullptr;
    Mesh* geometry3 = nullptr;
    Mesh* geometry4 = nullptr;

    D3D12_VIEWPORT view1;
    D3D12_VIEWPORT view2;
    D3D12_VIEWPORT view3;
    D3D12_VIEWPORT view4;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();
    void OnPause();

    void BuildRootSignature();
    void BuildPipelineState();
};
