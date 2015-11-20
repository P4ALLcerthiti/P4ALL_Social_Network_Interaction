#include "DynamicADFeatureEntry.h"

#include <sstream>
#include <algorithm>

DynamicADFeatureEntry::DynamicADFeatureEntry(const std::string& strKeyData, const std::string& strFeatureData, unsigned int iDiscardWordsWithSizeLessThan, bool bSplitData, const char chDelimiter/*=','*/)
{
	m_strKeyData = strKeyData;
	m_strRawFeatureData = strFeatureData;

	//clean the data:
	m_strKeyData.erase(std::remove(m_strKeyData.begin(), m_strKeyData.end(), '\"'), m_strKeyData.end());
	m_strRawFeatureData.erase(std::remove(m_strRawFeatureData.begin(), m_strRawFeatureData.end(), '\"'), m_strRawFeatureData.end());
	
	//save the feature data to a multimap:
	std::pair<std::string, std::string> pair;
	pair.first = m_strKeyData;
	if (bSplitData)
	{
		std::stringstream ss;
		ss << m_strRawFeatureData;
		char buffer[1024];
		while (!ss.eof())
		{
			ss.getline(buffer, 1024, chDelimiter);
			if (strcmp(buffer, ""))
			{
				pair.second = buffer;

				if (pair.second.size() >= iDiscardWordsWithSizeLessThan)
				{
					m_strFeatureData=pair.second;
					m_mFeatureData.insert(pair);
					m_setData.insert(pair.second);
				}
			}
		}
	}
	else
	{
		pair.second = m_strRawFeatureData;

		if (pair.second.size() >= iDiscardWordsWithSizeLessThan) 
		{
			m_mFeatureData.insert(pair);
			m_setData.insert(pair.second);
		}
	}

	m_timeEntry = NULL;
}

DynamicADFeatureEntry::~DynamicADFeatureEntry()
{

}

unsigned int DynamicADFeatureEntry::GetFeatureDataCount(const std::string& strData)
{
	return m_setData.count(strData);
}