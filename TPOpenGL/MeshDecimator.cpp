#include "MeshDecimator.h"

Model* MeshDecimator::_inputModel;

void MeshDecimator::setInputModel(Model* inputModel)
{
	_inputModel = inputModel;
}

Model* MeshDecimator::getDecimatedModel(int numTriangles)
{
	return nullptr;
}
