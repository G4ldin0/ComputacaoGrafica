#include "DXApp.h"
#include "Error.h"
#include <fstream>
#include <sstream>
#include <regex>

using namespace DirectX;

uint testeTamanho = ((sizeof(Teste) + 255) & ~255);

void DXApp::createObject(Geometry* geo)
{
	// Inicialização do objeto
	Object* geometry = new Object();
	geometry->mesh = &buffer[0];
	geometry->submesh.indexCount = geo->IndexCount();
	geometry->submesh.startIndex = indices.size();
	geometry->submesh.baseVertex = vertices.size();
	geometry->cbIndex = scene.size();

	// Inserção nos vetores globais : índice e vértice
	scene.push_back(*geometry);
	vertices.insert(vertices.end(), geo->vertices.begin(), geo->vertices.end());
	indices.insert(indices.end(), geo->indices.begin(), geo->indices.end());

	// Inserção nos vetors de constante
	ObjConstants c;
	c.selected = true;
	XMStoreFloat4x4(&c.w, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&c.v, XMMatrixTranspose(viewMatrix[0]));
	XMStoreFloat4x4(&c.p, XMMatrixTranspose(projP));
	constantes[0].push_back(c);

	XMStoreFloat4x4(&c.p, XMMatrixTranspose(projO));

	for (uint i = 1; i < 4; i++)
	{
		XMStoreFloat4x4(&c.v, XMMatrixTranspose(viewMatrix[i]));
		constantes[i].push_back(c);
	}
	updateBuffer();

}

void DXApp::LoadFile(string filename)
{
	Geometry obj;
	std::vector<Vertex> objVertices;
	std::vector<uint> objIndices;

	//OutputDebugString(std::to_string().c_str());
	std::ifstream in(filename);

	if (!in.is_open())
		return;

	string type;
	while (!in.eof())
	{
		in >> type;
		//OutputDebugString(string(type).append(" ").c_str());

		if (type._Equal("v")) {
			XMFLOAT3 point;
			in >> point.x >> point.y >> point.z;
			obj.vertices.push_back({point, XMFLOAT4(Colors::Azure)});
		}
		else if (type._Equal("f"))
		{
			string toParse;

			std::getline(in, toParse);
			if (toParse.empty())
				continue;


			std::stringstream parser(toParse);

			vector<uint> faceIndices;
			string elemento;

			//OutputDebugString(elemento.c_str());

			std::regex nums("\\d+");
			std::smatch match;
			
			while (!parser.eof()){
				parser >> elemento;
				if (std::regex_search(elemento, match, nums))
					faceIndices.push_back(std::stoi(match[0].str()));
			}

			obj.indices.push_back(faceIndices[0] - 1);
			obj.indices.push_back(faceIndices[1] - 1);
			obj.indices.push_back(faceIndices[2] - 1);
			if(faceIndices.size() == 4)
			{
				obj.indices.push_back(faceIndices[2] - 1);
				obj.indices.push_back(faceIndices[3] - 1);
				obj.indices.push_back(faceIndices[0] - 1);
			}
		}
		else {
			// ignora todos os outros comandos
			std::getline(in, type);
		}
	}
	in.close();

	/*Object newObj;
	newObj.mesh = &buffer[0];
	newObj.submesh.indexCount = obj.IndexCount();
	newObj.submesh.startIndex = indices.size();
	newObj.submesh.baseVertex = vertices.size();
	newObj.cbIndex = scene.size();

	indices.insert(indices.end(), obj.indices.begin(), obj.indices.end());
	vertices.insert(vertices.end(), obj.vertices.begin(), obj.vertices.end());
	ObjConstants c = {};
	XMStoreFloat4x4(&c.p, XMMatrixTranspose(projP));
	XMStoreFloat4x4(&c.v, XMMatrixTranspose(viewMatrix[0]));
	constantes[0].push_back(c);

	for(uint i = 1; i < 4; i++)
	{
		XMStoreFloat4x4(&c.v, XMMatrixTranspose(viewMatrix[i]));
		XMStoreFloat4x4(&c.p, XMMatrixTranspose(projO));
		constantes[i].push_back(c);
	}

	newObj.world = c.w;

	scene.push_back(newObj);
	updateBuffer();*/

	createObject(&obj);

	OutputDebugString("\n");

}

