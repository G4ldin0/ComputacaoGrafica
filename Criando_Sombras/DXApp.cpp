#include "DXApp.h"
#include <Error.h>
#include <Geometry.h>

#include <sstream>

void DXApp::Init()
{

	theta = XM_PIDIV2;
	phi = XM_PIDIV4;
	radius = 10.0f;

	mouseX = (float)input->MouseX();
	mouseY = (float)input->MouseY();


	worldPos = { 0.0f, 0.0f, 0.0f };

	World = View =
	{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};

	lightDirection = { 1.0f, 0.0f, 0.0f};

	XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
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


	if (input->KeyDown('A'))
		worldPos.x += 5 * frameTime;
	if (input->KeyDown('D'))
		worldPos.x -= 5 * frameTime;
	if (input->KeyDown('W'))
		worldPos.z += 5 * frameTime;
	if (input->KeyDown('S'))
		worldPos.z -= 5 * frameTime;

	float newmouseX = input->MouseX();
	float newmouseY = input->MouseY();

	if (input->KeyDown(VK_LBUTTON))
	{
		float dx = XMConvertToRadians(0.25f * (newmouseX - mouseX));
		float dy = XMConvertToRadians(0.25f * (newmouseY - mouseY));


		theta += dx;
		phi += dy;
	}

	radius += input->MouseWheel() * 0.05f;

	radius = radius < 3.0f ? 3.0f : (radius > 97.5f ? 97.5f : radius);

	lightTheta += frameTime;
	lightPhi += 1.25f * frameTime;

	float lightX = lightDirection.x + 1.0f * sinf(lightPhi) * cosf(lightTheta);
	float lightY = lightDirection.y + sinf(lightPhi);
	float lightZ = lightDirection.z + 1.0f * sinf(lightPhi) * sinf(lightTheta);
	XMVECTOR light = XMVectorSet(lightX, lightY, lightZ, .5f);

	//std::stringstream ss;
	//ss << lightX << ' ' << lightY << ' ' << lightZ << '\n';

	//OutputDebugString(ss.str().c_str());

	mouseX = newmouseX;
	mouseY = newmouseY;

	float x = worldPos.x + radius * sinf(phi) * cosf(theta);
	float z = worldPos.z + radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMLoadFloat3(&worldPos);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&View, view);

	XMMATRIX world = XMLoadFloat4x4(&World);
	XMMATRIX proj = XMLoadFloat4x4(&Proj);
	XMMATRIX WorldViewProj = world * view * proj;


	ObjectConstants objConstant;
	XMStoreFloat4x4(&objConstant.worldViewProj, XMMatrixTranspose(WorldViewProj));
	XMStoreFloat3(&objConstant.cameraPos, pos);
	XMStoreFloat3(&objConstant.ambientLight, light);
	memcpy(constantBufferData, &objConstant.worldViewProj, sizeof(ObjectConstants));


}

void DXApp::Draw()
{
	graphics->Clear(pipelineState);
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);

	graphics->CommandList()->SetDescriptorHeaps(1, &constantBufferHeap);

	graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
	graphics->CommandList()->IASetIndexBuffer(geometry->IndexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	graphics->CommandList()->SetGraphicsRootDescriptorTable(0, constantBufferHeap->GetGPUDescriptorHandleForHeapStart());

	graphics->CommandList()->DrawIndexedInstanced(geometry->subMesh["box"].indexCount, 1, 0, 0, 0);

	graphics->Present();
}

void DXApp::Finalize()
{
	pipelineState->Release();
	rootSignature->Release();
	delete geometry;
}

void DXApp::OnPause()
{
}

void DXApp::BuildConstantBuffers()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.NodeMask = 0;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	graphics->Device()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&constantBufferHeap));

	
	uint constantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;

	OutputDebugString(std::to_string(constantBufferSize).c_str());

	graphics->Allocate(UPLOAD, constantBufferSize, &constantBufferUpload);

	D3D12_GPU_VIRTUAL_ADDRESS uploadAddress = constantBufferUpload->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.SizeInBytes = constantBufferSize;
	cbvDesc.BufferLocation = uploadAddress;

	graphics->Device()->CreateConstantBufferView(
		&cbvDesc,
		constantBufferHeap->GetCPUDescriptorHandleForHeapStart()
	);


	constantBufferUpload->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferData));

}

void DXApp::BuildGeometry()
{
	
	Sphere geom(5.0f, 10, 5);

	// Encontrando o vetor normal dos pixels
	for (uint i = 0; i < geom.IndexCount() / 3.0f; i++)
	{
		uint i0 = geom.indices[i * 3 + 0];
		uint i1 = geom.indices[i * 3 + 1];
		uint i2 = geom.indices[i * 3 + 2];

		XMVECTOR e0 = XMLoadFloat3(&geom.vertices[i1].Position) - XMLoadFloat3(&geom.vertices[i0].Position);
		XMVECTOR e1 = XMLoadFloat3(&geom.vertices[i2].Position) - XMLoadFloat3(&geom.vertices[i0].Position);
		XMVECTOR normal = XMVector3Cross(e0, e1);
		
		 XMStoreFloat3(&geom.vertices[i0].Normal, XMLoadFloat3(&geom.vertices[i0].Normal) + normal);
		 XMStoreFloat3(&geom.vertices[i1].Normal, XMLoadFloat3(&geom.vertices[i1].Normal) + normal);
		 XMStoreFloat3(&geom.vertices[i2].Normal, XMLoadFloat3(&geom.vertices[i2].Normal) + normal);

	}

	

	for(uint i = 0; i < geom.VertexCount(); i++)
		XMStoreFloat3(&geom.vertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&geom.vertices[i].Normal)));
		


	uint vbSize = geom.VertexCount() * sizeof(Vertex);
	uint ibSize = geom.IndexCount() * sizeof(uint);

	geometry = new Mesh("base");
	geometry->VertexBuffer(geom.VertexData(), vbSize, sizeof(Vertex));
	geometry->IndexBuffer(geom.IndexData(), ibSize, DXGI_FORMAT_R32_UINT);
	geometry->subMesh["box"] = { geom.IndexCount(), 0, 0 };
}

void DXApp::BuildRootSignature()
{

	D3D12_DESCRIPTOR_RANGE range;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range.NumDescriptors = 1;
	range.BaseShaderRegister = 0;
	range.RegisterSpace = 0;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameter;
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter.DescriptorTable = { 1, &range };
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 1;
	desc.pParameters = &rootParameter;
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
		OutputDebugString((char*)error->GetBufferPointer());

	graphics->Device()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
}

void DXApp::BuildPipelineState()
{

	D3D12_INPUT_ELEMENT_DESC layout[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC layoutDesc = {};
	layoutDesc.NumElements = 3;
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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
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
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // Quero passar isto como parâmetro... tentar reconstruir a PSO em algum momento...
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc = { graphics->Antialiasing(), graphics->Quality() };
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	graphics->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	vertexShader->Release();
	pixelShader->Release();

}