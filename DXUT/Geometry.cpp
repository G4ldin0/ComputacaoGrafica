#include "Geometry.h"

Geometry::Geometry() : position(0, 0, 0), type(UNKNOWN_T) {}

Geometry::~Geometry() {}

void Geometry::Subdivide()
{

}

Box::Box()
{
	type = BOX_T;
}

Box::Box(float width, float height, float depth)
{
	type = BOX_T;

	float w = 0.5f * width;
	float h = 0.5f * height;
	float d = 0.5f * depth;

	Vertex boxVertices[8] = 
	{
		{XMFLOAT3(-w, -h, -d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(-w, +h, -d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+w, +h, -d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+w, -h, -d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(-w, -h, +d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(-w, +h, +d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+w, +h, +d), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(+w, -h, +d), XMFLOAT4(Colors::Yellow)},
	};

	for (const Vertex& v : boxVertices)
		vertices.push_back(v);

	ushort boxIndex[36] = 
	{
		// front face
		0,1,2,
		1,2,3,

		// back face
		4,7,5,
		7,6,5,

		//	left face
		4,5,1,
		4,1,0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	for (ushort i : boxIndex)
		indices.push_back(i);
}