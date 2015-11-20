//Qt includes
#include <QtGui>

//VIS-SENSE includes
#include "vsCSVFile.h"

//Other includes
#include "mainwindow.h"
#include "mdichild.h"
#include <iostream>

//REM:
#include "DynamicAD_GUISelect_Feature.h"
#include "DynamicAD_GUISelectDataForm2.h"
#include "DynamicAD_GUISelectDataForm.h"
#include <Eigen/SparseCore>
#include <fstream>
#include "DynamicAD.h"
//#include "DynamicAD_GUIPlot.h"
//:REM
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include "teststauros2.h"
#include "display_data_from_cas.h"
//#include <atlstr.h>
//#include <iostream>
#include <cstring>
#include<QMdiArea>
MainWindow::MainWindow(string ticketurl,string ticket,string timeperiod_url,string dataset_url,string staticgraphdata_url,string dynamicgraphdata_url,QString folder_with_timeperiod,QString timeperiod,string dataset_name) : QMainWindow()
{
#ifdef NDEBUG
	setWindowTitle(tr("VIS-SENSE Demo"));
#else
    setWindowTitle(tr("VIS-SENSE Attack Attribution Graphs"));
#endif
	fprintf(stderr,ticketurl.c_str());fflush(stderr);
	fprintf(stderr,ticket.c_str());fflush(stderr);
	fprintf(stderr,dataurl.c_str());fflush(stderr);
	this->resize(800, 600);

	this->setWindowIcon(QIcon(QString::fromUtf8("images/vslogo.png")));
	this->ticketurl=ticketurl;
	this->ticket=ticket;
	this->timeperiod_url=timeperiod_url;
	this->dataset_url=dataset_url;
	this->staticgraphdata_url=staticgraphdata_url;
	this->dynamicgraphdata_url=dynamicgraphdata_url;
	this->folder_with_timeperiod=folder_with_timeperiod;
	this->timeperiod=timeperiod;
	this->dataset_name=dataset_name;
	//this->setIconSize(QSize(42, 42)); //Sets the Size of all icons present in the Window

	//QWidget *pic = new QWidget(this);
    //pic->setStyleSheet("background-color:#FFF; background-image: url(images/logo.png); background-repeat:no-repeat; background-attachment:fixed; background-position: center");
    //this->setCentralWidget(pic);

	//Widget *native = new Widget(&helper, this);
//GLWidget *openGL = new GLWidget(this);
	//QLabel *nativeLabel = new QLabel(tr("Native"));
	//nativeLabel->setAlignment(Qt::AlignHCenter);
	//QLabel *openGLLabel = new QLabel(tr("OpenGL"));
	//openGLLabel->setAlignment(Qt::AlignHCenter);

	/*QGridLayout *layout = new QGridLayout;
	layout->addWidget(native, 0, 0);
	layout->addWidget(openGL, 0, 1);
	layout->addWidget(nativeLabel, 1, 0);
	layout->addWidget(openGLLabel, 1, 1);
	setLayout(layout);*/

	/*QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), native, SLOT(animate()));
	connect(timer, SIGNAL(timeout()), openGL, SLOT(animate()));
	timer->start(50);*/

	//this->mainVSGraph = NULL;

	//this->setCentralWidget(openGL);

    this->lastShownMdiChild = NULL;
	this->NoLastShownDocks = 0;

	this->Dock = NULL;
 	this->NoDocks = 0;

	this->mdiArea = new QMdiArea;
    this->mdiArea->setViewMode(QMdiArea::TabbedView);
    this->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //this->mdiArea->setBackground(Qt::white);
    this->setCentralWidget(this->mdiArea);

	connect(this->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    this->windowMapper = new QSignalMapper(this);
    connect(this->windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));


	this->createActions();
	this->createMenus();
	this->createToolBars();
	this->createStatusBar();

	this->updateMenus();

	m_guiDynAD_SelectDataForm = NULL;
	m_guiDynAD_SelectDataForm2=NULL;
	number_matp_widgets=0;
	this->dynamic_HitVertex=0;
	this->matp_inserted=false;
	//this->setFocus();
}

MainWindow::~MainWindow()
{
	if (m_guiDynAD_SelectDataForm) 
		delete m_guiDynAD_SelectDataForm;
	deleteDirectory(this->folder_with_timeperiod);
	

}

