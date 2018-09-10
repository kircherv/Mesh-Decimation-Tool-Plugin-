#pragma once

#include "learnopengl\model.h"

class MeshDecimator
{
public:
	static void setInputModel(Model* inputModel);
	static Model* getDecimatedModel(int numTriangles);

private:
	static Model* _inputModel;
};

