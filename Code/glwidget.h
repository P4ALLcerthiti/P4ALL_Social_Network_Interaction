#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include </usr/include/qt4/QtOpenGL/QGLWidget>
#include <QGLWidget>

#include <QBrush>
#include <QFont>
#include <QPen>
#include<QAction>
#include <math.h>
#include "vsNode.h"
#include "vsFeature.h"
#include<QApplication>
//#include "mdichild.h"
//Forward Qt declarations
QT_BEGIN_NAMESPACE
class QPaintEvent;
class QWidget;
//class MdiChild;
QT_END_NAMESPACE

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
		//this->Weight = t->Map->NoEdges;
		this->Source = s;
		this->Target = t;

		float weight = s->Map->searchEdge(t->Map)->getWeight();
		this->ActualWeight = weight * (float)s->Map->NoEdges / (float)s->Map->Size;
		//this->ActualWeight = weight;
		//this->ActualWeight = weight * (float)t->Map->NoEdges / (float)s->Map->NoEdges;
		//this->ActualWeight = weight * (float)t->Map->NoEdges; //KALO!!!
		//this->ActualWeight = weight * (float)s->Map->NoEdges / (float)s->Map->Size;
		//this->ActualWeight = weight * weight / sqrt((float)(s->Map->NoEdges * t->Map->NoEdges));
		//this->ActualWeight = weight / sqrt((float)(s->Map->NoEdges * t->Map->NoEdges));
		//float r = this->ActualWeight;

		float r = s->Map->NoEdges / (float)t->Map->NoEdges;
		if (r > 1)
			r = 1 / r;
		float E = 5 * log10(weight);
		//r = weight / sqrt((float)(s->Map->NoEdges * t->Map->NoEdges)); float E = 5 * log10(weight * r); //KALO
		//r = this->ActualWeight; float E = 5 * log10(weight) * r;
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

class GLWidget : public QGLWidget
//class GLWidget : public QWidget
{
	Q_OBJECT

public:
	//explicit GLWidget(QWidget *parent = NULL, vsGraph *graph = NULL, int drawingfeature = 0);
	explicit GLWidget(QWidget *parent, vsGraph *graph, int drawingfeature);
	~GLWidget();

	void setGraphDisplayOptions(bool, float, bool, bool, bool);
	vsGraph* getGraph();
	int getNoFeatures();

	bool abstractView;
	bool displayFeatureValues;
	bool displayEdges;
	bool displayLabels;
	bool displayClusters;
	bool displayAnomalies;

	bool hasSelectedVertices;
	bool hasClusteringResult;
	bool hasADResult;

	bool writePositionsToFile;

	int NoColors;
	vsColor **brushColor;
	vsColor **penColor;

	bool isVisible;
	
public slots:
    void animate();

	void setLayoutView(bool);
	void setCuttingThreshold(float);

	void setVertexSize(int);
	void setVertexTransparency(int);
	void setFeatureValuesView(bool);
	void setEdgesView(bool);
	void setEdgeTransparency(int);
	void setLabelsView(bool);

	void setClustersView(bool);
	void setAnomaliesView(bool);
	void unsetSelected();
	
	void setView(bool);
	virtual void setDrawnVertices();

signals:
	//void valueChanged();
	void refresh();
	void selectedVertex(int);
	void clearSelected();

	void changedDrawnVertices(bool, int);

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void paint(QPainter *painter, QPaintEvent *event);
	virtual void paintVertices(QPainter *painter);
	virtual void paintEdges(QPainter *painter);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent * event);
	virtual void emitupdateApplicationMenus()=0;

	void setupColors();

	void calculateVertexSize();

	void copySelected();

	void calculateEdgeMeanSD(bool onlyDrawn);
	void setAbstractDrawingVertices(bool onlyDrawn);


	QBrush background;
	QBrush circleBrush;
	QPen circlePen;
	QFont textFont;
	QPen textPen;
	QBrush selectedBrush;
	QPen selectedPen;

	QTimer *timer;

	QPoint lastPos;
	QPoint lastClickedPos;
	int HitVertex;
	bool mousePressed;
	QPoint moveD;
	float scaleFactor;
	QPoint drawingBorder;
	bool drawSelectionBox;
	QRect selectionBox;

	enum layout{Random, Geometric, FR, FromFile};
	layout Layout;
	bool PositionsSet;

	vsGraph *Graph;
	int DrawingFeature;
	vsGraph *displayGraph;
	vsGraph *setNewGraph;

	vsDrawingVertex* Vertex;
	int NoDrawingVertices;
	int NoDrawingEvents;
	int NoDrawingFeatureValues;
	int NoShownVertices;

	vsDrawingEdge* Edge;
	int NoEdges;

	int VertexOpacity;
	int EdgeOpacity;
	int EdgeWidth;
	int dVertexSize;
	int VertexSize;
	int ReferenceVertexSize;
	int minVertexSize;
	int maxVertexSize;

	QSize DrawingBox;

	float CuttingThreshold;
	//float EdgeMean;
	//float EdgeSD;

	QAction *isShownAction;

	vsSD *edgeStatistics;
private:
	//QPointF** V;
	//QPointF** DP;

};

#endif // WIDGET_H