void MainWindow::createActions(){

	this->newAction = new QAction(QIcon("images/new.png"), tr("&New"), this);
	this->newAction->setShortcuts(QKeySequence::New);
	this->newAction->setStatusTip(tr("Create a new file"));
	this->connect(this->newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	this->addFileAction = new QAction(QIcon("images/addfeature.png"), tr("&Add Feature"), this);
	this->addFileAction->setStatusTip(tr("Add an existing file"));
	this->connect(this->addFileAction, SIGNAL(triggered()), this, SLOT(addFile()));
 	//this->addFileAction->setEnabled(false);

	this->openAction = new QAction(QIcon("images/open.png"), tr("&Open"), this);
	this->openAction->setShortcuts(QKeySequence::Open);
	this->openAction->setStatusTip(tr("Open an existing file"));
	this->connect(this->openAction, SIGNAL(triggered()), this, SLOT(openFile()));

	//this->createGraphFromSelectedAction = new QAction(tr("&Selected Vertices"), this);

	this->createGraphFromClustersAction = new QAction(tr("&Cluster(s)"), this);
	this->createGraphFromClustersAction->setStatusTip(tr("Create Graph from cluster(s)"));
	this->connect(this->createGraphFromClustersAction, SIGNAL(triggered()), this, SLOT(createGraphFromClusters()));
	
	this->createGraphFromSelectedVerticesAction = new QAction(tr("&Selected Vertices"), this);
	this->createGraphFromSelectedVerticesAction->setStatusTip(tr("Create Graph from selected vertices"));
	this->connect(this->createGraphFromSelectedVerticesAction, SIGNAL(triggered()), this, SLOT(createGraphFromSelectedVertices()));
	
	this->printScreenAction = new QAction(tr("&Print Screen"), this);
	this->printScreenAction->setShortcuts(QKeySequence::Print);
	this->printScreenAction->setStatusTip(tr("Save a screenshot in a JPEG file."));
	this->connect(this->printScreenAction, SIGNAL(triggered()), this, SLOT(printScreen()));

	this->quitAction = new QAction(tr("&Quit"), this);
	this->quitAction->setShortcuts(QKeySequence::Quit);
	this->quitAction->setStatusTip(tr("Quit the application"));
	connect(this->quitAction, SIGNAL(triggered()), this, SLOT(close()));

	//REM:
	m_actionTestDynamic = new QAction(tr("Test..."), this);
    //m_actionTestDynamic->setShortcut(Qt::Key_C);
	m_actionTestDynamic->setStatusTip(tr("Test!!!"));
	connect(m_actionTestDynamic, SIGNAL(triggered()), this, SLOT(slotTestDynamicAlgorithm()));
	//:REM

	this->abstractLayoutAction = new QAction(tr("&Abstract Layout"), this);
	this->abstractLayoutAction->setStatusTip(tr("Set Abstract Graph Layout"));
	this->abstractLayoutAction->setCheckable(true);
	this->abstractLayoutAction->setChecked(false);
	connect(this->abstractLayoutAction, SIGNAL(triggered()), this, SLOT(setLayout()));

	this->increaseCuttingThresholdAction = new QAction(QIcon("images/vs+.png"), tr("Increase Cutting Threshold"), this);
	this->increaseCuttingThresholdAction->setStatusTip(tr("Increase Cutting Threshold"));
	connect(this->increaseCuttingThresholdAction, SIGNAL(triggered()), this, SLOT(increaseCuttingThreshold()));
	connect(this->abstractLayoutAction, SIGNAL(triggered(bool)), this->increaseCuttingThresholdAction, SLOT(setEnabled(bool)));

	this->reduceCuttingThresholdAction = new QAction(QIcon("images/vs-.png"), tr("Reduce Cutting Threshold"), this);
	this->reduceCuttingThresholdAction->setStatusTip(tr("Reduce Cutting Threshold"));
	connect(this->reduceCuttingThresholdAction, SIGNAL(triggered()), this, SLOT(reduceCuttingThreshold()));
	connect(this->abstractLayoutAction, SIGNAL(triggered(bool)), this->reduceCuttingThresholdAction, SLOT(setEnabled(bool)));


	this->importClusteringResultsAction = new QAction(tr("&Import Clustering Data..."), this);
	this->importClusteringResultsAction->setStatusTip(tr("Import clustering results."));
	connect(this->importClusteringResultsAction, SIGNAL(triggered()), this, SLOT(importClusteringResults()));

	this->SFAction = new QAction(tr("&Stochastic Flows..."), this);
	this->SFAction->setStatusTip(tr("Run the stochastic flows clustering algorithm."));
	connect(this->SFAction, SIGNAL(triggered()), this, SLOT(executeStochasticFlowsAlgorithm()));

	this->staticADAction = new QAction(tr("&Static Anomaly Detection..."), this);
	this->staticADAction->setStatusTip(tr("Run the anomaly detection algorithm for a graph."));
	connect(this->staticADAction, SIGNAL(triggered()), this, SLOT(executeStaticADAlgorithm()));

    m_actionDynamicAD = new QAction(tr("&Dynamic Anomaly Detection (qwt)..."), this);
	m_actionDynamicAD->setStatusTip(tr("Run the anomaly detection algorithm for a time series."));
    //m_actionDynamicAD->setShortcut(Qt::Key_D);
	connect(this->m_actionDynamicAD, SIGNAL(triggered()), this, SLOT(slot_executeDynamicADAlgorithm()));
	
    m_actionDynamicAD2 = new QAction(tr("&Dynamic Anomaly Detection..."), this);
	m_actionDynamicAD2->setStatusTip(tr("Run the anomaly detection algorithm for a time series."));
	//m_actionDynamicAD->setShortcut(Qt::Key_D);
	connect(this->m_actionDynamicAD2, SIGNAL(triggered()), this, SLOT(slot_executeDynamicADAlgorithm2()));

	this->toggleDockWindowsViewAction = new QAction(tr("&Show/Hide All"), this);
	this->toggleDockWindowsViewAction->setStatusTip(tr("Show/Hide all docked windows."));
	this->toggleDockWindowsViewAction->setCheckable(true);
	this->toggleDockWindowsViewAction->setChecked(true);

	this->closeAct = new QAction(tr("Cl&ose"), this);
	this->closeAct->setStatusTip(tr("Close the active window"));
	connect(this->closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

	closeAllAct = new QAction(tr("Close &All"), this);
	closeAllAct->setStatusTip(tr("Close all the windows"));
	connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

	tileAct = new QAction(tr("&Tile"), this);
	tileAct->setStatusTip(tr("Tile the windows"));
	connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

	cascadeAct = new QAction(tr("&Cascade"), this);
	cascadeAct->setStatusTip(tr("Cascade the windows"));
	connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

	nextAct = new QAction(tr("Ne&xt"), this);
	nextAct->setShortcuts(QKeySequence::NextChild);
	nextAct->setStatusTip(tr("Move the focus to the next window"));
	connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

	previousAct = new QAction(tr("Pre&vious"), this);
	previousAct->setShortcuts(QKeySequence::PreviousChild);
	previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(this->previousAct, SIGNAL(triggered()), this->mdiArea, SLOT(activatePreviousSubWindow()));

	this->separatorAct = new QAction(this);
	this->separatorAct->setSeparator(true);

	this->showFeatureValuesAction = new QAction(QIcon("images/f.png"), tr("Show/Hide Graph Feature Vertices"), this);
	this->showFeatureValuesAction->setCheckable(true);
	//this->showFeatureValuesAction->setChecked(true);
	this->showFeatureValuesAction->setStatusTip(tr("Show/Hide Graph Feature Vertices"));
	connect(this->showFeatureValuesAction, SIGNAL(triggered()), this, SLOT(toggleFeatureValuesView()));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->increaseVertexTransparencyAction = new QAction(QIcon("images/vt+.png"), tr("Increase Vertex Transparency"), this);
	this->increaseVertexTransparencyAction->setStatusTip(tr("Increase Vertex Transparency"));
	connect(this->increaseVertexTransparencyAction, SIGNAL(triggered()), this, SLOT(increaseVertexTransparency()));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->reduceVertexTransparencyAction = new QAction(QIcon("images/vt-.png"), tr("Reduce Vertex Transparency"), this);
	this->reduceVertexTransparencyAction->setStatusTip(tr("Reduce Vertex Transparency"));
	connect(this->reduceVertexTransparencyAction, SIGNAL(triggered()), this, SLOT(reduceVertexTransparency()));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->increaseVertexSizeAction = new QAction(QIcon("images/vs+.png"), tr("Increase Vertex Size"), this);
	this->increaseVertexSizeAction->setStatusTip(tr("Increase Vertex Size"));
	connect(this->increaseVertexSizeAction, SIGNAL(triggered()), this, SLOT(increaseVertexSize()));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->reduceVertexSizeAction = new QAction(QIcon("images/vs-.png"), tr("Reduce Vertex Size"), this);
	this->reduceVertexSizeAction->setStatusTip(tr("Reduce Vertex Size"));
	connect(this->reduceVertexSizeAction, SIGNAL(triggered()), this, SLOT(reduceVertexSize()));

	this->showEdgesAction = new QAction(QIcon("images/e.png"), tr("Show/Hide Graph Edges"), this);
	this->showEdgesAction->setCheckable(true);
	//this->showEdgesAction->setChecked(false);
	this->showEdgesAction->setStatusTip(tr("Show/Hide Graph Edges"));
	connect(this->showEdgesAction, SIGNAL(changed()), this, SLOT(toggleEdgesView()));
	connect(this->showFeatureValuesAction, SIGNAL(triggered(bool)), this->showEdgesAction, SLOT(setEnabled(bool)));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->increaseEdgeWeightAction = new QAction(QIcon("images/ew+.png"), tr("Increase Edge Weight"), this);
	this->increaseEdgeWeightAction->setStatusTip(tr("Increase Edge Weight"));
	connect(this->showEdgesAction, SIGNAL(toggled(bool)), this->increaseEdgeWeightAction, SLOT(setEnabled(bool)));
	connect(this->showFeatureValuesAction, SIGNAL(triggered(bool)), this->increaseEdgeWeightAction, SLOT(setEnabled(bool)));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->reduceEdgeWeightAction = new QAction(QIcon("images/ew-.png"), tr("Reduce Edge Weight"), this);
	this->reduceEdgeWeightAction->setStatusTip(tr("Reduce Edge Weight"));
	connect(this->showEdgesAction, SIGNAL(toggled(bool)), this->reduceEdgeWeightAction, SLOT(setEnabled(bool)));
	connect(this->showFeatureValuesAction, SIGNAL(triggered(bool)), this->reduceEdgeWeightAction, SLOT(setEnabled(bool)));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->increaseEdgeTransparencyAction = new QAction(QIcon("images/et+.png"), tr("Increase Edge Transparency"), this);
	this->increaseEdgeTransparencyAction->setStatusTip(tr("Increase Edge Transparency"));
	connect(this->increaseEdgeTransparencyAction, SIGNAL(triggered()), this, SLOT(increaseEdgeTransparency()));
	connect(this->showEdgesAction, SIGNAL(toggled(bool)), this->increaseEdgeTransparencyAction, SLOT(setEnabled(bool)));
	connect(this->showFeatureValuesAction, SIGNAL(triggered(bool)), this->increaseEdgeTransparencyAction, SLOT(setEnabled(bool)));
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->reduceEdgeTransparencyAction = new QAction(QIcon("images/et-.png"), tr("Reduce Edge Transparency"), this);
	this->reduceEdgeTransparencyAction->setStatusTip(tr("Reduce Edge Transparency"));
	connect(this->reduceEdgeTransparencyAction, SIGNAL(triggered()), this, SLOT(reduceEdgeTransparency()));
	connect(this->showEdgesAction, SIGNAL(toggled(bool)), this->reduceEdgeTransparencyAction, SLOT(setEnabled(bool)));
	connect(this->showFeatureValuesAction, SIGNAL(triggered(bool)), this->reduceEdgeTransparencyAction, SLOT(setEnabled(bool)));
 
	this->showLabelsAction = new QAction(QIcon("images/l.png"), tr("Show/Hide Graph Labels"), this);
	this->showLabelsAction->setCheckable(true);
	//this->showLabelsAction->setChecked(false);
	this->showLabelsAction->setStatusTip(tr("Show/Hide Graph Labels"));
	connect(this->showLabelsAction, SIGNAL(changed()), this, SLOT(toggleLabelsView()));

	this->showClustersAction = new QAction(QIcon("images/cl.png"), tr("Show/Hide Clusters"), this);
	this->showClustersAction->setStatusTip(tr("Show/Hide Clusters"));
	this->showClustersAction->setCheckable(true);
	//this->showClustersAction->setChecked(false);
	connect(this->showClustersAction, SIGNAL(triggered()), this, SLOT(toggleClustersView()));
 
	this->showAnomaliesAction = new QAction(QIcon("images/ad.png"), tr("Show/Hide Anomalous Vertices"), this);
	this->showAnomaliesAction->setStatusTip(tr("Show/Hide Anomalous Vertices"));
	this->showAnomaliesAction->setCheckable(true);
	//this->showAnomaliesAction->setChecked(false);
	//this->showAnomaliesAction->setEnabled(false);
	connect(this->showAnomaliesAction, SIGNAL(triggered()), this, SLOT(toggleAnomaliesView()));

	this->casAction = new QAction( tr("&cas_results"), this);
	this->casAction->setStatusTip(tr("Display CAS results"));
	this->connect(this->casAction, SIGNAL(triggered()), this, SLOT(display_cas()));
}
/*
void MainWindow::contextMenuEvent(QContextMenuEvent *event){
	QMenu menu(this);
	menu.addAction(this->abstractLayoutAction);
	//menu.addAction(copyAct);
	//menu.addAction(pasteAct);
	menu.exec(event->globalPos());
}
*/
void MainWindow::setLayout(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild)
		activeChild->toggleLayoutView(this->abstractLayoutAction->isChecked());
}

void MainWindow::increaseCuttingThreshold(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild)
		activeChild->toggleCuttingThreshold(0.1);
}

