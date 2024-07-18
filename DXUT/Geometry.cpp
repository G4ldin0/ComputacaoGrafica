#include "Geometry.h"

Geometry::Geometry() : position(0, 0, 0), type(UNKNOWN_T) {}

Geometry::~Geometry() {}

void Geometry::Subdivide()
{
	vector < Vertex > vertexCopy = vertices;
	vector < uint > indexCopy = indices;

	vertices.resize(0);
	indices.resize(0);


	uint numTris = (uint)indexCopy.size() / 3;

	for (uint i = 0; i < numTris; i++)
	{
		Vertex v0 = vertexCopy[indexCopy[size_t(i) * 3 + 0]];
		Vertex v1 = vertexCopy[indexCopy[size_t(i) * 3 + 1]];
		Vertex v2 = vertexCopy[indexCopy[size_t(i) * 3 + 2]];

		// acha os pontos centrais em cada aresta
		Vertex m0, m1, m2;
		XMStoreFloat3(&m0.Position, 0.5f * (XMLoadFloat3(&v0.Position) + XMLoadFloat3(&v1.Position)));
		XMStoreFloat3(&m1.Position, 0.5f * (XMLoadFloat3(&v1.Position) + XMLoadFloat3(&v2.Position)));
		XMStoreFloat3(&m2.Position, 0.5f * (XMLoadFloat3(&v0.Position) + XMLoadFloat3(&v2.Position)));

		vertices.push_back(v0);	// 0
		vertices.push_back(v1);	// 1
		vertices.push_back(v2);	// 2
		vertices.push_back(m0);	// 3
		vertices.push_back(m1);	// 4
		vertices.push_back(m2);	// 5

		indices.push_back(i * 6 + 0);
		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 5);

		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 4);
		indices.push_back(i * 6 + 5);

		indices.push_back(i * 6 + 5);
		indices.push_back(i * 6 + 4);
		indices.push_back(i * 6 + 2);

		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 1);
		indices.push_back(i * 6 + 4);



	}

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

	uint boxIndex[36] = 
	{
		// front face
		0,1,2,
		2,3,0,

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

	for (uint i : boxIndex)
		indices.push_back(i);
}

Cylinder::Cylinder()
{
	type = CYLINDER_T;
}

Cylinder::Cylinder(float bottom, float top, float height, uint sliceCount, uint stackCount)
{
	type = CYLINDER_T;

	// altura de uma camada
	float stackHeight = height / stackCount;

	// incremento de raio de cada camada
	float radiusStep = (top - bottom) / stackCount;

	// numero de aneis do cilindro
	uint ringCount = stackCount + 1;

	// calcula os vértices de cada anél
	for (uint i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottom + i * radiusStep;
		float theta = 2.0f * XM_PI / sliceCount;

		for (uint j = 0; j <= sliceCount; ++j)
		{
			float c = cosf(j * theta);
			float s = sinf(j * theta);

			Vertex vertex;
			vertex.Position = XMFLOAT3(r * c, y, r * s);
			vertex.Color = XMFLOAT4(Colors::Yellow);
			vertices.push_back(vertex);
		}
	}

	// número de vértices em cada anél do cilindro
	uint ringVertexCount = sliceCount + 1;

	// calcula indices para cada camada
	for (uint i = 0; i < stackCount; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(i * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j + 1);
			indices.push_back(i * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j + 1);
			indices.push_back(i * ringVertexCount + j + 1);
		}
	}


	// constroi vértices da tampa do cilindro
	for (uint k = 0; k < 2; ++k)
	{
		
		uint baseIndex = uint(vertices.size());

		float y = (k -  0.5f) * height;
		float theta = 2.0f * XM_PI / sliceCount;
		float r =  (k ? top : bottom);
		Vertex vertex;

		for (uint i = 0; i <= sliceCount; ++i)
		{
			float x = r * cosf(i * theta);
			float z = r * sinf(i * theta);

			vertex.Position = XMFLOAT3(x, y, z);
			vertex.Color = XMFLOAT4(Colors::Turquoise);
			vertices.push_back(vertex);
		}

		// vértice central da tampa
		vertex.Position = XMFLOAT3(0.0f, y, 0.0f);
		vertex.Color = XMFLOAT4(Colors::Turquoise);
		vertices.push_back(vertex);

		uint centerIndex = uint(vertices.size() - 1);

		// índices para a tampa
		for (uint i = 0; i < sliceCount; ++i)
		{
			indices.push_back(centerIndex);
			indices.push_back(baseIndex + i + k);
			indices.push_back(baseIndex + i + 1 - k);
		}

	}

}

