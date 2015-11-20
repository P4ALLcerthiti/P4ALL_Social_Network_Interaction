#include "DynamicADFeature.h"
#include "DynamicADFeatureEntry.h"

#include <fstream>
#include <iostream>
#include <sstream>

DynamicADFeature::DynamicADFeature()
{
	m_strName = "";
	m_strFileName = "";
}

DynamicADFeature::~DynamicADFeature()
{
	for (unsigned int i=0; i<m_vEntry.size(); i++) delete m_vEntry[i];
}

bool DynamicADFeature::LoadCSV(const std::string& strFilename, unsigned int iDiscardWordsWithSizeLessThan, bool bSplitFeatureData, char chSplitDelimiter)
{
	std::cout << "Parsing feature file: " << strFilename << "...\n";

	m_strFileName = strFilename;

	std::ifstream ifs(strFilename.c_str());

	if (!ifs.is_open() || !ifs.good()) return false;
	char buffer1[1024];
	char buffer2[1024];

	while (!ifs.eof())
	{
		ifs.getline(buffer1, 1024);
		if (strcmp(buffer1, ""))
		{
			std::stringstream ss(buffer1);
			ss.getline(buffer1, 1024, ',');
			ss.getline(buffer2, 1024, '\n');
			if(strcmp(buffer1,"153957")==0)
			{
				int a=0;
				a++;
			}
			m_vEntry.push_back(new DynamicADFeatureEntry(buffer1, (m_strName+buffer2).c_str(), iDiscardWordsWithSizeLessThan, bSplitFeatureData, chSplitDelimiter));
		}
	}
	ifs.close();
	std::cout << "Feature entries: " << m_vEntry.size() << "\n";

	createIdSet();
	
	return true;	
}

void DynamicADFeature::PrintIdSetToFile(const std::string& filename)
{
	std::ofstream ofs(filename.c_str());
	std::set<std::string>::iterator it;
	for (it=m_idSet.begin(); it!=m_idSet.end(); it++)
	{
		ofs << *it << '\n';
	}
	ofs.close();
}

/*
DynamicADFeature* DynamicADFeature::CreateNewBySplittingWords(const bool& bIgnoreSmallWords, const bool& bIgnoreEntriesWithoutTime)
{
	std::cout << "Creating new feature data by splitting words...\n";

	std::string strNewName = m_strName + "_splitWords";
	DynamicADFeature* newFeature = new DynamicADFeature(strNewName);

	unsigned int i,j;
//	#pragma omp parallel for
	for (i=0; i<m_vEntry.size(); i++)
	{
		if (bIgnoreEntriesWithoutTime && !m_vEntry[i]->GetTimeEntry()) continue;
		
		std::vector<DynamicADFeatureEntry*> vNewEntry = m_vEntry[i]->SplitFeatureData(',');
		for (j=0; j<vNewEntry.size(); j++)
		{
			if (!bIgnoreSmallWords || vNewEntry[j]->GetFeatureData().size() > 2)
			{
				newFeature->m_vEntry.push_back(vNewEntry[j]);
			}
		}
	}

	std::cout << "Number of new feature data entries: " << newFeature->m_vEntry.size() << "\n";

	newFeature->createIdSet();

	return newFeature;
}
*/

void DynamicADFeature::createIdSet()
{
	//we put data in m_mapId2Num<string,int>.in the first field we have the name of every feature and in the second 
	//how many times we found it in csv
	m_idSet.clear();
	std::multimap <std::string,std::string>::iterator it;

	for (unsigned int i=0; i<m_vEntry.size(); i++)
	{
		for (it=m_vEntry[i]->GetFeatureData().begin(); it!=m_vEntry[i]->GetFeatureData().end(); it++)
		{
			m_idSet.insert(it->second);
		}
	}
	std::cout << "Feature unique labels: " << m_idSet.size() << "\n";

	//compute distribution of unique labels:
	m_mapId2Num.clear();
	std::set<std::string>::iterator it2;
	std::map<std::string, unsigned int>::iterator it3(m_mapId2Num.end());
	for (it2=m_idSet.begin(); it2!=m_idSet.end(); it2++)
	{
		it3 = m_mapId2Num.insert(it3, std::pair<std::string, unsigned int>(*it2, 0));
	}
    //edo mporei na glitosoume ena for an valoume to deytero for sto kato diplo blog mesa sto pano mono blog for
	//p.x.
	/*std::vector<DynamicADFeatureEntry*>::iterator entryIt;
	  for (int i=0; i<m_idSet.size(); i++)
	  {
		int second;
		for (entryIt=m_vEntry.begin(); entryIt!=m_vEntry.end(); entryIt++)
		{
			second += (*entryIt)->GetFeatureDataCount(m_idSet[i]);
		}
		it3 = m_mapId2Num.insert(it3, std::pair<std::string, unsigned int>(m_idSet[i], second));
	  }
	*/


	std::vector<DynamicADFeatureEntry*>::iterator entryIt;
	for (it3=m_mapId2Num.begin(); it3!=m_mapId2Num.end(); it3++)
	{
		for (entryIt=m_vEntry.begin(); entryIt!=m_vEntry.end(); entryIt++)
		{
			(*it3).second += (*entryIt)->GetFeatureDataCount(it3->first);
		}
	}
}
