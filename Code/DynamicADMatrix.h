#ifndef _DYNAMIC_AD_MATRIX_H_
#define _DYNAMIC_AD_MATRIX_H_

#include <string>

class DynamicADMatrix
{
public:
	DynamicADMatrix(const std::string& strName);
	~DynamicADMatrix();

	const std::string& GetName() {return m_strName;}


private:
	std::string m_strName;
};

#endif