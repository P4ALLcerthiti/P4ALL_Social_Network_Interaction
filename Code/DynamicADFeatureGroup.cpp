#include "DynamicADFeatureGroup.h"
#include "DynamicADFeatureEntry.h"

DynamicADFeatureGroup::DynamicADFeatureGroup(const std::string& strName, const std::string& strCommonAttribute)
{
	m_strName = strName;
	m_strCommonAttribute = strCommonAttribute;
}

DynamicADFeatureGroup::~DynamicADFeatureGroup()
{

}