void MainWindow::reduceCuttingThreshold(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild)
		activeChild->toggleCuttingThreshold(-0.1);
}

void MainWindow::increaseVertexSize(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleVertexSize(1);
}

void MainWindow::reduceVertexSize(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleVertexSize(-1);
}

void MainWindow::increaseVertexTransparency(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleVertexTransparency(1);
}

void MainWindow::reduceVertexTransparency(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleVertexTransparency(-1);
}

void MainWindow::toggleFeatureValuesView(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleFeatureValuesView(this->showFeatureValuesAction->isChecked());
}

void MainWindow::toggleEdgesView(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleEdgesView(this->showEdgesAction->isChecked());
}

void MainWindow::increaseEdgeTransparency(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleEdgeTransparency(1);
}

void MainWindow::reduceEdgeTransparency(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleEdgeTransparency(-1);
}

void MainWindow::toggleLabelsView(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleLabelsView(this->showLabelsAction->isChecked());
}

void MainWindow::toggleAnomaliesView(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleAnomaliesView(this->showAnomaliesAction->isChecked());
}
 
void MainWindow::toggleClustersView(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL)
		activeChild->toggleClustersView(this->showClustersAction->isChecked());
}

void MainWindow::printScreen(){
	//MdiChild* activeChild = this->activeMdiChild();

//	if (activeChild != NULL){
		//QImage img(activeChild->size(), QImage::Format_RGB32);
		QImage img(this->size(), QImage::Format_RGB32);
		QPainter painter(&img);
		this->render(&painter);
		img.save("screen.png");
//	}


//	QPrinter printer;
//	printer.setOutputFormat(QPrinter::PdfFormat);
//	printer.setOutputFileName("nonwritable.pdf");
	//QPainter painter;
//	QPainter painter(&printer);
/*	if (! painter.begin(&printer)) { // failed to open file
		qWarning("failed to open file, is it writable?");
		return 1;
	}
	painter.drawText(10, 10, "Test");
	if (! printer.newPage()) {
		qWarning("failed in flushing page to disk, disk full?");
		return 1;
	}
	painter.drawText(10, 10, "Test 2");
	painter.end();
*/
//	this->render(&painter);

}

