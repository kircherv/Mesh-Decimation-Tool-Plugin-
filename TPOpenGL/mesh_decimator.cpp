#include "mesh_decimator.h"


Model* MeshDecimator::_inputModel;
Model* MeshDecimator::_outputModel;
std::vector<List<Vector>> MeshDecimator::_vertices;
std::vector<List<Vector>> MeshDecimator::_normals;
std::vector<List<tridata>> MeshDecimator::_triangles;
std::vector<List<int>> MeshDecimator::_collapse_map;
std::vector<List<int>> MeshDecimator::_permutation;

void MeshDecimator::setInputModel(Model* inputModel, Model* outputModel)
{
	_inputModel = inputModel;
	_outputModel = outputModel;

	_vertices = std::vector<List<Vector>>(1);
	_normals = std::vector<List<Vector>>(1);
	_triangles = std::vector<List<tridata>>(1);
	_collapse_map = std::vector<List<int>>(1);
	_permutation = std::vector<List<int>>(1);

	List<Vector>& vertices = _vertices[0];
	List<Vector>& normals = _normals[0];
	List<tridata>& triangles = _triangles[0];

	std::vector<Vector> allVertices;
	
	auto vertexOffset = 0;
	for (auto m = 0; m < _inputModel->meshes.size(); m++)
	{
		const auto& mesh = _inputModel->meshes[m];

		for (const auto& vertex : mesh.vertices)
		{
			vertices.Add(Vector(vertex.Position.x, vertex.Position.y, vertex.Position.z));
			normals.Add(Vector(vertex.Normal.x, vertex.Normal.y, vertex.Normal.z));
			allVertices.push_back(Vector(vertex.Position.x, vertex.Position.y, vertex.Position.z));
		}
		unsigned int numTriangles = mesh.indices.size() / 3;
		for (auto i = 0; i < numTriangles; i++)
		{
			tridata t;
			t.v[0] = vertexOffset + mesh.indices[i * 3];
			t.v[1] = vertexOffset + mesh.indices[i * 3 + 1];
			t.v[2] = vertexOffset + mesh.indices[i * 3 + 2];

			triangles.Add(t);
		}

		vertexOffset += mesh.vertices.size();
	}

	std::sort(allVertices.begin(), allVertices.end());

	ProgressiveMesh(vertices, triangles, _collapse_map[0], _permutation[0]);
	PermuteVertices(_permutation[0], vertices, triangles);

	decimate(1.0);
}

void MeshDecimator::decimate(double decimatePercentage)
{
	_outputModel->meshes.clear();

	auto numMeshes = 1;
	for (auto m = 0; m < numMeshes; m++)
	{
		List<Vector>& vertices = _vertices[m];
		List<Vector>& normals = _normals[m];
		List<tridata>& triangles = _triangles[m];
		List<int>& collapse_map = _collapse_map[m];

		std::vector<Vertex> meshVertices;
		std::vector<unsigned int> meshIndices;

		int render_num = int(decimatePercentage * vertices.num);
		for (auto i = 0; i < triangles.num; i++)
		{
			int p0 = Map(collapse_map, triangles[i].v[0], render_num);
			int p1 = Map(collapse_map, triangles[i].v[1], render_num);
			int p2 = Map(collapse_map, triangles[i].v[2], render_num);
			// note:  serious optimization opportunity here,
			//  by sorting the triangles the following "continue" 
			//  could have been made into a "break" statement.
			if (p0 == p1 || p1 == p2 || p2 == p0) continue;

			meshIndices.push_back(p0);
			meshIndices.push_back(p1);
			meshIndices.push_back(p2);
		}

		for (auto i = 0; i < vertices.num; i++)
		{
			auto& vector = vertices[i];
			auto& normal = normals[i];
			Vertex v;
			v.Position = glm::vec3(vector.x, vector.y, vector.z);
			v.Normal = glm::vec3(normal.x, normal.y, normal.z);
			meshVertices.push_back(v);
		}

		_outputModel->meshes.emplace_back(meshVertices, meshIndices, _inputModel->meshes[m].textures);
	}
}

void MeshDecimator::PermuteVertices(List<int> &permutation, List<Vector>& vert, List<tridata>& tri)
{
	// rearrange the vertex list 
	List<Vector> temp_list;
	int i;
	assert(permutation.num == vert.num);
	for (i = 0; i<vert.num; i++) {
		temp_list.Add(vert[i]);
	}
	for (i = 0; i<vert.num; i++) {
		vert[permutation[i]] = temp_list[i];
	}
	// update the changes in the entries in the triangle list
	for (i = 0; i<tri.num; i++) {
		for (int j = 0; j<3; j++) {
			tri[i].v[j] = permutation[tri[i].v[j]];
		}
	}
}

int MeshDecimator::Map(List<int>& collapse_map, int a, int mx)
{
	if (mx <= 0) return 0;
	while (a >= mx) {
		a = collapse_map[a];
	}
	return a;
}
