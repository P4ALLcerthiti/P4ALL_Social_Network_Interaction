#ifndef VSGRAPH_H_
#define VSGRAPH_H_
#include <vector>
//Forward declarations
class vsMatrix;

class vsClusteringResult{
public:
	vsClusteringResult(bool SL = false, bool DG = false, bool W = false, float I = 0, float T = 0, int N = 0, int NoF = 0, float *FW = NULL, float AT = 0){
		this->addSelfLoops = SL;
		this->downgrade = DG;
		this->weighted = W;
		this->Inflation = I;
		this->Threshold = T;
		this->ApproximationThreshold = AT;

		this->NoVertices = N;
		this->Vertex = (int*) malloc(N * sizeof(int));
		for (int i = 0; i < this->NoVertices; ++i)
			this->Vertex[i] = 0;

		this->NoFeatures = NoF;
		this->featureWeight = (float*) malloc(NoF * sizeof(float));
		for (int i = 0; i < this->NoFeatures; ++i)
			this->featureWeight[i] = FW[i];

		this->NoClusters = 0;

		this->Type = StochasticFlows;
	};
	~vsClusteringResult(){
		free(this->Vertex);
		free(this->featureWeight);
	}

	bool addSelfLoops;
	bool downgrade;
	bool weighted;
	float Inflation;
	float Threshold;
	float ApproximationThreshold;

	int *Vertex;
	int NoVertices;

	float *featureWeight;
	int NoFeatures;

	int NoClusters;

	enum type{StochasticFlows, DominantSets, Other};
	type Type;
};
class vsFeature;
class vsNode;
class vsEdge;
class vsCSVFile;


class vsGraph{
public:
	vsGraph();
	~vsGraph();

	int getNoVertices();
	int getNoVertices(int feature);

	int getNoFeatures();
	vsFeature* addFeature(char* label);
	int searchFeature(char* label);
	vsFeature* getFeature(char* label);
	char* getFeatureLabel(int i);

	int searchVertex(char* label, char *featurelabel);
	int searchVertex(vsNode* node);
	vsNode* getVertex(char* label);
	vsNode* addVertex(char* featureLabel, char* label);
	vsNode* addVertex(vsFeature* currentFeature, char* label);

	vsEdge* getEdge(vsNode* source, vsNode* target);
	void addEdge(char*, char*, char*, char*);
	void addEdge(vsNode*, vsNode*);

	void sortGraph(vsGraph*);

	int addCluster(char* label);
	void assignVertexToCluster(int node, int cid);

	int translateLocalIDtoGlobal(int fid, int id);

	bool createBipartiteFromCSVFile(vsCSVFile* f);
	bool importClusteringReasultsFromCSVFile(vsCSVFile* f);
	void createGraphFromCluster(vsGraph *Graph, int ClusterID);
	void createGraphFromSelectedVertices(vsGraph *Graph);
	void createGraphFromSelectedAbstractVertices(vsGraph *Graph);
	void createFullMatrix(bool addSelfLoops, bool downgrade, bool weighted);
	void transferWeightValuesToFullMatrix(bool addSelfLoops);
	vsMatrix* transferWeightValuesToSparseMatrix(bool addSelfLoops);
	vsMatrix* transferWeightValuesToSparseMatrix2(bool addSelfLoops);

	void Stochastic_Flows_Algorithm(bool addSelfLoops, bool downgrade, bool weighted, float Inflation, float Threshold, float ApproximationThreshold, bool speedup = false);
	vsClusteringResult* createClusteringResult(bool addSelfLoops, bool downgrade, bool weighted, float Inflation, float Threshold, int NoVert, int NoF, float *FW, float AT);
	vsClusteringResult* checkExistenceOfClusteringResult(bool SL, bool DG, bool W, float I, float T, int N, int NoF, float *FW, float AT);
	void calculateClusters(vsMatrix *M);
	void printClusters();
	void setClustership();

	void Static_AD_Algorithm(bool addSelfLoops, bool downgrade, bool weighted);
	float Pearson(float *Matrix, int Rows, int a, int b, int BottomLimit, int UpperLimit);
	void computeNormalityScores();

	void printSelf();

	void createAbstractGraph();
	void createnormalgraph_fromabstract();
	int findCommonVertices(int f1, int f2);

	std::vector<vsFeature*> Feature;
	float* FeatureWeight;
	int NoFeatures;

	std::vector<vsEdge*> Edge;
	int NoEdges;

	std::vector<vsNode*> Vertex;
	int NoVertices;

	vsGraph** Cluster;
	int NoClusters;
	
	vsClusteringResult **ClusteringResult;
	vsClusteringResult *currentCR;
	int NoClusteringResults;

	float* NormalityScore;

	vsGraph *abstractGraph;
	
protected:
	float* FullMatrix;
	float* SimilarityMatrix;
	float* NormalityMatrix;
};
#endif

