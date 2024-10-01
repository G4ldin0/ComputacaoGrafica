#include <Engine.h>
#include <Error.h>
#include "Resources.h"
#include "Mesh.h"
#include <DirectXMath.h>
#include <vector>
#include <fstream>

using namespace DirectX;

std::ostream& operator<<(std::ostream&, XMFLOAT3&);
std::istream& operator>>(std::istream&, XMFLOAT3&);


struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

const Vertex modeloDisplayApoio[5] = {
	{XMFLOAT3(-0.01f, -0.01f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(0.01f, -0.01f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(0.01f, 0.01f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(-0.01f, 0.01f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(-0.01f, -0.01f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
};

class BezierCurves : public App
{
	ID3D12PipelineState* pipelineState = nullptr;
	ID3D12RootSignature* rootSignature = nullptr;

	Mesh * curvasBezier = nullptr;
	Mesh * displayApoio = nullptr;
	Mesh * displayLinhas = nullptr;

	std::vector<XMFLOAT3> pontosApoio;

	std::vector<Vertex> vetorCurvas;
	std::vector<Vertex> bufferApoio;
	std::vector<Vertex> bufferLinhasApoio;

	uint pontoApoioCount = 1;

	uint displayApoioIndex = 0;

	const uint vertexPerCurve = 20;

	const uint curveType = 4;

	// Herdado por meio de App
	void Init() override;
	void Update() override;
	void Finalize() override;
	void OnPause() override;

	void Display() override;

	// Configura assinatura raiz da GPU
	void BuildRootSignature();

	// Configura estados de função fixa e indexa Shaders do Pipeline
	void BuildPipelineState();

	void GerarLinhas();

	void UpdateDisplayApoio();

	void SaveCurve();
	void LoadCurve();
};


void BezierCurves::Init()
{
	graphics->ResetCommands();

	BuildRootSignature();
	BuildPipelineState();

	pontosApoio = 
	{
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	};


	for(uint i = 0; i < curveType; i++)
		for (uint j = 0; j < 5; j++)
			bufferApoio.push_back(
			{XMFLOAT3(
				modeloDisplayApoio[j].position.x + pontosApoio[0].x,
				modeloDisplayApoio[j].position.y + pontosApoio[0].y,
				0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});

	vetorCurvas = std::vector<Vertex>(vertexPerCurve);

	bufferLinhasApoio = std::vector<Vertex>(curveType);

	curvasBezier = new Mesh();
	curvasBezier->VertexBuffer(vetorCurvas.data(), vertexPerCurve * 20.0f * sizeof(Vertex), sizeof(Vertex));

	displayApoio = new Mesh();
	displayApoio->VertexBuffer(bufferApoio.data(), bufferApoio.size() * 3.0f * sizeof(Vertex), sizeof(Vertex));

	displayLinhas = new Mesh();
	displayLinhas->VertexBuffer(bufferLinhasApoio.data(), bufferLinhasApoio.size() * sizeof(Vertex), sizeof(Vertex));

	graphics->SubmitCommands();

	Display();
}

void BezierCurves::Update()
{
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	if (input->KeyPress(VK_DELETE)){
		pontosApoio = std::vector<XMFLOAT3>({{0.0f, 0.0f, 0.0f}});
		pontoApoioCount = 1;
		displayApoioIndex = 0;
	}

	if (input->KeyPress('S'))
		SaveCurve();

	if (input->KeyPress('L'))
		LoadCurve();


	float cx = float(window->CenterX());
	float cy = float(window->CenterY());
	float mx = float(input->MouseX());
	float my = float(input->MouseY());

	if (input->KeyPress(VK_LBUTTON)){
		if (pontoApoioCount % curveType == 0 && pontoApoioCount > 0) {

			pontosApoio.push_back({ (mx / cx) - 1.0f, 1.0f - (my / cy), 0.0f });

			displayApoioIndex = (displayApoioIndex + 1) % curveType;
			UpdateDisplayApoio();


			pontosApoio.push_back(
				XMFLOAT3(
					pontosApoio[pontoApoioCount - 1].x + pontosApoio[pontoApoioCount - 1].x - pontosApoio[pontoApoioCount - 2].x,
					pontosApoio[pontoApoioCount - 1].y + pontosApoio[pontoApoioCount - 1].y - pontosApoio[pontoApoioCount - 2].y,
					0.0f
					)
			);
			pontoApoioCount+=2;
			displayApoioIndex = (displayApoioIndex + 1) % curveType;

			UpdateDisplayApoio();
		}

		pontoApoioCount++;
		pontosApoio.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
		displayApoioIndex = (displayApoioIndex + 1) % curveType;

	}

	pontosApoio.back() = { (mx / cx) - 1.0f, 1.0f - (my / cy), 0.0f };

	UpdateDisplayApoio();

	graphics->ResetCommands();
	graphics->Copy(bufferApoio.data(), displayApoio->vertexBufferSize, displayApoio->vertexBufferUpload, displayApoio->vertexBufferGPU);
	graphics->Copy(bufferLinhasApoio.data(), displayLinhas->vertexBufferSize, displayLinhas->vertexBufferUpload, displayLinhas->vertexBufferGPU);
	
	if (pontoApoioCount > 0 && pontoApoioCount % curveType == 0) GerarLinhas();
	graphics->SubmitCommands();
	Display();

}

void BezierCurves::UpdateDisplayApoio()
{
	for (uint j = 0; j < 5; j++)
		bufferApoio[j + (displayApoioIndex * 5)] = {
			XMFLOAT3(
					modeloDisplayApoio[j].position.x + pontosApoio.back().x,
					modeloDisplayApoio[j].position.y + pontosApoio.back().y,
					0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
	};
	
	bufferLinhasApoio[displayApoioIndex].position = { pontosApoio.back().x, pontosApoio.back().y, 0.0f };
}


void BezierCurves::Display()
{
	graphics->Clear(pipelineState);

	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);

	// Posições dos pontos de apoio
	graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	graphics->CommandList()->IASetVertexBuffers(0, 1, displayApoio->VertexBufferView());

	for (uint i = 0; i < min(pontosApoio.size(), curveType); i++)
		graphics->CommandList()->DrawInstanced(5, 1, i * 5, 0);


	graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	graphics->CommandList()->IASetVertexBuffers(0, 1, displayLinhas->VertexBufferView());

	graphics->CommandList()->DrawInstanced(displayApoioIndex + 1, 1, 0, 0);


	// Construir curva de bezier
	if(pontoApoioCount > curveType - 1){
		graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		graphics->CommandList()->IASetVertexBuffers(0, 1, curvasBezier->VertexBufferView());

		graphics->CommandList()->DrawInstanced(vertexPerCurve * (pontoApoioCount / curveType), 1, 0, 0);
	}
	graphics->Present();
} 

void BezierCurves::SaveCurve()
{
	std::ofstream fout("teste.sve", std::ios::binary);
	fout.write((char*)&pontoApoioCount, 4);
	fout.write((char*)&displayApoioIndex, 4);

	uint paSize = pontosApoio.size();
	fout.write((char*)&paSize, 4);
	fout.write((char*)pontosApoio.data(), sizeof(XMFLOAT3) * pontosApoio.size());

	//for (XMFLOAT3 e : pontosApoio)
	//{
	//	fout.write((char*)&e.x, 4);
	//	fout.write((char*)&e.y, 4);
	//	fout.write((char*)&e.z, 4);
	//}
	uint baSize = bufferApoio.size();
	fout.write((char*)&baSize, 4);
	fout.write((char*) bufferApoio.data(), sizeof(Vertex) * bufferApoio.size());

	//for (const Vertex& e : bufferApoio)
	//{
	//	fout.write((char*)&e.position.x, 4);
	//	fout.write((char*)&e.position.y, 4);
	//	fout.write((char*)&e.position.z, 4);
	//	fout.write((char*)&e.color.x, 4);
	//	fout.write((char*)&e.color.y, 4);
	//	fout.write((char*)&e.color.z, 4);
	//	fout.write((char*)&e.color.w, 4);
	//}

	uint blaSize = bufferLinhasApoio.size();
	fout.write((char*)&blaSize, 4);
	fout.write((char*)bufferLinhasApoio.data(), sizeof(Vertex) * bufferLinhasApoio.size());
	//for (const Vertex& e : bufferLinhasApoio)
	//{
	//	fout.write((char*)&e.position.x, 4);
	//	fout.write((char*)&e.position.y, 4);
	//	fout.write((char*)&e.position.z, 4);
	//	fout.write((char*)&e.color.x, 4);
	//	fout.write((char*)&e.color.y, 4);
	//	fout.write((char*)&e.color.z, 4);
	//	fout.write((char*)&e.color.w, 4);
	//}

	uint vcSize = vetorCurvas.size();
	fout.write((char*)&vcSize, 4);
	fout.write((char*)vetorCurvas.data(), sizeof(Vertex) * vetorCurvas.size());


	fout.close();

}

void BezierCurves::LoadCurve()
{

	std::ifstream fin("teste.sve", std::ios::binary);

	uint teste0size;
	std::vector<XMFLOAT3> teste0;
	uint teste1size;
	std::vector<Vertex> teste1;
	uint teste12size;
	std::vector<Vertex> teste12;
	uint teste2size;
	std::vector<Vertex> teste2;
	

	fin.read((char*)&pontoApoioCount, sizeof(uint));
	fin.read((char*)&displayApoioIndex, 4);
	
	// Pontos de apoio
	fin.read((char*)&teste0size, 4);
	for(uint i = 0; i < teste0size; i++){
		XMFLOAT3 ph{};
		fin.read((char*)&ph.x, 4);
		fin.read((char*)&ph.y, 4);
		fin.read((char*)&ph.z, 4);
		teste0.push_back(ph);
	}


	// Buffer de apoio
	fin.read((char*)&teste1size, 4);
	for (uint i = 0; i < teste1size; i++) {
		Vertex ph{};
		fin.read((char*)&ph.position.x, 4);
		fin.read((char*)&ph.position.y, 4);
		fin.read((char*)&ph.position.z, 4);
		fin.read((char*)&ph.color.x, 4);
		fin.read((char*)&ph.color.y, 4);
		fin.read((char*)&ph.color.z, 4);
		fin.read((char*)&ph.color.w, 4);
		teste1.push_back(ph);
	}
	
	fin.read((char*)&teste12size, 4);
	for (uint i = 0; i < teste12size; i++) {
		Vertex ph{};
		fin.read((char*)&ph.position.x, 4);
		fin.read((char*)&ph.position.y, 4);
		fin.read((char*)&ph.position.z, 4);
		fin.read((char*)&ph.color.x, 4);
		fin.read((char*)&ph.color.y, 4);
		fin.read((char*)&ph.color.z, 4);
		fin.read((char*)&ph.color.w, 4);
		teste12.push_back(ph);
	}

	// Buffer de curvas
	fin.read((char*)&teste2size, 4);
	for (uint i = 0; i < teste2size; i++) {
		Vertex ph{};
		fin.read((char*)&ph.position.x, 4);
		fin.read((char*)&ph.position.y, 4);
		fin.read((char*)&ph.position.z, 4);
		fin.read((char*)&ph.color.x, 4);
		fin.read((char*)&ph.color.y, 4);
		fin.read((char*)&ph.color.z, 4);
		fin.read((char*)&ph.color.w, 4);
		teste2.push_back(ph);

		OutputDebugString(std::to_string(ph.position.x).append(" ").c_str());
		OutputDebugString(std::to_string(ph.position.y).append(" ").c_str());
	}
	OutputDebugString("\n");

	//OutputDebugString(std::to_string(pontosApoio.size()).append(" -> ").append(std::to_string(teste0.size())).c_str());
	//OutputDebugString(std::to_string(pontosApoio.size()).append(" -> ").append(std::to_string(teste0.size())).c_str());
	//OutputDebugString(std::to_string(pontosApoio.size()).append(" -> ").append(std::to_string(teste0.size())).c_str());
	pontosApoio = teste0;
	bufferApoio = teste1;
	bufferLinhasApoio = teste12;
	vetorCurvas = teste2;
}

void BezierCurves::Finalize()
{
	
	pipelineState->Release();
	rootSignature->Release();
	//delete displayApoio;
	//delete curvasBezier;

}
void BezierCurves::OnPause()
{
}

// GerarLinhas() deve ser chamado antes de enviar os comandos
void BezierCurves::GerarLinhas()
{
	uint curveCount = pontoApoioCount / curveType;

	if (vetorCurvas.size() < vertexPerCurve * curveCount) 
	{
		for (uint i = 0; i < vertexPerCurve; i++) vetorCurvas.push_back({ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		//OutputDebugString(std::to_string(pontoApoioCount).append(" ").append(std::to_string(vetorCurvas.size())).append("\n").c_str());

		curvasBezier->vertexBufferSize = vetorCurvas.size() * sizeof(Vertex);

		curvasBezier->vertexBufferUpload->Release();
		curvasBezier->vertexBufferGPU->Release();

		graphics->Allocate(UPLOAD, curvasBezier->vertexBufferSize, &curvasBezier->vertexBufferUpload);
		graphics->Allocate(GPU, curvasBezier->vertexBufferSize, &curvasBezier->vertexBufferGPU);
		
	}

	uint backIndex = pontoApoioCount - 1;

	for(uint i = 0; i < vertexPerCurve; i ++){

		float t = float(i) / (vertexPerCurve - 1);
		float oneMinusT = 1.0f - t;
		float cubicT = t * t * t;

		uint index = i + (curveCount - 1) * vertexPerCurve;
		vetorCurvas[index].position = XMFLOAT3(
					(pow(oneMinusT, 3) * pontosApoio[backIndex - 3].x) + (3.0f * t * oneMinusT * oneMinusT * pontosApoio[backIndex - 2].x) + (3.0f * t * t * oneMinusT * pontosApoio[backIndex - 1].x) + (cubicT * pontosApoio[backIndex].x),
					(pow(oneMinusT, 3) * pontosApoio[backIndex - 3].y) + (3.0f * t * oneMinusT * oneMinusT * pontosApoio[backIndex - 2].y) + (3.0f * t * t * oneMinusT * pontosApoio[backIndex - 1].y) + (cubicT * pontosApoio[backIndex].y),
					0.0f );
		vetorCurvas[i + (curveCount - 1) * vertexPerCurve].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	
	graphics->Copy(vetorCurvas.data(), curvasBezier->vertexBufferSize, curvasBezier->vertexBufferUpload, curvasBezier->vertexBufferGPU);

}


// ------------------------------------------------------------------------------
//                                     D3D                                      
// ------------------------------------------------------------------------------

void BezierCurves::BuildRootSignature()
{

	D3D12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumParameters = 0;
	rsDesc.NumStaticSamplers = 0;
	rsDesc.pParameters = nullptr;
	rsDesc.pStaticSamplers = nullptr;

	ID3DBlob * rs = nullptr;
	ID3D10Blob * error = nullptr;

	ThrowIfFailed(D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&rs,
		&error
	));

	graphics->Device()->CreateRootSignature(
		0, 
		rs->GetBufferPointer(),
		rs->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);

}

void BezierCurves::BuildPipelineState()
{

	D3D12_INPUT_ELEMENT_DESC layouts[2] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_LAYOUT_DESC layoutDesc = {};
	layoutDesc.NumElements = 2;
	layoutDesc.pInputElementDescs = layouts;

	ID3DBlob* vertex;
	ID3DBlob* pixel;

	D3DReadFileToBlob(L"x64/Debug/Vertex.cso", &vertex);
	D3DReadFileToBlob(L"x64/Debug/Pixel.cso", &pixel);

	D3D12_BLEND_DESC blend;
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
	
	D3D12_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultDSop =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	dsDesc.FrontFace = defaultDSop;
	dsDesc.BackFace = defaultDSop;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc = {};
	psDesc.pRootSignature = rootSignature;
	psDesc.InputLayout = layoutDesc;
	psDesc.VS = { reinterpret_cast<BYTE*>(vertex->GetBufferPointer()), vertex->GetBufferSize() };
	psDesc.PS = { reinterpret_cast<BYTE*>(pixel->GetBufferPointer()), pixel->GetBufferSize() };
	psDesc.BlendState = blend;
	psDesc.SampleMask = UINT_MAX;
	psDesc.RasterizerState = rasterizerDesc;
	psDesc.DepthStencilState = dsDesc;
	psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psDesc.SampleDesc = {graphics->Antialiasing(), graphics->Quality()};
	psDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	graphics->Device()->CreateGraphicsPipelineState(&psDesc ,IID_PPV_ARGS(&pipelineState));

	vertex->Release();
	pixel->Release();
}


// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	try
	{
		// cria motor
		Engine* engine = new Engine();

		// configura a janela
		engine->window->Mode(WINDOWED);
		engine->window->Size(600, 600);
		engine->window->Color(100, 100, 255);
		engine->window->Title("DXGI DXUT");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);

		// aplicação pausa/resume ao perder/ganhar o foco
		engine->window->LostFocus(Engine::Pause);
		engine->window->InFocus(Engine::Resume);

		// cria e executa a aplicação
		int exitCode = engine->Start(new BezierCurves());

		// finaliza execução
		delete engine;
		return exitCode;
	}
	catch (Error& e)
	{
		MessageBox(nullptr, e.ToString().data() , "DXGI DXUT", MB_OK);
		return 0;
	}
}

std::ostream& operator<<(std::ostream& o, XMFLOAT3& v)
{
	o << v.x << " " << v.y << " " << v.z;
	return o;
}

std::istream& operator>>(std::istream& i, XMFLOAT3& v)
{
	i >> v.x >> v.y >> v.z;
	return i;
}
