#include "DXApp.h"
using namespace DirectX;

// ------------------------------------------------------------------------------

void Cube::Init()
{

    // controla rotação do cubo
    theta = XM_PIDIV4;
    phi = XM_PIDIV4;
    radius = 10.0f;

    // pega última posição do mouse
    lastMousePosX = (float)input->MouseX();
    lastMousePosY = (float)input->MouseY();

    graphics->ResetCommands();

    // ---------------------------------------

    Vertex vertices[VertexCount] = {
        { XMFLOAT4(-0.5f, -0.5f, -0.5f, 1.0f), XMFLOAT4(Colors::Yellow) }, // 0
        { XMFLOAT4(-0.5f, +0.5f, -0.5f, 1.0f), XMFLOAT4(Colors::Red) },    // 1
        { XMFLOAT4(+0.5f, +0.5f, -0.5f, 1.0f), XMFLOAT4(Colors::Red) },    // 2
        { XMFLOAT4(+0.5f, -0.5f, -0.5f, 1.0f), XMFLOAT4(Colors::Yellow) }, // 3

        { XMFLOAT4(-0.5f, -0.5f, +0.5f, 1.0f), XMFLOAT4(Colors::Red) }, // 4
        { XMFLOAT4(-0.5f, +0.5f, +0.5f, 1.0f), XMFLOAT4(Colors::Yellow) },    // 5
        { XMFLOAT4(+0.5f, +0.5f, +0.5f, 1.0f), XMFLOAT4(Colors::Yellow) },    // 6
        { XMFLOAT4(+0.5f, -0.5f, +0.5f, 1.0f), XMFLOAT4(Colors::Red) }, // 7

    };

    ushort indices[IndexCount] = {
        0, 1, 2,    // frente
        0, 2, 3,    
        3, 2, 6,    // lateral direita
        3, 6, 7,    
        4, 5, 1,    // lateral esquerda 
        4, 1, 0, 
        7, 6, 5,    // trás
        7, 5, 4,
        1, 5, 6,    // cima
        1, 6, 2,
        4, 0, 3,    // baixo
        4, 3, 7
    };

    // --------------------------------------
    // Transformação, Visualização e Projeção
    // --------------------------------------

    // matriz de mundo
    XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    XMMATRIX R = DirectX::XMMatrixRotationX(XMConvertToRadians(30));  // 30 graus
    XMMATRIX T = DirectX::XMMatrixTranslation(0, 0, 0);
    XMMATRIX W = S * R * T;

    XMStoreFloat4x4(&matrixWorld, W);

    // campo de visão em graus
    float fovAngle = 45;

    // matriz de visualização
    XMVECTOR pos1 = XMVectorSet(0, 0, -10, 1);
    XMVECTOR pos2 = XMVectorSet(15, 0, -5, 1);
    XMVECTOR pos3 = XMVectorSet(0, 15, 0, 1);
    XMVECTOR pos4 = XMVectorSet(-7, 5, -10, 1);

    XMVECTOR up1 = XMVectorSet(0, 1, 0, 0);
    XMVECTOR up2 = XMVectorSet(0, 1, 0, 0);
    XMVECTOR up3 = XMVectorSet(0, 0, 1, 0);
    XMVECTOR up4 = XMVectorSet(1, 1, 0, 0);

    XMVECTOR target = XMVectorZero();

    XMMATRIX V1 = DirectX::XMMatrixLookAtLH(pos1, target, up1);
    XMStoreFloat4x4(&matrixView, V1);
    XMMATRIX V2 = DirectX::XMMatrixLookAtLH(pos2, target, up2);
    XMMATRIX V3 = DirectX::XMMatrixLookAtLH(pos3, target, up3);
    XMMATRIX V4 = DirectX::XMMatrixLookAtLH(pos4, target, up4);


    // projeção perspectiva
    XMMATRIX Pp = DirectX::XMMatrixPerspectiveFovLH(
        XMConvertToRadians(fovAngle),
        window->AspectRatio(),
        1.0f, 100.0f);
    
    XMStoreFloat4x4(&matrixProj, Pp);


    XMMATRIX Po = DirectX::XMMatrixOrthographicLH(5.0f, 5.0f, 1.0f, 100.0f);

    // projeção ortográfica
    //XMMATRIX P = XMMatrixOrthographicLH(10, 10, 1.0f, 100.0f);

    // matriz combinada: mundo - câmera - projeção 
    XMMATRIX WorldViewProj1 = W * V1 * Pp;
    XMMATRIX WorldViewProj2 = W * V2 * Po;
    XMMATRIX WorldViewProj3 = W * V3 * Po;
    XMMATRIX WorldViewProj4 = W * V4 * Po;

    //Vertex vertices2[VertexCount];
    //memcpy(vertices2, vertices, VertexCount * sizeof(Vertex));
    //Vertex vertices3[VertexCount];
    //memcpy(vertices3, vertices, VertexCount * sizeof(Vertex));
    //Vertex vertices4[VertexCount];
    //memcpy(vertices4, vertices, VertexCount * sizeof(Vertex));

    // coloca vértices na janela de projeção

    // ---------------------------------------

    // tamanho em bytes dos vértices
    const uint vbSize = VertexCount * sizeof(Vertex);
    const uint ibSize = IndexCount * sizeof(Vertex);

    // cria malha 3D
    geometry1 = new Mesh();
    geometry2 = new Mesh();
    geometry3 = new Mesh();
    geometry4 = new Mesh();

    geometry1->VertexBuffer(vertices, vbSize, sizeof(Vertex));
    geometry1->IndexBuffer(indices, ibSize, DXGI_FORMAT_R16_UINT);
    geometry1->ConstantBuffer(sizeof(ObjConstants));

    XMStoreFloat4x4(&newCbv[0].World, WorldViewProj1);
    //XMStoreFloat4x4(&newCbv[0].World, W);
    //XMStoreFloat4x4(&newCbv[0].View, V1);
    //XMStoreFloat4x4(&newCbv[0].Proj, P);

    
    geometry1->CopyConstants(&newCbv[0], sizeof(ObjConstants));

    // --------------------------------------------------------------

    geometry2->ConstantBuffer(sizeof(ObjConstants));

    XMStoreFloat4x4(&newCbv[1].World, WorldViewProj2);

    geometry2->CopyConstants(&newCbv[1], sizeof(ObjConstants));

    // --------------------------------------------------------------

    geometry3->ConstantBuffer(sizeof(ObjConstants));

    XMStoreFloat4x4(&newCbv[2].World, WorldViewProj3);
    geometry3->CopyConstants(&newCbv[2], sizeof(ObjConstants));

    // --------------------------------------------------------------

    geometry4->ConstantBuffer(sizeof(ObjConstants));

    XMStoreFloat4x4(&newCbv[3].World, WorldViewProj4);
    geometry4->CopyConstants(&newCbv[3], sizeof(ObjConstants));

    //geometry2 = new Mesh(vertices2, vbSize, sizeof(Vertex));
    //geometry3 = new Mesh(vertices3, vbSize, sizeof(Vertex));
    //geometry4 = new Mesh(vertices4, vbSize, sizeof(Vertex));

    // --------------------------------------
    view1.TopLeftX = 0.0f;
    view1.TopLeftY = 0.0f;
    view1.Width = float(window->Width() / 2);
    view1.Height = float(window->Height() / 2);
    view1.MinDepth = 0.0f;
    view1.MaxDepth = 1.0f;
    // --------------------------------------
    view2.TopLeftX = float(window->CenterX());
    view2.TopLeftY = 0.0f;
    view2.Width = float(window->Width() / 2);
    view2.Height = float(window->Height() / 2);
    view2.MinDepth = 0.0f;
    view2.MaxDepth = 1.0f;
    // --------------------------------------
    view3.TopLeftX = 0.0f;
    view3.TopLeftY = (float)window->CenterY();
    view3.Width = float(window->Width() / 2);
    view3.Height = float(window->Height() / 2);
    view3.MinDepth = 0.0f;
    view3.MaxDepth = 1.0f;
    // --------------------------------------
    view4.TopLeftX = (float)window->CenterX();
    view4.TopLeftY = (float)window->CenterY();
    view4.Width = float(window->Width() / 2);
    view4.Height = float(window->Height() / 2);
    view4.MinDepth = 0.0f;
    view4.MaxDepth = 1.0f;
    // ---------------------------------------

    BuildRootSignature();
    BuildPipelineState();

    // ---------------------------------------

    graphics->SubmitCommands();
}

