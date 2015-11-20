#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QT includes
#include <QMainWindow>
#include <string>
#include <Eigen/SparseCore>
#include<QMenu>
#include<QMenuBar>
#include<QToolBar>
#include<QStatusBar>
#include<QDockWidget>
#include<QMdiSubWindow>
using namespace std;
// Forward Qt class declarations
QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class DockWidgetArea;
QT_END_NAMESPACE

// Forward VIS-SENSE declarations
class MdiChild;
class DynamicAD_GUISelectDataForm;
class DynamicAD_GUISelectDataForm2;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(string ticketurl,string ticket,string timeperiod_url,string dataset_url,string staticgraphdata_url,string dynamicgraphdata_url,QString folder_with_timeperiod,QString timeperiod,string dataset_name);
	~MainWindow();
	void deleteDirectory(QString directory_to_delete);
	bool removeDir(const QString &dirName);
	void executeDynamicADAlgorithm2_atbeginning();
	//bool DeleteFolder( const CString& csDeleteFolderPath_i );
	void updateDockWindows();
	int dynamic_nummeroffeatures;
	vector<std::string> names_of_files;
	vector<Eigen::MatrixXd> MatP;
	int number_of_days;
	QMdiArea *mdiArea;

    MdiChild* activeMdiChild();
    MdiChild* createMdiChild();

public slots:
	void newFile(vector<std::string> names_of_files=vector<std::string> (),vector<Eigen::MatrixXd> MatP=vector<Eigen::MatrixXd>(),int number_of_days=0,int dynamic_numberoffeatures=0,int specific_day=0, vector<int> selectedvertices=vector<int>(),vector<bool> is_vector_i_selected=vector<bool>());
	//void newfile_atbeginning();
private slots:
	void setActiveSubWindow(QWidget *window);
	void updateMenus();
	void updateWindowMenu();
	//void contextMenuEvent(QContextMenuEvent *event);

	
	void openFile();
	void addFile();
	void createGraphFromClusters();
	void createGraphFromSelectedVertices();
	void printScreen();

	void setLayout();
	void increaseCuttingThreshold();
	void reduceCuttingThreshold();

	void increaseVertexSize();
	void reduceVertexSize();
	void increaseVertexTransparency();
	void reduceVertexTransparency();
	void toggleFeatureValuesView();
	void toggleEdgesView();
	void increaseEdgeTransparency();
	void reduceEdgeTransparency();
	void toggleLabelsView();

	void toggleClustersView();
	void toggleAnomaliesView();

	void importClusteringResults();
	void executeStochasticFlowsAlgorithm();

	void executeStaticADAlgorithm();
	void slot_executeDynamicADAlgorithm();
	void slot_executeDynamicADAlgorithm2();

	//REM:
	void slotTestDynamicAlgorithm();
	//:REM
	void display_cas();

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();

	void createDockWindow();
	

	bool maybeSave();

    QMdiSubWindow *findMdiChild(const QString &fileName);

    
    QSignalMapper *windowMapper;

	QMenu *fileMenu;
	QMenu *createGraphFromMenu;
	//--------------------------------------
	QMenu *layoutMenu;
	//--------------------------------------
	QMenu *clusteringMenu;
	//--------------------------------------
	QMenu *ADMenu;
	//--------------------------------------
	QMenu *windowMenu;
	QMenu *dockMenu;

	QToolBar *fileToolBar;
	QToolBar *layoutToolBar;
	QToolBar *graphViewToolBar;
	QToolBar *clusteringToolBar;

	QAction *newAction;
	QAction *addFileAction;
	QAction *openAction;
	//QAction *createGraphFromSelectedAction;
	QAction *createGraphFromSelectedVerticesAction;
	QAction *createGraphFromClustersAction;
	QAction *printScreenAction;
	QAction *quitAction;
	//--------------------------------------
	QAction *abstractLayoutAction;
	QAction *increaseCuttingThresholdAction;
	QAction *reduceCuttingThresholdAction;
	//--------------------------------------
	QAction *importClusteringResultsAction;
	QAction *SFAction;
	//-------------------------------------
	QAction *staticADAction;
	QAction *m_actionDynamicAD;
	QAction *m_actionDynamicAD2;
	//-------------------------------------
	QAction *toggleDockWindowsViewAction;
	QAction *closeAct;
	QAction *closeAllAct;
	QAction *tileAct;
	QAction *cascadeAct;
	QAction *nextAct;
	QAction *previousAct;
	QAction *separatorAct;
	//-------------------------------------
	QAction *increaseVertexSizeAction;
	QAction *reduceVertexSizeAction;
	QAction *increaseVertexTransparencyAction;
	QAction *reduceVertexTransparencyAction;
	QAction *showFeatureValuesAction;
	QAction *showEdgesAction;
	QAction *increaseEdgeTransparencyAction;
	QAction *reduceEdgeTransparencyAction;
	QAction *increaseEdgeWeightAction;
	QAction *reduceEdgeWeightAction;
	QAction *showLabelsAction;
	//-------------------------------------
	QAction *showAnomaliesAction;
	QAction *showClustersAction;

	QAction *m_actionTestDynamic; //for debugging the dynamic case...
	QAction *casAction;
	MdiChild* lastShownMdiChild;

	QDockWidget **Dock;
	int NoDocks;
	int NoLastShownDocks;

    DynamicAD_GUISelectDataForm* m_guiDynAD_SelectDataForm;
    DynamicAD_GUISelectDataForm2* m_guiDynAD_SelectDataForm2;
	string ticketurl;
	string ticket;
	string dataurl;
	
	int number_matp_widgets;
	
	int dynamic_HitVertex;
	MainWindow* parentwindow;
	bool matp_inserted;
	string timeperiod_url;
	string dataset_url;
	string staticgraphdata_url;
	string dynamicgraphdata_url;
	QString folder_with_timeperiod;
	QString timeperiod;
	string dataset_name;
	//void keyReleaseEvent(QKeyEvent *event);
protected:
};

#endif // MAINWINDOW_H
