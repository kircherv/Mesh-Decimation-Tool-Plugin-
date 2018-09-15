#pragma once

#include "learnopengl\model.h"

#include "list.h"
#include "vector.h"
#include "progmesh.h"

class MeshDecimator
{
public:
	static void setInputModel(Model* inputModel, Model* outputModel);
	static void decimate(double decimatePercentage);

private:
	static Model* _inputModel;
	static Model* _outputModel;

	static void PermuteVertices(List<int> &permutation, List<Vector>& vert, List<tridata>& tri);
	static int Map(List<int>& collapse_map, int a, int mx);

	static std::vector<List<Vector>> _vertices;
	static std::vector<List<tridata>> _triangles;
	static std::vector<List<int>> _collapse_map;
	static std::vector<List<int>> _permutation;
};