void MainWindow::createMenus(){

	//this->fileMenu = menuBar()->addMenu((ticketurl+"  "+ticket+"  "+"&File").c_str());
	this->fileMenu = menuBar()->addMenu("&File");
	this->fileMenu->addAction(this->newAction);
	this->fileMenu->addAction(this->addFileAction);
	this->fileMenu->addAction(this->openAction);
	this->fileMenu->addSeparator();
	this->createGraphFromMenu = this->fileMenu->addMenu(tr("&Create New Graph From"));
	//this->createGraphFromMenu->addAction(this->createGraphFromSelectedAction);
	this->createGraphFromMenu->addAction(this->createGraphFromSelectedVerticesAction);
	this->createGraphFromMenu->addAction(this->createGraphFromClustersAction);
	
	this->fileMenu->addSeparator();
	this->fileMenu->addAction(this->printScreenAction);
//this->createGraphFromMenu = this->fileMenu->addMenu(tr("&Create New Graph From"));
	//REM:
	fileMenu->addSeparator();
    //fileMenu->addAction(m_actionTestDynamic);
    //fileMenu->addAction(m_actionDynamicAD);
	fileMenu->addAction(m_actionDynamicAD2);
	//:REM
	this->fileMenu->addSeparator();
	this->fileMenu->addAction(this->quitAction);
    //this->fileMenu->addAction(this->casAction);

	this->layoutMenu = menuBar()->addMenu(tr("&Layout"));
	this->layoutMenu->addAction(this->abstractLayoutAction);

	this->clusteringMenu = menuBar()->addMenu(tr("&Clustering"));
	this->clusteringMenu->addAction(this->importClusteringResultsAction);
	this->clusteringMenu->addSeparator();
	this->clusteringMenu->addAction(this->SFAction);

	this->ADMenu = menuBar()->addMenu(tr("&Anomaly Detection"));
	this->ADMenu->addAction(this->staticADAction);

	//this->windowMenu = menuBar()->addMenu((dataurl+" "+"&Window").c_str());
    this->windowMenu = menuBar()->addMenu("&Window");
	this->updateWindowMenu();
    connect(this->windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

	this->dockMenu = this->windowMenu->addMenu(tr("&Dock Windows"));
}

void MainWindow::createToolBars(){
	QAction *separator1 = new QAction(this);
	separator1->setSeparator(true);
	QAction *separator2 = new QAction(this);
	separator2->setSeparator(true);
	QAction *separator3 = new QAction(this);
	separator3->setSeparator(true);
	QAction *separator4 = new QAction(this);
	separator4->setSeparator(true);


	this->fileToolBar = addToolBar(tr("File"));
	this->fileToolBar->addAction(this->newAction);
	this->fileToolBar->addAction(this->addFileAction);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->fileToolBar->addAction(separator1);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->fileToolBar->addAction(this->openAction);

	this->layoutToolBar = addToolBar(tr("Layout"));
	this->layoutToolBar->addAction(this->increaseCuttingThresholdAction);
	this->layoutToolBar->addAction(this->reduceCuttingThresholdAction);

	this->graphViewToolBar = addToolBar(tr("Graph View Properties"));
	this->graphViewToolBar->addAction(this->increaseVertexTransparencyAction);
	this->graphViewToolBar->addAction(this->reduceVertexTransparencyAction);
	this->graphViewToolBar->addAction(this->increaseVertexSizeAction);
	this->graphViewToolBar->addAction(this->reduceVertexSizeAction);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->graphViewToolBar->addAction(separator2);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->graphViewToolBar->addAction(this->showFeatureValuesAction);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->graphViewToolBar->addAction(separator3);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->graphViewToolBar->addAction(this->showEdgesAction);
	this->graphViewToolBar->addAction(this->increaseEdgeWeightAction);
	this->graphViewToolBar->addAction(this->reduceEdgeWeightAction);
	this->graphViewToolBar->addAction(this->increaseEdgeTransparencyAction);
	this->graphViewToolBar->addAction(this->reduceEdgeTransparencyAction);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->graphViewToolBar->addAction(separator4);
	//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	this->graphViewToolBar->addAction(this->showLabelsAction);
	this->graphViewToolBar->setEnabled(false);

	this->clusteringToolBar = addToolBar(tr("Clustering"));
	this->clusteringToolBar->addAction(this->showClustersAction);
	this->clusteringToolBar->addAction(this->showAnomaliesAction);
}

void MainWindow::createStatusBar(){
	
	this->statusBar()->showMessage(tr("Ready"));
}

MdiChild* MainWindow::createMdiChild(){
	MdiChild* child = new MdiChild(this,this);

/*	QScrollArea *sa = new QScrollArea();
	sa->setWidgetResizable( true );
	sa->setWidget(child);
	this->mdiArea->addSubWindow(sa);
*/
	//connect(child, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
	//connect(child, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
	//connect(child, SIGNAL(selectedVertex(int)), this, SLOT(updateMenus()));

    return child;
}

void MainWindow::newFile(vector<std::string> names_of_files,vector<Eigen::MatrixXd> MatP,int number_of_days,int dynamic_numberoffeatures,int specific_day,vector<int> selectedvertices,vector<bool> is_vector_i_selected){
    //QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    fprintf(stderr, "Creating Main Window ... ");fflush(stderr);
    MdiChild* child = this->createMdiChild();

    if (child->loadFile(ticketurl,ticket,dataurl,this->folder_with_timeperiod,"any",names_of_files,MatP,number_of_days,dynamic_numberoffeatures,specific_day,selectedvertices,is_vector_i_selected)){
        this->mdiArea->addSubWindow(child);
        connect(child, SIGNAL(updateApplicationMenus()), this, SLOT(updateMenus()));
        child->show();
        fprintf(stderr, "OK with load file\n");fflush(stderr);
    }
    else{
        delete child;
    }
}
/*void MainWindow::newfile_atbeginning()
{
	MdiChild* child = this->createMdiChild();

	if (child->loadFiles_atbeginning(this->folder_with_timeperiod))
	{
		this->mdiArea->addSubWindow(child);
		connect(child, SIGNAL(updateApplicationMenus()), this, SLOT(updateMenus()));
		child->show();
		fprintf(stderr, "OK\n");fflush(stderr);
	}
	else{
		delete child;
	}
}*/

void MainWindow::addFile(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL){
		activeChild->loadFile(ticketurl,ticket,dataurl,this->folder_with_timeperiod);
		//statusBar()->showMessage(tr("File %1 was successfully added") .arg(fileName), 2000);
	}

	activeChild->show();
}

