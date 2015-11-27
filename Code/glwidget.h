#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include </usr/include/qt4/QtOpenGL/QGLWidget>
#include <math.h>
#include "vsNode.h"
#include "vsFeature.h"
//#include "mdichild.h"
//Forward Qt declarations

//Forward VIS-SENSE declarations
class vsColor{
public:
	vsColor(int r, int g, int b){
		this->R = r;
		this->B = b;
		this->G = g;
	};
	~vsColor(){};

	int R;
	int G;
	int B;
};
class vsPoint{
public:
	float x;
	float y;
	float z;
};
class vsSize{
public:
	int x;
	int y;
	int z;
};
class vsSD{
public:
	long int N;
	float mean;
	float SD;
};
class vsGraph;
class vsDrawingVertex{
public:
	vsDrawingVertex(int i, vsNode* node = NULL){
		this->initialize(i, node);
	}

	void initialize(int i, vsNode* node = NULL){
		this->ID = i;
		this->Map = node;
		//this->Size = 15;
		float E;
		if (this->Map->Size == 0)
			E = 15 * log10((float)this->Map->NoEdges);
		else
			E = 15 * log10((float)this->Map->Size);
		this->NormalSize = E > 10 ? E : 10;
		this->Size = this->NormalSize;
		this->Pos.x = this->Pos.y = this->Pos.z = 0;
		this->DrawingPos.x = this->DrawingPos.y = this->DrawingPos.z = 0;
		this->F.x = this->F.y = this->F.z = 0;
		this->Velocity.x = this->Velocity.y = this->Velocity.z = 0;
		this->Selected = &(node->Selected);
		this->SecondSelection = false;
		this->ClusterID = &(node->ClusterID);
		this->Draw = true;
		this->is_suspicious=&(node->is_suspicious);
	}
	int ID;

	vsNode* Map; 

	int Size;
	int NormalSize;

	vsPoint Pos;
	vsSize	DrawingPos;
	vsPoint F;
	vsPoint Velocity;

	bool *Selected;
	bool SecondSelection;
	int *ClusterID;
	bool Draw;
	bool *is_suspicious;
};

class vsDrawingEdge{

public:
	void initialize(vsDrawingVertex* s, vsDrawingVertex* t){

		this->Source = s;
		this->Target = t;

		float weight = s->Map->searchEdge(t->Map)->getWeight();
		this->ActualWeight = weight * (float)s->Map->NoEdges / (float)s->Map->Size;

		float r = s->Map->NoEdges / (float)t->Map->NoEdges;
		if (r > 1)
			r = 1 / r;
		float E = 5 * log10(weight);

		this->NormalWeight = E > 1 ? E : 1;
		this->Weight = this->NormalWeight;

		this->Draw = true;
	}

	vsDrawingVertex* Source;
	vsDrawingVertex* Target;
	float Weight;
	float NormalWeight;
	float ActualWeight;
	bool Draw;
};

#endif // WIDGET_H
