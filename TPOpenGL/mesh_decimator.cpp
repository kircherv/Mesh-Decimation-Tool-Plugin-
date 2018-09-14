#include "mesh_decimator.h"


Model* MeshDecimator::_inputModel;

void MeshDecimator::setInputModel(Model* inputModel)
{
	_inputModel = inputModel;
}

Model* MeshDecimator::getDecimatedModel(double decimatePercentage)
{
	Model* resultModel = new Model();

	for (const auto& mesh : _inputModel->meshes)
	{
		List<Vector> vertices;
		List<tridata> triangles;

		std::vector<Vertex> meshVertices;
		std::vector<unsigned int> meshIndices;

		for (const auto& vertex : mesh.vertices)
		{
			vertices.Add(Vector(vertex.Position.x, vertex.Position.y, vertex.Position.z));
			meshVertices.push_back(vertex);
		}
		unsigned int numTriangles = mesh.indices.size() / 3;
		for (auto i = 0; i < numTriangles; i++)
		{
			tridata t;
			t.v[0] = i * 3;
			t.v[1] = i * 3 + 1;
			t.v[2] = i * 3 + 2;
			
			triangles.Add(t);
		}
		List<int> map, permutation;

		ProgressiveMesh(vertices, triangles, map, permutation);
		PermuteVertices(permutation, vertices, triangles);
		int render_num = int(decimatePercentage * mesh.vertices.size());
		for (auto i = 0; i < triangles.num; i++)
		{
			int p0 = Map(map, triangles[i].v[0], render_num);
			int p1 = Map(map, triangles[i].v[1], render_num);
			int p2 = Map(map, triangles[i].v[2], render_num);
			// note:  serious optimization opportunity here,
			//  by sorting the triangles the following "continue" 
			//  could have been made into a "break" statement.
			if (p0 == p1 || p1 == p2 || p2 == p0) continue;
			
			meshIndices.push_back(p0);
			meshIndices.push_back(p1);
			meshIndices.push_back(p2);
		}
	
		resultModel->meshes.emplace_back(meshVertices, meshIndices, mesh.textures);
	}

	return resultModel;
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
