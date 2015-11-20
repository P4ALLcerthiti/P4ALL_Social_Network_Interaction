#ifndef VSFEATURE_H_
#define VSFEATURE_H_
#include <vector>
//Forward declarations
class vsNode;
class vsFilename;

class vsFeature{
public:
	vsFeature(int id, char* label);
	~vsFeature();

	int searchVertex(char* label);
	vsNode* searchVertex(vsNode*);
	vsNode* getVertex(char* label);
	vsNode* addVertex(char* label, bool &newVertexAdded);

	char* getLabel();
	int getID();

	std::vector<vsNode*> Vertex;
	int NoVertices;

	vsFilename *FileName;

protected:
	int ID;
	char* Label;
	float Weight;

};


#endif
