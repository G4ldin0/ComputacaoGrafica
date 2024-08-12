#include "DXApp.h"
#include <DirectXColors.h>
#include <Error.h>
#include <sstream>

using namespace DirectX;

void DXApp::Init()
{
	// Procedimentos básicos de configuração dos gráficos
	graphics->ResetCommands();

	BuildGeometry();
	BuildRootSignature();
	BuildPipelineState();

	graphics->SubmitCommands();

	// Configurando valores do programa
	vIndex = 0;
	count = 0;
	vertices = new Vertex[vMax]{};

	cor = XMFLOAT4(Colors::White);


}

void DXApp::Update()
{
	if (input->KeyPress(VK_ESCAPE))
		window->Close();


	if (input->KeyPress(MK_LBUTTON))
	{

		// Recebe os valores da interface
		float cx = float(window->CenterX());
		float cy = float(window->CenterY());
		float mx = float(input->MouseX());
		float my = float(input->MouseY());

		// Processa para informações da GPU
		float posX = (mx / cx) - 1.0f;
		float posY = 1.0f - (my / cy);

		// Instancia novo vértice no vetor
		vertices[vIndex] = Vertex{ XMFLOAT3(posX, posY, 0.0f), XMFLOAT4(cor) };

		// Atualização de informações do vetor
		vIndex = (vIndex + 1) % vMax;
		if (count < vMax)
			count++;

		Debug("adicionando vértice");

		UpdateBuffer();

		// Mostra na tela
		Display();
	}

	if (input->KeyPress(VK_DELETE)) {

		if (count > 513)
			cor = XMFLOAT4(Colors::White);

		// Reincia buffer
		delete vertices;

		//Reseta valores do vetor
		vertices = new Vertex[vMax];
		vIndex = 0;
		count = 0;

		Debug("Reiniciando buffer");

		UpdateBuffer();

		// Mostra na tela
		Display();
	}


	if (input->KeyPress(VK_RETURN))
	{
		if (count < 2)
			Debug("Número de vértices insuficiente");
		else if((2 * count) - 2 > vMax)
			Debug("Tamanho de buffer insuficiente");
		else {

			Vertex* placeholder = new Vertex[vMax];
			uint phCount = 0;
			for (uint i = 0; i < count - 1; ++i)
			{
				float x = vertices[i].Pos.x * 0.75f + vertices[i + 1].Pos.x * 0.25f;
				float y = vertices[i].Pos.y * 0.75f + vertices[i + 1].Pos.y * 0.25f;

				placeholder[phCount] = { XMFLOAT3(x, y, 0.0f), cor };
				++phCount;

				x = vertices[i].Pos.x * 0.25f + vertices[i + 1].Pos.x * 0.75f;
				y = vertices[i].Pos.y * 0.25f + vertices[i + 1].Pos.y * 0.75f;

				placeholder[phCount] = { XMFLOAT3(x, y, 0.0f), cor };
				++phCount;
			}

			delete vertices;
			vertices = placeholder;
			count = phCount;
			vIndex = count;

			Debug("Aplicando Curva de bézier");
			UpdateBuffer();
			Display();
		}
	}

}

void DXApp::Debug(const char* text)
{
	std::stringstream t;
	t << text << " index: " << vIndex << " count: " << count << "\n";
	OutputDebugString(t.str().c_str());
}

void DXApp::Display()
{
	graphics->Clear(pipelineState);

	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	graphics->CommandList()->DrawInstanced(count, 1, 0, 0);

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


void DXApp::UpdateBuffer()
{

	// Checa Treshold para trocar de cor dos pontos
	if (count >= 513)
	{
		cor = XMFLOAT4(Colors::Yellow);
		for (uint z = 0; z < count; ++z)
			vertices[z].Color = cor;
	}

	//Envia vetor vazio para buffer
	//graphics->Copy(vertices, geometry->vertexBufferSize, geometry->vertexBufferCPU);
	graphics->ResetCommands();
	graphics->Copy(vertices, geometry->vertexBufferSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
	graphics->SubmitCommands();
}

// -------------------------------------------------------------------------------
//                                     D3D
// -------------------------------------------------------------------------------

void DXApp::BuildGeometry()
{
	geometry = new Mesh("Lines");

	geometry->vertexBufferStride = sizeof(Vertex);
	geometry->vertexBufferSize = vMax * sizeof(Vertex);

	graphics->Allocate(geometry->vertexBufferSize, &geometry->vertexBufferCPU);
	graphics->Allocate(UPLOAD, geometry->vertexBufferSize, &geometry->vertexBufferUpload);
	graphics->Allocate(GPU, geometry->vertexBufferSize, &geometry->vertexBufferGPU);

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
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
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
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc = { graphics->Antialiasing(), graphics->Quality() };
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	graphics->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));

	vertexShader->Release();
	pixelShader->Release();

}