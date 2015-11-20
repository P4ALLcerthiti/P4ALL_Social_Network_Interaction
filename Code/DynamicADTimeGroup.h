#ifndef _DYNAMIC_AD_TIME_GROUP_H_
#define _DYNAMIC_AD_TIME_GROUP_H_

#include <vector>

class DynamicADTimeEntry;

class DynamicADTimeGroup
{
public:
	DynamicADTimeGroup(const std::string& strCommonTimeData);
	~DynamicADTimeGroup();

	const std::string& GetCommonTimeData() {return m_strCommonTimeData;}

	void AddEntry(DynamicADTimeEntry* timeEntry);
	DynamicADTimeEntry* GetEntry(const unsigned int& i) {return m_vEntry[i];}
	unsigned int GetNumEntries() {return m_vEntry.size();}

	void SetIdx(const unsigned int& idx) {m_idx = idx;}
	const unsigned int& GetIdx() {return m_idx;}

private:
	std::string m_strCommonTimeData; //this is commont for all the entries
	std::vector <DynamicADTimeEntry*> m_vEntry;
	unsigned int m_idx;
};

#endif