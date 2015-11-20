#include "DynamicADTimeGroup.h"
#include "DynamicADTimeEntry.h"

DynamicADTimeGroup::DynamicADTimeGroup(const std::string& strCommonTimeData)
{
	m_strCommonTimeData = strCommonTimeData;
	m_idx = -1;
}
	
DynamicADTimeGroup::~DynamicADTimeGroup()
{

}

void DynamicADTimeGroup::AddEntry(DynamicADTimeEntry* timeEntry)
{
	m_vEntry.push_back(timeEntry);
	timeEntry->SetTimeGroup(this);
	//m_mEntry->insert(pair<char*,DynamicADTimeEntry*> (timeEntry->))
}