//C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <omp.h>

//VIS-SENSE includes
#include "vsGraph.h"
#include "vsFeature.h"
#include "vsNode.h"
#include "vsCSVFile.h"
#include "vsSparseMatrix.h"

#include "fstream"
vsGraph::vsGraph(){

	this->NoFeatures = 0;
	//this->Feature = NULL;
	this->FeatureWeight = NULL;

	this->NoVertices = 0;
	//this->Vertex = NULL;

	this->NoEdges = 0;
	//this->Edge = NULL;

	this->Cluster = NULL;
	this->NoClusters = 0;

	this->ClusteringResult = NULL;
	this->NoClusteringResults = 0;
	this->currentCR = NULL;

	this->FullMatrix = NULL;
	this->SimilarityMatrix = NULL;
	this->NormalityMatrix = NULL;
	this->NormalityScore = NULL;

	this->abstractGraph = NULL;
}

vsGraph::~vsGraph(){
	for (int i = 0; i < this->NoFeatures; i++)
		delete this->Feature[i];
	//free(this->Feature);
	this->Feature.clear();
	free(this->FeatureWeight);
	//this->Feature = NULL;
	
	this->FeatureWeight = NULL;

	//free(this->Vertex);
	//this->Vertex = NULL;
	this->Vertex.clear();
	//free(this->Edge);
	//this->Edge = NULL;
	this->Edge.clear();
	for (int i = 0; i < this->NoClusters; i++)
		delete this->Cluster[i];
	free(this->Cluster);
	this->Cluster = NULL;

	free(this->ClusteringResult);
	this->ClusteringResult = NULL;

	free(this->FullMatrix);
	free(this->SimilarityMatrix);
	free(this->NormalityMatrix);
	free(this->NormalityScore);
	this->FullMatrix = NULL;
	this->SimilarityMatrix = NULL;
	this->NormalityMatrix = NULL;
	this->NormalityScore = NULL;

	delete this->abstractGraph;
}

int vsGraph::getNoVertices(){
	return this->NoVertices;
}

int vsGraph::getNoVertices(int feature){
	return this->Feature[feature]->NoVertices;
}

int vsGraph::searchFeature(char* label){
	int i;

	for (i = 0; i < this->NoFeatures; i++){
		if (strcmp(this->Feature[i]->getLabel(), label) == 0)
			break;
	}

	return i;
}

vsFeature* vsGraph::getFeature(char* label){
	for (int i = 0; i < this->NoFeatures; i++){
		if (strcmp(this->Feature[i]->getLabel(), label) == 0)
			return this->Feature[i];
	}

	return NULL;
}

char* vsGraph::getFeatureLabel(int i){
	return Feature[i]->getLabel();
}

int vsGraph::getNoFeatures(){
	return this->NoFeatures;
}

vsFeature* vsGraph::addFeature(char* label){

	int featureID = this->searchFeature(label);

	if (featureID == this->NoFeatures){
		/*this->Feature = (vsFeature**) realloc(this->Feature, (featureID + 1) * sizeof(vsFeature*));
		if (!this->Feature){
			fprintf (stderr, "Cannot reallocate memory for array Feature\n");
			exit(1);
		}

		this->Feature[featureID] = new vsFeature(featureID, label);*/
		this->Feature.push_back(new vsFeature(featureID, label));
		this->NoFeatures++;

		this->FeatureWeight = (float*) realloc(this->FeatureWeight, this->NoFeatures * sizeof(float));
		for (int i = 0; i < this->NoFeatures; i++)
			this->FeatureWeight[i] = 1.0f / (float)(this->NoFeatures - 1);
	}

	return this->Feature[featureID];
}

int vsGraph::searchVertex(char *label, char *featurelabel){
	int i;
    
	for (i = 0; i < this->NoVertices; i++){
		if ((strcmp(this->Vertex[i]->getLabel(), label) == 0) && (strcmp(this->Vertex[i]->Feature->getLabel(), featurelabel) == 0))
			break;
	}   
            
	return i;
}

int vsGraph::searchVertex(vsNode* node){
	int i;
    
	for (i = 0; i < this->NoVertices; i++){
		if (this->Vertex[i] == node)
			break;
	}   
            
	return i;
}

vsNode* vsGraph::addVertex(char* featureLabel, char* label){

	bool newVertexAdded = false;

	vsFeature* currentFeature = this->addFeature(featureLabel);
	vsNode* currentVertex = currentFeature->addVertex(label, newVertexAdded);
 
	//int vertexID = this->searchVertex(label, featureLabel);
	//int vertexID = this->searchVertex(currentVertex);

	//if (vertexID == this->NoVertices){
	if (newVertexAdded){
		/*this->Vertex = (vsNode**) realloc(this->Vertex, (this->NoVertices + 1) * sizeof(vsNode*));
		if (!this->Vertex){
			fprintf (stderr, "Cannot reallocate memory for array Vertex\n");
			exit(1);
		}

		this->Vertex[this->NoVertices] = currentVertex;
		*/
		this->NoVertices++;
		this->Vertex.push_back(currentVertex);

	}

	return currentVertex;
}

vsNode* vsGraph::addVertex(vsFeature* currentFeature, char* label){

	bool newVertexAdded = false;

	vsNode* currentVertex = currentFeature->addVertex(label, newVertexAdded);

	if (newVertexAdded){
		/*this->Vertex = (vsNode**) realloc(this->Vertex, (this->NoVertices + 1) * sizeof(vsNode*));
		if (!this->Vertex){
			fprintf (stderr, "Cannot reallocate memory for array Vertex\n");
			exit(1);
		}
		this->Vertex[this->NoVertices] = currentVertex;*/
		this->NoVertices++;
		this->Vertex.push_back(currentVertex);
		
	}

	return currentVertex;
}

vsEdge* vsGraph::getEdge(vsNode* source, vsNode* target){

	for (int i = 0; i < this->NoEdges; i++)
		if ((this->Edge[i]->getSource() == source) && (this->Edge[i]->getTarget() == target))
			return this->Edge[i];

	return NULL;
}

void vsGraph::addEdge(char* feature1Label, char* sourceLabel, char* feature2Label, char* targetLabel){

	vsFeature* Feature1 = this->addFeature(feature1Label);
	vsFeature* Feature2 = this->addFeature(feature2Label);

	vsNode* source = this->addVertex(Feature1, sourceLabel);
	vsNode* target = this->addVertex(Feature2, targetLabel);

	//vsNode* source = this->addVertex(feature1Label, sourceLabel);
	//vsNode* target = this->addVertex(feature2Label, targetLabel);

	vsEdge* edge1 = source->addEdge(target);
	vsEdge* edge2 = target->addEdge(source);

	/*this->Edge = (vsEdge**) realloc(this->Edge, (this->NoEdges + 2) * sizeof(vsEdge*));
	if (!this->Edge){
		fprintf (stderr, "Cannot reallocate memory for array Edge\n");
		exit(1);
	}

	this->Edge[this->NoEdges] = edge1;
	this->Edge[this->NoEdges + 1] = edge2;*/
	
	this->Edge.push_back(edge1);
	this->Edge.push_back(edge2);

	this->NoEdges += 2;
}

void vsGraph::addEdge(vsNode* source, vsNode* target){

	vsEdge* edge1 = source->addEdge(target);
	vsEdge* edge2 = target->addEdge(source);

	/*this->Edge = (vsEdge**) realloc(this->Edge, (this->NoEdges + 2) * sizeof(vsEdge*));
	if (!this->Edge){
		fprintf (stderr, "Cannot reallocate memory for array Edge\n");
		exit(1);
	}

	this->Edge[this->NoEdges] = edge1;
	this->Edge[this->NoEdges + 1] = edge2;*/
	this->Edge.push_back(edge1);
	this->Edge.push_back(edge2);

	this->NoEdges += 2;

	
}

int vsGraph::addCluster(char* label){

	int i;
    
	for (i = 0; i < this->NoClusters; i++){
		if (strcmp(this->Cluster[i]->Feature[0]->getLabel(), label) == 0)
			break;
	}   
            
	//Add node if not found, do not change the value of k
	if ( i == this->NoClusters){

		this->Cluster = (vsGraph**) realloc(this->Cluster, (i + 1) * sizeof(vsGraph*));
		if ( !this->Cluster ){
			fprintf (stderr, "Cannot allocate memory for array Cluster\n");
			exit(1);
		}

		this->Cluster[i] = new vsGraph();
		this->Cluster[i]->addFeature(label);
		this->NoClusters++;
	}

	return i;
}