void MainWindow::createGraphFromClusters(){
	MdiChild* activeChild = this->activeMdiChild();

	MdiChild* child = NULL;
	vsGraph *G = NULL;

	if (activeChild != NULL){
		G = activeChild->createGraphFromClusters();

		if (G){
			child = new MdiChild(this,this, G, 0);
			this->mdiArea->addSubWindow(child);
			connect(child, SIGNAL(updateApplicationMenus()), this, SLOT(updateMenus()));
			child->setPathName(activeChild->getPathName());
			child->show();
		}
	}
		
}

void MainWindow::createGraphFromSelectedVertices(){
	MdiChild* activeChild = this->activeMdiChild();

	MdiChild* child = NULL;
	vsGraph *G = NULL;

	if (activeChild != NULL){
		G = activeChild->createGraphFromSelectedVertices();

		if (G){
			child = new MdiChild(this,this, G, 0,activeChild);
			if(activeChild->abstractView)
				child->abstractView=true;
			this->mdiArea->addSubWindow(child);
			connect(child, SIGNAL(updateApplicationMenus()), this, SLOT(updateMenus()));
			child->setPathName(activeChild->getPathName());
			child->show();
		}
	}

}
void MainWindow::openFile(){
	/*QFontDialog *dialog = new QFontDialog(this);
	dialog->exec();*/

	if (maybeSave()) {
	
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "Data", tr("CSV Files (*.dataf *.data *.dat *.csv)"));

		if (!fileName.isEmpty()){
			QFile file(fileName);

			//if (file.open(QFile::ReadOnly | QFile::Text))
				//this->deleteObjects();

				//if (this->loadFile(fileName)){

				//	fprintf(stderr, "Creating Main Window ... ");fflush(stderr);
					//this->createvtkGraphAnimation2(this->mainVSGraph);
				//	fprintf(stderr, "OK\n");fflush(stderr);
					//this->setCentralWidget(openGL);

					//this->show();
					//QCoreApplication::processEvents();

					/*fprintf(stderr, "Creating Dock Window ... ");fflush(stderr);
					this->createDockWindows();
					fprintf(stderr, "OK\n");fflush(stderr);

					this->addFileAction->setEnabled(true);
					this->importClusteringDataAction->setEnabled(true);
					this->viewMenu->setEnabled(true);
					this->showEventsAction->setChecked(true);
					this->showEdgesAction->setChecked(true);
					this->edgeToolBar->setEnabled(true);
					//this->algorithmsMenu->setEnabled(true);
					this->StochasticFlowsAlgorithmAction->setEnabled(true);
					this->AnomalyDetectionAlgorithmAction->setEnabled(true);
					this->positionFeaturesAction->setEnabled(true);

					this->disableAlgorithms();*/
		//		}
		}
	}
}

bool MainWindow::maybeSave(){

	//if (this->VSGraph != NULL) {
	if (5 == 3) {
		QMessageBox::StandardButton ret;

		ret = QMessageBox::warning(this, tr("Application"), tr("The document has been modified.\n""Do you want to save your changes?"),
									//QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
									QMessageBox::Discard | QMessageBox::Cancel);

		if (ret == QMessageBox::Cancel)
			return false;
		//else if (ret == QMessageBox::Save)
			//return save();
	}

	return true;
}

void MainWindow::updateMenus(){
	bool hasMdiChild = (activeMdiChild() != NULL);

	if (hasMdiChild)
		fprintf(stderr, "App has child!\n");fflush(stderr);


	this->addFileAction->setEnabled(hasMdiChild);
	this->createGraphFromSelectedVerticesAction->setEnabled(hasMdiChild && activeMdiChild()->hasSelectedVertices);
	if(hasMdiChild)
		activeMdiChild()->hasSelectedVertices?fprintf(stderr, "has selected vertices true "):fprintf(stderr, "has selected vertices false ");fflush(stderr);
	this->createGraphFromClustersAction->setEnabled(hasMdiChild && activeMdiChild()->hasClusteringResults());
	//this->addFileAction->setVisible(hasMdiChild);
    this->closeAct->setEnabled(hasMdiChild);
    this->closeAllAct->setEnabled(hasMdiChild);
    this->tileAct->setEnabled(hasMdiChild);
    this->cascadeAct->setEnabled(hasMdiChild);
    this->nextAct->setEnabled(hasMdiChild);
    this->previousAct->setEnabled(hasMdiChild);
    this->separatorAct->setVisible(hasMdiChild);

	this->layoutMenu->setEnabled(hasMdiChild);
	this->clusteringMenu->setEnabled(hasMdiChild);
	this->ADMenu->setEnabled(hasMdiChild);

	this->layoutToolBar->setEnabled(hasMdiChild);
	this->graphViewToolBar->setEnabled(hasMdiChild);

	if (hasMdiChild){
		this->abstractLayoutAction->setChecked(activeMdiChild()->abstractView);
		this->increaseCuttingThresholdAction->setEnabled(activeMdiChild()->abstractView);
		this->reduceCuttingThresholdAction->setEnabled(activeMdiChild()->abstractView);

		this->showFeatureValuesAction->setChecked(activeMdiChild()->displayFeatureValues);
		this->showEdgesAction->setChecked(activeMdiChild()->displayEdges);
		this->showEdgesAction->setEnabled(this->showFeatureValuesAction->isChecked());
		this->increaseEdgeWeightAction->setEnabled(this->showFeatureValuesAction->isChecked() && this->showEdgesAction->isChecked());
		this->reduceEdgeWeightAction->setEnabled(this->showFeatureValuesAction->isChecked() && this->showEdgesAction->isChecked());
		this->increaseEdgeTransparencyAction->setEnabled(this->showFeatureValuesAction->isChecked() && this->showEdgesAction->isChecked());
		this->reduceEdgeTransparencyAction->setEnabled(this->showFeatureValuesAction->isChecked() && this->showEdgesAction->isChecked());
		this->showLabelsAction->setChecked(activeMdiChild()->displayLabels);

		this->showClustersAction->setEnabled(activeMdiChild()->hasClusteringResults());
		this->showClustersAction->setChecked(activeMdiChild()->displayClusters && this->showClustersAction->isEnabled());

		this->showAnomaliesAction->setEnabled(activeMdiChild()->hasADResults());
		this->showAnomaliesAction->setChecked(activeMdiChild()->displayAnomalies && this->showAnomaliesAction->isEnabled());
	}
	else{
		this->abstractLayoutAction->setChecked(false);

		this->showFeatureValuesAction->setChecked(false);
		this->showEdgesAction->setChecked(false);
		this->showLabelsAction->setChecked(false);

		this->showClustersAction->setChecked(false);
		this->showClustersAction->setEnabled(false);

		this->showAnomaliesAction->setEnabled(false);
		this->showAnomaliesAction->setChecked(false);
	}

	//return;
	//if (mdiArea->subWindowList().size() > 0  && (activeMdiChild() != this->lastShownMdiChild))
	if ((activeMdiChild() != this->lastShownMdiChild) || (mdiArea->subWindowList().size() == 0))
		this->updateDockWindows();
}

