#pragma once

#include "learnopengl\model.h"

#include "list.h"
#include "vector.h"
#include "progmesh.h"

class MeshDecimator
{
public:
	static void setInputModel(Model* inputModel);
	static Model* getDecimatedModel(double decimatePercentage);

private:
	static Model* _inputModel;

	static void PermuteVertices(List<int> &permutation, List<Vector>& vert, List<tridata>& tri);
	static int Map(List<int>& collapse_map, int a, int mx);
};

