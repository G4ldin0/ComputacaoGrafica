#include "DXApp.h"
#include <Error.h>

void DXApp::Init()
{

	theta = XM_PIDIV4;
	phi = XM_PIDIV2;
	radius = 5.0f;

	mouseX = (float) input->MouseX();
	mouseY = (float) input->MouseY();

	World = View = 
	{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};

	XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(30.0f),
		window->AspectRatio(),
		1.0f, 100.0f
	));

	graphics->ResetCommands();

	BuildConstantBuffers();
	BuildGeometry();
	BuildRootSignature();
	BuildPipelineState();

	graphics->SubmitCommands();
}

void DXApp::Update()
{

	if(input->KeyPress(VK_ESCAPE))
		window->Close();

	if(input->KeyPress('I'))
		lines = !lines;
	if(input->KeyPress('R'))
		rotateMode = !rotateMode;


	float newmouseX = input->MouseX();
	float newmouseY = input->MouseY();

	if (input->KeyDown(VK_LBUTTON))
	{
		float dx = XMConvertToRadians(0.25f * (newmouseX - mouseX));
		float dy = XMConvertToRadians(0.25f * (newmouseY - mouseY));


		theta += dx;
		phi += dy;

	}
	else if (input->KeyDown(VK_RBUTTON))
	{
		float dx = 0.05f * (newmouseX - mouseX);
		float dy = 0.05f * (newmouseY - mouseY);

		radius += dx - dy;

		radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
	}

	mouseX = newmouseX;
	mouseY = newmouseY;

	if(rotateMode){
		theta += XMConvertToRadians(50.0f * frameTime);

		rotateAngle = sinf(timer += frameTime) * 0.03f;
		phi += XMConvertToRadians(rotateAngle);
		OutputDebugString(std::to_string(rotateAngle).c_str());
		OutputDebugString("\n");
	}

	phi = phi < 0.1f ? 0.1f : (phi > (XM_PI - 0.1f) ? XM_PI - 0.1f : phi);


	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);


	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&View, view);

	XMMATRIX world  = XMLoadFloat4x4(&World);
	XMMATRIX proj = XMLoadFloat4x4(&Proj);
	XMMATRIX WorldViewProj = world * view * proj;


	ObjectConstants objConstant;
	XMStoreFloat4x4(&objConstant.worldViewProj, XMMatrixTranspose(WorldViewProj));
	memcpy(constantBufferData, &objConstant.worldViewProj, sizeof(ObjectConstants));



}

void DXApp::Draw()
{

	graphics->Clear(pipelineState);


	ID3D12DescriptorHeap* descriptorHeaps[] = { constantBufferHeap };
	graphics->CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
	graphics->CommandList()->IASetIndexBuffer(geometry->IndexBufferView());

	if (lines)
		graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	else
		graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	graphics->CommandList()->SetGraphicsRootDescriptorTable(0, constantBufferHeap->GetGPUDescriptorHandleForHeapStart());

	graphics->CommandList()->DrawIndexedInstanced(18, 1, 0, 0, 0);





	// apresenta o backbuffer na tela
	graphics->Present();

}

void DXApp::Finalize()
{

	rootSignature->Release();
	pipelineState->Release();
	delete geometry;

}

void DXApp::OnPause()
{
}

// ------------------------------------------------------------------------------------------------

void DXApp::BuildConstantBuffers()
{
	// descritor do buffer constante
	D3D12_DESCRIPTOR_HEAP_DESC constantBufferHeapDesc = {};
	constantBufferHeapDesc.NumDescriptors = 1;
	constantBufferHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	constantBufferHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


	// cria descritor para buffer constante
	graphics->Device()->CreateDescriptorHeap(&constantBufferHeapDesc, IID_PPV_ARGS(&constantBufferHeap));

	// propriedade da heap do buffer de upload
	D3D12_HEAP_PROPERTIES uploadHeapProperties = {};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	uploadHeapProperties.CreationNodeMask = 1;
	uploadHeapProperties.VisibleNodeMask = 1;

	// o tamanho dos "constant buffer" precisam ser multiplos
	// do tamanho de alocação mínima do hardware (256 bytes)
	uint constantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;


	// criação do buffer de upload
	D3D12_RESOURCE_DESC uploadBufferDesc = {};
	uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadBufferDesc.Alignment = 0;
	uploadBufferDesc.Width = constantBufferSize;
	uploadBufferDesc.Height = 1;
	uploadBufferDesc.DepthOrArraySize = 1;
	uploadBufferDesc.MipLevels = 1;
	uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadBufferDesc.SampleDesc = { 1, 0 };
	uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	graphics->Device()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBufferupload)
	);

	// endereço do buffer de upload da GPU
	D3D12_GPU_VIRTUAL_ADDRESS uploadAdress = constantBufferupload->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = uploadAdress;
	cbvDesc.SizeInBytes = constantBufferSize;

	// cria um view para o buffer constante
	graphics->Device()->CreateConstantBufferView(
		&cbvDesc,
		constantBufferHeap->GetCPUDescriptorHandleForHeapStart()
	);

	// mapeia a memória do upload buffer para um endereço acessível pela CPU
	constantBufferupload->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData));

}