void MainWindow::updateDockWindows(){
	if (mdiArea->subWindowList().size() == 0)
		fprintf(stderr, "No Dock Windows\n");fflush(stderr);
	
	int no_of_files_readed =0;
	int NoFeatures= 0;
	MdiChild* activeChild = this->activeMdiChild();

	this->dockMenu->clear();
	this->dockMenu->addAction(this->toggleDockWindowsViewAction);
	this->dockMenu->addSeparator();

	//if (activeMdiChild() == this->lastShownMdiChild)
		//return;
	//int number_matp_widgets=0;
	if (activeChild != NULL)
	{
		//NoFeatures=6;
		if(activeChild->matp_has_to_be_inserted==true)
		{
			
			no_of_files_readed =activeChild->getNoFeatures() - 1;
			NoFeatures = dynamic_nummeroffeatures;
			if (no_of_files_readed <= 0)
				return;

			for (int i = this->NoDocks; i < no_of_files_readed; ++i)
			{
				this->createDockWindow();

				if ( i % 2 == 0 )
					this->addDockWidget(Qt::RightDockWidgetArea, this->Dock[i]);
				else
					this->addDockWidget(Qt::LeftDockWidgetArea, this->Dock[i]);

				this->Dock[i]->setVisible(true);
			}
			this->dynamic_HitVertex=activeChild->getDockWin_matp()->get_HitVertex();
			if(this->dynamic_HitVertex==activeChild->getDockWin_matp()->getNoDrawingVertices())
				this->dynamic_HitVertex=0;

		}

		else
		{
			NoFeatures = activeChild->getNoFeatures() - 1;
			no_of_files_readed = activeChild->getNoFeatures() - 1;
			if (NoFeatures <= 0)
				return;

			for (int i = this->NoDocks; i < NoFeatures; ++i){
				this->createDockWindow();

				if ( i % 2 == 0 )
					this->addDockWidget(Qt::RightDockWidgetArea, this->Dock[i]);
				else
					this->addDockWidget(Qt::LeftDockWidgetArea, this->Dock[i]);

				this->Dock[i]->setVisible(true);
			}
		}	
		if(activeChild->matp_has_to_be_inserted==true && this->matp_inserted==false)
		{
			activeChild->matp_inserted=true;
			if(this->NoDocks==no_of_files_readed) 
			{
				this->createDockWindow();
				//edo den ipirxe
				this->createDockWindow();
			}
			//edo to eixa number_matp_widgets=1;
			number_matp_widgets=2;
			
			this->addDockWidget(Qt::RightDockWidgetArea, this->Dock[no_of_files_readed]);
			this->Dock[no_of_files_readed]->setVisible(true);
			this->Dock[no_of_files_readed]->setWidget(activeChild->getDockWin_matp());
			this->Dock[no_of_files_readed]->setWindowTitle(activeChild->getDockWin_matp()->windowTitle());
            
			this->Dock[no_of_files_readed]->setVisible(true);
			this->Dock[no_of_files_readed]->setMinimumHeight(120);
			this->Dock[no_of_files_readed]->setMinimumWidth(120);
			this->dockMenu->addAction(this->Dock[no_of_files_readed]->toggleViewAction());
			//connect(this->toggleDockWindowsViewAction, SIGNAL(triggered(bool)), this->Dock[i]->toggleViewAction(), SLOT(setChecked(bool)));
			connect(this->toggleDockWindowsViewAction, SIGNAL(triggered(bool)), this->Dock[no_of_files_readed]->toggleViewAction(), SIGNAL(triggered(bool)));
			//this->Dock[i]->show();
			this->Dock[no_of_files_readed]->updateGeometry();


			//apo edo kai pera den ipirxan
			this->addDockWidget(Qt::BottomDockWidgetArea, this->Dock[no_of_files_readed+1]);
			this->Dock[no_of_files_readed+1]->setVisible(true);
			this->Dock[no_of_files_readed+1]->setWidget(activeChild->getDockWin_bar());
			this->Dock[no_of_files_readed+1]->setWindowTitle(activeChild->getDockWin_bar()->windowTitle());

			this->Dock[no_of_files_readed+1]->setVisible(true);
			this->Dock[no_of_files_readed+1]->setMinimumHeight(100);
			this->Dock[no_of_files_readed+1]->setMinimumWidth(120);
			this->dockMenu->addAction(this->Dock[no_of_files_readed+1]->toggleViewAction());
			//connect(this->toggleDockWindowsViewAction, SIGNAL(triggered(bool)), this->Dock[i]->toggleViewAction(), SLOT(setChecked(bool)));
			connect(this->toggleDockWindowsViewAction, SIGNAL(triggered(bool)), this->Dock[no_of_files_readed+1]->toggleViewAction(), SIGNAL(triggered(bool)));
			//this->Dock[i]->show();
			this->Dock[no_of_files_readed+1]->updateGeometry();
		}
		fprintf(stderr, "00oo! %d %d %d\n", NoFeatures, this->NoDocks, this->NoLastShownDocks);fflush(stderr);
		int maximum_number_of_dock_to_delete;
		if(activeChild->matp_has_to_be_inserted==false && no_of_files_readed<this->NoDocks)
			maximum_number_of_dock_to_delete=this->NoDocks;
		else
			maximum_number_of_dock_to_delete=this->NoDocks-number_matp_widgets;
		for (int i = no_of_files_readed; i < maximum_number_of_dock_to_delete; i++){
			//fprintf(stderr, "Check2: %d\n", this->dockArea[i]);fflush(stderr);
			removeDockWidget(this->Dock[i]);
			this->Dock[i]->setWidget(NULL);
			this->Dock[i]->setVisible(false);
			this->NoDocks--;
		}
		
		if(activeChild->matp_has_to_be_inserted==true && activeChild->matp_inserted==true)
		{
			for (int i = 0; i < no_of_files_readed; i++){
				this->Dock[i]->setWidget(NULL);
			}
		}
		fprintf(stderr, "%d %d\n", NoFeatures, this->NoDocks);fflush(stderr);
		this->toggleDockWindowsViewAction->disconnect();

		for (int i = 0; i < no_of_files_readed; i++){
			//int number_of_activechild_dock=NoFeatures*this->dynamic_HitVertex+i;
			this->Dock[i]->setWidget(activeChild->getDockWin(i));
			this->Dock[i]->setWindowTitle(activeChild->getDockWin(i)->windowTitle());

			if (i >= this->NoLastShownDocks)
				restoreDockWidget(this->Dock[i]);

			//this->Dock[i]->setVisible(true);
			this->Dock[i]->setMinimumHeight(120);
			this->Dock[i]->setMinimumWidth(120);
			this->dockMenu->addAction(this->Dock[i]->toggleViewAction());
			//connect(this->toggleDockWindowsViewAction, SIGNAL(triggered(bool)), this->Dock[i]->toggleViewAction(), SLOT(setChecked(bool)));
			connect(this->toggleDockWindowsViewAction, SIGNAL(triggered(bool)), this->Dock[i]->toggleViewAction(), SIGNAL(triggered(bool)));
			//this->Dock[i]->show();
			this->Dock[i]->updateGeometry();
		}

		this->NoLastShownDocks = NoFeatures;
	}
	else
	{
		
		for (int i = 0; i < this->NoDocks; i++){
			//fprintf(stderr, "Check2: %d\n", this->dockArea[i]);fflush(stderr);
			removeDockWidget(this->Dock[i]);
			this->Dock[i]->setWidget(NULL);
			this->Dock[i]->setVisible(false);
			
		}
		int nodocks=this->NoDocks;
		for (int i = 0; i < nodocks; i++)
			this->NoDocks--;
		
	}
	
}	


