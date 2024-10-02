#include "DXApp.h"
#include "Error.h"
using namespace DirectX;

void DXApp::Init()
{

	
	viewports[0] = 	{0.0f					, 0.0f						, window->Width() / 2.0f, window->Height() / 2.0f};
	viewports[1] = 	{window->Width() / 2.0f , 0.0f						, window->Width() / 2.0f, window->Height() / 2.0f};
	viewports[2] = 	{0.0f					, window->Height() / 2.0f	, window->Width() / 2.0f, window->Height() / 2.0f};
	viewports[3] = 	{window->Width() / 2.0f	, window->Height() / 2.0f	, window->Width() / 2.0f, window->Height() / 2.0f};


	const uint vbSize = sizeof(Vertex);


	XMMATRIX world = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixRotationY(XMConvertToRadians(-30.0f)) * XMMatrixTranslation(0.0f, 0.0f, 5.0f);
	XMStoreFloat4x4(&cb.w, XMMatrixTranspose(world));

	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	//view *= XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), window->AspectRatio(), 1.0f, 100.0f);
	XMStoreFloat4x4(& cb.p, XMMatrixTranspose(XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), window->AspectRatio(), 1.0f, 100.0f)));


	XMStoreFloat4x4(& cb.v,XMMatrixTranspose(view));


	graphics->ResetCommands();


	// mesh da ui
	ui = new Mesh();

	const Vertex vertices[4] =
	{
		{{-1.0f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{1.0f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{0.0f, -1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		{{0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
	}; 

	ui->VertexBuffer(vertices, 4 * vbSize, vbSize);
	ui->IndexBuffer(new uint[4]{0, 1, 2, 3}, 4 * sizeof(uint), DXGI_FORMAT_R32_UINT);


	// mesh das geometrias
	geometry = new Object();

	geometry->mesh = new Mesh();
	geometry->cbIndex = 0;

	Box caixa(1.0f, 1.0f, 1.0f);
	for(Vertex& e : caixa.vertices){
		e.Color = XMFLOAT4(Colors::Chocolate);
		OutputDebugString(std::to_string(e.Color.x).c_str());
		OutputDebugString(std::to_string(e.Color.y).c_str());
		OutputDebugString(std::to_string(e.Color.z).c_str());
		OutputDebugString(std::to_string(e.Color.w).c_str());
		OutputDebugString("\n");
	}

	geometry->submesh = { caixa.IndexCount(), 0, 0 };
	
	geometry->mesh->VertexBuffer(caixa.VertexData(), caixa.VertexCount() * sizeof(Vertex), sizeof(Vertex));
	geometry->mesh->IndexBuffer(caixa.IndexData(), caixa.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
	geometry->mesh->ConstantBuffer(sizeof(ObjConstants), 1);
	XMStoreFloat4x4(&geometry->world, XMMatrixTranspose(world));
	geometry->mesh->CopyConstants(&cb, 0);

	BuildRootSignature();
	BuildPipelineState();

	graphics->SubmitCommands();

}

void DXApp::Update()
{

	if(input->KeyDown(VK_ESCAPE))
		window->Close();

	delta += frameTime;

	XMMATRIX W = XMLoadFloat4x4(&geometry->world);
	W *= XMMatrixRotationY(frameTime * 0.5f * (1 + sin(delta) * cos(delta * 1.5f)));
	XMStoreFloat4x4(&geometry->world, W);
	XMStoreFloat4x4(&cb.w, W);
	

	graphics->ResetCommands();

	geometry->mesh->CopyConstants(&cb, 0);

	graphics->SubmitCommands();

}

void DXApp::Draw()
{

	graphics->Clear(pipelineStateUI);
	//graphics->Clear(pipelineStateSceneWireframe);

	
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);


	// Desenhando a UI
	graphics->CommandList()->IASetVertexBuffers(0, 1, ui->VertexBufferView());
	graphics->CommandList()->IASetIndexBuffer(ui->IndexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	
	graphics->CommandList()->DrawIndexedInstanced(2, 1, 0, 0, 0);
	graphics->CommandList()->DrawIndexedInstanced(2, 1, 0, 2, 0);


	// Desenhando a geometria

	for(uint i = 0; i < 4; i++)
	{
		graphics->CommandList()->RSSetViewports(1, &viewports[i]);

		graphics->CommandList()->SetPipelineState(pipelineStateSceneWireframe);
		graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->mesh->VertexBufferView());
		graphics->CommandList()->IASetIndexBuffer(geometry->mesh->IndexBufferView());
		ID3D12DescriptorHeap* heaps[] = { geometry->mesh->ConstantBufferHeap() };
		graphics->CommandList()->SetDescriptorHeaps(1, heaps);
		graphics->CommandList()->SetGraphicsRootDescriptorTable(0, geometry->mesh->ConstantBufferHandle(0));
		graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		graphics->CommandList()->DrawIndexedInstanced(
			geometry->submesh.indexCount,
			1,
			geometry->submesh.startIndex,
			geometry->submesh.baseVertex,
			0);
	}



	graphics->Present();

}

void DXApp::Finalize()
{

	pipelineStateSceneSolid->Release();
	pipelineStateSceneWireframe->Release();
	pipelineStateUI->Release();
	delete geometry;
	delete ui;
	//delete

}

void DXApp::OnPause()
{
}

void DXApp::BuildRootSignature() {

	D3D12_DESCRIPTOR_RANGE cbvTable = {
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		1,
		0,
		0,
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
	};

	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType =	D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {
		1,
		rootParameters,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	};

	ID3D10Blob* serializedRootSig	 = nullptr;
	ID3D10Blob* error				 = nullptr;

	ThrowIfFailed(D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig,
		&error
	));

	if (error != nullptr)
	{
		OutputDebugString((char*)error->GetBufferPointer());
	}

	ThrowIfFailed(graphics->Device()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)));

}

void DXApp::BuildPipelineState()
{
	D3D12_INPUT_ELEMENT_DESC inputLayout[2] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	ID3D10Blob* vertexShader;
	ID3D10Blob* vertexShaderUI;
	ID3D10Blob* pixelShader;

	D3DReadFileToBlob(L"x64/Debug/Vertex.cso", &vertexShader);
	D3DReadFileToBlob(L"x64/Debug/uiVertex.cso", &vertexShaderUI);
	D3DReadFileToBlob(L"x64/Debug/Pixel.cso", &pixelShader);

	D3D12_RASTERIZER_DESC rasterizer = {};
	rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizer.CullMode = D3D12_CULL_MODE_BACK;
	rasterizer.FrontCounterClockwise = FALSE;
	rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer.DepthClipEnable = TRUE;
	rasterizer.MultisampleEnable = FALSE;
	rasterizer.AntialiasedLineEnable = FALSE;
	rasterizer.ForcedSampleCount =	0;
	rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


	D3D12_BLEND_DESC blender = {};
	blender.AlphaToCoverageEnable = FALSE;
	blender.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		blender.RenderTarget[i] = {
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};

	
	D3D12_DEPTH_STENCIL_DESC depthStencil = {};
	depthStencil.DepthEnable = TRUE;
	depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencil.StencilEnable = FALSE;
	depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = 
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS};
	depthStencil.FrontFace = defaultStencilOp;
	depthStencil.BackFace = defaultStencilOp;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = rootSignature;
	pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	pso.BlendState = blender;
	pso.SampleMask = UINT_MAX;
	pso.RasterizerState = rasterizer;
	pso.DepthStencilState = depthStencil;
	pso.InputLayout = {inputLayout, 2};
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso.DSVFormat =	DXGI_FORMAT_D24_UNORM_S8_UINT;
	pso.SampleDesc.Count = graphics->Antialiasing();
	pso.SampleDesc.Quality = graphics->Quality();

	ThrowIfFailed(graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineStateSceneWireframe)));
	

	rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
	pso.RasterizerState = rasterizer;

	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineStateSceneSolid));

	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	pso.VS = { reinterpret_cast<BYTE*>(vertexShaderUI->GetBufferPointer()), vertexShaderUI->GetBufferSize() };


	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineStateUI));

	vertexShader->Release();
	vertexShaderUI->Release();
	pixelShader->Release();

}