#ifndef _DYNAMIC_AD_GRAPH_H_
#define _DYNAMIC_AD_GRAPH_H_

#include <vector>
#include <string>
#include <utility>
#include <set>
#include <map>
#include <Eigen/SparseCore>

class DynamicADFeatureEntry;

class DynamicADGraph
{
public:
	static enum DistanceType {
		DistanceType_L1, //abs(diff) - weighted
		DistanceType_L2, //diff^2 - weighted 
		DistanceType_EUCLIDEAN, //sqrt(sum(diff^2))) - weighted 
		DistanceType_NodeOperations, //node additions+subtractions - unweighted
		DistanceType_EdgeOperations, //edge additions+subtractions - unweighted
		DistanceType_GraphOperations, //node+edge additions+substraction - unweighted
		NUM_DISTANCE_TYPES
	};

	static const std::string s_DistanceType2Str[];

public:
	DynamicADGraph(const std::string& strName, const unsigned int& numFeatures);
	~DynamicADGraph();

	const std::string& GetName() {return m_strName;}

	void AddFeatureEntry(DynamicADFeatureEntry* entry, const unsigned int& idxFeature);
	void CreateGraph(const std::map<std::string, int>& idMap, const std::vector< std::set<std::string> > &vIdSet);

	const unsigned int& GetNumNodes() {return m_numNodes;}
	const unsigned int& GetNumEdges() {return m_numEdges;}
	std::set<std::string> * Get_m_vnode(){return m_vNode;}
	std::string Get_mstrname(){return m_strName;}
	int Get_numfeatures(){return m_numFeatures;}
	//distance:
	double GetDistanceToGraph(DynamicADGraph* graph, const DistanceType& distanceType);
	Eigen::SparseMatrix<double> m_matrix;
	std::vector<DynamicADFeatureEntry*> *GetFeatureEntry(){return m_vFeatureEntry;}
private:
	unsigned int getNumNodeOperations(DynamicADGraph* graph);
	unsigned int getNumEdgeOperations(DynamicADGraph* graph);

private:
	std::string m_strName;

	//feature & entries
	unsigned int m_numFeatures;
	std::vector<DynamicADFeatureEntry*> *m_vFeatureEntry;

	//nodes
	std::set<std::string> *m_vNode; //unique entry names

	unsigned int m_numNodes;
	unsigned int m_numEdges;
	int nodeoperations,edgeoperations;

	//sparse matrix:
	
};

#endif