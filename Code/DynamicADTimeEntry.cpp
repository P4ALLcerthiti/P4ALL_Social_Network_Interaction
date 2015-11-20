#include "DynamicADTimeEntry.h"

#include <algorithm>

const std::string DynamicADTimeEntry::s_vStrTimeType[] =
{
	"Day",
	"Hour"
};

DynamicADTimeEntry::DynamicADTimeEntry(const DynamicADTimeEntry::TimeType &timeType)
{
	m_timeType = timeType;
	m_timeGroup = NULL;
}

DynamicADTimeEntry::~DynamicADTimeEntry()
{

}

void DynamicADTimeEntry::FixDataFormat()
{
	//remove the quotes:
	m_strKeyData.erase(remove(m_strKeyData.begin(), m_strKeyData.end(), '\"'), m_strKeyData.end());
	m_strTimeData.erase(remove(m_strTimeData.begin(), m_strTimeData.end(), '\"'), m_strTimeData.end());

	m_strKeyDataInv = m_strKeyData;
	std::reverse(m_strKeyDataInv.begin(), m_strKeyDataInv.end());
}