#ifndef _DYNAMIC_AD_FEATURE_ENTRY_H_
#define _DYNAMIC_AD_FEATURE_ENTRY_H_

#include <string>
#include <vector>
#include <map>
#include <set>

class DynamicADTimeEntry;

class DynamicADFeatureEntry
{
public:
	DynamicADFeatureEntry(const std::string& strKeyData, const std::string& strFeatureData, unsigned int iDiscardWordsWithSizeLessThan, bool bSplitData, const char chDelimiter=',');
	~DynamicADFeatureEntry();

	const std::string& GetKeyData() {return m_strKeyData;}

	std::multimap<std::string, std::string>& GetFeatureData() {return m_mFeatureData;}
	unsigned int GetFeatureDataCount(const std::string& strData);
	
	void SetTimeEntry(DynamicADTimeEntry* timeEntry) {m_timeEntry = timeEntry;}
	DynamicADTimeEntry* GetTimeEntry() {return m_timeEntry;}
	std::string GetFeatureDataSingle(){return m_strFeatureData;}

private:
	std::string m_strKeyData;
	std::string m_strRawFeatureData;
	//den xreiazetai multimap efoson exei mia grammi me dio stoixeia
	std::multimap<std::string, std::string> m_mFeatureData;
	std::multiset<std::string> m_setData;
	std::string m_strFeatureData;
	DynamicADTimeEntry* m_timeEntry;
};

#endif