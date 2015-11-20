#include "DynamicADResult.h"

#include "DynamicAD.h"
#include "DynamicADFeature.h"

DynamicADResult::DynamicADResult(DynamicAD* dynamicADExperiment)
{
	m_strTimeData.first = dynamicADExperiment->GetTimeDataName();
	m_strTimeData.second = dynamicADExperiment->GetTimeDataFilename();
	
	for (unsigned int i=0; i<dynamicADExperiment->GetNumFeatures(); i++)
	{
		std::pair<std::string, std::string> newPair;
		newPair.first = dynamicADExperiment->GetFeature(i)->GetName();
		newPair.second = dynamicADExperiment->GetFeature(i)->GetFileName();
		m_vStrFeatureData.push_back(newPair);
	}
}

DynamicADResult::~DynamicADResult()
{

}