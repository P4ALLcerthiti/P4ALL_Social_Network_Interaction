#ifndef _DYNAMIC_AD_FEATURE_H_
#define _DYNAMIC_AD_FEATURE_H_

#include <string>
#include <vector>
#include <set>
#include <map>

class DynamicADFeatureEntry;

class DynamicADFeature
{
public:
	DynamicADFeature();
	~DynamicADFeature();

	void SetName(const std::string& strName) {m_strName = strName;}
	const std::string& GetName() {return m_strName;}

	bool LoadCSV(const std::string& strFilename, unsigned int iDiscardWordsWithSizeLessThan, bool bSplitFeatureData, char chSplitDelimiter=',');
	const std::string& GetFileName() {return m_strFileName;}
	
	DynamicADFeatureEntry* GetEntry(const unsigned int& i) {return m_vEntry[i];}
	unsigned int GetNumEntries() {return m_vEntry.size();}
	std::vector <DynamicADFeatureEntry*>* Get_m_vEntry(){return &m_vEntry;}
	//post process:
	std::map<std::string, unsigned int>& GetTimesPerDataWord() {return m_mapId2Num;}
	std::set<std::string>& GetIdSet() {return m_idSet;}
	void PrintIdSetToFile(const std::string& filename);

private:
	void createIdSet();

private:
	std::string m_strName;
	std::string m_strFileName;
	std::vector <DynamicADFeatureEntry*> m_vEntry;

	std::set <std::string> m_idSet;
	std::map <std::string, unsigned int> m_mapId2Num; //distribution of unique IDs
};

#endif