void vsGraph::assignVertexToCluster(int node, int cid){

	char n[10], c[10];
	sprintf(n, "%d", node);
	sprintf(c, "%d", cid);

	int clusterID = this->addCluster(c);
	//this->Cluster[clusterID]->addEdge(c, n, c, c);
	this->Cluster[clusterID]->addVertex(c, n);
}


int compareEdgeTargets(const void* a, const void* b){
	if ( (*(vsEdge**)a)->getTarget()->Feature->getID() != (*(vsEdge**)b)->getTarget()->Feature->getID() )
		return ( (*(vsEdge**)a)->getTarget()->Feature->getID() - (*(vsEdge**)b)->getTarget()->Feature->getID() );

	return ( (*(vsEdge**)a)->getTarget()->getLocalID() - (*(vsEdge**)b)->getTarget()->getLocalID() );
}

int compareVertices(const void* a, const void* b){

	if ((*(vsNode**)a)->Feature->getID() != (*(vsNode**)b)->Feature->getID())
		return ((*(vsNode**)a)->Feature->getID() - (*(vsNode**)b)->Feature->getID());

	return ( (*(vsNode**)a)->getLocalID() - (*(vsNode**)b)->getLocalID() );
}

int compareEdges(const void* a, const void* b){

	if ((*(vsEdge**)a)->getSource()->Feature->getID() != (*(vsEdge**)b)->getSource()->Feature->getID())
		return ((*(vsEdge**)a)->getSource()->Feature->getID() - (*(vsEdge**)b)->getSource()->Feature->getID());
	else if ((*(vsEdge**)a)->getSource()->getLocalID() != (*(vsEdge**)b)->getSource()->getLocalID())
		return ((*(vsEdge**)a)->getSource()->getLocalID() - (*(vsEdge**)b)->getSource()->getLocalID());
	else if ((*(vsEdge**)a)->getTarget()->Feature->getID() != (*(vsEdge**)b)->getTarget()->Feature->getID())
		return ((*(vsEdge**)a)->getTarget()->Feature->getID() - (*(vsEdge**)b)->getTarget()->Feature->getID());

	return ((*(vsEdge**)a)->getTarget()->getLocalID() - (*(vsEdge**)b)->getTarget()->getLocalID());
}