void MainWindow::updateWindowMenu(){
	
	this->windowMenu->clear();
	if (this->NoDocks > 0){
		//this->updateDockMenu();
		this->windowMenu->addMenu(this->dockMenu);
		this->windowMenu->addSeparator();
	}
	this->windowMenu->addAction(closeAct);
	this->windowMenu->addAction(closeAllAct);
	this->windowMenu->addSeparator();
	this->windowMenu->addAction(tileAct);
	this->windowMenu->addAction(cascadeAct);
	this->windowMenu->addSeparator();
	this->windowMenu->addAction(nextAct);
	this->windowMenu->addAction(previousAct);
	this->windowMenu->addAction(separatorAct);

	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	this->separatorAct->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); ++i) {
		MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

		QString text;
		if (i < 9) {
			text = tr("&%1 %2").arg(i + 1).arg(child->windowTitle());
        } 
		else {
			//text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
		}
		QAction *action  = windowMenu->addAction(text);
		action->setCheckable(true);
		action ->setChecked(child == activeMdiChild());
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		this->windowMapper->setMapping(action, windows.at(i));
    }
}

void MainWindow::createDockWindow(){
	int i = this->NoDocks;

	this->Dock = (QDockWidget**) realloc(this->Dock, (i + 1) * sizeof(QDockWidget*));
	
	if ( !this->Dock ){
		fprintf(stderr, "Cannot allocate memory for QDockWidget. Exiting ...\n");fflush(stderr);
		exit(1);
	}

	this->Dock[i] = new QDockWidget(tr(""), this);
	this->Dock[i]->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
	//this->Dock[i]->setVerticalPolicy(QSizePolicy::Expanding);

	this->NoDocks++;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
		return;
	this->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
	this->lastShownMdiChild = qobject_cast<MdiChild *>(qobject_cast<QMdiSubWindow *>(window)->widget());
}

void MainWindow::importClusteringResults(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL){
		activeChild->importClusteringResults(ticketurl,ticket,dataurl);
	}
}

void MainWindow::executeStochasticFlowsAlgorithm(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL){
		//this->showClustersAction->setChecked(false);
		//this->showClustersAction->setEnabled(false);
		//this->clusteringMenu->setEnabled(false);

		activeChild->executeSF();

		//this->clusteringMenu->setEnabled(true);
		//this->showClustersAction->setChecked(true);
		//activeChild->toggleClustersView(true);
		//this->updateMenus();
	}
}

void MainWindow::executeStaticADAlgorithm(){
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL){
		activeChild->executeStaticAD();
	}
}