// ------------------------------------------------------------------------------------------------


void DXApp::BuildGeometry()
{

	//// Vértices da geometria
	// 
	//// Quadrado
	//Vertex vertices[8] = 
	//{
	//	{XMFLOAT4(-.5f, .5f, -.5f, 1.0f), XMFLOAT4(Colors::IndianRed)},
	//	{XMFLOAT4(.5f, .5f, -.5f, 1.0f), XMFLOAT4(Colors::Yellow)},
	//	{XMFLOAT4(-.5f, -.5f, -.5f, 1.0f), XMFLOAT4(Colors::Yellow)},
	//	{XMFLOAT4(.5f, -.5f, -.5f, 1.0f), XMFLOAT4(Colors::IndianRed)},

	//	{XMFLOAT4(-.5f, .5f, .5f, 1.0f), XMFLOAT4(Colors::IndianRed)},
	//	{XMFLOAT4(.5f, .5f, .5f, 1.0f), XMFLOAT4(Colors::Yellow)},
	//	{XMFLOAT4(-.5f, -.5f, .5f, 1.0f), XMFLOAT4(Colors::Yellow)},
	//	{XMFLOAT4(.5f, -.5f, .5f, 1.0f), XMFLOAT4(Colors::IndianRed)},
	//};


	//// Indices da geometria
	//ushort indices[36] = {
	//	
	//	0, 1, 2,
	//	1, 3, 2,

	//	1, 0, 4,
	//	4, 5, 1,

	//	4, 0, 6,
	//	0, 2, 6,

	//	2, 3, 6,
	//	6, 3, 7,

	//	1, 5, 3,
	//	3, 5, 7,

	//	5, 4, 6,
	//	5, 6, 7,
	//};

	// Triangulo
	Vertex vertices[5] =
	{
		{XMFLOAT3(-.5f, -.5f, -.5f), XMFLOAT4(Colors::Red)},
		{XMFLOAT3(.5f, -.5f, -.5f), XMFLOAT4(Colors::Red)},

		{XMFLOAT3(.5f, -.5f, .5f), XMFLOAT4(Colors::Green)},
		{XMFLOAT3(-.5f, -.5f, .5f), XMFLOAT4(Colors::Green)},

		{XMFLOAT3(0.0f, .75f, 0.0f), XMFLOAT4(Colors::Blue)},

	};

	ushort indices[18] =
	{

		1, 4, 0,
		2, 4, 1,
		3, 4, 2,
		0, 4, 3,

		0, 2, 1,
		2, 0, 3

	};


	const uint vbSize = sizeof(vertices);
	const uint ibSize = sizeof(indices);


	geometry = new Mesh("Plane");

	geometry->vertexBufferSize = vbSize;
	geometry->indexBufferSize = ibSize;
	geometry->vertexByteStride = sizeof(Vertex);
	geometry->indexFormat = DXGI_FORMAT_R16_UINT;


	graphics->Allocate(vbSize, &geometry->vertexBufferCPU);
	graphics->Allocate(UPLOAD, vbSize, &geometry->vertexBufferUpload);
	graphics->Allocate(GPU, vbSize, &geometry->vertexBufferGPU);

	graphics->Allocate(ibSize, &geometry->indexBufferCPU);
	graphics->Allocate(UPLOAD, ibSize, &geometry->indexBufferUpload);
	graphics->Allocate(GPU, ibSize, &geometry->indexBufferGPU);

	graphics->Copy(vertices, vbSize, geometry->vertexBufferCPU);
	graphics->Copy(indices, ibSize, geometry->indexBufferCPU);


	graphics->Copy(vertices, vbSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
	graphics->Copy(indices, ibSize, geometry->indexBufferUpload, geometry->indexBufferGPU);

}


void DXApp::BuildRootSignature()
{

	D3D12_DESCRIPTOR_RANGE cbvTable = {};
	cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvTable.NumDescriptors = 1;
	cbvTable.BaseShaderRegister = 0;
	cbvTable.RegisterSpace = 0;
	cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].DescriptorTable = { 1, &cbvTable};

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 1;
	desc.pParameters = rootParameters;
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

	if (error != nullptr) 
		OutputDebugString((char*) error->GetBufferPointer());

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
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc = { graphics->Antialiasing(), graphics->Quality() };
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	graphics->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	vertexShader->Release();
	pixelShader->Release();
}