bool vsGraph::createBipartiteFromCSVFile(vsCSVFile* f){

	fprintf(stderr, "Creating Graph ... ");fflush(stderr);
	int NoVerticesOld = this->NoVertices;

	char Feature1Label[] = "Events";
	vsFeature* feature1 = this->addFeature(Feature1Label);
	vsFeature* feature2 = this->addFeature(f->FileName->FullName);

	int NoEvents = this->Feature[feature1->getID()]->NoVertices;
	int NoFeatureValues = this->Feature[feature2->getID()]->NoVertices;
 
	fprintf(stderr, "adding Events and Features ... ");fflush(stderr);
/*	for (int i = 0; i < f->NoLines; i++){

		vsNode* source = this->addVertex(feature1, f->Line[i]->Word[0]);
	}*/
	this->Edge.reserve(f->NoLines);

	for (int i = 0; i < f->Line.size(); i++){
		std::string feature_first_lab= f->Line[i]->Word[1];
		/*if(feature_first_lab=="null")
			continue;*/
		vsNode* source = this->addVertex(feature1, f->Line[i]->Word[0]);

		for (int j = 1; j < f->Line[i]->NoWords; j++){
			//this->addEdge(Feature1Label, f->Line[i]->Word[0], f->FileName, f->Line[i]->Word[j]);
			vsNode* target = this->addVertex(feature2, f->Line[i]->Word[j]);
			this->addEdge(source, target);
		}
	}

	fprintf(stderr, "sorting events and features ... ");fflush(stderr);
	//Sort vertices of "Events" and Feature
	int fID = this->searchFeature(Feature1Label);
	for (int i = 0; i < this->Feature[fID]->NoVertices; i++){
		qsort(&this->Feature[fID]->Vertex[i]->Edge[0], this->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
	}
	fID = this->searchFeature(f->FileName->FullName);
	for (int i = 0; i < this->Feature[fID]->NoVertices; i++){
		qsort(&this->Feature[fID]->Vertex[i]->Edge[0], this->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
	}

	
	this->sortGraph(this);

	fprintf(stderr, "OK\n");fflush(stderr);
	fprintf(stderr, "Total Number of Vertices: %d -> %d new Vertices added\n", this->NoVertices, this->NoVertices - NoVerticesOld);fflush(stderr);
	fprintf(stderr, "Number of Events: %d -> %d new Events added\n", this->Feature[feature1->getID()]->NoVertices,  this->Feature[feature1->getID()]->NoVertices - NoEvents);fflush(stderr);
	fprintf(stderr, "Number of Values for Feature %s: %d -> %d new FeatureValues added\n", feature2->getLabel(), this->Feature[feature2->getID()]->NoVertices,  this->Feature[feature2->getID()]->NoVertices - NoFeatureValues);fflush(stderr);

	return true;
}

void vsGraph::sortGraph(vsGraph *Graph){
	//Sort vertices in the big graph
	fprintf(stderr, "sorting events and features in the big graph... ");fflush(stderr);
	qsort(&Graph->Vertex[0], Graph->NoVertices, sizeof(vsNode*), compareVertices);

	//Sort edges
	qsort(&Graph->Edge[0], Graph->NoEdges, sizeof(vsEdge*), compareEdges);

	//Set Edges' Source and Target ID
	fprintf(stderr, "setting edges source and target ID ... ");fflush(stderr);

	int sourceGID, targetGID;
	vsNode* curNode; 
	vsNode* targetNode;
	for (int i = 0; i < Graph->NoVertices; i++){
		curNode = Graph->Vertex[i];

		sourceGID = Graph->translateLocalIDtoGlobal(curNode->Feature->getID(), curNode->getLocalID());
		curNode->setGlobalID(sourceGID);

		for (int j = 0; j < curNode->NoEdges; j++){
			curNode->Edge[j]->setSourceID(sourceGID);

			targetNode = curNode->Edge[j]->getTarget();
			targetGID = Graph->translateLocalIDtoGlobal(targetNode->Feature->getID(), targetNode->getLocalID());
			curNode->Edge[j]->setTargetID(targetGID);
		}
	}

}

int vsGraph::translateLocalIDtoGlobal(int fid, int id){

	int N = 0;

	for (int i = 0; i < fid; ++i){
		N += this->Feature[i]->NoVertices;
	}

	return (N + id);
}

void vsGraph::createGraphFromCluster(vsGraph *Graph, int ClusterID){

	if (ClusterID >= this->currentCR->NoClusters){
		fprintf(stderr, "\n\nGiven ClusterID greater than Number of Clusters!\n\n");fflush(stderr);
		return;
	}
	
	char Feature1[] = "Events";

	fprintf(stderr, "Creating Graph from Cluster ... ");fflush(stderr);
	fprintf(stderr, "No Vertices: %d ...  ", this->Cluster[ClusterID]->NoVertices);fflush(stderr);

	//for (int i = 0; i < this->Cluster[ClusterID]->NoVertices; i++){
	for (int i = 0; i < this->currentCR->NoVertices; i++){
		//int v = atoi(this->Cluster[ClusterID]->Vertex[i]->getLabel());

		if (this->currentCR->Vertex[i] == ClusterID + 1){
			//int v = atoi(this->Cluster[ClusterID]->Vertex[i]->getLabel());
		
			for (int j = 0; j < this->Vertex[i]->NoEdges; j++){
				vsNode *target = this->Vertex[i]->Edge[j]->getTarget();
				Graph->addEdge(Feature1, this->Vertex[i]->getLabel(), target->Feature->getLabel(), target->getLabel());
			}
		}
	}

	//Sort vertices of "Events" and Feature
	int fID = Graph->searchFeature(Feature1);
	for (int i = 0; i < Graph->Feature[fID]->NoVertices; i++){
		qsort(&Graph->Feature[fID]->Vertex[i]->Edge[0], Graph->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
	}
	for (fID = 1; fID < Graph->getNoFeatures(); ++fID){
		for (int i = 0; i < Graph->Feature[fID]->NoVertices; i++){
			qsort(&Graph->Feature[fID]->Vertex[i]->Edge[0], Graph->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
		}
	}

	this->sortGraph(Graph);

	fprintf(stderr, "OK\n");fflush(stderr);
}

void vsGraph::createGraphFromSelectedVertices(vsGraph *Graph){

	
	char Feature1[] = "Events";
    Graph->addFeature(Feature1);
	fprintf(stderr, "Creating Graph from Cluster ... ");fflush(stderr);
	
	//for (int i = 0; i < this->Cluster[ClusterID]->NoVertices; i++){
	for (int i = 0; i < this->NoVertices; i++){
		//int v = atoi(this->Cluster[ClusterID]->Vertex[i]->getLabel());

		if (this->Vertex[i]->Selected == true){
			//int v = atoi(this->Cluster[ClusterID]->Vertex[i]->getLabel());
			//Graph->addVertex(this->Vertex[i]->Feature->getLabel(),this->Vertex[i]->getLabel());
			for (int j = 0; j < this->Vertex[i]->NoEdges; j++){
				vsNode *target = this->Vertex[i]->Edge[j]->getTarget();
				//if(target->Selected)
					Graph->addEdge(this->Vertex[i]->Feature->getLabel(), this->Vertex[i]->getLabel(), target->Feature->getLabel(), target->getLabel());
			}
		}
	}

	//Sort vertices of "Events" and Feature
	int fID = Graph->searchFeature(Feature1);
	for (int i = 0; i < Graph->Feature[fID]->NoVertices; i++){
		if(Graph->Feature[fID]->Vertex[i]->NoEdges>0)
			qsort(&Graph->Feature[fID]->Vertex[i]->Edge[0], Graph->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
	}
	for (fID = 1; fID < Graph->getNoFeatures(); ++fID){
		for (int i = 0; i < Graph->Feature[fID]->NoVertices; i++){
			if(Graph->Feature[fID]->Vertex[i]->NoEdges>0)
				qsort(&Graph->Feature[fID]->Vertex[i]->Edge[0], Graph->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
		}
	}

	this->sortGraph(Graph);

	fprintf(stderr, "OK\n");fflush(stderr);
}
void vsGraph::createGraphFromSelectedAbstractVertices(vsGraph *Graph){


	char Feature1[] = "Events";
    Graph->addFeature(Feature1);
	fprintf(stderr, "Creating Graph from Cluster ... ");fflush(stderr);

	//for (int i = 0; i < this->Cluster[ClusterID]->NoVertices; i++){
	for (int i = 0; i < this->abstractGraph->NoVertices; i++){
		//int v = atoi(this->Cluster[ClusterID]->Vertex[i]->getLabel());

		if (this->abstractGraph->Vertex[i]->Selected == true){
			//int v = atoi(this->abstractGraph->Cluster[ClusterID]->Vertex[i]->getLabel());
			//Graph->addVertex(this->abstractGraph->Vertex[i]->Feature->getLabel(),this->abstractGraph->Vertex[i]->getLabel());
			for (int j = 0; j < this->abstractGraph->Vertex[i]->NoEdges; j++){
				vsNode *target = this->abstractGraph->Vertex[i]->Edge[j]->getTarget();
				//if(target->Selected)
				Graph->addEdge(this->abstractGraph->Vertex[i]->Feature->getLabel(), this->abstractGraph->Vertex[i]->getLabel(), target->Feature->getLabel(), target->getLabel());
			}
		}
	}

	//Sort vertices of "Events" and Feature
	int fID = Graph->searchFeature(Feature1);
	for (int i = 0; i < Graph->Feature[fID]->NoVertices; i++){
		if(Graph->Feature[fID]->Vertex[i]->NoEdges>0)
			qsort(&Graph->Feature[fID]->Vertex[i]->Edge[0], Graph->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
	}
	for (fID = 1; fID < Graph->getNoFeatures(); ++fID){
		for (int i = 0; i < Graph->Feature[fID]->NoVertices; i++){
			if(Graph->Feature[fID]->Vertex[i]->NoEdges>0)
				qsort(&Graph->Feature[fID]->Vertex[i]->Edge[0], Graph->Feature[fID]->Vertex[i]->NoEdges, sizeof(vsEdge*), compareEdgeTargets);
		}
	}

	this->abstractGraph->sortGraph(Graph);

	fprintf(stderr, "OK\n");fflush(stderr);
}
void vsGraph::printSelf(){

	//Print the Graph with respect to Feature fID
	int fID = 0;
	for (int i = 0; i < this->Feature[fID]->NoVertices; i++){
		fprintf(stderr, "%s", this->Feature[fID]->Vertex[i]->getLabel());fflush(stderr);

		for (int j = 0; j < this->Feature[fID]->Vertex[i]->NoEdges; j++){
			fprintf(stderr, ",%s", this->Feature[fID]->Vertex[i]->Edge[j]->getTarget()->getLabel());fflush(stderr);
			fprintf(stderr, ",%d", this->Feature[fID]->Vertex[i]->Edge[j]->getTarget()->getLocalID());fflush(stderr);
		}

		fprintf(stderr, "\n");fflush(stderr);
	}

	//Print the Graph 
	for (int i = 0; i < this->NoVertices; i++){
		fprintf(stderr, "%d:", this->Vertex[i]->getLocalID());fflush(stderr);
		fprintf(stderr, "%s\n", this->Vertex[i]->getLabel());fflush(stderr);
	}

	//Print Graph Edges
	for (int i = 0; i < this->NoEdges; i++){
		vsEdge* e = this->Edge[i];
		fprintf(stderr, "%d(%d) --> %d(%d)\n", e->getSource()->getLocalID(), e->getSource()->Feature->getID(), e->getTarget()->getLocalID(), e->getTarget()->Feature->getID());fflush(stderr);
	}
}

void vsGraph::createFullMatrix(bool /*addSelfLoops*/, bool downgrade, bool weighted){

	//This is to build the Full Matrix. If no weights
	//are taken into account the k-partite graph is
	//treated as a bipartite one.

	fprintf(stderr, "Building k-partite Full Adjacency Matrix ... ");fflush(stderr);

	float *sum = new float[this->NoFeatures];

	float TotalFeatureWeight = 0;
	for (int i = 1; i < this->NoFeatures; i++){
		TotalFeatureWeight += this->FeatureWeight[i];
	}
	for (int i = 1; i < this->NoFeatures; i++){
		if (weighted)
			this->FeatureWeight[i] /= TotalFeatureWeight;
		else
			this->FeatureWeight[i] = 1.0f;
	}

	//Compute the Values for Events
	for (int i = 0; i < this->Feature[0]->NoVertices; i++){
		for (int j = 0; j < this->NoFeatures; j++)
			sum[j] = 0;

		float weight;

		for (int j = 0; j < this->Feature[0]->Vertex[i]->NoEdges; j++){
			vsEdge* e = this->Feature[0]->Vertex[i]->Edge[j];
			if (downgrade)
				weight = e->getTarget()->NoEdges;
			else
				weight = 1.0f;
			int k = e->getTarget()->Feature->getID();
			sum[k] += e->setValue(e->getWeight() / weight);
		}

		float sumOfSums = 0;
		if (!weighted){
			for (int j = 0; j < this->NoFeatures; j++)
				sumOfSums += sum[j];
			for (int j = 0; j < this->NoFeatures; j++)
				sum[j] = sumOfSums;
		}

		float SUM = 0;
		for (int j = 0; j < this->Feature[0]->Vertex[i]->NoEdges; j++){
			vsEdge* e = this->Feature[0]->Vertex[i]->Edge[j];
			int k = e->getTarget()->Feature->getID();
			/*float u = */e->setValue(e->getValue() * this->FeatureWeight[k] / sum[k]);
			SUM += e->getValue();
			//fprintf(stderr, "%4.2f ", u);fflush(stderr);
		}
		//fprintf(stderr, "\n");fflush(stderr);

		//Please take care that if a node does not have connections to a specific feature, the sum of its row will be less than 1.
		if (SUM < 1){
			//fprintf(stderr, "At %d: %10.8f <1\n", i, sum);fflush(stderr);
			for (int j = 0; j < this->Feature[0]->Vertex[i]->NoEdges; j++){
				vsEdge* e = this->Feature[0]->Vertex[i]->Edge[j];
				/*float u = */e->setValue(e->getValue() / SUM);
			}
		}
	}

	//Compute the Values for FeatureValues
	for (int k = 1; k < this->NoFeatures; k++){
		for (int i = 0; i < this->Feature[k]->NoVertices; i++){
			float sum = 0;
			for (int j = 0; j < this->Feature[k]->Vertex[i]->NoEdges; j++){
				vsEdge* e = this->Feature[k]->Vertex[i]->Edge[j];
				//float targetValue = this->getEdge(e->getTarget(), this->Feature[k]->Vertex[i])->getValue();
				float targetValue = this->getEdge(e->getTarget(), this->Feature[k]->Vertex[i])->getWeight();
				sum += e->setValue(targetValue);
	 		}
			for (int j = 0; j < this->Feature[k]->Vertex[i]->NoEdges; j++){
				vsEdge* e = this->Feature[k]->Vertex[i]->Edge[j];
				e->setValue(e->getValue() / sum);
			}
		}
	}

	delete []sum;

	fprintf(stderr, "OK\n");fflush(stderr);
}

vsMatrix* vsGraph::transferWeightValuesToSparseMatrix2(bool addSelfLoops){
	int *FW = new int[this->NoFeatures];

	int S = this->getNoVertices(0);
	FW[0] = 0;
	int V = this->getNoVertices(0);

	fprintf(stderr, "Feature %d: %d\n", 0, FW[0]);fflush(stderr);
	for (int f = 1 ; f < this->NoFeatures; ++f){
		if (this->FeatureWeight[f] == 0){
			FW[f] = FW[f - 1];
		}
		else{
			FW[f] = FW[f - 1] + V;
			V = this->getNoVertices(f);
			S += V;
		}
		fprintf(stderr, "Feature %d: %d\n", f, FW[f]);fflush(stderr);
	}

	vsMatrix* M = new vsMatrix(S);

	float prod = 1.0f;
	float value;
	int k;
	bool inserted = false;
	float sum = 0;
	int N = 0;
	vsNode* curNode;
	int F;

	//Construct Full Matrix and add SelfLoops

	for (int f = 0 ; f < this->NoFeatures; ++f){
		if (this->FeatureWeight[f] == 0 && f > 0)
			continue;

		for (int i = 0; i < this->getNoVertices(f); i++){
			if((N + i)==60981)
			{
				int a=5;
				a=a+7;
			}
			M->Row[N + i].GlobalIndex = this->Feature[f]->Vertex[i]->Edge[0]->getSourceID();

			prod = 1.0f;
			sum = 0;
			inserted = false;

			curNode = this->Feature[f]->Vertex[i];

			if (addSelfLoops){
				prod = curNode->NoEdges / (1.0f + curNode->NoEdges);
			}

			for (int j = 0; j < curNode->NoEdges; j++){
				
				F = curNode->Edge[j]->getTarget()->Feature->getID();
				value = curNode->Edge[j]->getValue() * prod;
				k = curNode->Edge[j]->getTarget()->getLocalID() + FW[F];

				if (!inserted && addSelfLoops && k > N + i){
					M->Row[N + i].insertValue(N + i, 1.0f / (1.0f + curNode->NoEdges));
					inserted = true;
					sum += (1.0f / (1.0f + curNode->NoEdges));
				}

				M->Row[N + i].insertValue(k, value);
				curNode->Edge[j]->setValue(value);
				sum += value;
			}

			if (!inserted && addSelfLoops){
				M->Row[N + i].insertValue(N + i, 1.0f / (1.0f + curNode->NoEdges));
				inserted = true;
				sum += (1.0f / (1.0f + curNode->NoEdges));
			}

			if (fabs(1 - sum) > 0.0001){
				fprintf(stderr, "\nExiting because sum per row is %10.8f < 1. Did you use the weighting approach?\n\n", sum);fflush(stderr);
				//exit(1);
			}
		}

		N += this->getNoVertices(f);
	}

	delete []FW;
	fprintf(stderr, "Transfer completed: %d\n", N);fflush(stderr);
	return M;
}


void vsGraph::transferWeightValuesToFullMatrix(bool addSelfLoops){
	int *FW = new int[this->NoFeatures];

	int S = this->getNoVertices(0);
	FW[0] = 0;
	int V = this->getNoVertices(0);

	fprintf(stderr, "Feature %d: %d\n", 0, FW[0]);fflush(stderr);
	for (int f = 1 ; f < this->NoFeatures; ++f){
		if (this->FeatureWeight[f] == 0){
			FW[f] = FW[f - 1];
		}
		else{
			FW[f] = FW[f - 1] + V;
			V = this->getNoVertices(f);
			S += V;
		}
		fprintf(stderr, "Feature %d: %d\n", f, FW[f]);fflush(stderr);
	}

	this->FullMatrix = (float*) realloc(this->FullMatrix, S * S * sizeof(float) );
	if ( !this->FullMatrix ){
		fprintf (stderr, "Cannot reallocate memory for array Full_Matrix\n");
		exit(1);
	}	
	for (int i = 0; i < S * S; i++){
		this->FullMatrix[i] = 0;
	}


	float prod = 1.0f;
	float value;
	vsNode* curNode;
	int F;
	int f;
	int k, l;
	int N = 0;
	float SUM = 0;

	//Construct Full Matrix and add SelfLoops
	for (int i = 0; i < this->NoVertices; i++){
		curNode = this->Vertex[i];
		F = curNode->Feature->getID();

		if (this->FeatureWeight[F] == 0 && F > 0)
			continue;

		SUM = 0;

		l = N;

		prod = 1.0f;

		if (addSelfLoops){
			this->FullMatrix[l * S + l] = 1.0f / (1.0f + curNode->NoEdges);
			prod = curNode->NoEdges / (1.0f + curNode->NoEdges);
			SUM +=  this->FullMatrix[l * S + l];
		}

		for (int j = 0; j < curNode->NoEdges; j++){
			f = curNode->Edge[j]->getTarget()->Feature->getID();
			if (this->FeatureWeight[f] == 0 && f > 0)
				continue;

			k = curNode->Edge[j]->getTarget()->getLocalID() + FW[f];

			value = curNode->Edge[j]->getValue() * prod;
			this->FullMatrix[l * S + k] = value;
			curNode->Edge[j]->setValue(value);
			SUM += this->FullMatrix[l * S + k];
		}
		if (fabs(1 - SUM) > 0.0001){
			fprintf(stderr, "\nExiting because sum per row at row %d is %10.8f < 1. Did you use the weighting approach?\n\n", i, SUM);fflush(stderr);
			fprintf(stderr, "N = %d, Feature: %d\n", N, F);fflush(stderr);
			exit(1);
		}

		N++;
	}

	fprintf(stderr, "Transfer completed!\n");fflush(stderr);
}

vsClusteringResult* vsGraph::checkExistenceOfClusteringResult(bool SL, bool DG, bool W, float I, float T, int N, int NoF, float *FW, float AT){
	for (int i = 0; i < this->NoClusteringResults; ++i){
		bool exists = false;

		if (this->ClusteringResult[i]->addSelfLoops == SL &&
			this->ClusteringResult[i]->downgrade == DG &&
			this->ClusteringResult[i]->weighted == W &&
			this->ClusteringResult[i]->Inflation == I &&
			this->ClusteringResult[i]->Threshold == T &&
			this->ClusteringResult[i]->NoVertices == N && 
			this->ClusteringResult[i]->NoFeatures == NoF &&
			this->ClusteringResult[i]->ApproximationThreshold == AT)
		{
			exists = true;

			if (this->ClusteringResult[i]->weighted){
			
				for (int j = 0; j < this->ClusteringResult[i]->NoFeatures; ++j){
					if (this->ClusteringResult[i]->featureWeight[j] == FW[j])
						continue;

					exists = false;
					break;
				}
			}
		}

		if (exists)	
			return this->ClusteringResult[i];
	}

	return NULL;
}

vsClusteringResult* vsGraph::createClusteringResult(bool addSelfLoops, bool downgrade, bool weighted, float Inflation, float Threshold, int NoVert, int NoF, float *FW, float AT){

	this->ClusteringResult = (vsClusteringResult**) realloc(this->ClusteringResult, (this->NoClusteringResults + 1) * sizeof(vsClusteringResult*));
	if (!this->ClusteringResult){
		fprintf(stderr, "Cannot allocat memory for array Clustering Result. Exiting ...\n");fflush(stderr);
		exit(1);
	}
	this->ClusteringResult[this->NoClusteringResults] = new vsClusteringResult(addSelfLoops, downgrade, weighted, Inflation, Threshold, NoVert, NoF, FW, AT);
	this->NoClusteringResults++;

	return this->ClusteringResult[this->NoClusteringResults - 1];
}


void calcuclateMinMax(int min1, int min2, int max1, int max2, int &min, int &max){
	min = (min1 > min2) ? min1 : min2;
	max = (max1 < max2) ? max1 : max2;
}

void vsGraph::setClustership(){
	//Set clustership of events
	for (int i = 0; i < this->NoVertices; ++i){
		this->Vertex[i]->ClusterID = this->currentCR->Vertex[i];
	}

	//Set clustership of featurevalues
	int c;
	int *A = (int*) malloc((this->currentCR->NoClusters + 1) * sizeof(int));
	memset(A, 0, (this->currentCR->NoClusters + 1) * sizeof(int));

	for (int i = this->getNoVertices(0); i < this->NoVertices; ++i){

		for (int j = 0; j < this->Vertex[i]->NoEdges; ++j){
			c = this->Vertex[i]->Edge[j]->getTarget()->ClusterID;
			//fprintf(stderr, "%d: %d %d\n", i, c, this->currentCR->NoClusters);fflush(stderr);
			A[c]++;
		}

		int max = 0;
		int index = 0;
		for (int j = 0; j < this->currentCR->NoClusters + 1; ++j){
			if (A[j] > max){
				max = A[j];
				index = j;
				A[j] = 0;
			}
			else
				A[j] = 0;
		}

		this->Vertex[i]->ClusterID = index;
	}

	//fprintf(stderr, "OK %d %d\n", this->NoVertices, this->currentCR->NoVertices);fflush(stderr);
	free(A);
	
	fprintf(stderr, "OK %d\n", this->NoVertices);fflush(stderr);
	
	if (!this->abstractGraph)
		return;
	
	for (int i = 0; i < this->abstractGraph->NoVertices; ++i){
		int f = this->abstractGraph->Vertex[i]->Feature->getID();
		int k =	this->Feature[f]->getVertex(this->abstractGraph->Vertex[i]->getLabel())->getGlobalID();
		//fprintf(stderr, "%d (%d) %d (%d)\n", i, this->abstractGraph->NoVertices, k, this->NoVertices);fflush(stderr);
		this->abstractGraph->Vertex[i]->ClusterID = this->Vertex[k]->ClusterID;
	}
}

void vsGraph::Stochastic_Flows_Algorithm(bool addSelfLoops, bool downgrade, bool weighted, float Inflation, float Threshold, float ApproximationThreshold, bool speedup){

	//omp_set_num_threads(4);
	std::ofstream ofs("kainourio_smnew.txt");
	this->currentCR = this->checkExistenceOfClusteringResult(addSelfLoops, downgrade, weighted, Inflation, Threshold, this->NoVertices, this->getNoFeatures() - 1, (this->FeatureWeight + 1), ApproximationThreshold);
	if (this->currentCR)
	{
		this->setClustership();
		return;
	}
	//std::ofstream ofs("diafores");
	this->currentCR = this->createClusteringResult(addSelfLoops, downgrade, weighted, Inflation, Threshold, this->NoVertices, this->getNoFeatures() - 1, (this->FeatureWeight + 1), ApproximationThreshold);
	//this->currentCR = NULL;


	int tid, nthreads, chunk = 10;


	int ActiveVertices = this->getNoVertices(0);

	for (int z = 1; z < this->NoFeatures; z++)
	{
		fprintf(stderr, "Feature \"%50s\" Weight: %f\n", this->Feature[z]->getLabel(), this->FeatureWeight[z]);fflush(stderr);
		if (this->FeatureWeight[z] > 0)
			ActiveVertices += this->getNoVertices(z);
	}
	fprintf(stderr, "\n");fflush(stderr);

	int COUNTER = ActiveVertices;


	this->createFullMatrix(addSelfLoops, downgrade, weighted);

	//bool speedup = true;
	//bool speedup = false;

	vsMatrix *SMnew = new vsMatrix(ActiveVertices);
	if (speedup)
	{
		this->transferWeightValuesToFullMatrix(addSelfLoops);
		//SMnew = this->transferWeightValuesToSparseMatrix2(addSelfLoops);
		SMnew->createFromDenseMatrix(this->FullMatrix, ActiveVertices, ActiveVertices);
	}
	else
	{
		delete SMnew;
		SMnew = this->transferWeightValuesToSparseMatrix2(addSelfLoops);
		//SMnew->printSelf("smnew.txt");
	}
	int Rows = SMnew->NoRows;

	fprintf(stderr, "%d %d\n", ActiveVertices, Rows);fflush(stderr);


	vsMatrix *SM = new vsMatrix(Rows);
	SM->transpose(SMnew);
	//SM->printSelf("sm.txt");
	vsMatrix *DistanceM = new vsMatrix(Rows);
	DistanceM->createFromSparseMatrix(SMnew);
	vsMatrix *AM = new vsMatrix(Rows);
	AM->createFromSparseMatrix(SMnew);
	//SM->createColumnMatrixFromDenseMatrix(this->FullMatrix, Rows, Rows);
	//DistanceM->createFromDenseMatrix(this->FullMatrix, Rows, Rows);

	

	int i, j, k;
	float sum = 0; 
	float max;
	int min, maxi;

	int *mina = new int[Rows];
	int *maxa = new int[Rows];


	//edo mporoume na to ensomatosoume sto epomeno for i<Rows
    for (i = 0; i < Rows; i++)
	{
		mina[i] = AM->Row[i].Element[0]->Index;
		maxa[i] = AM->Row[i].Element[AM->Row[i].NoElements - 1]->Index;
		//mina[i] = this->Vertex[i]->Edge[0]->getTargetID();
		//maxa[i] = this->Vertex[i]->Edge[this->Vertex[i]->NoEdges - 1]->getTargetID();
	}
	
	int Walk = 0;
	double Distance = 1;
	double error = 0.001;
	float power = Inflation;
	double T = Threshold;
	int target; 
	int nextindex;
	float prod;


	while ((Distance > error) || (Walk % 2) == 0)
	{
		//if ( (Distance < 10*error) && (power == Inflation) ){
		if ( (Distance < 10*error || COUNTER < ActiveVertices * ApproximationThreshold) && (power == Inflation) ){
			power = Inflation + 2;
			T = 0.3;
			COUNTER = ActiveVertices;
			fprintf(stderr, "Power changed!\n");fflush(stderr);
		}
		else if ( (power != Inflation) && (COUNTER < ActiveVertices * ApproximationThreshold) && (Walk-2*(Walk/2) == 1) )
			break;

		#ifdef QT_QTGUI_MODULE_H
			QCoreApplication::processEvents(); //Uncomment this to let Qt process events
			//QCoreApplication::sendPostedEvents();
		#endif

		SMnew->clear();

		/*#pragma omp parallel shared(Rows, chunk, mina, maxa, power, T) private(tid, i, j, k, sum, max, min, maxi, target, nextindex, prod)
		{
		tid = omp_get_thread_num();
		nthreads = omp_get_num_threads();*/
		//if ( tid==1)
			//fprintf(stderr, "MM started ...");fflush(stderr);

		//#pragma omp for schedule (static, chunk)
		for (i = 0; i < Rows; i++)
		{
			//ofs<<"\n"<<i<<" ";
			//fprintf(stderr, " i=%d  ", i);fflush(stderr);
			max = 0;
			prod = (float)T * float(max);
			int number_in_if=0;
			for (j = 0; j < Rows; j++)
			{
				sum = 0;
				nextindex = 0;

				if (SM->Row[j].Element.size()>0)
				{

					calcuclateMinMax(mina[i], SM->Row[j].Element[0]->Index, maxa[i], SM->Row[j].Element[SM->Row[j].NoElements - 1]->Index, min, maxi);

					if (min <= maxi)
					{

						for (k = 0; k < AM->Row[i].NoElements; k++){
							target = AM->Row[i].Element[k]->Index;

							if (target > maxi || min > maxi)
								break;
							else if (target < min)
								continue;

							if (speedup)
								sum += (float)(AM->Row[i].Element[k]->Value * (float)this->FullMatrix[target * Rows + j]);//LEFT multiplication with AM is the right choice
							else
							{
								//ofs<<" j="<<j<<" proti timi "<<AM->Row[i].Element[k]->Value<<" deyteri timi "<<SM->Row[j].getValue(target, min, nextindex)<<" ";
								float returnedvalue=SM->Row[j].getValue(target, min, nextindex);
								if(returnedvalue >0)
									sum += (AM->Row[i].Element[k]->Value )* returnedvalue; //LEFT multiplication with AM is the right choice
							}
						}

					}

					//if ( (i < mina[i]) || (i > maxa[i]) ){ //This SHOULD ALWAYS HAPPEN
					//if (addSelfLoops && this->FullMatrix[i * Rows + j] > 0){

/*
					if (addSelfLoops){
						nextindex = 0;
						if (speedup)
							sum += (float)this->FullMatrix[i * Rows + j] / (float)(1.0f + (AM->Row[i].NoElements - 1)); //LEFT multiplication with AM is the right choice
						else
							sum += (float)SM->getValue(j, i, min, nextindex) / (float)(1.0f + (AM->Row[i].NoElements - 1)); //LEFT multiplication with AM is the right choice
					}
*/
					if(Rows>10000&&sum<(10.0/Rows))
						continue;
					if (sum > 0)
					{
						float sum_power = pow(sum, power);
						if (sum_power > max)
						{
							max = sum_power;
							prod = (float)T * float(max);
							SMnew->Row[i].insertValue(j, sum_power);
							//ofs<<" "<<j<<" "<<sum;
							//number_in_if++;
						}
						else if (sum_power >= prod)
						{
							SMnew->Row[i];
							//fprintf(stderr, " i=%d  j=%d sum=%d", i,j,sum);fflush(stderr);
							SMnew->Row[i].insertValue(j, sum_power);
							//ofs<<" "<<j<<" "<<sum;
							number_in_if++;
						}
					}
					
					//this->FullMatrix[i * Rows + j] = sum;

				}
			}

			sum = 0;
			prod = (float)T * float(max);

			for (j = 0; j < SMnew->Row[i].NoElements; ++j){
				if (SMnew->Row[i].Element[j]->Value < prod)
				{
					SMnew->Row[i].Element.erase(SMnew->Row[i].Element.begin()+j);
					SMnew->Row[i].NoElements--;
					j--;
					//SMnew->Row[i].Element[j]->Value = 0;
				}
				else
					sum += SMnew->Row[i].Element[j]->Value;
			}
			for (j = 0; j < SMnew->Row[i].NoElements; ++j)
				if (sum > 0)
					SMnew->Row[i].Element[j]->Value /= (float)sum;


		}
		//if ( tid==1)
			//fprintf(stderr, "MM ended ...");fflush(stderr);

//}edo gia to pragma to vgazo
		//SMnew->printSelf("smnew_after_praxeis.txt");
		SM->transpose(SMnew);
		if (speedup)
			SMnew->createFullMatrixFromSparseMatrix(this->FullMatrix);


		Walk++;
		k = Rows;
		int next = 0;
		int nextindex = 0;
		if (Walk % 2 == 0){ //Uncomment for bipartite Graph
			COUNTER = 0;
			Distance = 0;
			int l;
			float v;

			for (i = 0; i < Rows; ++i){
				sum = 0;
				next = 0;
				nextindex = 0;

				for (j = 0; j < DistanceM->Row[i].NoElements; ++j){
					l =  DistanceM->Row[i].Element[j]->Index;
					v = DistanceM->Row[i].Element[j]->Value;
					//v -= SMnew->Row[i].searchanderaseElement(l);
					if (l >= next)
						v -= SMnew->Row[i].getValue(l, next, nextindex, true);
					sum += (float)v * (float)v;
				}
				for (j = 0; j < SMnew->Row[i].NoElements; ++j){
					v = SMnew->Row[i].Element[j]->Value;
					if (v > 0)
						sum += (float)v * (float)v;
				}
				
				
				//ofs<<SMnew->Row[i].NoElements-DistanceM->Row[i].NoElements<<"\n";
				

				sum = sqrt(sum);
				if (sum > Distance){
					Distance = sum;
					k = i;
				}
				if (sum > error)
					COUNTER++;

			}

			DistanceM->transpose(SM);
			//DistanceM->printSelf("DistanceM_after_praxeis.txt");
			fprintf(stderr, "Walk %3d completed. Distance: %f at Row %5d. Error: %f\t %d / %d vectors > 0\n", Walk, Distance, k, error, COUNTER, ActiveVertices);fflush(stderr);	
		}
		else
			fprintf(stderr, "Walk %3d completed.\n", Walk);fflush(stderr);	
	
	}
	SMnew->transpose(SM);
	this->calculateClusters(SMnew);


	delete []mina;
	delete []maxa;
	free(DistanceM);
	free(SM);
	free(SMnew);
	
	free(this->FullMatrix);
	this->FullMatrix = NULL;
	ofs.close();
}


int compareClusters (const void* a, const void* b){
	return ((*(vsGraph**)b)->Feature[0]->NoVertices - (*(vsGraph**)a)->Feature[0]->NoVertices);
}

void vsGraph::calculateClusters(vsMatrix* M){

	if (this->NoClusters){
		for (int i = 0; i < this->NoClusters; i++)
			delete this->Cluster[i];
		free(this->Cluster);
		this->Cluster = NULL;
		this->NoClusters = 0;
	}

	int Rows = this->NoVertices;

	//Find Vertices that have two or more entries per row and keep one
	float max, sum;
	int k;
	//for (int i = 0; i < M->NoRows; i++){
	for (int i = 0; i < this->Feature[0]->NoVertices; i++){
		max = 0;
		k = Rows;
		sum = 0;

		for (int j = 0; j < M->Row[i].NoElements; ++j){

			if (M->Row[i].Element[j]->Value > 0){

				if (M->Row[i].Element[j]->Value > max){
					max = M->Row[i].Element[j]->Value;
					k = M->Row[i].Element[j]->Index;
					//k = M->Row[M->Row[i].Element[j].Index].GlobalIndex;
				}

				sum += 1;
			}
		}
		//if (sum > 0)
		if ((sum > 0) && (i < this->Feature[0]->NoVertices))
			this->assignVertexToCluster(i, k);
	}

	qsort(this->Cluster, this->NoClusters, sizeof(vsGraph*), compareClusters);

	this->printClusters();//Do not uncomment!! It also sets the ClusterID to currentCR (see code)

	this->currentCR->NoClusters = this->NoClusters;
	
	this->setClustership();
}

void vsGraph::printClusters(){
	for (int i = 0; i < this->NoClusters; i++){
		fprintf(stderr, "Cluster %3d: Centroid %4s, %4d members\n", i, this->Cluster[i]->Feature[0]->getLabel(), this->Cluster[i]->NoVertices);fflush(stderr);
		//this->currentCR->Vertex[atoi(this->Cluster[i]->Feature[0]->getLabel())] = i + 1;

		for (int j = 0 ; j < this->Cluster[i]->NoVertices; j++){
			fprintf(stderr, "%s ", this->Cluster[i]->Vertex[j]->getLabel());fflush(stderr);
			this->currentCR->Vertex[atoi(this->Cluster[i]->Vertex[j]->getLabel())] = i + 1;
		}
		fprintf(stderr, "\n");fflush(stderr);
	}
}

bool vsGraph::importClusteringReasultsFromCSVFile(vsCSVFile* f){

	fprintf(stderr, "Importing Clustering Results ... ");fflush(stderr);

	if (this->NoClusters){
		for (int i = 0; i < this->NoClusters; i++)
			delete this->Cluster[i];
		free(this->Cluster);
		this->Cluster = NULL;
		this->NoClusters = 0;
	}
	//vsClusteringResult(bool SL = false, bool DG = false, bool W = false, float I = 0, float T = 0, int N = 0, int NoF = 0, float *FW = NULL)
	this->currentCR = this->createClusteringResult(false, false, false, 0, 0, this->NoVertices, this->getNoFeatures() - 1, (this->FeatureWeight + 1), 0);


	for (int i = 1; i < f->NoLines; i++){

		int id = searchVertex(f->Line[i]->Word[0], this->Feature[0]->getLabel());
		std::string fline_i_word_0=f->Line[i]->Word[0];
		std::string Feature_0__getLabel=Feature[0]->getLabel();
		if ((id < this->getNoVertices(0)) && (atoi(f->Line[i]->Word[1]) != 0))
			this->assignVertexToCluster(id, atoi(f->Line[i]->Word[1]));
		/*if(atoi(f->Line[i]->Word[1]) != 0)
		this->assignVertexToCluster(atoi(f->Line[i]->Word[0]), atoi(f->Line[i]->Word[1]));*/
		//the second argument is the cluster,the first is the number of the node in vertex table
		//for (int j = 1; j < f->Line[i]->NoWords; j++)
			//this->addEdge(Feature1, f->Line[i]->Word[0], f->FileName, f->Line[i]->Word[j]);
	}

	qsort(this->Cluster, this->NoClusters, sizeof(vsGraph*), compareClusters);

	this->printClusters();

	this->currentCR->NoClusters = this->NoClusters;

	this->setClustership();

	fprintf(stderr, "OK\n");fflush(stderr);

	return true;
}

void vsGraph::Static_AD_Algorithm(bool addSelfLoops, bool downgrade, bool weighted){
//NF_algorithm(float* AdjacencyMatrix, int Rows){

	fprintf(stderr, "Executing Anomaly Detection Algorithm ...\n");fflush(stderr);
	int tid, nthreads, chunk = 10;

	this->createFullMatrix(addSelfLoops, downgrade, weighted);
	this->transferWeightValuesToFullMatrix(addSelfLoops);//to evala ego na to vgalo an einai
	float* AdjacencyMatrix = this->FullMatrix;

	int Rows = this->NoVertices;

	int target;

	int i, j, k;
	float c = 0.15f;
	float error = pow(10.0f, -4);
	float Distance;

	float *Ua, *Qa, *Ua_new;

	if ( !this->NormalityMatrix){
		this->NormalityMatrix = (float *)realloc(this->NormalityMatrix, Rows * Rows * sizeof(float));
		if (!this->NormalityMatrix){
			fprintf (stderr, "Cannot allocate memory for array NormalityMatrix\n");
			exit(1);
		}
	}


#pragma omp parallel shared(error, Rows, AdjacencyMatrix, nthreads, c) private(tid, i, j, k, Ua_new, Ua, Qa, Distance, target)
{
	Ua = new float[Rows];
	Qa = new float[Rows];
	Ua_new = new float[Rows];
 
	tid = omp_get_thread_num();
	nthreads = omp_get_num_threads();

#pragma omp for schedule (static, chunk)
	for (i = 0; i < Rows; i++){

		for (j = 0; j < Rows; j++){
			Ua[j] = 0;
			Ua_new[j] = 0;
			Qa[j] = 0;
		}

		Ua[i] = 1;
		Qa[i] = 1;

		Distance = 1;
		/*for(int i=0;i<Rows;i++)
		{
			for(int j=0;j<Rows;j++)
			fprintf(stderr, "%6.4f ", AdjacencyMatrix[i * Rows + j]);fflush(stderr);
		}*/
		while (Distance > error){
			//fprintf(stderr, "Thread %d: %f\n", tid, Distance);fflush(stderr);
			Distance = 0;

			for (j = 0; j < Rows; j++){
				//fprintf (stderr, "%d->%d: ", i, j);

				for (k = 0; k < this->Vertex[j]->NoEdges; k++){
					target = this->Vertex[j]->Edge[k]->TargetID;
					//Ua_new[j] += (float)AdjacencyMatrix[j * Rows + target] * (float)Ua[target];
					Ua_new[j] += (float)AdjacencyMatrix[target * Rows + j] * (float)Ua[target];
					//fprintf(stderr, "(%d)%6.4f ", k, (float)AdjacencyMatrix[target * Rows + j]);fflush(stderr);
				}
				//fprintf (stderr, "\n");

				/*if (j < E){
					for (k = 0; k < this->Event[j]->NoLinks; k++){
						target = this->Event[j]->Links[k].Target + this->NoEvent;
						Ua_new[j] += (float)AdjacencyMatrix[j * Rows + target] * (float)Ua[target];
					}
				}
				else{
					for (k = 0; k < this->FeatureValue[j - E]->NoLinks; k++){
						target = this->FeatureValue[j - E]->Links[k].Target;
						Ua_new[j] += (float)AdjacencyMatrix[j * Rows + target] * (float)Ua[target];
					}
				}*/
				
				Ua_new[j] += (float)AdjacencyMatrix[j * Rows + j] * (float)Ua[j];

				Ua_new[j] = (1 - c) * Ua_new[j] + c * Qa[j];

				Distance += pow(Ua_new[j] - Ua[j], 2);
				//fprintf(stderr, "%f ", Ua_new[j]);fflush(stderr);
			}

			//fprintf (stderr, "%d: ", i);
			for (j = 0; j < Rows; j++){
				Ua[j] = Ua_new[j];
				Ua_new[j] = 0;
				//fprintf(stderr, "%6.4f ", Ua[j]);fflush(stderr);
			}
			//fprintf (stderr, "\n");

			Distance = sqrt(Distance);
		}

		for (j = 0; j < Rows; j++){
			this->NormalityMatrix[i * Rows + j] = Ua[j];
		}
		if (i % 100 == 0)
			fprintf(stderr,"Row: %6d %7.2f%%\n", i, 100 * i / (float)Rows);fflush(stderr);
	}

	delete []Ua;
	delete []Qa;
	delete []Ua_new;	
}

	/*for (i = 0; i < Rows; i++){
		int counter = 0;
		for (j = 0; j < Rows; j++)
			if (this->NormalityMatrix[i * Rows + j] > 0)
				counter++;
			//this->NormalityMatrix[i * Rows + i] = (1 - this->NormalityMatrix[i * Rows + i]) / (counter - 1);
			this->NormalityMatrix[i * Rows + i] /= (counter - 1);
			//sum = 1 + this->NormalityMatrix[i * Rows + i];
			//for (j = 0 ; j < Rows; j++){
				//this->NormalityMatrix[i * Rows + j] /= sum;
			//}
	}*/

	//Print NormalityMatrix
	//printf ("\n");
	/*std::ofstream ofs("normalitymatrix.txt");
	for (i = 0; i < Rows; i++){
		for (j = 0 ; j < Rows; j++)
			ofs<<this->NormalityMatrix[i * Rows + j]<<" ";
		ofs<<"\n";
			//fprintf(stderr, "%6.4f ", this->NormalityMatrix[i * Rows + j]);fflush(stderr);
		//fprintf (stderr, "\n");
	}*/	
 
	//COMPUTE NORMALITY SCORES
	this->computeNormalityScores();

	free(this->NormalityMatrix);
	this->NormalityMatrix = NULL;
}


float vsGraph::Pearson(float *Matrix, int Rows, int a, int b, int BottomLimit, int UpperLimit){

	//Check whether the vectors have at least one non-zero relevance score to a common vertex
	bool common = false;
	for (int i = BottomLimit; i < UpperLimit; i++){
		if ( ((Matrix + Rows * a)[i] > 0) && ((Matrix + Rows * b)[i] > 0) ){
			common = true;
			break;
		}
	}
	if (!common)
		return 0;


	float mean1 = 0;	
	float mean2 = 0;
	float var1 = 0;
	float var2 = 0;
	float coeff = 0;

	float counter = 0;

	for (int i = BottomLimit; i < UpperLimit; i++){
		float e1 = (Matrix + Rows * a)[i];
		float e2 = (Matrix + Rows * b)[i];
		//if ( (i != a) && (i != b) ){
		if ( (e1 > 0) || (e2 > 0) ){
		//if (((e1 > 0) || (e2 > 0)) && ((i != a) && (i != b))){
			if (i != a)
				mean1 += e1;
			if (i != b)
				mean2 += e2;
		}
		else counter++;
	}

	mean1 /= (float)(UpperLimit - BottomLimit - counter - 1); //This SHOULD OMMIT -2, but it gives better results
	mean2 /= (float)(UpperLimit - BottomLimit - counter - 1);

	for (int i = BottomLimit; i < UpperLimit; i++){
		float e1 = (Matrix + Rows * a)[i];
		float e2 = (Matrix + Rows * b)[i];

		//if ( (i != a) && (i != b) ){
		if ( (e1 > 0) || (e2 > 0) ){
		//if (((e1 > 0) || (e2 > 0)) && ((i != a) && (i != b))){
			if (i != a)
				var1 += (float)(e1 - mean1) * (float)(e1 - mean1);
			if (i != b)
				var2 += (float)(e2 - mean2) * (float)(e2 - mean2);
			if (i != a && i != b)
				coeff += (float)(e1 - mean1) * (float)(e2 - mean2);
		}
	}
	float e1 = (Matrix + Rows * a)[b];
	float e2 = (Matrix + Rows * b)[a];
	coeff += (float)(e1 - mean1) * (float)(e2 - mean2);

	var1 = sqrt(var1);
	var2 = sqrt(var2);

	coeff /= (var1 * var2);
 
	return coeff;
}

void vsGraph::computeNormalityScores(){

	int numVerts = this->getNoVertices();

	float *CovMatrix = (float *)calloc(numVerts * numVerts, sizeof(float));
	if (!CovMatrix){
		fprintf(stderr, "Cannot allocate memory for CovMatrix. Exiting ... \n");fflush(stderr);
		exit(1);
	}
	for (int i = 0; i < numVerts * numVerts; i++){
		CovMatrix[i] = 0;
	}

	this->NormalityScore = (float *) realloc(this->NormalityScore, numVerts * sizeof(float));
	if (!this->NormalityScore){
		fprintf(stderr, "Cannot reallocate memory for array NormalityScore. Exiting ... \n");fflush(stderr);
		exit(1);
	}

 
	//Create Covariance Matrix
	fprintf(stderr, "Building Covariance Matrix ... ");fflush(stderr);
	int i, j;
	int tid, nthreads, chunk = 10;

#pragma omp parallel shared(CovMatrix) private(tid, i, j)
{
	tid = omp_get_thread_num();
	nthreads = omp_get_num_threads();

#pragma omp for schedule (static, chunk)
	for (i = 0; i < numVerts - 1; ++i){
		for (j = i + 1; j < numVerts; ++j){
			//CovMatrix[i * Rows + j] = Pearson(this->NormalityMatrix, Rows, i, j, 0, this->NoEvent);
			CovMatrix[i * numVerts + j] = Pearson(this->NormalityMatrix, numVerts, i, j, 0, numVerts);
			CovMatrix[j * numVerts + i] = CovMatrix[i * numVerts + j];
		}
	}
}
	fprintf(stderr, "OK\n");fflush(stderr);

	
	/*for (int i = 0; i < numVerts; i++){
		for (int j = 0; j < numVerts; j++){
			fprintf(stderr, "%5.2f ", CovMatrix[i * numVerts + j]);fflush(stderr);
		}
		fprintf(stderr, "\n");fflush(stderr);
	}*/


	for (int i = 0; i < numVerts; i++){

		this->NormalityScore[i] = 0;


		for (int j = 0; j < this->Vertex[i]->NoEdges - 1; ++j){

			int a = this->Vertex[i]->Edge[j]->TargetID;

			for (int k = j + 1; k < this->Vertex[i]->NoEdges; ++k){
				int b = this->Vertex[i]->Edge[k]->TargetID;

				if (CovMatrix[a * numVerts + b] < 0)
					this->NormalityScore[i] -= 2 * sqrt(fabs(CovMatrix[a * numVerts + b]));
				else
				//this->NormalityScore[i] += 2 * CovMatrix[a * Rows + b];
				this->NormalityScore[i] += 2 * CovMatrix[a * numVerts + b] * CovMatrix[a * numVerts + b];
			}
		}

		if (this->Vertex[i]->NoEdges > 1)
			this->NormalityScore[i] /= (this->Vertex[i]->NoEdges * (this->Vertex[i]->NoEdges - 1));
		else
			this->NormalityScore[i] = 1;
		//fprintf(stderr, "%s: %f\n", this->Event[i]->Name, this->NormalityScore[i]);fflush(stderr);
	}
	fprintf(stderr, "normalitysores ");fflush(stderr);
	for (int i = 0; i < numVerts; i++)
	{
		if (this->NormalityScore[i] <0)
		{
			fprintf(stderr, "%6s: %5.2f\n", this->Vertex[i]->getLabel(), this->NormalityScore[i]);fflush(stderr);
			this->Vertex[i]->is_suspicious=true;
			this->Vertex[i]->Feature->getVertex(this->Vertex[i]->getLabel())->is_suspicious=true;
			//fprintf(stderr, "%5.2f ",  this->NormalityScore[i]);fflush(stderr);
		}
	}

	free(CovMatrix);
}

void vsGraph::createAbstractGraph(){
	fprintf(stderr, "Creating abstract graph ... ");fflush(stderr);

	if (!this->abstractGraph)
		this->abstractGraph = new vsGraph;

	vsGraph *g = this->abstractGraph;

	int fID = g->NoFeatures;

	for (int i = fID; i < this->NoFeatures; ++i)
		g->addFeature(this->Feature[i]->getLabel());

	//fprintf(stderr, "%d %d %d\n", fID, this->NoFeatures, g->NoFeatures);fflush(stderr);

	vsNode *t1, *t2, *curTarget;
	int f1, f2;

	for (int i = 0; i < this->Feature[0]->NoVertices; ++i){
		vsNode *curNode = this->Feature[0]->Vertex[i];
		
		for (int j = 0; j < curNode->NoEdges; ++j){
			curTarget = curNode->Edge[j]->getTarget();
			f1 = curTarget->Feature->getID();

			if ( !curTarget->AV ){
				t1 = g->addVertex(curTarget->Feature->getLabel(), curTarget->getLabel());
				t1->Size = curTarget->NoEdges;
				curTarget->AV = t1;
			}
			else{
				t1 = curTarget->AV;
			}
/*
			if (f1->getID() > f->getID())
				continue;
*/
			for (int k = j + 1; k < curNode->NoEdges; ++k){
				curTarget = curNode->Edge[k]->getTarget();
				f2 = curTarget->Feature->getID();

				if ( f1 < fID && f2 < fID)
					continue;

				if ( !curTarget->AV ){
					t2 = g->addVertex(curTarget->Feature->getLabel(), curTarget->getLabel());
					t2->Size = curTarget->NoEdges;
					curTarget->AV = t2;
				}
				else{
					t2 = curTarget->AV;
				}
			
				g->addEdge(t1, t2);
				//t1->addEdge(t2);
			}
		}
	}
	
	fprintf(stderr, "OK\n");fflush(stderr);
}	
void vsGraph::createnormalgraph_fromabstract(){
	fprintf(stderr, "Creating abstract graph ... ");fflush(stderr);

	if (!this->abstractGraph)
		this->abstractGraph = new vsGraph;

	vsGraph *g = this->abstractGraph;

	int fID = g->NoFeatures;

	for (int i = fID; i < this->NoFeatures; ++i)
		g->addFeature(this->Feature[i]->getLabel());

	//fprintf(stderr, "%d %d %d\n", fID, this->NoFeatures, g->NoFeatures);fflush(stderr);

	vsNode *t1, *t2, *curTarget;
	int f1, f2;

	for (int i = 0; i < this->Feature[0]->NoVertices; ++i){
		vsNode *curNode = this->Feature[0]->Vertex[i];
		
		for (int j = 0; j < curNode->NoEdges; ++j){
			curTarget = curNode->Edge[j]->getTarget();
			f1 = curTarget->Feature->getID();

			if ( !curTarget->AV ){
				t1 = g->addVertex(curTarget->Feature->getLabel(), curTarget->getLabel());
				t1->Size = curTarget->NoEdges;
				curTarget->AV = t1;
			}
			else{
				t1 = curTarget->AV;
			}
/*
			if (f1->getID() > f->getID())
				continue;
*/
			for (int k = j + 1; k < curNode->NoEdges; ++k){
				curTarget = curNode->Edge[k]->getTarget();
				f2 = curTarget->Feature->getID();

				if ( f1 < fID && f2 < fID)
					continue;

				if ( !curTarget->AV ){
					t2 = g->addVertex(curTarget->Feature->getLabel(), curTarget->getLabel());
					t2->Size = curTarget->NoEdges;
					curTarget->AV = t2;
				}
				else{
					t2 = curTarget->AV;
				}
			
				g->addEdge(t1, t2);
				//t1->addEdge(t2);
			}
		}
	}
	fprintf(stderr, "OK\n");fflush(stderr);
}	
