//C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//VIS-SENSE includes
#include "vsFeature.h"
#include "vsCSVFile.h"
#include "vsNode.h"

vsFeature::vsFeature(int id, char *label){

	this->ID = id;
	this->Label = new char [strlen(label) + 1];
	strcpy(this->Label, label);
	this->Weight = 1.0f;

	this->NoVertices = 0;
	//this->Vertex = NULL;

	this->FileName = new vsFilename(label);
}

vsFeature::~vsFeature(){
	delete []this->Label;

	for (int i = 0; i < this->NoVertices; i++)
		delete this->Vertex[i];
	//free(this->Vertex);
	//this->Vertex = NULL;
	this->Vertex.clear();
	delete this->FileName;
}

char* vsFeature::getLabel(){
	return this->Label;
}

int vsFeature::getID(){
	return this->ID;
}

int vsFeature::searchVertex(char *label){
	int i;

	for (i = 0; i < this->NoVertices; i++){
		if (strcmp(this->Vertex[i]->getLabel(), label) == 0)
			break;
	}

	return i;
}

vsNode* vsFeature::searchVertex(vsNode* N){
	for (int i = 0; i < this->NoVertices; i++){
		if (this->Vertex[i] == N)
			return this->Vertex[i];
	}

	return NULL;
}

vsNode* vsFeature::getVertex(char *label){
	for (int i = 0; i < this->NoVertices; i++){
		if (strcmp(this->Vertex[i]->getLabel(), label) == 0)
			return this->Vertex[i];
	}

	return NULL;
}

vsNode* vsFeature::addVertex(char *label, bool &newVertexAdded){

	newVertexAdded = false;
	int vertexID = this->searchVertex(label);

	if (vertexID == this->NoVertices){
		newVertexAdded = true;
		/*
		this->Vertex = (vsNode**) realloc(this->Vertex, (vertexID + 1) * sizeof(vsNode*));
		if (!this->Vertex){ 
			fprintf (stderr, "Cannot reallocate memory for array Vertex\n");
			exit(1);
		}
   
		this->Vertex[vertexID] = new vsNode(this, vertexID, label);*/
		this->Vertex.push_back(new vsNode(this, vertexID, label));
		this->NoVertices++;
	}

	return this->Vertex[vertexID];
}