void MainWindow::slot_executeDynamicADAlgorithm()
{
	if (m_guiDynAD_SelectDataForm) delete m_guiDynAD_SelectDataForm;
	m_guiDynAD_SelectDataForm = new DynamicAD_GUISelectDataForm(this,"Data/SpamCloud/March/33692/",std::string(this->folder_with_timeperiod.toLatin1()));
	m_guiDynAD_SelectDataForm->exec();

	delete m_guiDynAD_SelectDataForm;
	m_guiDynAD_SelectDataForm = NULL;
}
void MainWindow::slot_executeDynamicADAlgorithm2()
{
	if (m_guiDynAD_SelectDataForm2) delete m_guiDynAD_SelectDataForm2;
	m_guiDynAD_SelectDataForm2 = new DynamicAD_GUISelectDataForm2(this,"Data/SpamCloud/March/33692/",std::string(this->folder_with_timeperiod.toLatin1()));
	m_guiDynAD_SelectDataForm2->exec();
	if(m_guiDynAD_SelectDataForm2->Getcomplete_clicked()==1) 
		names_of_files=m_guiDynAD_SelectDataForm2->Getnames_of_files();
	else
		return;
	MatP=m_guiDynAD_SelectDataForm2->getMatP();
	dynamic_nummeroffeatures=m_guiDynAD_SelectDataForm2->getnumber_of_features();
	number_of_days=names_of_files.size()/dynamic_nummeroffeatures;
	delete m_guiDynAD_SelectDataForm2;
	m_guiDynAD_SelectDataForm2 = NULL;
	//this->NoDocks=0;
	int a=MatP.size();
	if(MatP.size()==0)
		return;
	else
		this->newFile(names_of_files,MatP,number_of_days,dynamic_nummeroffeatures,0);

}
void MainWindow::executeDynamicADAlgorithm2_atbeginning()
{
	if (m_guiDynAD_SelectDataForm2) delete m_guiDynAD_SelectDataForm2;
	m_guiDynAD_SelectDataForm2 = new DynamicAD_GUISelectDataForm2(this,"Data/SpamCloud/March/33692/",std::string(this->folder_with_timeperiod.toLatin1()));
	m_guiDynAD_SelectDataForm2->LoadTimeData_atbeginning(this->dataset_name,this->timeperiod);
	m_guiDynAD_SelectDataForm2->LoadFeatureData_atbeginning();
	m_guiDynAD_SelectDataForm2->Run_atbeginning();
	//m_guiDynAD_SelectDataForm2->exec();
	if(m_guiDynAD_SelectDataForm2->Getcomplete_clicked()==1) names_of_files=m_guiDynAD_SelectDataForm2->Getnames_of_files();
	MatP=m_guiDynAD_SelectDataForm2->getMatP();
	dynamic_nummeroffeatures=m_guiDynAD_SelectDataForm2->getnumber_of_features();
	number_of_days=names_of_files.size()/dynamic_nummeroffeatures;
	delete m_guiDynAD_SelectDataForm2;
	m_guiDynAD_SelectDataForm2 = NULL;
	vector<int> selected_days;
	vector<bool> day_selected_or_not;
	for(int i=0;i<number_of_days;i++)
	{
		selected_days.push_back(i);
		day_selected_or_not.push_back(true);
	}
	
	if(MatP.size()==0)
		return;
	else
		this->newFile(names_of_files,MatP,number_of_days,dynamic_nummeroffeatures,0,selected_days,day_selected_or_not);
	MdiChild* activeChild = this->activeMdiChild();

	if (activeChild != NULL){
		activeChild->importClusteringResults_atbeginning(ticketurl,ticket,dataurl);
	}
}
void MainWindow::slotTestDynamicAlgorithm()
{ 
	//*
	std::cout << "TEST DYNAMIC ALGORITHM START...\n";
	
	DynamicAD* dynAD = new DynamicAD("SpamCloud");
	dynAD->LoadTimeCSV("Data/SpamCloud/March/8423/Rspams_day.csv", "Days", DynamicADTimeEntry::TimeType_Day);
	//dynAD->LoadTimeCSV("Data/SpamCloud/March/33692/Rspams_day.csv", "day", DynamicADTimeEntry::TimeType_Day);
	//dynAD->LoadTimeCSV("Data/SpamCloud/March/Full/spams_day.csv", DynamicADTimeEntry::TimeType_Day);
	//dynAD->LoadTimeCSV("Data/SpamCloud/March/8423/Rspams_hour.csv", DynamicADTimeEntry::TimeType_Day);
	//dynAD->LoadTimeCSV("Data/SpamCloud/March/33692/Rspams_hour.csv", DynamicADTimeEntry::TimeType_Day);

	//1st feature:
	dynAD->LoadFeatureCSV("Data/SpamCloud/March/8423/Rspams_bot.csv", "Bots", true, true);
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/33692/Rspams_bot.csv", "Bots", false, false);
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/Full/spams_bot.csv", "Bots", false, false);

	//2nd feature:
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/8423/Rspams_subj_keys2_ore.csv", "Keywords", true, true);
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/8423/Rspams_subject_ore_4.csv", "Keywords", true, true);
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/33692/Rspams_subj_keys2_ore.csv", "Keywords", true, true);
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/Full/spams_subj_keys2_ore.csv", "Keywords", true, true);
	

	//3rd feature:
	dynAD->LoadFeatureCSV("Data/SpamCloud/March/8423/Rspams_country.csv", "Country", true, true);
	//dynAD->LoadFeatureCSV("Data/SpamCloud/March/33692/Rspams_country.csv", "Country", false, false);

	//dynAD->PrintTimeSetToFile("RemTest/Time_labels.txt");
	//dynAD->GetFeature(0)->PrintIdSetToFile("RemTest/Keywords_labels.txt");
	//dynAD->GetFeature(1)->PrintIdSetToFile("RemTest/Bots_labels.txt");

	dynAD->CreateGraphs();
//	dynAD->PerformMDS(DynamicADGraph::DistanceType_GraphOperations);

	for (unsigned int i=0; i<DynamicADGraph::NUM_DISTANCE_TYPES; i++)
	{
		dynAD->PerformMDS((DynamicADGraph::DistanceType)i);
	}
	
	dynAD->LoadFeatureCSV("Data/SpamCloud/March/8423/Rspams_subj_keys2_ore.csv", "Keywords", true, true);
	dynAD->CreateGraphs();
	for (unsigned int i=0; i<DynamicADGraph::NUM_DISTANCE_TYPES; i++)
	{
		dynAD->PerformMDS((DynamicADGraph::DistanceType)i);
	}

	
	//system("pause");

	

	//TEST PLOT

//	DynamicAD_GUIPlotDialog myPlot(dynAD, this);
//	myPlot.exec();
	


	std::cout << "...TEST DYNAMIC ALGORITHM END!\n";
    delete dynAD;
	//*/
}

MdiChild* MainWindow::activeMdiChild(){
	if (QMdiSubWindow* activeSubWindow = this->mdiArea->activeSubWindow()){
		if (qobject_cast<MdiChild *>(activeSubWindow->widget())){
			fprintf(stderr, "Found active mdichild\n");fflush(stderr);
			return qobject_cast<MdiChild *>(activeSubWindow->widget());
		}
	}
	return NULL;
}
void MainWindow::display_cas()
{
	//timeperiod,spamclouddata,harmur2data
	teststauros2 w(ticketurl,ticket,timeperiod_url,this->dataset_url,staticgraphdata_url,dynamicgraphdata_url);
    string data_to_display=w.data_from_url.toUtf8();
    string session_id=w.mysesion_id.toUtf8();
	bool fileSuccessfullyAdded=false;
	QString fileName;

	display_data_from_cas* cas_display=new display_data_from_cas(ticketurl,ticket,dataurl,data_to_display,session_id,this);
}
void MainWindow::deleteDirectory(QString directory_to_delete)
{
	QDir dir_to_del(directory_to_delete);

	//First delete any files in the current directory
	QFileInfoList files = dir_to_del.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
	for(int file = 0; file < files.count(); file++)
	{
		dir_to_del.remove(files.at(file).fileName());
	}

	QFileInfoList dirs = dir_to_del.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	for(int dir = 0; dir < dirs.count(); dir++)
	{
		this->deleteDirectory(dirs.at(dir).absoluteFilePath ());
	}

	dir_to_del.rmdir(dir_to_del.path());
	dir_to_del.cdUp();
	dir_to_del.rmdir(directory_to_delete);
}
/*void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	int selected_point_for_new_graph=0;

	if((event->key()==Qt::Key_Alt))
	{
		for(int i=0;i<this->points_location_x.size();i++)
			this->is_vector_i_selected.push_back(false);
		for(int i=0;i<this->selected_vertices.size();i++)
			this->is_vector_i_selected[selected_vertices[i]]=true;
		if(this->selected_point_hitted==true||this->which_point_hitted<this->number_of_dots||selected_vertices.size()>0)
			this->parent_dbar->update_parent_mainwindow(selected_point_for_new_graph,selected_vertices,this->ctrl_clicked,is_vector_i_selected);
	}
}
*/
bool MainWindow::removeDir(const QString &dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDir(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirName);
	}

	return result;
}
