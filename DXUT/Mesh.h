#ifndef DXUT_MESH_H
#define DXUT_MESH_H

#include <d3d12.h>
#include "types.h"
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;

struct SubMesh
{
	uint indexCount = 0;
	uint startLocation = 0;
	uint baseVertexLocation = 0;
};

struct Mesh
{
	// identificador para recuperar a malha pelo seu nome
	string id;



	// buffers de Upload CPU -> GPU
	ID3D12Resource* vertexBufferUpload;
	ID3D12Resource* indexBufferUpload;


	// buffers na GPU
	ID3D12Resource* vertexBufferGPU;
	ID3D12Resource* indexBufferGPU;

	// descritor do vertex buffer
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	// caracteristicas dos vertex buffers
	uint vertexBufferStride;
	uint vertexBufferSize;

	// caracteristicas dos index buffers
	DXGI_FORMAT indexFormat;
	uint indexBufferSize;

	// uma malha pode armazenar múltiplas sub-malhas
	unordered_map<string, SubMesh> subMesh;

	Mesh();                                             // construtor
	~Mesh();                                            // destrutor

	void VertexBuffer(const void* vb, uint vbSize, uint vbStride);          // aloca e copia vértices para vertex buffer 
	void IndexBuffer(const void* ib, uint ibSize, DXGI_FORMAT ibFormat);    // aloca e copia índices para index buffer 

	// retorna descritor (view) do Vertex Buffer
	D3D12_VERTEX_BUFFER_VIEW * VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW * IndexBufferView();

};

// -------------------------------------------------------------------------------

#endif