void DXApp::updateBuffer()
{
	OutputDebugString("Entrei\n");
	graphics->ResetCommands();

	buffer[0].VertexBuffer(vertices.data(), (vertices.size() > 0 ? vertices.size() : 1) * sizeof(Vertex), sizeof(Vertex));
	buffer[0].IndexBuffer(indices.data(), (indices.size() > 0 ? indices.size() : 1) * sizeof(uint), DXGI_FORMAT_R32_UINT);
	
	for ( uint i = 0; i < 4; i++){
		buffer[i].ConstantBuffer(sizeof(ObjConstants), scene.size());

		for(uint j = 0; j < scene.size(); j++)
			buffer[i].CopyConstants(&constantes[i][j], j);
		
	}

	graphics->SubmitCommands();

}

void DXApp::Init()
{

	// -----------------------------
	// Parâmetros Iniciais da Câmera
	// -----------------------------

	// controla rotação da câmera
	theta = XM_PIDIV4;
	phi = 1.3f;
	radius = 5.0f;

	// pega última posição do mouse
	lastMousePosX = (float)input->MouseX();
	lastMousePosY = (float)input->MouseY();

	const uint vbSize = sizeof(Vertex);

	// mesh da ui
	{
		ui = new Mesh();

		const Vertex uiVertices[4] =
		{
			{{-1.0f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
			{{1.0f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
			{{0.0f, -1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
			{{0.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}},
		};

		graphics->ResetCommands();

		ui->VertexBuffer(uiVertices, 4 * vbSize, vbSize);
		ui->IndexBuffer(new uint[4]{ 0, 1, 2, 3 }, 4 * sizeof(uint), DXGI_FORMAT_R32_UINT);

		graphics->SubmitCommands();

	}
	
	// ViewPorts
	{
		viewports[0] = { 0.0f					, 0.0f						, window->Width() / 2.0f, window->Height() / 2.0f };
		viewports[1] = { window->Width() / 2.0f , 0.0f						, window->Width() / 2.0f, window->Height() / 2.0f };
		viewports[2] = { 0.0f					, window->Height() / 2.0f	, window->Width() / 2.0f, window->Height() / 2.0f };
		viewports[3] = { window->Width() / 2.0f	, window->Height() / 2.0f	, window->Width() / 2.0f, window->Height() / 2.0f };
	}




	// Matrizes de mundo

	projO = XMMatrixIdentity();
	projP = XMMatrixIdentity();
	
	projP *= XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), window->AspectRatio(), 1.0f, 100.0f);
	projO *= XMMatrixOrthographicLH(10.0f, 10.0f, 0.0f, 100.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);

	viewMatrix[0] = XMMatrixIdentity();
	viewMatrix[1] = XMMatrixLookAtLH(XMVectorSet(-10.0f, 0.0f, 0.0f, 1.0f), target, up);
	viewMatrix[2] = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f), target, up);
	viewMatrix[3] = XMMatrixLookAtLH(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), target, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
	//viewMatrix *= XMMatrixLookAtLH(pos, target, up);


	//Grid grid(15.0f, 15.0f, 15, 15);
	Box grid(1.0f, 1.0f, 1.0f);
	for(Vertex& e : grid.vertices) e.Color = XMFLOAT4(Colors::Chocolate);


	// mesh das geometrias
	buffer = new Mesh[4];


	createObject(&grid);


	BuildRootSignature();
	BuildPipelineState();



}

void DXApp::updateSelected(uint selection)
{
	for(uint i = 0; i < 4; i++)
		constantes[i][select].selected = false;

	select = min(selection, scene.size() - 1);
	select = max(0, selection);
	
	for (uint i = 0; i < 4; i++)
		constantes[i][select].selected = true;
}

void DXApp::Update()
{

	if(input->KeyDown(VK_ESCAPE))
		window->Close();

	if(input->KeyPress(VK_TAB))
	{
		updateSelected((input->KeyDown(VK_SHIFT) ? (select - 1) : (select + 1)) % scene.size());
	}

	if (input->KeyPress('V'))
		visualizacao = !visualizacao;

	if (input->KeyPress('B'))
	{
		createObject(new Box(1.0f, 1.0f, 1.0f));
		updateSelected(scene.size() - 1);

	}

	if (input->KeyPress('C'))
	{
		createObject(new Cylinder(1.0f, 1.0f, 2.0f, 10, 10));
		updateSelected(scene.size() - 1);

	}

	if (input->KeyPress('S'))
	{
		createObject(new Sphere(1.0f, 10, 5));
		updateSelected(scene.size() - 1);

	}
	if (input->KeyPress('G'))
	{
		createObject(new GeoSphere(1.0f, 2));
		updateSelected(scene.size() - 1);

	}
	if (input->KeyPress('P'))
	{
		createObject(new Grid(5.0f, 5.0f, 10, 10));
		updateSelected(scene.size() - 1);

	}

	if (input->KeyPress('Q'))
	{
		createObject(new Quad(1.0f, 1.0f));
		updateSelected(scene.size() - 1);

	}

	if (input->KeyPress('1'))
	{
		LoadFile("Cache/ball.obj");
		updateSelected(scene.size() - 1);
	}

	if (input->KeyPress('2'))
	{
		LoadFile("Cache/capsule.obj");
		updateSelected(scene.size() - 1);
	}

	if (input->KeyPress('3'))
	{
		LoadFile("Cache/house.obj");
		updateSelected(scene.size() - 1);
	}

	if (input->KeyPress('4'))
	{
		LoadFile("Cache/monkey.obj");
		updateSelected(scene.size() - 1);
	}

	if (input->KeyPress('5'))
	{
		LoadFile("Cache/thorus.obj");
		updateSelected(scene.size() - 1);
	}





	if (input->KeyPress(VK_DELETE) && scene.size() > 1)
	{
		// pega objeto da cena
		Object toDelete = scene[select];

		scene.erase(scene.begin() + select);

		// tira seus elementos das listas
		auto end = vertices.begin();
		if (scene.size() == select)
			end = vertices.end();
		else
			end += scene[select].submesh.baseVertex;

		vertices.erase(vertices.begin() + toDelete.submesh.baseVertex, end);
		indices.erase(indices.begin() + toDelete.submesh.startIndex, indices.begin() + toDelete.submesh.startIndex + toDelete.submesh.indexCount);
		for(uint i = 0; i < 4; i++)
			constantes[i].erase(constantes[i].begin() + toDelete.cbIndex);

		//constantes[select].selected = true;
		updateBuffer();

		// atualiza proximas submeshes

		for(auto e = scene.begin() + select; e < scene.end(); e++)
		{
			e->submesh.baseVertex -= scene[select].submesh.baseVertex - toDelete.submesh.baseVertex;
			e->submesh.startIndex -= scene[select].submesh.startIndex - toDelete.submesh.startIndex;
			e->cbIndex--;
		}

		select = min(select, scene.size() - 1);
		select = max(0, select);
		
		for(uint i = 0; i < 4; i++)
			constantes[i][select].selected = true;

		OutputDebugString(std::to_string(select).c_str());
	}

	// ======================  TRANSFORMAÇÕES  ========================
	XMMATRIX W = XMMatrixIdentity();
	{

		if (input->KeyDown(VK_CONTROL))
		{
			if (input->KeyDown('L')) W *= XMMatrixRotationY(frameTime);
			if (input->KeyDown('J')) W *= XMMatrixRotationY(-frameTime);
			if (input->KeyDown('I')) W *= XMMatrixRotationX(-frameTime);
			if (input->KeyDown('K')) W *= XMMatrixRotationX(frameTime);
			if (input->KeyDown('O')) W *= XMMatrixRotationZ(frameTime);
			if (input->KeyDown('U')) W *= XMMatrixRotationZ(-frameTime);

			W *= XMLoadFloat4x4(&scene[select].world);

		}
		else if (input->KeyDown(VK_SHIFT))
		{
			if (input->KeyDown('L')) W *= XMMatrixScaling(1.0f + frameTime, 1.0f, 1.0f);
			if (input->KeyDown('J')) W *= XMMatrixScaling(1.0f - frameTime, 1.0f, 1.0f);
			if (input->KeyDown('I')) W *= XMMatrixScaling(1.0f, 1.0f + frameTime, 1.0f);
			if (input->KeyDown('K')) W *= XMMatrixScaling(1.0f, 1.0f - frameTime, 1.0f);
			if (input->KeyDown('O')) W *= XMMatrixScaling(1.0f, 1.0f, 1.0f + frameTime);
			if (input->KeyDown('U')) W *= XMMatrixScaling(1.0f, 1.0f, 1.0f - frameTime);

			W *= XMLoadFloat4x4(&scene[select].world);
		}
		else
		{
			W *= XMLoadFloat4x4(&scene[select].world);

			if (input->KeyDown('L')) W *= XMMatrixTranslation(frameTime, 0.0f, 0.0f);
			if (input->KeyDown('J')) W *= XMMatrixTranslation(-frameTime, 0.0f, 0.0f);
			if (input->KeyDown('I')) W *= XMMatrixTranslation(0.0f, frameTime, 0.0f);
			if (input->KeyDown('K')) W *= XMMatrixTranslation(0.0f, -frameTime, 0.0f);
			if (input->KeyDown('O')) W *= XMMatrixTranslation(0.0f, 0.0f, frameTime);
			if (input->KeyDown('U')) W *= XMMatrixTranslation(0.0f, 0.0f, -frameTime);
			
		}
	}

	// modificações na matriz de mundo da geometria selecionada
	XMStoreFloat4x4(&scene[select].world, W);
	for(uint i = 0; i < 4; i++)
		XMStoreFloat4x4(&constantes[i][select].w, XMMatrixTranspose(W));



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
	else if (input->KeyDown(VK_RBUTTON))
	{
		// cada pixel corresponde a 0.05 unidades
		float dx = 0.05f * (mousePosX - lastMousePosX);
		float dy = 0.05f * (mousePosY - lastMousePosY);

		// atualiza o raio da câmera com base no deslocamento do mouse 
		radius += dx - dy;

		// restringe o raio (3 a 15 unidades)
		radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
	}

	lastMousePosX = mousePosX;
	lastMousePosY = mousePosY;

	// converte coordenadas esféricas para cartesianas
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	// constrói a matriz da câmera (view matrix)
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	viewMatrix[0] = view;

	for (auto& e : scene){
		XMStoreFloat4x4(&constantes[0][e.cbIndex].v, XMMatrixTranspose(view));
		buffer[0].CopyConstants(&constantes[0][e.cbIndex], e.cbIndex);

		for(uint i = 1; i < 4; i++)
		{
			
			buffer[i].CopyConstants(&constantes[i][e.cbIndex], e.cbIndex);
		}
	}

	if (visualizacao)
		DisplayViews();
	else
		DisplayPadrao();
}

void DXApp::DisplayPadrao()
{
	graphics->Clear(pipelineStateSceneWireframe);

	//graphics->CommandList()->
	//graphics->CommandList()->SetPipelineState(pipelineStateSceneWireframe);  



	graphics->CommandList()->IASetVertexBuffers(0, 1, buffer[0].VertexBufferView());
	graphics->CommandList()->IASetIndexBuffer(buffer->IndexBufferView());
	graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
	ID3D12DescriptorHeap* heaps[] = { buffer->ConstantBufferHeap() };
	graphics->CommandList()->SetDescriptorHeaps(1, heaps);


	for(auto& e : scene)
	{
		graphics->CommandList()->SetGraphicsRootDescriptorTable(0, buffer->ConstantBufferHandle(e.cbIndex));
		graphics->CommandList()->DrawIndexedInstanced(
			e.submesh.indexCount,
			1,
			e.submesh.startIndex,
			e.submesh.baseVertex,
			0);
	}

	graphics->Present();

}

void DXApp::DisplayViews()
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

		graphics->CommandList()->SetPipelineState(pipelineStateSceneSolid);
		graphics->CommandList()->IASetVertexBuffers(0, 1, buffer->VertexBufferView());
		graphics->CommandList()->IASetIndexBuffer(buffer->IndexBufferView());
		graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for(uint i = 0; i < 4; i++)
	{
		ID3D12DescriptorHeap* heaps[] = { buffer[i].ConstantBufferHeap()};
		graphics->CommandList()->SetDescriptorHeaps(1, heaps);
		graphics->CommandList()->RSSetViewports(1, &viewports[i]);

		for(auto& e : scene){
			graphics->CommandList()->SetGraphicsRootDescriptorTable(0, buffer[i].ConstantBufferHandle(e.cbIndex));
		
			graphics->CommandList()->DrawIndexedInstanced(
				e.submesh.indexCount,
				1,
				e.submesh.startIndex,
				e.submesh.baseVertex,
				0);
		}
	}


	graphics->Present();

}

void DXApp::Finalize()
{

	pipelineStateSceneSolid->Release();
	pipelineStateSceneWireframe->Release();
	pipelineStateUI->Release();


	delete testeViewBuffer;

	delete ui;
	//delete

}

void DXApp::OnPause()
{
}

void DXApp::BuildRootSignature() {

	D3D12_DESCRIPTOR_RANGE cbvTable = { 
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
		1, 0, 0, 
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND 
	};

	D3D12_ROOT_PARAMETER rootParameters[2];

	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].ParameterType =	D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;
	
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].Descriptor.ShaderRegister = 1;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {
		2,
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
	

	rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
	pso.RasterizerState = rasterizer;

	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineStateSceneSolid));

	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	pso.VS = { reinterpret_cast<BYTE*>(vertexShaderUI->GetBufferPointer()), vertexShaderUI->GetBufferSize() };


	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineStateUI));

	vertexShader->Release();
	vertexShaderUI->Release();
	pixelShader->Release();

}