// ------------------------------------------------------------------------------

void Cube::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    float mousePosX = (float)input->MouseX();
    float mousePosY = (float)input->MouseY();

    if (input->KeyDown(VK_LBUTTON))
    {
        // cada pixel corresponde a 1/4 de grau
        float dx = XMConvertToRadians(0.25f * (mousePosX - lastMousePosX));
        float dy = XMConvertToRadians(0.25f * (mousePosY - lastMousePosY));

        // atualiza ângulos com base no deslocamento do mouse 
        // para orbitar a câmera ao redor da caixa
        theta += dx;
        phi += dy;


        // restringe o ângulo de phi ]0-180[ graus
        phi = phi < 0.1f ? 0.1f : (phi > (XM_PI - 0.1f) ? XM_PI - 0.1f : phi);
    }

    float newRadius = XMConvertToRadians(input->MouseWheel() * 0.25f);

    radius += newRadius;
    radius = (radius < 3.0f ? 3.0f : (radius > 25.0f ? 25.0f : radius));

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    // converte coordenadas esféricas para cartesianas
    float x = radius * sinf(phi) * cosf(theta);
    float z = radius * sinf(phi) * sinf(theta);
    float y = radius * cosf(phi);

    // constrói a matriz da câmera (view matrix)
    XMVECTOR pos = XMVectorSet(x, y, z, 1);

    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&matrixView, view);

    //// constrói matriz combinada (world x view x proj)
    //XMMATRIX world = XMLoadFloat4x4(&matrixWorld);
    //XMMATRIX proj = XMLoadFloat4x4(&matrixProj);
    //XMMATRIX WorldViewProj = world * view * proj;

    // atualiza o buffer com a matriz combinada
    XMMATRIX WorldViewProj = XMLoadFloat4x4(&matrixWorld) * view * XMLoadFloat4x4(&matrixProj);

    XMStoreFloat4x4(&newCbv[0].World, WorldViewProj);
    geometry1->CopyConstants(&newCbv[0], sizeof(ObjConstants));


}

