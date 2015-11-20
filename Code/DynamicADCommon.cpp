#include "DynamicADCommon.h"

#include <sstream>
#include <fstream>

void DynamicADCommon::WriteMatrixToFile(const Eigen::MatrixXd& M, const std::string& strFile)
{
	std::ofstream ofs(strFile.c_str());
	int i,j;
	for (i=0; i<M.rows(); i++)
	{
		for (j=0; j<M.cols(); j++)
		{
			ofs << M.coeff(i,j) << (j+1 == M.cols()?'\n':'\t');
		}
	}
	ofs.close();
}