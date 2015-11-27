#ifndef MDICHILD_H
#define MDICHILD_H

//VIS_SENSE includes
//#include <Eigen/SparseCore>
#include "glwidget.h"
//Forward Qt class declarations
#include <QtCore/QTime>
#include <string>
using namespace std;
//Forward VIS-SENSE Qt class declarations
class executeSFDialog;
class createGraphFromClustersDialog;

//Forward VIS-SENSE class declarations
class vsCSVFile;
//class DockChild;
class QLabel;
class QProgressBar;
class QNetworkAccessManager;
class QNetworkReply;
class QTime;
class QProcess;
class MainWindow;
class MdiChild
{

public:
    MdiChild();

	void readFile(vector<string> files,std::vector<int> numberof_time_periods=std::vector<int>(),std::string name_of_feature="");
    void checkConsistency();
    void addToCSVList(vsCSVFile *f);
    void addDrawingVertices(int N);
    void setDrawingVertices();
    void setDrawnVertices();
    void RandomLayout();
    bool setPositions(int MaxFRRuns = 300,float dampenLast = 0.65f, float Dimension = 3000.0f, float restDistance = 10.0f, float repulseConstant =-1.0, float springConstant=-1.0);
    void checkExistenceOfNewFiles();
    void FRLayout(int MaxFRRuns = 300,float dampenLast =0.65f, float Dimension=3000.0f, float restDistance=10.0f, float repulseConstant = -1, float springConstant = -1);
    void writeVertexPositionstoFile2(QString outputPath);
    void toggleLayoutView(bool enabled);
    void setAbstractDrawingVertices(bool onlyDrawn);
    void calculateEdgeMeanSD(bool onlyDrawn);
    int getNoFeatures();

protected:

	MdiChild** DockWin;
	MdiChild *parentmdichild;
	int NoDockWins;

	vsCSVFile** csvList;
	int csvListSize;

	vsPoint Min;
	vsPoint Max;

	vsSize LastScreenSize;

	//Fruchterman-Reingold parameters
	bool FRRunning;
    int FRRuns;
    int MaxFRRuns;

	executeSFDialog *SFDialog;
	createGraphFromClustersDialog *graphFromClustersDialog;

	QString PathName;
	QNetworkAccessManager *m_network;


	QString folder_with_timeperiod;
	//void keyReleaseEvent(QKeyEvent *event);
	//QTime downloadTime;


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

    bool abstractView;

    vsSD *edgeStatistics;

    enum layout{Random, Geometric, FR, FromFile};
    layout Layout;
    bool PositionsSet;

    float CuttingThreshold;

};

#endif
