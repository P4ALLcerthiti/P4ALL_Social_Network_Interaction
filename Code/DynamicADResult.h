#ifndef _DYNAMIC_AD_RESULT_H_
#define _DYNAMIC_AD_RESULT_H_

#include "DynamicADGraph.h"

class DynamicAD;
class DynamicADFeature;

class DynamicADResult
{
public:
	DynamicADResult(DynamicAD* dynamicADExperiment);
	~DynamicADResult();

	void SetDistanceType(const DynamicADGraph::DistanceType& distType) {m_distanceType = distType;}
	const DynamicADGraph::DistanceType& GetDistanceType() {return m_distanceType;}

	void SetMatD(const Eigen::MatrixXd& D) {m_matD = D;}
	void SetMatP(const Eigen::MatrixXd& P) {m_matP = P;}
	const Eigen::MatrixXd& GetMatD() {return m_matD;}
	const Eigen::MatrixXd& GetMatP() {return m_matP;}

	const std::string& GetNameTimeData() {return m_strTimeData.first;}
	const std::string& GetFilenameTimeData() {return m_strTimeData.second;}

	const std::string& GetNameFeatureData(const unsigned int& i) {return m_vStrFeatureData[i].first;}
	const std::string& GetFilenameFeatureData(const unsigned int& i) {return m_vStrFeatureData[i].second;}
	unsigned int GetNumFeatures() {return m_vStrFeatureData.size();}
	
private:
	std::pair <std::string, std::string> m_strTimeData;
	std::vector <std::pair <std::string, std::string> > m_vStrFeatureData;

	DynamicADGraph::DistanceType m_distanceType;
	Eigen::MatrixXd m_matD; //symmetric distance matrix
	Eigen::MatrixXd m_matP; 

};

#endif