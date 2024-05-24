
#ifndef DXUT_GEOMETRY_H
#define DXUT_GEOMETRY_H

#include "types.h"
#include <vector>
#include <DirectXMath.h>
#include <DirectXColors.h>
using namespace DirectX;
using std::vector;

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT4 Color;
};

class Geometry
{
protected:
	XMFLOAT3 position;								// posi��o da geometria
	uint type;										// tipo da geometria
	vector<Vertex> vertices;						// v�rtice de geometria
	vector<ushort> indices;							// �ndice de geometria

	void Subdivide();								// subdivide os tri�ngulos
	
public:
	Geometry();										// construtor
	~Geometry();									// destrutor

	virtual float X() const { return position.x; }	// retorna x da geometria
	virtual float Y() const { return position.y; }	// retorna y da geometria
	virtual float Z() const { return position.z; }	// retorna z da geometria

	// retorna o tipo da geometria
	virtual uint Type() const 
	{ return type; }

	// retorna posi��o da geometria
	virtual XMFLOAT3 Position() const 
	{ return position; }
	
	// move a geometria pelo delta (dx, dy, dz)
	virtual void Translate(float dx, float dy, float dz)
	{ position.x += dx; position.y += dy; position.z += dz; } 


	// move a geometria pelo delta (dx, dy, dz)
	virtual void MoveTo(float px, float py, float pz)
	{ position.x = px; position.y = py; position.z = pz; }

	// retorna v�rtices da geometria
	const Vertex* VertexData() const
	{ return vertices.data(); }

	const ushort* IndexData() const
	{ return indices.data(); }
	
	// retorna o n�mero de v�rtices
	uint VertexCount() const
	{ return uint(vertices.size()); }


	// retorna o n�mero de �ndices
	uint IndexCount() const
	{ return uint(indices.size()); }
};

class Box : public Geometry {
public:
	Box();
	Box(float width, float height, float depth);
};

class Cylinder : public Geometry {
public:
	Cylinder();
	Cylinder(float bottom, float top, float height, uint sliceCount, uint StackCount);
};

#endif