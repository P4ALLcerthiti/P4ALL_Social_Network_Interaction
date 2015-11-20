#ifndef _DYNAMIC_AD_FEATURE_GROUP_H_
#define _DYNAMIC_AD_FEATURE_GROUP_H_

#include <string>
#include <vector>

class DynamicADFeatureEntry;

class DynamicADFeatureGroup
{
public:
	DynamicADFeatureGroup(const std::string& strName, const std::string& strCommonAttribute);
	~DynamicADFeatureGroup();

	const std::string& GetName() {return m_strName;}
	const std::string& GetCommonAttribute() {return m_strCommonAttribute;}

	void AddEntry(DynamicADFeatureEntry* entry) {m_vEntry.push_back(entry);}
	unsigned int GetNumEntries() {return m_vEntry.size();}

private:
	std::string m_strName;
	std::string m_strCommonAttribute;
	std::vector <DynamicADFeatureEntry*> m_vEntry;
};

#endif