Sphere::Sphere()
{
	type = SPHERE_T;
}

Sphere::Sphere(float radius, uint sliceCount, uint stackCount)
{
	type = SPHERE_T;

	float phi = XM_PI / stackCount;
	float theta = 2 * XM_PI / sliceCount;

	vertices.push_back({ XMFLOAT3(0.0f, radius, 0.0f), XMFLOAT4(Colors::Blue) });


	for(uint i = 1; i <= stackCount - 1; ++i)
	{
		float y = cosf(i * phi) * radius;

		for (uint j = 0; j <= sliceCount; ++j)
		{
			float x = cosf(j * theta) * sinf(phi * i) * radius;
			float z = sinf(j * theta) * sinf(phi * i) * radius;

			vertices.push_back({ XMFLOAT3(x, y, z), XMFLOAT4(Colors::Blue) });
		}
	}

	vertices.push_back({ XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT4(Colors::Blue) });
	float center = vertices.size() - 1;

	for (uint i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint baseIndex = 1;
	uint ringVertexCount = sliceCount + 1;
	for (uint i = 0; i < stackCount - 2; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}


	uint south = (uint) vertices.size()-1;

	baseIndex = south - ringVertexCount;

	for (uint j = 0; j < sliceCount; ++j)
	{
		indices.push_back(south);
		indices.push_back(baseIndex + j);
		indices.push_back(baseIndex + j + 1);
	}

}

GeoSphere::GeoSphere()
{
	type = GEOSPHERE_T;
}

GeoSphere::GeoSphere(float radius, uint subdivisions)
{
	type = GEOSPHERE_T;

	// evitar sobreprocessamento
	subdivisions = ( subdivisions > 6U ?6U : subdivisions);

	// aproxima  uma esfera pela subdivisão de um icosaédro
	const float X = 0.525731f;
	const float Z = 0.859651f;

	// vértices de um icosaédro
	XMFLOAT3 pos[12] = 
	{
		XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f),
	};

	uint k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	vertices.resize(12);
	indices.assign(&k[0], &k[60]);

	for (uint i = 0; i < 12; ++i)
		vertices[i].Position = pos[i];

	// subdivide os triangulos do icosaedro um certo número de vezes
	for(uint i = 0; i < subdivisions; i++)
		Subdivide();

	// projeta vértices em uma esfera e ajusta a escala
	for (uint i = 0; i < vertices.size(); i++)
	{
		// normaliza vetor (ponto)
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&vertices[i].Position));

		// projeta na esfera
		XMVECTOR p = radius * n;

		XMStoreFloat3(&vertices[i].Position, p);
		vertices[i].Color = XMFLOAT4(Colors::DeepSkyBlue);
	}

}

Grid::Grid()
{
	type = GRID_T;
}

Grid::Grid(float width, float depth, uint m, uint n)
{
	type = GRID_T;

	float halfWidth = width * 0.5f;
	float halfDepth = depth * 0.5f;
	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	for (uint i = 0; i < m; ++i)
	{
		float z = - halfDepth + i * dz;
		for (uint j = 0; j < n; ++j)
		{	
			float x = -halfWidth + j * dx;
			Vertex vertex = {
				XMFLOAT3(x, 0.0f, z),
				XMFLOAT4(Colors::Fuchsia)
			};

			vertices.push_back(vertex);
		}
	}

	for (uint i = 0; i < m - 1; ++i)
	{
		for (uint j = 0; j < n - 1; ++j)
		{
			indices.push_back((i * n) + j);
			indices.push_back(((i + 1) * n) + j);
			indices.push_back(((i + 1) * n) + j + 1);

			indices.push_back(((i + 1) * n) + j + 1);
			indices.push_back((i * n) + j + 1);
			indices.push_back((i * n) + j);
		}
	}

}

Quad::Quad()
{
	type = QUAD_T;
}

Quad::Quad(float width, float height)
{

	type = QUAD_T;

	float w = width / 2.0f;
	float h = height / 2.0f;

	Vertex quadVertices[4] =
	{
		{ XMFLOAT3(-w, -h, 0.0f), XMFLOAT4(Colors::Indigo) },
		{ XMFLOAT3(-w, +h, 0.0f), XMFLOAT4(Colors::Indigo) },
		{ XMFLOAT3(+w, +h, 0.0f), XMFLOAT4(Colors::Indigo) },
		{ XMFLOAT3(+w, -h, 0.0f), XMFLOAT4(Colors::Indigo) },
	};

	for(auto& v : quadVertices)
		vertices.push_back(v);

	uint quadIndices[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	for (uint i : quadIndices)
		indices.push_back(i);

}
