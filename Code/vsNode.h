#ifndef VSNODE_H_
#define VSNODE_H_
#include <vector>
//Forward declarations
class vsFeature;
class vsEdge;
class vsDrawingVertex;

class vsNode{
public:
	vsNode(vsFeature* feature, int id, char *name);
	~vsNode();
	
	vsEdge* searchEdge(vsNode* target);
	vsEdge* addEdge(vsNode* target);

	char* getLabel();
	int getLocalID();
	int getGlobalID();
	void setGlobalID(int);

	std::vector<vsEdge*> Edge;
	int NoEdges;

	vsFeature* Feature;

//Global Attributes
	int Size;
	bool Selected;
	int ClusterID;
	vsNode *AV;
	vsDrawingVertex* DV;
	bool is_suspicious;
protected:
	int LocalID;
	int GlobalID;
	char* Label;
	
};


class vsEdge{
public:
	vsEdge(vsNode* source, vsNode* target, float weight = 1.0f){
		this->Source = source;
		this->Target = target;
		this->SourceID = 0; //Global Value
		this->TargetID = 0;	//Global Value
		this->Weight = weight;
		this->Value = 0.0f;
	};
	~vsEdge(){};

	vsNode* getSource(){
		return this->Source;
	};
	int getSourceID(){
		return this->SourceID;
	};
	vsNode* getTarget(){
		return this->Target;
	};
	int getTargetID(){
		return this->TargetID;
	};
	void setSourceID(int m){
		this->SourceID = m;
	};
	void setTargetID(int m){
		this->TargetID = m;
	};
	float getWeight(){
		return this->Weight;
	}
	void increaseWeight(float m){
		this->Weight += m;
	}
	float getValue(){
		return this->Value;
	}
	float setValue(float m){
		this->Value = m;
		return this->Value;
	}

	int TargetID;

protected:
	int ID;

	vsNode* Source;
	vsNode* Target;

	int SourceID;

	float Weight;
	float Value;
};

#endif
