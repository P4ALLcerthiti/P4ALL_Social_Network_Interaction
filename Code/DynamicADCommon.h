#ifndef _DYNAMIC_AD_COMMON_H_
#define _DYNAMIC_AD_COMMON_H_

#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <string>

class DynamicADCommon
{
public:
	static void WriteMatrixToFile(const Eigen::MatrixXd& M, const std::string& strFile);
};

#endif