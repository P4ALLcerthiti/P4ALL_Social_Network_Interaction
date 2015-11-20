//C++ includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//VIS-SENSE includes
#include "vsNode.h"


vsNode::vsNode(vsFeature* feature, int id, char *name){

	this->LocalID = id;
	this->Label = new char [strlen(name) + 1];
	strcpy(this->Label, name);

	//this->Edge = NULL;
	this->NoEdges = 0;

	this->Feature = feature;

//Global Attributes
	this->Size = 0;
	this->GlobalID = 0;
	this->ClusterID = 0;
	this->Selected = false;
	this->DV = NULL;
	this->AV = NULL;
	is_suspicious=false;
}
 
vsNode::~vsNode(){

	delete []this->Label;

	for (int i = 0; i < this->NoEdges; i++)
		delete this->Edge[i];
	//free(this->Edge);
	this->Edge.clear();
	//this->Edge = NULL;
}

char* vsNode::getLabel(){
	return this->Label;
}

int vsNode::getLocalID(){
	return this->LocalID;
}

int vsNode::getGlobalID(){
	return this->GlobalID;
}

void vsNode::setGlobalID(int i){
	this->GlobalID = i;
}

vsEdge* vsNode::searchEdge(vsNode* target){

	for (int i = 0; i < this->NoEdges; i++){
		if (this->Edge[i]->getTarget() == target)
			return this->Edge[i];
	}

	return NULL;
}

vsEdge* vsNode::addEdge(vsNode* target){
	vsEdge* Edge = this->searchEdge(target);

	if ( !Edge ){
		/*this->Edge = (vsEdge**) realloc(this->Edge, (this->NoEdges + 1) * sizeof(vsEdge*));
		if (!this->Edge){  
			fprintf (stderr, "Cannot reallocate memory for array Edge\n");
			exit(1);
		}

		this->Edge[this->NoEdges] = new vsEdge(this, target);*/
		this->Edge.push_back(new vsEdge(this, target));
		this->NoEdges++;

		return this->Edge[this->NoEdges - 1];
	}
	else Edge->increaseWeight(1);

	return Edge;

}
