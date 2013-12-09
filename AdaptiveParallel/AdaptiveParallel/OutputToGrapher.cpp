#include "OutputToGrapher.h"
#include <fstream>

void OutputToGrapher(std::shared_ptr<TValueParamObject<double, double>> iObject, std::string iPath, std::size_t N/* = 100*/)
{
	std::vector<double> params;
	std::vector<std::size_t> dimensions;
	iObject->GetParams(N, params, dimensions);

	std::ofstream f(iPath);

	for (std::size_t i = 0; i < params.size(); ++i)
	{
		f << params[i] << "," << iObject->Evaluate(params[i]) << ",0" << std::endl;
	}

	f.close();
}

void OutputToGrapher(std::shared_ptr<TValueParamObject<Vector3D, Vector2D>> iObject, std::string iPath, std::size_t N/* = 100*/)
{
	std::vector<Vector2D> params;
	std::vector<std::size_t> dimensions;
	iObject->GetParams(N, params, dimensions);

	std::ofstream f(iPath);

	Vector3D vector;

	for (std::size_t i = 0; i < params.size(); ++i)
	{
		vector = iObject->Evaluate(params[i]);
		f << vector[0]<< "," << vector[1] << "," << vector[2] << std::endl;
	}

	f.close();
}