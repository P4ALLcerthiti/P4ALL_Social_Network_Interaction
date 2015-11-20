#ifndef _DYNAMIC_AD_H_
#define _DYNAMIC_AD_H_

#include "DynamicADTimeEntry.h"
#include "DynamicADTimeGroup.h"
#include "DynamicADFeature.h"
#include "DynamicADFeatureEntry.h"
#include "DynamicADGraph.h"
#include <utility>
#include <set>
#include <map>
using namespace std;
class DynamicADResult;

class DynamicAD
{
public:
	DynamicAD(const std::string& strName);
	~DynamicAD();

	//for time:
	bool LoadTimeCSV(const std::string& strFilename, const std::string& strName,int num_of_semantic_characters,string time_type="",string dataset_name="",string timeperiod="");
	DynamicADTimeEntry* GetTimeEntry(const unsigned int& idx) {return m_vTimeEntry[idx];}
	unsigned int GetNumTimeEntries() {return m_vTimeEntry.size();}
	void PrintTimeSetToFile(const std::string& strFile);
	unsigned int GetNumTimeGroups() {return m_vTimeGroup.size();}
	DynamicADTimeGroup* GetTimeGroup(const std::string& strTime);
	DynamicADTimeGroup* GetTimeGroup(int i){return m_vTimeGroup[i];}
	std::vector< std::pair<std::string, unsigned int> > GetDataDistributionPerTime();
	const std::string& GetTimeDataFilename() {return m_strTimeDataFile;}
	const std::string& GetTimeDataName() {return m_strTimeDataName;}

	//for features:
	void LoadFeatureCSV(const std::string& strFilename, const std::string& strName, unsigned int iDiscardWordsWithSizeLessThan, bool bSplitData, char chDelimiter=',');
	void RemoveFeatureCSV(DynamicADFeature* feature);
	DynamicADFeature* GetFeature(const unsigned int& i) {return m_vFeature[i];}
	DynamicADFeature* GetFeature(const std::string& strName);
	unsigned int GetNumFeatures() {return m_vFeature.size();}

	//for graphs:
	void CreateGraphs();
	void PerformMDS(const DynamicADGraph::DistanceType& distanceType, std::string strDir="");	

	//for results:
	DynamicADResult* GetResult(const unsigned int& idx) {return m_vResult[idx];}
	unsigned int GetNumResults() {return m_vResult.size();}
	void m_vResult_clear(){ m_vResult.clear();}
	std::vector <DynamicADGraph*>& GetGraph(){return m_vGraph;}
	static bool sortmvtimegroup (DynamicADTimeGroup* i,DynamicADTimeGroup* j);
	static bool sortmvtimeentry (DynamicADTimeEntry* i,DynamicADTimeEntry* j);
private:
	void distributeDataIntoTimeGroups();
	void clearAll();

private:
	std::string m_strName;
	std::vector <DynamicADGraph*> m_vGraph;
	//time attributes:
	std::string m_strTimeDataFile;
	std::string m_strTimeDataName;
	std::vector<DynamicADTimeEntry*> m_vTimeEntry; //size of all entries of the time 
	std::map<std::string, DynamicADTimeEntry*> m_mapTimeEntry;
	std::vector<DynamicADTimeGroup*> m_vTimeGroup; //size equals to the number of times instances
	
	//feature attributes:
	std::vector <DynamicADFeature*> m_vFeature;

	//graphs:
	

	std::vector <DynamicADResult*> m_vResult;
};


#endif