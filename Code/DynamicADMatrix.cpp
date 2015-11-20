#include "DynamicADMatrix.h"

#include <Eigen\Eigen>

DynamicADMatrix::DynamicADMatrix(const std::string& strName)
{
	m_strName = strName;
}

DynamicADMatrix::~DynamicADMatrix()
{
}