#include "Mesh.h"

// ------------------------------------------------------------------------------

Mesh::Mesh(string name)
{
    id = name;

    vertexBufferCPU = nullptr;
    vertexBufferGPU = nullptr;
    vertexBufferUpload = nullptr;

	indexBufferCPU = nullptr;
    indexBufferGPU = nullptr;
    indexBufferUpload = nullptr;

	ZeroMemory(&vertexBufferView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	ZeroMemory(&indexBufferView, sizeof(D3D12_INDEX_BUFFER_VIEW));
	ZeroMemory(&indexFormat, sizeof(DXGI_FORMAT));

    vertexByteStride = 0;
    vertexBufferSize = 0;
	indexBufferSize = 0;
}

// ------------------------------------------------------------------------------

Mesh::~Mesh()
{
	if (vertexBufferCPU) vertexBufferCPU->Release();
	if (vertexBufferUpload) vertexBufferUpload->Release();
	if (vertexBufferGPU) vertexBufferGPU->Release();
	
	if (indexBufferCPU) indexBufferCPU->Release();
	if (indexBufferUpload) indexBufferUpload->Release();
	if (indexBufferGPU) indexBufferGPU->Release();
}

// ------------------------------------------------------------------------------

D3D12_VERTEX_BUFFER_VIEW* Mesh::VertexBufferView()
{
	vertexBufferView.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = vertexByteStride;
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

