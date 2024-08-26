#include "Mesh.h"
#include "Engine.h"

// ------------------------------------------------------------------------------

Mesh::Mesh()
{
    vertexBufferUpload = nullptr;
    vertexBufferGPU = nullptr;
    ZeroMemory(&vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
    vertexBufferSize = 0;
    vertexBufferStride = 0;

    indexBufferUpload = nullptr;
    indexBufferGPU = nullptr;
    ZeroMemory(&indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));
    indexBufferSize = 0;
    ZeroMemory(&indexFormat, sizeof(DXGI_FORMAT));
}

// -------------------------------------------------------------------------------

Mesh::~Mesh()
{
    if (vertexBufferUpload) vertexBufferUpload->Release();
    if (vertexBufferGPU) vertexBufferGPU->Release();
    if (indexBufferUpload) indexBufferUpload->Release();
    if (indexBufferGPU) indexBufferGPU->Release();
}

// -------------------------------------------------------------------------------

void Mesh::VertexBuffer(const void* vb, uint vbSize, uint vbStride)
{
    // guarda tamanho do buffer e vértice
    vertexBufferSize = vbSize;
    vertexBufferStride = vbStride;

    // aloca recursos para o vertex buffer
    Engine::graphics->Allocate(UPLOAD, vbSize, &vertexBufferUpload);
    Engine::graphics->Allocate(GPU, vbSize, &vertexBufferGPU);

    // copia vértices para o buffer da GPU usando o buffer de Upload
    Engine::graphics->Copy(vb, vbSize, vertexBufferUpload, vertexBufferGPU);
}

// -------------------------------------------------------------------------------

void Mesh::IndexBuffer(const void* ib, uint ibSize, DXGI_FORMAT ibFormat)
{
    // guarda tamanho do buffer e formato dos índices
    indexBufferSize = ibSize;
    indexFormat = ibFormat;

    // aloca recursos para o index buffer
    Engine::graphics->Allocate(UPLOAD, ibSize, &indexBufferUpload);
    Engine::graphics->Allocate(GPU, ibSize, &indexBufferGPU);

    // copia índices para o buffer da GPU usando o buffer de Upload
    Engine::graphics->Copy(ib, ibSize, indexBufferUpload, indexBufferGPU);
}

// ------------------------------------------------------------------------------

D3D12_VERTEX_BUFFER_VIEW* Mesh::VertexBufferView()
{
	vertexBufferView.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = vertexBufferStride;
	vertexBufferView.SizeInBytes = vertexBufferSize;

	return &vertexBufferView;
}

// ------------------------------------------------------------------------------

D3D12_INDEX_BUFFER_VIEW* Mesh::IndexBufferView()
{
	indexBufferView.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
	indexBufferView.Format = indexFormat;
	indexBufferView.SizeInBytes = indexBufferSize;

	return &indexBufferView;
}

