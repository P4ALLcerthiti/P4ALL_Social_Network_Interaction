#ifndef MDICHILD_H
#define MDICHILD_H

//VIS_SENSE includes
#include <Eigen/SparseCore>
#include "dynamic_matp_widget.h"
#include "dynamic_bar_widget.h"
#include "glwidget.h"
#include "DynamicAD_GUISelect_Feature.h"
//Forward Qt class declarations
#include <QtCore/QTime>
#include <string>
#include<QMdiArea>
#include<QMessageBox>
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
class MdiChild : public GLWidget
{
    Q_OBJECT

public:
    MdiChild(QWidget *parent = NULL,MainWindow* parentwindow=NULL, vsGraph *graph = NULL, int drawingfeature = 0,MdiChild *parentmdichild=NULL);

	bool loadFile(string ticketurl,string ticket,string dataurl,QString folder_with_timeperiod,const char *s = "csv",vector<std::string> names_of_files=vector<std::string> (),vector<Eigen::MatrixXd> MatP=vector<Eigen::MatrixXd>(),int number_of_days=0,int dynamic_numberoffeatures=0,int specific_day=0,vector<int> selectedvertices=vector<int > (),vector<bool> is_vector_i_selected=vector<bool>());
	//bool loadFiles_atbeginning(QString folder_with_timeperiod);
	void readFile(vector<string> files,std::vector<int> numberof_time_periods=std::vector<int>(),std::string name_of_feature="");
	bool checkFeatureExistence(const QString &fileName);

	MdiChild* getDockWin(int i);
	dynamic_matp_widget* getDockWin_matp(){ return DockWin_matp;}
	dynamic_bar_widget* getDockWin_bar(){ return DockWin_bar;}
	vsGraph* createGraphFromClusters();
	vsGraph* createGraphFromSelectedVertices();
	
	bool importClusteringResults(string ticketurl,string ticket,string dataurl);
	bool importClusteringResults_atbeginning(string ticketurl,string ticket,string dataurl);
	void executeSF();
	bool hasClusteringResults();
	void executeStaticAD();
	bool hasADResults();
	//virtual void update_parent(){update();}
    /*void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
	*/

	void paint(QPainter *painter, QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void update_parent_mainwindow(int Hitvertex,vector<int> selectedvertices,bool ctrl_clicked,vector<bool> is_vector_i_selected);
	bool matp_has_to_be_inserted;
	bool matp_inserted;
	MainWindow* parentwindow;
	void emitupdateApplicationMenus(){emit this->updateApplicationMenus();}

public slots:
	void ShowContextMenu(const QPoint& pos);
	void ShowMatpChoices(const QPoint& pos);
	void ShowbarChoices(const QPoint& pos);
	void toggleLayoutView(bool);
	void toggleCuttingThreshold(float);

	void toggleVertexSize(int click);
	void toggleVertexTransparency(int click);
	void toggleFeatureValuesView(bool);
	void toggleEdgesView(bool);
	void toggleEdgeTransparency(int click);
	void toggleLabelsView(bool);
	void VertexSelection(int id);

	void toggleClustersView(bool);
	void toggleAnomaliesView(bool);

	void setDrawnVertices();
	void dynamic_matp_set_numbers_displayed();
	void dynamic_matp_set_dates_displayed();
	void slotRequestFinished(QNetworkReply *reply);
	void slotSetProgress(qint64,qint64);
	void dynamic_matp_set_L1_clicked();
	void dynamic_matp_set_L2_clicked();
	void dynamic_matp_set_eyclidean_clicked();
	void dynamic_matp_set_nodeoperations_clicked();
	void dynamic_matp_set_edgeoperations_clicked();
	void dynamic_matp_set_graphoperations_clicked();
	void dynamic_matp_show_hide_selected_clicked();

	void dynamic_bar_set_numbers_displayed();
	void dynamic_bar_set_dates_displayed();
	void dynamic_bar_show_only_selected_clicked();
public:
    //void closeEvent(QCloseEvent *event);
	void setPathName(QString);
	QString getPathName();


signals:
	void updateApplicationMenus();

	void changedLayoutView(bool);
	void changedCuttingThreshold(float);

	void changedVertexSize(int);
	void changedVertexTransparency(int);
	void changedFeatureValuesView(bool);
	void changedEdgesView(bool);
	void changedEdgeTransparency(int);
	void changedLabelsView(bool);

	void changedClustersView(bool);
	void changedAnomaliesView(bool);

private slots:
    //void documentWasModified();
public:
        bool setPositions(float dampenLast = 0.65f, float Dimension = 3000.0f, float restDistance = 10.0f, float epsilon = 0.0000001f, float repulseConstant =-1.0, float springConstant=-1.0);
        void writeVertexPositionstoFile2(QString outputPath);

protected:
    /*bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    bool isUntitled;
	*/
	
	void createDockWin();
	void createDockWin_matp(vector<Eigen::MatrixXd> MatP,int dynamic_numberoffeatures);
	void createDockWin_bar(vector<Eigen::MatrixXd> MatP,int dynamic_numberoffeatures);
	void RandomLayout();
	void GeometricLayout();
    void FRLayout(float dampenLast =0.65f, float Dimension=3000.0f, float restDistance=10.0f, float epsilon=0.0000001f, float repulseConstant = -1, float springConstant = -1);


	bool loadVertexPositionsFile();
	bool readVertexPositionsfromFile(vsCSVFile *f);
	void writeVertexPositionstoFile();


	void calculateGraphPhysicalBorders();
	void setDrawingPositions();

	void checkConsistency();
	void addDrawingVertices(int N);
	void setDrawingVertices();

	void postFileAdditionActions();
	void addToCSVList(vsCSVFile* f);
	void addCSVFileToGraph();
	void checkExistenceOfNewFiles();

	int translateIDintoGlobalValue(int);

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
	dynamic_matp_widget* DockWin_matp ;
	dynamic_bar_widget* DockWin_bar;
	QString PathName;
	QNetworkAccessManager *m_network;
	QAction *dynamic_numbers_displayed_matp;
	QAction *dynamic_dates_displayed_matp;
	QAction* L1_matp ;
	QAction* L2_matp ;
	QAction* eyclidean_matp ;
	QAction* nodeoperations_matp ;
	QAction* edgeoperations_matp ;
	QAction* graphoperations_matp ;
	QAction* show_selected_graphs_clicked_matp;


	QAction *dynamic_numbers_displayed_bar;
	QAction *dynamic_dates_displayed_bar;
	QAction* show_selected_graphs_clicked_bar;
	QString folder_with_timeperiod;
	//void keyReleaseEvent(QKeyEvent *event);
	//QTime downloadTime;
};

#endif
