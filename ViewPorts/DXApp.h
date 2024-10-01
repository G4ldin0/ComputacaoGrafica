#pragma once

#include <App.h>
#include "DXUT.h"
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;

// ------------------------------------------------------------------------------


struct ObjConstants {
    XMFLOAT4X4 World =
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };
    //XMFLOAT4X4 View =
    //{ 1.0f, 0.0f, 0.0f, 0.0f,
    //  0.0f, 1.0f, 0.0f, 0.0f,
    //  0.0f, 0.0f, 1.0f, 0.0f,
    //  0.0f, 0.0f, 0.0f, 1.0f };
    //XMFLOAT4X4 Proj =
    //{ 1.0f, 0.0f, 0.0f, 0.0f,
    //  0.0f, 1.0f, 0.0f, 0.0f,
    //  0.0f, 0.0f, 1.0f, 0.0f,
    //  0.0f, 0.0f, 0.0f, 1.0f };

};


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
    static const uint VertexCount = 8;
    static const uint IndexCount = 36;
    Mesh* geometry1 = nullptr;
    Mesh* geometry2 = nullptr;
    Mesh* geometry3 = nullptr;
    Mesh* geometry4 = nullptr;

    D3D12_VIEWPORT view1;
    D3D12_VIEWPORT view2;
    D3D12_VIEWPORT view3;
    D3D12_VIEWPORT view4;

    ObjConstants newCbv[4];

    XMFLOAT4X4 matrixWorld;
    XMFLOAT4X4 matrixView;
    XMFLOAT4X4 matrixProj;

    float theta = 0;
    float phi = 0;
    float radius = 0;
    float lastMousePosX = 0;
    float lastMousePosY = 0;

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
    void OnPause();

    void BuildRootSignature();
    void BuildPipelineState();
};
