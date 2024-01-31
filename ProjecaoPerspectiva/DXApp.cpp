#include "DXApp.h"
#include <Error.h>

void DXApp::Init()
{
	graphics->ResetCommands();

	BuildGeometry();
	BuildRootSignature();
	BuildPipelineState();

	graphics->SubmitCommands();
}

void DXApp::Update()
{

	if(input->KeyPress(VK_ESCAPE))
		window->Close();

}

void DXApp::Display()
{

	graphics->Clear(pipelineState);

	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	graphics->CommandList()->DrawInstanced(6, 1, 0, 0);

	graphics->Present();

}

void DXApp::Finalize()
{

	delete geometry;
	rootSignature->Release();
	pipelineState->Release();

}

void DXApp::OnPause()
{
}

// ------------------------------------------------------------------------------------------------

void DXApp::BuildGeometry()
{
	// V�rtices da geometria
	Vertex vertices[6]
	{
		{XMFLOAT4(-.5f, .5f, .0f, 1.0f), XMFLOAT4(Colors::Red)},
		{XMFLOAT4(.5f, .5f, .0f, 1.0f), XMFLOAT4(Colors::Red)},
		{XMFLOAT4(-.5f, -.5f, .0f, 1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT4(.5f, -.5f, .0f, 1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT4(-.5f, -.5f, .0f, 1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT4(.5f, .5f, .0f, 1.0f), XMFLOAT4(Colors::Red)},
	};


	// Matriz de mundo
	XMMATRIX S = XMMatrixScaling(5.0f, 5.0f, 1.0f);
	XMMATRIX R = XMMatrixRotationX(XMConvertToRadians(0));
	XMMATRIX T = XMMatrixTranslation(0, 0, 10);
	XMMATRIX W =  R * S * T;

	// Campo de vis�o em graus
	float fovAngle = 45.0f;

	// Matriz de visualiza��o
	XMVECTOR pos = XMVectorSet(0, 0, -1, 1);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);

	XMMATRIX P = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(fovAngle),
		window->AspectRatio(),
		1.0f, 100.0f);

	XMMATRIX WorldView = W * V * P;

	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR vertex = XMLoadFloat4(&vertices[i].Position);
		XMVECTOR proj = XMVector3TransformCoord(vertex, WorldView);
		XMStoreFloat4(&vertices[i].Position, proj);
	}

	const uint vbSize = sizeof(Vertex) * 6;

	geometry = new Mesh("Plane");

	geometry->vertexBufferSize = vbSize;
	geometry->vertexByteStride = sizeof(Vertex);

	graphics->Allocate(vbSize, &geometry->vertexBufferCPU);
	graphics->Allocate(UPLOAD, vbSize, &geometry->vertexBufferUpload);
	graphics->Allocate(GPU, vbSize, &geometry->vertexBufferGPU);

	graphics->Copy(vertices, vbSize, geometry->vertexBufferCPU);
	graphics->Copy(vertices, vbSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);

}


void DXApp::BuildRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 0;
	desc.pParameters = nullptr;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* error = nullptr;


	ThrowIfFailed(D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig,
		&error));

	graphics->Device()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
}

void DXApp::BuildPipelineState()
{
	D3D12_INPUT_ELEMENT_DESC layout[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC layoutDesc = {};
	layoutDesc.NumElements = 2;
	layoutDesc.pInputElementDescs = layout;

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;


	D3DReadFileToBlob(L"x64/Debug/Vertex.cso", &vertexShader);
	D3DReadFileToBlob(L"x64/Debug/Pixel.cso", &pixelShader);


	D3D12_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = FALSE;
	blend.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultBlendRT =
	{
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};
	for (uint i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		blend.RenderTarget[i] = defaultBlendRT;


	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


	D3D12_DEPTH_STENCIL_DESC DSdesc = {};
	DSdesc.DepthEnable = TRUE;
	DSdesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DSdesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	DSdesc.StencilEnable = FALSE;
	DSdesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	DSdesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultDSop =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	DSdesc.FrontFace = defaultDSop;
	DSdesc.BackFace = defaultDSop;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.pRootSignature = rootSignature;
	desc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	desc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	desc.BlendState = blend;
	desc.SampleMask = UINT_MAX;
	desc.RasterizerState = rasterizerDesc;
	desc.DepthStencilState = DSdesc;
	desc.InputLayout = layoutDesc;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;;
	desc.SampleDesc = { graphics->Antialiasing(), graphics->Quality() };
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	graphics->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	vertexShader->Release();
	pixelShader->Release();
}