// ------------------------------------------------------------------------------

void Cube::Draw()
{
    // limpa backbuffer
    graphics->Clear(pipelineState);

    // ---------------------------------------------------

    // submete comandos de configuração do pipeline
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    


    ID3D12DescriptorHeap * descriptorHeap = geometry1->ConstantBufferHeap();
    graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    graphics->CommandList()->RSSetViewports(1, &view1);
    graphics->CommandList()->IASetVertexBuffers(0, 1, geometry1->VertexBufferView());
    graphics->CommandList()->IASetIndexBuffer(geometry1->IndexBufferView());
    graphics->CommandList()->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);


    descriptorHeap = geometry2->ConstantBufferHeap();
    graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    graphics->CommandList()->RSSetViewports(1, &view2);
    graphics->CommandList()->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);


    descriptorHeap = geometry3->ConstantBufferHeap();
    graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    graphics->CommandList()->RSSetViewports(1, &view3);
    graphics->CommandList()->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);


    descriptorHeap = geometry4->ConstantBufferHeap();
    graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
    graphics->CommandList()->SetGraphicsRootDescriptorTable(0, descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    graphics->CommandList()->RSSetViewports(1, &view4);
    graphics->CommandList()->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);


    // ---------------------------------------------------

    // apresenta backbuffer
    graphics->Present();
}

// ------------------------------------------------------------------------------

void Cube::OnPause()
{

}

void Cube::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();
    delete geometry1;
    //delete geometry2;
    //delete geometry3;
    //delete geometry4;
}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void Cube::BuildRootSignature()
{
    D3D12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    cbvTable.NumDescriptors = 1;
    cbvTable.BaseShaderRegister = 0;
    cbvTable.RegisterSpace = 0;
    cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rp;
    rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rp.DescriptorTable = {1, &cbvTable};

    // descrição para uma assinatura vazia
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 1;
    rootSigDesc.pParameters = &rp;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // serializa assinatura raiz
    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    // cria uma assinatura raiz vazia
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

// ------------------------------------------------------------------------------

void Cube::BuildPipelineState()
{
    // --------------------
    // --- Input Layout ---
    // --------------------

    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // --------------------
    // ----- Shaders ------
    // --------------------

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"x64/Debug/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"x64/Debug/Pixel.cso", &pixelShader);

    // --------------------
    // ---- Rasterizer ----
    // --------------------

    D3D12_RASTERIZER_DESC rasterizer = {};
    //rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ---------------------
    // --- Color Blender ---
    // ---------------------

    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    // ---------------------
    // --- Depth Stencil ---
    // ---------------------

    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;

    // -----------------------------------
    // --- Pipeline State Object (PSO) ---
    // -----------------------------------

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

    vertexShader->Release();
    pixelShader->Release();
}
