#include <QtGui>

//VIS-SENSE includes
#include "vsCSVFile.h"
#include "vsGraph.h"
#include "vsFeature.h"
#include "vsNode.h"
#include "teststauros2.h"
//VIS-SENSE Qt includes
#include "mainwindow.h"
#include "mdichild.h"
#include "glwidget.h"
#include "executeSFDialog.h"
#include "createGraphFromClustersDialog.h"
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QUrl>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <QtXml/QDomDocument>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include "mainwindow.h"
//Other incldues
#include "omp.h"
//#include "dirent.h"
#include <fstream>
#include "testclass.h"

MdiChild::MdiChild(QWidget *parent,MainWindow* parentwindow, vsGraph *graph, int drawingfeature,MdiChild *parentmdichild) : GLWidget(parent, graph, drawingfeature){
	this->parentmdichild=parentmdichild;
	setAttribute(Qt::WA_DeleteOnClose);
	//isUntitled = true;
	this->parentwindow=parentwindow;
	this->DockWin = NULL;
	this->NoDockWins = 0;

	//QWidget *pic = new QWidget(this);
	//pic->setStyleSheet("background-image: url(images/vslogo.png)");
	//pic->setGeometry(QRect(50,50,128,128));

	this->Min.x = this->Min.y = this->Min.z = 0;
	this->Min.x = this->Min.y = this->Min.z = 0;

	this->csvList = NULL;
	this->csvListSize = 0;

	this->LastScreenSize.x = 0;
	this->LastScreenSize.y = 0;

	this->FRRuns = 0;//frlayout runs
 	this->MaxFRRuns = 300;
	this->FRRunning = false;

	this->SFDialog = NULL;
	this->graphFromClustersDialog = NULL;

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

	connect(this, SIGNAL(changedLayoutView(bool)), this, SLOT(setLayoutView(bool)));
	connect(this, SIGNAL(changedCuttingThreshold(float)), this, SLOT(setCuttingThreshold(float)));
	connect(this, SIGNAL(changedVertexSize(int)), this, SLOT(setVertexSize(int)));
	connect(this, SIGNAL(changedVertexTransparency(int)), this, SLOT(setVertexTransparency(int)));
	connect(this, SIGNAL(changedFeatureValuesView(bool)), this, SLOT(setFeatureValuesView(bool)));
	connect(this, SIGNAL(changedEdgesView(bool)), this, SLOT(setEdgesView(bool)));
	connect(this, SIGNAL(changedEdgeTransparency(int)), this, SLOT(setEdgeTransparency(int)));
	connect(this, SIGNAL(changedLabelsView(bool)), this, SLOT(setLabelsView(bool)));
	connect(this, SIGNAL(changedClustersView(bool)), this, SLOT(setClustersView(bool)));
	connect(this, SIGNAL(changedAnomaliesView(bool)), this, SLOT(setAnomaliesView(bool)));
	connect(this, SIGNAL(clearSelected()), this, SLOT(unsetSelected()));
	connect(this, SIGNAL(selectedVertex(int)), this, SLOT(VertexSelection(int)));
	connect(this, SIGNAL(refresh()), this, SLOT(animate()));

	fprintf(stderr, "Checking Consistency ... ");fflush(stderr);fflush(stderr);
	this->checkConsistency();
	fprintf(stderr, "OK\n");fflush(stderr);fflush(stderr);

	//this->setMinimumSize(120, 120);
	this->setMinimumSize(100, 100);
	//this->resize(250, 100);
	this->PathName = QString("");
	this->setWindowTitle(this->PathName);
	matp_has_to_be_inserted=false;
	matp_inserted=false;
	//this->setFocus();
}
void MdiChild::update_parent_mainwindow(int Hitvertex,vector<int> selectedvertices,bool ctrl_clicked,vector<bool> is_vector_i_selected)
{
	
	//this->update();
	//this->parentwindow->updateDockWindows();
	//this->parentwindow->
	//this->parentwindow->mdiArea->removeSubWindow(this);
	
	if(ctrl_clicked==true)
		this->parentwindow->mdiArea->closeActiveSubWindow();
		//this->parentwindow->mdiArea->closeAllSubWindows();
    
	this->parentwindow->newFile(this->parentwindow->names_of_files,this->parentwindow->MatP,this->parentwindow->number_of_days,this->parentwindow->dynamic_nummeroffeatures,Hitvertex,selectedvertices,is_vector_i_selected);	
}
void MdiChild::ShowMatpChoices(const QPoint& pos)
{
	QPoint globalPos = this->DockWin_matp->mapToGlobal(pos);
	QMenu* myMenu=new QMenu(this);
	QMenu* display_types_menu;
	QMenu* metric_menu,* show_selected;
	display_types_menu=myMenu->addMenu("select display type");
	metric_menu=myMenu->addMenu("select metric");
	show_selected=myMenu->addMenu("show selected graphs");

	
	this->dynamic_numbers_displayed_matp = new QAction(tr("display numbers"), this);
	this->dynamic_dates_displayed_matp = new QAction(tr("display dates"), this);
	this->L1_matp = new QAction(tr("L1"), this);
	this->L2_matp = new QAction(tr("L2"), this);
	this->eyclidean_matp = new QAction(tr("EUCLIDEAN"), this);
	this->nodeoperations_matp = new QAction(tr("NodeOperations"), this);
	this->edgeoperations_matp = new QAction(tr("EdgeOperations"), this);
	this->graphoperations_matp = new QAction(tr("GraphOperations"), this);
	this->show_selected_graphs_clicked_matp=new QAction("show/hide selected",this);

	

	
	connect(dynamic_numbers_displayed_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_numbers_displayed()));
	connect(dynamic_dates_displayed_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_dates_displayed()));
	connect(this->L1_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_L1_clicked()));
	connect(this->L2_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_L2_clicked()));
	connect(this->eyclidean_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_eyclidean_clicked()));
	connect(this->nodeoperations_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_nodeoperations_clicked()));
	connect(this->edgeoperations_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_edgeoperations_clicked()));
	connect(this->graphoperations_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_set_graphoperations_clicked()));
	connect(this->show_selected_graphs_clicked_matp, SIGNAL(triggered()), this, SLOT(dynamic_matp_show_hide_selected_clicked()));
	display_types_menu->addAction(dynamic_numbers_displayed_matp);
	display_types_menu->addAction(dynamic_dates_displayed_matp);

	metric_menu->addAction(this->L1_matp);
	metric_menu->addAction(this->L2_matp);
	metric_menu->addAction(this->eyclidean_matp);
	metric_menu->addAction(this->nodeoperations_matp);
	metric_menu->addAction(this->edgeoperations_matp);
	metric_menu->addAction(this->graphoperations_matp);
	show_selected->addAction(this->show_selected_graphs_clicked_matp);
	//if(this->DockWin_matp->hasSelectedVertices_dmatp==false)
	if(this->DockWin_matp->is_vector_i_selected_global.size()==0)
		show_selected->menuAction()->setVisible(false);
	QAction* selectedItem = myMenu->exec(globalPos);
	if (selectedItem){
		fprintf(stderr, "Executed\n");fflush(stderr);fflush(stderr);
		}
	else{
		fprintf(stderr, "Not Executed\n");fflush(stderr);fflush(stderr);
		}
	delete myMenu;
	//delete display_types_menu;
	//delete metric_menu;
}
void MdiChild::ShowbarChoices(const QPoint& pos)
{
	QPoint globalPos = this->DockWin_bar->mapToGlobal(pos);
	QMenu* myMenu=new QMenu(this);
	QMenu* display_types_menu;
	QMenu* metric_menu,* show_selected;
	display_types_menu=myMenu->addMenu("select display type");
	show_selected=myMenu->addMenu("show selected graphs");


	this->dynamic_numbers_displayed_bar = new QAction(tr("display numbers"), this);
	this->dynamic_dates_displayed_bar = new QAction(tr("display dates"), this);
	this->show_selected_graphs_clicked_bar=new QAction("show label only for selected",this);




	connect(dynamic_numbers_displayed_bar, SIGNAL(triggered()), this, SLOT(dynamic_bar_set_numbers_displayed()));
	connect(dynamic_dates_displayed_bar, SIGNAL(triggered()), this, SLOT(dynamic_bar_set_dates_displayed()));
	connect(this->show_selected_graphs_clicked_bar, SIGNAL(triggered()), this, SLOT(dynamic_bar_show_only_selected_clicked()));
	display_types_menu->addAction(dynamic_numbers_displayed_bar);
	display_types_menu->addAction(dynamic_dates_displayed_bar);

	show_selected->addAction(this->show_selected_graphs_clicked_bar);
	//if(this->DockWin_matp->hasSelectedVertices_dmatp==false)
	if(this->DockWin_bar->is_vector_i_selected_global.size()==0)
		show_selected->menuAction()->setVisible(false);
	QAction* selectedItem = myMenu->exec(globalPos);
	if (selectedItem){
		fprintf(stderr, "Executed\n");fflush(stderr);fflush(stderr);
	}
	else{
		fprintf(stderr, "Not Executed\n");fflush(stderr);fflush(stderr);
	}
	delete myMenu;
	//delete display_types_menu;
	//delete metric_menu;
}
void MdiChild::ShowContextMenu(const QPoint& pos){
	if (this->DrawingFeature != 0 || this->getNoFeatures() <= 2)
		return;

	// for most widgets
	QPoint globalPos = this->mapToGlobal(pos);
	// for QAbstractScrollArea and derived classes you would use:
	// QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

	QMenu myMenu(this);
	for (int i = 0; i < this->NoDockWins; ++i){
		this->DockWin[i]->isShownAction->setText(this->DockWin[i]->windowTitle());
		myMenu.addAction(this->DockWin[i]->isShownAction);
	}
	//menu.addAction(copyAct);
	//menu.addAction(pasteAct);

	QAction* selectedItem = myMenu.exec(globalPos);
	//myMenu.exec(event->globalPos());
    if (selectedItem){
		fprintf(stderr, "Executed\n");fflush(stderr);fflush(stderr);
		// something was chosen, do stuff
	}
    else{
		fprintf(stderr, "Not Executed\n");fflush(stderr);fflush(stderr);
        // nothing was chosen
	}

} 

void MdiChild::mouseMoveEvent(QMouseEvent *event){
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {

		if (this->HitVertex != this->NoDrawingVertices){
			if (this->scaleFactor == 1){
				this->Vertex[this->HitVertex].DrawingPos.x += dx;
				this->Vertex[this->HitVertex].DrawingPos.y += dy;
			}
			else{
				this->Vertex[this->HitVertex].DrawingPos.x += dx / this->scaleFactor + 0.5; //Add 0.5 so that the pointer doesn't lose focus due to multiplication effect
				this->Vertex[this->HitVertex].DrawingPos.y += dy / this->scaleFactor + 0.5;
			}
			//this->DrawingPos[this->HitVertex].x = (event->x() - this->moveD.x() - this->drawingBorder.x() * this->scaleFactor) / this->scaleFactor;

			if (this->PositionsSet){
				this->PositionsSet = false;
				repaint();
			}
			this->FRRuns = this->MaxFRRuns / 3;
		}
		else{
			this->drawSelectionBox = true;
			//this->selectionBoxPos.setX();
			int Mx = event->x();
			int My = event->y();

			int x = (this->lastClickedPos.x() < Mx)? this->lastClickedPos.x() : Mx;
			x = (x - this->moveD.x()) / this->scaleFactor;
			int y = (this->lastClickedPos.y() < My)? this->lastClickedPos.y() : My;
			y = (y - this->moveD.y()) / this->scaleFactor;
			int w = fabs(float(this->lastClickedPos.x() - event->x())) / this->scaleFactor;
			int h = fabs(float(this->lastClickedPos.y() - event->y())) / this->scaleFactor;
			this->selectionBox.setRect(x, y, w, h);
			
			if (!(QApplication::keyboardModifiers() & 0x04000000))
				emit clearSelected();

			for (int i = this->NoDrawingVertices - 1; i >= 0; --i) {
				if (!this->Vertex[i].Draw)
					continue;

				//this->Vertex[i].SecondSelection = false;

				if ( this->selectionBox.contains(this->Vertex[i].DrawingPos.x, this->Vertex[i].DrawingPos.y) ){
					//this->Vertex[i].SecondSelection = true;
					emit this->selectedVertex(i);
				}

			}

			update();
			//emit this->updateApplicationMenus();
		}
	}
	else if (event->buttons() & Qt::MiddleButton){
		this->moveD.setX(this->moveD.x() + dx);
		this->moveD.setY(this->moveD.y() + dy);
		//fprintf(stderr, "yes\n");fflush(stderr);fflush(stderr);
		update();
	}
	
	this->lastPos = event->pos();
	fprintf(stderr, "%d %d\n", lastPos.x(), lastPos.y());fflush(stderr);fflush(stderr);
}

void MdiChild::paint(QPainter *painter, QPaintEvent *event){
    
	painter->fillRect(event->rect(), background);
    painter->translate(this->moveD);
	painter->scale(this->scaleFactor, this->scaleFactor);
	//painter->save();
	//ofs<<1;
	if ( !this->PositionsSet || this->csvListSize != 0 || this->setNewGraph){
        if (!this->setPositions())
			return;
	}
	
	if ((this->LastScreenSize.x != this->size().width() || this->LastScreenSize.y != this->size().height()) || (!this->PositionsSet)){
		fprintf(stderr, "CP %d %d %d %d\n", this->LastScreenSize.x, this->size().width(), this->LastScreenSize.x, this->size().height());fflush(stderr);fflush(stderr);
		this->setDrawingPositions();
		this->LastScreenSize.x = this->size().width();
		this->LastScreenSize.y = this->size().height();
	}
	this->textFont.setPixelSize(this->VertexSize + 2);
	painter->setFont(this->textFont);
	this->selectedBrush.setColor(QColor(255, 0, 0, this->VertexOpacity));
	this->selectedPen.setColor(QColor(200, 0, 0, this->VertexOpacity));
	//this->circlePen.setWidth( (this->VertexSize/10) / this->scaleFactor); //Uncomment only if you have specified width

	
	if (this->displayFeatureValues && this->displayEdges)
		this->paintEdges(painter);
	this->paintVertices(painter);
	//ofs<<3;
	if (this->drawSelectionBox){
		this->circleBrush.setColor(QColor(this->brushColor[0]->R, this->brushColor[0]->G, this->brushColor[0]->B, 30));
		this->circlePen.setColor(QColor(this->penColor[0]->R, this->penColor[0]->G, this->penColor[0]->B, 210));
		painter->setBrush(this->circleBrush);
		painter->setPen(this->circlePen);

		painter->drawRect(this->selectionBox);
		//painter->drawPolygon(this->selectionBox, Qt::OddEvenFill);
		//painter->drawPolyline(this->selectionBox);
	}
	

    //painter->save();
    //painter->rotate(elapsed * 0.030);
    //painter->restore();

}


bool MdiChild::loadFile(string ticketurl,string ticket,string dataurl,QString folder_with_timeperiod,const char *s,vector<std::string> names_of_files,vector<Eigen::MatrixXd> MatP,int number_of_days,int dynamic_numberoffeatures,int specific_day,vector<int> selectedvertices,vector<bool> is_vector_i_selected){
	
	
	bool direct_fromfiledialog=true;
	if(this->folder_with_timeperiod.size()==0)
		this->folder_with_timeperiod=folder_with_timeperiod;
	string url="";
	bool fileSuccessfullyAdded=false;

	if (names_of_files.size()>0)
	{
		
		createDockWin_matp(MatP,dynamic_numberoffeatures);
		createDockWin_bar(MatP,dynamic_numberoffeatures);
		if(is_vector_i_selected.size()>0)
		{
			this->DockWin_matp->is_vector_i_selected_global=is_vector_i_selected;
			this->DockWin_bar->is_vector_i_selected_global=is_vector_i_selected;
			this->DockWin_bar->has_many_selected_points=true;
			for(int i=0;i<is_vector_i_selected.size();i++)
				if(is_vector_i_selected[i]==true)
					this->DockWin_bar->selected_point=i;
		}
		url="";
		string name_of_feature;
		if(selectedvertices.size()>0)
		{
			for(int i=0;i<dynamic_numberoffeatures;i++)
			{
				vector<string> files_merged;
				for(int j=0;j<selectedvertices.size();j++)
				{
					files_merged.push_back(names_of_files[selectedvertices[j]*dynamic_numberoffeatures+i]);
					
				}
				this->readFile(files_merged,selectedvertices);
			}
		}
		else
		{
			for (unsigned int i=specific_day*dynamic_numberoffeatures; i<(specific_day+1)*dynamic_numberoffeatures; i++)
			{
				vector<string> files;
				files.push_back(names_of_files[i]);
				this->readFile(files);
				//this->readFile(QString::fromStdString(files));
			}
		}
		fileSuccessfullyAdded = true;
		update();
		return fileSuccessfullyAdded;
	}
	
	QString filename;
	if(direct_fromfiledialog==true)
	{
		QStringList files = QFileDialog::getOpenFileNames(this, tr("Select one or more files to open"), "Data", tr("CSV Files (*.csv *.clu)") );

		QStringList list = files;
		QStringList::Iterator it = list.begin();

		while(it != list.end()) {
			filename = *it;

			if (!filename.isEmpty()){
				QFile file(filename);

				if (file.open(QFile::ReadOnly | QFile::Text)){

					if ( (this->checkFeatureExistence(filename)) ){
						QMessageBox::warning(this, tr("Application"), tr("File \"%1\" already added.") .arg(QFileInfo(filename).fileName()));
					}   
					else {
						vector<string> filenames;
						filenames.push_back(filename.toLatin1().data());
						this->readFile(filenames);
						//qobject_cast<MainWindow *>(this->parent())->statusBar()->showMessage(tr("File %1 loaded.") .arg(fileName), 2000);
						if (!fileSuccessfullyAdded)
							fileSuccessfullyAdded = true;
					}   

				}   
				else{
					QMessageBox::warning(this, tr("Application"), tr("Cannot read file %1:\n%2.") .arg(filename) .arg(file.errorString()));
				}   
			}   
			++it;
		}
	}
	else
	{
		QString fileName;
		DynamicAD_GUISelect_Feature* sel_feat;
		if(s=="clu")
            sel_feat=new DynamicAD_GUISelect_Feature("static_cluster","","","","","",std::string(this->folder_with_timeperiod.toUtf8()),this);
		else if(s=="clu_at_beginning")
		{
            url=this->folder_with_timeperiod.toUtf8();
			vector<string> files;
			files.push_back(url+"/clusters.clu");
			this->readFile(files);
			fileSuccessfullyAdded = true;
			update();
			return fileSuccessfullyAdded;
		}
		else
            sel_feat=new DynamicAD_GUISelect_Feature("static","","","","","",std::string(this->folder_with_timeperiod.toUtf8()),this);
		
		if (sel_feat->allfeatures.size()==0||sel_feat->get_complete_clicked()==0) 
			return false;
		for (unsigned int i=0; i<sel_feat->allfeatures.size(); i++)
		{
			
            url=this->folder_with_timeperiod.toUtf8();
			vector<string> files;
			files.push_back(url+"/"+sel_feat->allfeatures[i][0]);
			this->readFile(files);
		}
		fileSuccessfullyAdded = true;
	}

	update();
	return fileSuccessfullyAdded;
}

/*bool MdiChild::loadFiles_atbeginning(QString folder_with_timeperiod)
{
	this->folder_with_timeperiod=folder_with_timeperiod;
	bool fileSuccessfullyAdded=false;
	vector<string> all_files_inside_folder;
	DIR *dir = opendir(folder_with_timeperiod.toLatin1().data()); 
	int counter=0;
	if(dir) 
	{ 
		struct dirent *ent; 
		while((ent = readdir(dir)) != NULL ) 
		{ 

			if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0) 
				continue;

			
			std::string filename=ent->d_name;
			int pos_of_dot = filename.find_last_of( "." );	
			string extension = filename.substr( pos_of_dot + 1 ); 
			if(extension=="clu"||extension=="txt")
				continue;
			all_files_inside_folder.push_back(filename);
			counter++;
			if(counter>1)
				break;
		}
	}
	
	for (unsigned int i=0; i<all_files_inside_folder.size(); i++)
	{
		vector<string> files_absolutepath;
		string url=folder_with_timeperiod.toAscii();		
		files_absolutepath.push_back(url+"/"+all_files_inside_folder[i]);
		this->readFile(files_absolutepath);
		
	}
	
	fileSuccessfullyAdded = true;
	update();
	return fileSuccessfullyAdded;
}*/

void MdiChild::slotRequestFinished(QNetworkReply *reply)
{
	QByteArray data = reply->readAll();
}
void MdiChild::slotSetProgress(qint64 received, qint64 total)
{
    int i=8;
}

void MdiChild::readFile(vector<string> files,std::vector<int> numberof_time_periods,std::string name_of_feature){

	//QByteArray bytes = fileName.toAscii();
	//char *ptr = bytes.data();

	#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::WaitCursor);
	#endif

//	if (!this->Graph)
//		this->Graph = new vsGraph;

	vsCSVFile* CSVFile = new vsCSVFile(files,numberof_time_periods,name_of_feature); //CSVFile->printSelf();
	this->addToCSVList(CSVFile);
	//this->postFileAdditionActions();
	//delete CSVFile;

	#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
	#endif
}

void MdiChild::checkExistenceOfNewFiles(){

	if (this->csvListSize == 0)
		return;

	if (!this->Graph){
		this->Graph = new vsGraph;
		this->displayGraph = this->Graph;
	}

	#ifndef QT_NO_CURSOR
		QApplication::setOverrideCursor(Qt::WaitCursor);
	#endif

	for (int i = 0; i < this->csvListSize; i++){
		if (strcmp(this->csvList[i]->FileName->Extension, "csv") == 0){
			this->Graph->createBipartiteFromCSVFile(this->csvList[i]); //graph->printSelf();
			this->Graph->createAbstractGraph();
			if (this->abstractView){
				emit this->changedLayoutView(true);
			}
			this->createDockWin();
		}
		else if (strcmp(this->csvList[i]->FileName->Extension, "clu") == 0){
			this->Graph->importClusteringReasultsFromCSVFile(this->csvList[i]);
			this->hasClusteringResult = true;
			this->toggleClustersView(true);
		}
		else if (strcmp(this->csvList[i]->FileName->Extension, "pos") == 0){
			if (this->readVertexPositionsfromFile(this->csvList[i])){
				this->FRRuns = this->MaxFRRuns;
				this->PositionsSet = true;
				this->setDrawingPositions();
			}
			this->Layout = FR;
		}

		delete this->csvList[i];
	}
	free(this->csvList);
	this->csvList = NULL;
	this->csvListSize = 0;

	#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
	#endif

	emit this->updateApplicationMenus();
}

void MdiChild::addToCSVList(vsCSVFile *f){

	this->csvList = (vsCSVFile**) realloc(this->csvList, (this->csvListSize + 1) * sizeof(vsCSVFile*));
	if (!this->csvList){
		fprintf(stderr, "Cannot allocate memory for array CSVList. Exiting ...\n");fflush(stderr);fflush(stderr);
		exit(9);
	}

	this->csvList[this->csvListSize] = f;
	this->csvListSize++;
}

void MdiChild::addCSVFileToGraph(){
}

bool MdiChild::checkFeatureExistence(const QString &fileName){

	//QByteArray bytes = (QFileInfo(fileName).fileName()).toAscii();
    QByteArray bytes = fileName.toUtf8();
	char *ptr = bytes.data();

	if (this->Graph && this->Graph->getFeature(ptr) != NULL)
		return true;

	return false;
}

void MdiChild::createDockWin(){

	//Clear the Window
	this->PositionsSet = false;
	this->FRRuns = 0;
	if (this->SFDialog){
		delete this->SFDialog;
		this->SFDialog = NULL;
	}
	this->hasClusteringResult = false;
	emit changedClustersView(false);
	emit changedAnomaliesView(false);
	emit clearSelected();
	//this->calculateEdgeMeanSD();
	free(this->edgeStatistics);
	this->edgeStatistics = NULL;

	this->NoDockWins++;
	int i = this->NoDockWins;

	this->DockWin = (MdiChild**) realloc(this->DockWin, i * sizeof(MdiChild*));

	if ( !this->DockWin ){
		fprintf(stderr, "Cannot allocate memory for DockWin. Exiting ...\n");fflush(stderr);fflush(stderr);
		exit(1);
	}
 
	this->DockWin[i - 1] = new MdiChild(NULL,NULL, this->Graph, i);
	this->DockWin[i - 1]->setGraphDisplayOptions(this->abstractView, this->CuttingThreshold, this->displayFeatureValues, this->displayEdges, this->displayLabels);	

	QString fname(this->Graph->Feature[i]->FileName->name_of_feature);
	QString fname2(this->Graph->Feature[i]->FileName->BaseName);
	//fname.replace("\\", "/");
	//int x = fname.lastIndexOf(QRegExp("[/]"));

	this->DockWin[i - 1]->PathName = QString(this->Graph->Feature[i]->FileName->Path);
	//this->DockWin[i - 1]->PathName = fname.left(x + 1);
	//fname = fname.remove(0, x + 1);

	//x = fname.lastIndexOf(QRegExp("[.]"));
	//fname = fname.left(x);

	this->DockWin[i - 1]->setWindowTitle(fname);
	this->DockWin[i - 1]->setWindowFilePath(fname2);
	//this->DockWin[i - 1]->setWindowTitle(this->DockWin[i - 1]->PathName);

	//connect(this->DockWin[i - 1], SIGNAL(changedDrawnVertices(bool, int)), this, SLOT(setDrawnVertices(bool, int)));
	connect(this->DockWin[i - 1]->isShownAction, SIGNAL(triggered()), this, SLOT(setDrawnVertices()));
	
	
	//connect(this, SLOT(toggleFeatureValuesView(bool enabled)), this->DockWin[i - 1], SLOT(setFeatureValuesView(bool enabled)));
	connect(this, SIGNAL(changedLayoutView(bool)), this->DockWin[i - 1], SLOT(setLayoutView(bool)));
	connect(this, SIGNAL(changedCuttingThreshold(float)), this->DockWin[i - 1], SLOT(setCuttingThreshold(float)));
	connect(this, SIGNAL(changedFeatureValuesView(bool)), this->DockWin[i - 1], SLOT(setFeatureValuesView(bool)));
	connect(this, SIGNAL(changedEdgesView(bool)), this->DockWin[i - 1], SLOT(setEdgesView(bool)));
	connect(this, SIGNAL(changedLabelsView(bool)), this->DockWin[i - 1], SLOT(setLabelsView(bool)));
	connect(this, SIGNAL(changedClustersView(bool)), this->DockWin[i - 1], SLOT(setClustersView(bool)));
	connect(this, SIGNAL(changedAnomaliesView(bool)), this->DockWin[i - 1], SLOT(setAnomaliesView(bool)));

	connect(this->DockWin[i - 1], SIGNAL(clearSelected()), this, SIGNAL(clearSelected()));
	connect(this, SIGNAL(clearSelected()), this->DockWin[i - 1], SLOT(unsetSelected()));

	//connect(this->DockWin[i - 1], SIGNAL(selectedVertex(int)), this, SIGNAL(selectedVertex(int)));
	connect(this->DockWin[i - 1], SIGNAL(refresh()), this, SLOT(animate()));
	connect(this, SIGNAL(refresh()), this->DockWin[i - 1], SLOT(animate()));

	this->PathName = this->DockWin[i - 1]->PathName;
	//this->setWindowTitle(this->PathName);
	this->setWindowTitle("k-partite Graph");
}
void MdiChild::createDockWin_matp(vector<Eigen::MatrixXd> MatP,int dynamic_numberoffeatures)
{
	
	this->DockWin_matp = new dynamic_matp_widget(this,this, MatP,dynamic_numberoffeatures,this->parentwindow->names_of_files);
	QString fname("Dynamic Graph");
	this->DockWin_matp->setPathName(QString("test1"));
	
	this->DockWin_matp->setWindowTitle(fname);
	this->matp_has_to_be_inserted=true;
	this->DockWin_matp->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->DockWin_matp, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowMatpChoices(const QPoint&)));
	
}
void MdiChild::createDockWin_bar(vector<Eigen::MatrixXd> MatP,int dynamic_numberoffeatures)
{
	
	this->DockWin_bar = new dynamic_bar_widget(this,this, MatP,dynamic_numberoffeatures,this->parentwindow->names_of_files);
	QString fname("Dynamic Bar");
	this->DockWin_bar->setPathName(QString("test1"));
	this->DockWin_bar->setWindowTitle(fname);
	this->matp_has_to_be_inserted=true;
	this->DockWin_bar->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->DockWin_bar, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowbarChoices(const QPoint&)));

}
bool MdiChild::loadVertexPositionsFile(){

	QString fileName = this->windowTitle();
	fileName.prepend(this->PathName);
	fileName.append(".pos"); 

	if (!fileName.isEmpty()){
		QFile file(fileName);

		if (file.open(QFile::ReadOnly | QFile::Text)){
			vector<string> files;
			string nameleme=fileName.toUtf8().constData();
			files.push_back(nameleme);
			this->readFile(files);
			return true;
		}
	}

	return false;
}

bool MdiChild::readVertexPositionsfromFile(vsCSVFile *f){

	int fID, vID;
	vsNode *node;
	int NoEvents;

	//fprintf(stderr, "%d\n", f->NoLines);fflush(stderr);fflush(stderr);

	for (int i = 0; i < f->NoLines; i++){

		if ( atoi(f->Line[i]->Word[1]) == 0 )
			fID = 0;
		else
			fID = this->DrawingFeature;

		node = this->Graph->Feature[fID]->getVertex(f->Line[i]->Word[0]);

		if (!node)
			return false;

		vID = node->getLocalID();

		if (fID != 0){
			NoEvents = this->NoDrawingVertices - this->Graph->getNoVertices(fID);
			vID += NoEvents;
		}
		//fprintf(stderr, "%d --> %d %d %d %d\n", this->DrawingFeature, vID, NoEvents, this->NoDrawingVertices, this->Graph->getNoVertices(this->DrawingFeature));fflush(stderr);fflush(stderr);


 		this->Vertex[vID].Pos.x = atof(f->Line[i]->Word[2]);
 		this->Vertex[vID].Pos.y = atof(f->Line[i]->Word[3]);

	}

	fprintf(stderr, "%d\n", f->NoLines);fflush(stderr);fflush(stderr);

	return true;
}

void MdiChild::writeVertexPositionstoFile2(QString outputPath){

    if (this->DrawingFeature != 0)
        return;
    FILE * pFile;

    QString str2 = outputPath;

    QByteArray bytes = str2.toUtf8();
    char *ptr = new char[bytes.size()];
    ptr = bytes.data();
    fprintf(stderr, "\n\n%s\n\n", ptr);fflush(stderr);fflush(stderr);
    //fprintf(stderr, "\n\n%s\t %d\n\n", ptr1, this->PathName.size());fflush(stderr);fflush(stderr);

    pFile = fopen (ptr,"w");

    for (int i = 0; i < this->NoDrawingVertices; ++i)
    {

        fprintf(pFile, "%s,%d,%f,%f,%d\n", this->Vertex[i].Map->getLabel(), this->Vertex[i].Map->Feature->getID(), this->Vertex[i].Pos.x, this->Vertex[i].Pos.y, this->Vertex[i].Map->NoEdges);
    }

    fclose (pFile);
    //delete[] ptr;
}

void MdiChild::writeVertexPositionstoFile(){
//    if (this->DrawingFeature == 0)
//        return;

	FILE * pFile;
	
    //QString str = this->windowTitle();
    QString str2 =this->windowFilePath();
    str2.prepend(this->PathName);

    str2.append(".pos");

    QByteArray bytes = str2.toUtf8();
	char *ptr = new char[bytes.size()];
	ptr = bytes.data();
    fprintf(stderr, "\n\n%s\n\n", ptr);fflush(stderr);fflush(stderr);
	//fprintf(stderr, "\n\n%s\t %d\n\n", ptr1, this->PathName.size());fflush(stderr);fflush(stderr);

	pFile = fopen (ptr,"w");

    for (int i = 0; i < this->NoDrawingVertices; ++i){
		if (!this->Vertex[i].Draw)
			continue;
        fprintf(pFile, "%s,%d,%f,%f\n", this->Vertex[i].Map->getLabel(), this->Vertex[i].Map->Feature->getID(), this->Vertex[i].Pos.x, this->Vertex[i].Pos.y);
	}

	fclose (pFile);
	//delete[] ptr;
}

void MdiChild::setPathName(QString S){
/*
	QDir CD(S);
	QString ND;

	for (int i = 1; i <= 100; ++i){
		ND = QString("ClusterGraph%1") .arg(i);
		if (CD.mkdir(ND)){
			break;
		}	
	}
	S = S.append(ND.append("/"));
*/
    S = S.append("ClusterGraph/");
	this->PathName = S;
	this->setWindowTitle(this->PathName);

	for (int i = 0; i < NoDockWins; ++i){
		//this->DockWin[i]->setPathName(this->PathName);
		this->DockWin[i]->PathName = this->PathName;
		this->DockWin[i]->writePositionsToFile = false;
	}
}

QString MdiChild::getPathName(){
	return this->PathName;
}

MdiChild* MdiChild::getDockWin(int i){
	return DockWin[i];
}

bool MdiChild::setPositions(float dampenLast, float Dimension, float restDistance, float epsilon, float repulseConstant, float springConstant){

	if (this->csvListSize != 0 || this->setNewGraph){
        fprintf(stderr,"%d\n",this->csvListSize);fflush(stderr);fflush(stderr);
		this->checkExistenceOfNewFiles();
		this->checkConsistency();
	}

	if (!this->Graph){
		return false;
	}

	switch (this->Layout){
        case 0:
			this->RandomLayout();
			//fprintf(stderr, "%d\n", this->Layout);fflush(stderr);fflush(stderr);
			this->PositionsSet = true;
	    break;
        case 1:
			this->GeometricLayout();
			this->PositionsSet = true;
    	break;
        case 2:
			if (!this->FRRunning){
				int N = 5;
				if (this->DrawingFeature != 0)
					N = 5;


                for (int i = 0; i < N; i++)
                    if (this->FRRuns < this->MaxFRRuns)
                        this->FRLayout( dampenLast,  Dimension,  restDistance,  epsilon, repulseConstant, springConstant);
            }
            if (this->FRRuns >= this->MaxFRRuns){
				this->PositionsSet = true;
//                if (this->writePositionsToFile && this->abstractView)
//                    this->writeVertexPositionstoFile2("C:\\TwitterData\\testFileAbstractGui.pos");
//                if (this->writePositionsToFile && !this->abstractView)
//                    this->writeVertexPositionstoFile();
            }

			/*this->PositionsSet = true;
			if (this->writePositionsToFile && !this->abstractView)
				this->writeVertexPositionstoFile();*/
			fprintf(stderr, "%d\n", this->FRRuns);fflush(stderr);fflush(stderr);
    	break;
        case 3:
			//this->RandomLayout();
			if ( !this->loadVertexPositionsFile() )
				this->Layout = FR;
    	break;
		default: 
			this->RandomLayout();
			this->PositionsSet = true;
    	break;
	}

	fprintf(stderr, "Physical Positions Set \n");fflush(stderr);fflush(stderr);

	return true;
}

void MdiChild::checkConsistency(){
	if (this->setNewGraph){
		this->displayGraph = this->setNewGraph;
	}

	if (!this->displayGraph)
		return;

	int NoVertices = 0;

	switch (this->DrawingFeature){
		case 0:
			NoVertices = this->displayGraph->getNoVertices();
		break;
		default:
			NoVertices = this->displayGraph->getNoVertices(0) + this->displayGraph->getNoVertices(this->DrawingFeature);
	}

	if (NoVertices != this->NoDrawingVertices || this->setNewGraph){
		this->addDrawingVertices(NoVertices);

		if ( !this->abstractView){
			this->setDrawingVertices();
		}

		this->setDrawnVertices();
		this->RandomLayout();
		this->PositionsSet = false;
		this->FRRuns = 0;
		if (this->DrawingFeature != 0 && !this->abstractView)
			this->Layout = FromFile;
	}
	if (this->DrawingFeature == 0)
		for (int i = this->NoDockWins; i < this->displayGraph->getNoFeatures() - 1; ++i)
			this->createDockWin();
	this->setNewGraph = NULL;
	fprintf(stderr, "New Graph Set\n");fflush(stderr);fflush(stderr);
}

void MdiChild::addDrawingVertices(int N){
	//fprintf(stderr, "\n\n\n\n Feature: %d\n\n\n", this->DrawingFeature);fflush(stderr);fflush(stderr);

	this->Vertex = (vsDrawingVertex*) realloc(this->Vertex, N * sizeof(vsDrawingVertex));
	if ( !this->Vertex ){
		fprintf(stderr, "Cannot allocate memory for array Vertex. Exiting ...\n");fflush(stderr);fflush(stderr);
		exit(9);
     }

	this->NoDrawingVertices = N;
	this->NoDrawingEvents = this->displayGraph->getNoVertices(0);
	this->NoDrawingFeatureValues = N - this->NoDrawingEvents;
	

	if (this->DrawingFeature == 0){
		for (int i = 0; i < N; i++){
			this->Vertex[i].initialize(i, this->displayGraph->Vertex[i]);
			this->displayGraph->Vertex[i]->DV = (&this->Vertex[i]);
		}
	}
	else{
		int i = 0;
		int f = this->DrawingFeature;
		for (int k = 0; k < this->NoDrawingEvents; ++k, ++i){
			this->Vertex[i].initialize(i, this->displayGraph->Feature[0]->Vertex[k]);
			this->displayGraph->Feature[0]->Vertex[k]->DV = (&this->Vertex[i]);
		}
		for (int k = 0; k < this->NoDrawingFeatureValues; ++k, ++i){
			this->Vertex[i].initialize(i, this->displayGraph->Feature[f]->Vertex[k]);
			this->displayGraph->Feature[f]->Vertex[k]->DV = &(this->Vertex[i]);
		}
	
	}

	free(this->Edge);
	this->Edge = NULL;
	this->NoEdges = 0;

	for (int i = this->NoDrawingEvents; i < this->NoDrawingVertices; ++i){
		for (int j = 0; j < this->Vertex[i].Map->NoEdges; ++j){
			if ( !(this->Vertex[i].Map->Edge[j]->getSource()->Feature == this->Vertex[i].Map->Edge[j]->getTarget()->Feature || this->Vertex[i].Map->Edge[j]->getTarget()->Feature->getID() == 0) && this->DrawingFeature != 0)
			continue;

			this->NoEdges++;
			this->Edge = (vsDrawingEdge*) realloc(this->Edge, this->NoEdges * sizeof(vsDrawingEdge));
			if ( !this->Edge ){
				fprintf(stderr, "Cannot allocate memory for array Edge. Exiting ...\n");fflush(stderr);fflush(stderr);
				exit(9);
			}
			vsDrawingVertex* s = this->Vertex[i].Map->Edge[j]->getSource()->DV;
			vsDrawingVertex* t = this->Vertex[i].Map->Edge[j]->getTarget()->DV;
			this->Edge[this->NoEdges - 1].initialize(s, t);
		}
	}
}

void MdiChild::setDrawingVertices(){
	if (this->DrawingFeature == 0){
		this->NoShownVertices = this->NoDrawingVertices;
		return;
	}

	vsFeature *curFeature = this->displayGraph->Feature[0];
	vsNode *curVertex;

	for (int i = 0; i < curFeature->NoVertices; i++){
		curVertex = curFeature->Vertex[i];
		this->Vertex[i].Draw = false;

		for (int j = 0; j < curVertex->NoEdges; j++){
			if (curVertex->Edge[j]->getTarget()->Feature->getID() == this->DrawingFeature){
				this->Vertex[i].Draw = true;
				break;
			}
			//fprintf(stderr, "%d: %d ->> %d\n", i, curVertex->Edge[j]->getTarget()->Feature->getID(), this->DrawingFeature);fflush(stderr);fflush(stderr);
		}
	}

	for (int i = 0; i < this->NoDrawingVertices; i++)
		if (this->Vertex[i].Draw)
			this->NoShownVertices++;

	fprintf(stderr, "%d %d\n", this->NoShownVertices, this->NoDrawingVertices);fflush(stderr);fflush(stderr);
}


void MdiChild::RandomLayout(){

	srand(1268);

	for (int i = 0; i < this->NoDrawingVertices; i++){
		float RN1 = rand() / static_cast<float>( RAND_MAX - 1);
		float RN2 = rand() / static_cast<float>( RAND_MAX - 1);
		this->Vertex[i].Pos.x = 3000 * RN1;
		this->Vertex[i].Pos.y = 3000 * RN2;
	}
}

float radians(float x){
	return 3.14159265 * x /180;
}

void MdiChild::GeometricLayout(){

	int Featurelength = this->getNoFeatures() - 1;

	float EventOffset = 70;
	float FeatureOffset = 10 + 150 / Featurelength;

	float X = 0;
	float Y = 0;
	float W = 3000;
	float H = 3000;
	float Diameter = W < H ? W : H;
	int k = 0;

	if (this->DrawingFeature != 0){
	}


	if (Featurelength <= 2){
		k = 0;

		for (int i = 0; i < this->Graph->getNoVertices(0); ++i, ++k){
			this->Vertex[k].Pos.x = X + W / ((Featurelength + 1) * 2) + W * (Featurelength - 1) / (Featurelength + 1);
			this->Vertex[k].Pos.y  = Y + i * H / (this->NoDrawingEvents - 1);
		}
		fprintf(stderr, "Vertices drawn\n");fflush(stderr);fflush(stderr);

		for (int i = 1; i < this->getNoFeatures(); ++i){
			for (int j = 0; j < this->Graph->getNoVertices(i); ++j, ++k){
				this->Vertex[k].Pos.x = X + ( 1 + (2 * Featurelength) - 4 * (i - 1) ) * W / ((Featurelength + 1) * 2);
				this->Vertex[k].Pos.y = Y + j * H / (this->Graph->getNoVertices(i) - 1);
			}
		}

		return;
	}


	int N = floor(Featurelength / 2.0f) + 1;
	float angle = 360.0f / Featurelength;
	int M = floor(90.0f / angle);
	float D;
	float D1 = 0;
	float D2 = 0;
	
	for (int i = 1; i < N; i++){
		D1 += sin(radians(i * angle));
	}
	
	for (int i = -M; i <= M; i++){
		D2 += cos(radians(i * angle));
	}

    D = D1 > D2 ? D1 : D2;

	if (D >= 1)
		D = Diameter / D;
	else
		D = Diameter / D;
      
	float X1 = X + (W - D) / 2;
	float Y1 = Y + (H - D1 * D) / 2;

	for (int i = 0; i < this->NoDrawingEvents; ++i, ++k){
		this->Vertex[k].Pos.x = X + W / 2;
		this->Vertex[k].Pos.y = Y + (H - D1 * D) / 2 + EventOffset + i * (D1 * D - 2 * EventOffset) / (this->NoDrawingEvents - 1);
	}

    //if (Feature.length > 4)
    //Diameter = Diameter/2;
    
	for (int i = 1; i < this->getNoFeatures(); ++i){
		for (int j = 0; j < this->Graph->getNoVertices(i); ++j, ++k){
			this->Vertex[k].Pos.x = X1 + FeatureOffset * cos(radians((i - 1) * angle)) + j * (D - 2 * FeatureOffset)/(this->Graph->getNoVertices(i) - 1) * cos(radians((i - 1) * angle));
			this->Vertex[k].Pos.y = Y1 + FeatureOffset * sin(radians((i - 1) * angle)) + j * (D - 2  *FeatureOffset)/(this->Graph->getNoVertices(i) - 1) * sin(radians((i - 1) * angle));
		}
		X1 = X1 + D * cos(radians((i - 1) * angle));
		Y1 = Y1 + D * sin(radians((i - 1) * angle));
	}
       
}

void MdiChild::FRLayout(float dampenLast, float Dimension, float restDistance, float epsilon, float repulseConstant, float springConstant)
{
	fprintf(stderr, "FR Started (%d)...", this->NoShownVertices);fflush(stderr);
/*
	if (this->DrawingFeature == 0 && this->NoDockWins == 1){
		for (int i = 0; i < this->DockWin[0]->NoDrawingVertices; ++i){
			this->Vertex[i].Pos.x =  this->DockWin[0]->Vertex[i].Pos.x;
			this->Vertex[i].Pos.y =  this->DockWin[0]->Vertex[i].Pos.y;
		}
		return;	
	}
*/
	this->FRRunning = true;

	int numVerts = this->NoDrawingVertices;

    //float dampenLast = 0.65f;
    //float dampenLast = dampenLast;

    //float Dimension = 3000.0f;
    //float Dimension = Dimension;
	float area = Dimension * Dimension;
	float k = sqrt(area/numVerts);

    if (repulseConstant  < 0)
    {
        repulseConstant = pow(k, 2);
    }

    if(springConstant < 0)
    {
        springConstant = 1.0/k;
    }
    //float repulseConstant = pow(k, 2);
    //float springConstant = 1.0/k;
	//float restDistance = 40.0f;
    //float restDistance = 10.0f;
    //float restDistance = restDistance;
    //float epsilon = 0.0000001f;
    //float epsilon = epsilon;
	float minD = 100 * pow(k, 2);
	minD /= 5;

	//float T = pow((300 - FRRuns)/float(301), 2) * numVerts (10 + 0.5*FRRuns);
	//float T = sqrt(k) * (300 - FRRuns)/float(301);
	//float T = k * pow((300 - FRRuns)/float(301), 2);
	float FRRatio = (this->MaxFRRuns - this->FRRuns) / (float)(this->MaxFRRuns + 1);
	float T = k * FRRatio;
	//float T = 1000;
	//fprintf(stderr, "T: %f\n", T);fflush(stderr);fflush(stderr);

	int i, j, l;
	float deltaX, deltaY, distSquared, dist, prod, force, w;
	float Fo;
	float first, second;
	float a, b;
	vsPoint **Force;
	int tid, nthreads;
	int chunk = 10;

#pragma omp parallel
{
	nthreads = omp_get_num_threads();
}
	Force = new vsPoint*[nthreads];
	for (i = 0; i < nthreads; i++){
		Force[i] = new vsPoint[this->NoDrawingVertices];
		for (j = 0; j < this->NoDrawingVertices; ++j){
			Force[i][j].x = 0;
			Force[i][j].y = 0;
		}
	}


#pragma omp parallel shared(Force) private(tid, i, j, l, deltaX, deltaY, distSquared, dist, prod, force, w, first, second, a, b)
{
	
#pragma omp for schedule (static, chunk)
	for (i = 0; i < numVerts - 1; ++i){
		if (!this->Vertex[i].Draw)
			continue;
		
		tid = omp_get_thread_num();

		for (j = i + 1; j < numVerts; ++j){
			if (!this->Vertex[j].Draw)
				continue;
			//fprintf(stderr," fr layout %d\n", i);fflush(stderr);
			deltaX = this->Vertex[i].Pos.x - this->Vertex[j].Pos.x;
			deltaY = this->Vertex[i].Pos.y - this->Vertex[j].Pos.y;

			distSquared = (float)deltaX*(float)deltaX + (float)deltaY*(float)deltaY + epsilon;
			// Avoid divide by zero
			//if (distSquared == 0)
				//distSquared += epsilon;

			if ( distSquared < minD ){
				//a = log10(this->Vertex[i].Map->NoEdges);
				//b = log10(this->Vertex[j].Map->NoEdges);
				a = this->Vertex[i].NormalSize;
				b = this->Vertex[j].NormalSize;
				//a = 1;
				//b = 1;
				//a = 1 + (this->Vertex[i].NormalSize - 1) * (1 - FRRatio);
				//b = 1 + (this->Vertex[j].NormalSize - 1) * (1 - FRRatio);
				a = a > b ? a : b;
				//a = a * a;

				prod = a * (float)repulseConstant / (float)distSquared;
				first = (float)prod * (float)deltaX;
				second = (float)prod * (float)deltaY;

				Force[tid][i].x += first;
				Force[tid][i].y += second;
				Force[tid][j].x -= first;
				Force[tid][j].y -= second;
			}

		}
	}

#pragma omp for schedule (static, chunk)
	for (l = 0; l < this->NoEdges; ++l){
		if (!this->Edge[l].Draw)
			continue;
		//fprintf(stderr," fr layout a %d\n", i);fflush(stderr);
		tid = omp_get_thread_num();

		//w = this->Edge[l].NormalWeight;
//		if (this->FRRuns < this->MaxFRRuns / 2)
//			w = 1;
//		else
			//w = 1.0f / this->Edge[l].Target->Map->NoEdges;
		//w = 1.0f / sqrt(this->Edge[l].Source->Map->NoEdges * this->Edge[l].Target->Map->NoEdges);

		w = 1.0f / this->Edge[l].Target->Map->NoEdges + (1 - 1.0f / this->Edge[l].Target->Map->NoEdges) * FRRatio;
		
		i = this->Edge[l].Source->ID;
		j = this->Edge[l].Target->ID;

		if (!this->Vertex[i].Draw || !this->Vertex[j].Draw)
			continue;

		deltaX = this->Vertex[i].Pos.x - this->Vertex[j].Pos.x;
		deltaY = this->Vertex[i].Pos.y - this->Vertex[j].Pos.y;

		distSquared = (float)deltaX*(float)deltaX + (float)deltaY*(float)deltaY;
		dist = sqrt(distSquared) + epsilon;
		//dist = sqrt(distSquared) + epsilon - restDistance;

		force = springConstant * pow( (dist - 1 * restDistance), 2);
		prod = (float)w * (float)force / (float)dist;

		//prod = (float)w * (float)springConstant * (float)dist;
		first = (float)prod * (float)deltaX;
		second = (float)prod * (float)deltaY;

		Force[tid][i].x -= first;
		Force[tid][i].y -= second;
		Force[tid][j].x += first;
		Force[tid][j].y += second;
	}
}
//#pragma omp parallel shared(numVerts) private(i, j, Fo, dampenLast)
//{
//#pragma omp for schedule (static, chunk)
	for (i = 0; i < numVerts; ++i){
/*		if ((this->MouseButtonPressed == 0 || this->MouseButtonPressed == 2) && u == this->HitVertex){
			continue;
		}
		if (fixedPos && vtkVariant(fixedPos->GetTuple1(u)).ToInt() == 1)
			continue;
*/		//fprintf(stderr," fr layout b %d\n", i);fflush(stderr);
		if (!this->Vertex[i].Draw)
			continue;

		if (this->mousePressed && (this->HitVertex == i))
			continue;

		for (j = 0; j < nthreads; j++){
			this->Vertex[i].F.x += Force[j][i].x;
			this->Vertex[i].F.y += Force[j][i].y;
		}

		Fo = sqrt((float)this->Vertex[i].F.x*(float)this->Vertex[i].F.x + (float)this->Vertex[i].F.y*(float)this->Vertex[i].F.y) + epsilon;
		//T = this->Impl->F[u].first;
		this->Vertex[i].F.x = (fabs(this->Vertex[i].F.x) < T? fabs(this->Vertex[i].F.x) : T) * this->Vertex[i].F.x / Fo;
		//T = this->Impl->F[u].second;
		this->Vertex[i].F.y = (fabs(this->Vertex[i].F.y) < T? fabs(this->Vertex[i].F.y) : T) * this->Vertex[i].F.y / Fo;

		if (this->FRRuns == 0)
			dampenLast = 0;

		this->Vertex[i].Velocity.x = dampenLast * this->Vertex[i].Velocity.x + (1 - dampenLast) * this->Vertex[i].F.x;
		this->Vertex[i].Velocity.y = dampenLast * this->Vertex[i].Velocity.y + (1 - dampenLast) * this->Vertex[i].F.y;
		//uVel[0] = dampenLast*uVel[0] + this->Impl->F[u].first;
		//uVel[1] = dampenLast*uVel[1] + this->Impl->F[u].second;
		this->Vertex[i].Pos.x += 1.00 * this->Vertex[i].Velocity.x;
		this->Vertex[i].Pos.y += 1.00 * this->Vertex[i].Velocity.y;
		//fprintf(stderr, "%d %f %f %f %f\n", i, this->Vertex[i].Pos.x, this->Vertex[i].Pos.y, this->Vertex[i].Pos.x - this->Vertex[i].Velocity.x, this->Vertex[i].Pos.y - this->Vertex[i].Velocity.y);fflush(stderr);fflush(stderr);

		this->Vertex[i].F.x = 0;
		this->Vertex[i].F.y = 0;
		for (j = 0; j < nthreads; j++){
			Force[j][i].x = 0;
			Force[j][i].y = 0;
		}
	}
//}

	for (i = 0; i < nthreads; i++)
		delete []Force[i];
	//free(Force);
	delete []Force;
	

	//if (this->MouseButtonPressed == -1)
	if ( !(this->mousePressed && (this->HitVertex != this->NoDrawingVertices)) )
		this->FRRuns += 1;

	this->FRRunning = false;
	fprintf(stderr, "OK\n");fflush(stderr);fflush(stderr);
}

void MdiChild::calculateGraphPhysicalBorders(){
	int N = 0;

	for (int i = 0; i < this->NoDrawingVertices; i++){
		if ( !this->Vertex[i].Draw)
			continue;

		this->Min.x = this->Vertex[i].Pos.x;
		this->Max.x = this->Vertex[i].Pos.x;
		this->Min.y = this->Vertex[i].Pos.y;
		this->Max.y = this->Vertex[i].Pos.y;
		N = i;
		break;
	}


	for (int i = N; i < this->NoDrawingVertices; i++){
		if ( !this->Vertex[i].Draw)
			continue;

		if (this->Vertex[i].Pos.x < this->Min.x)
			this->Min.x = this->Vertex[i].Pos.x;
		else if (this->Vertex[i].Pos.x > this->Max.x)
			this->Max.x = this->Vertex[i].Pos.x;
		if (this->Vertex[i].Pos.y < this->Min.y)
			this->Min.y = this->Vertex[i].Pos.y;
		else if (this->Vertex[i].Pos.y > this->Max.y)
			this->Max.y = this->Vertex[i].Pos.y;
	}
}

void MdiChild::setDrawingPositions(){
   
	QSize L = this->size();
	L.setWidth(L.width() - 2 * this->drawingBorder.x());
	L.setHeight(L.height() - 2 * this->drawingBorder.y());

	int offsetX = 0;
	int offsetY = 0;
	int positonFeaturesOffsetX = 30;
	int positonFeaturesOffsetY = 30;

	this->calculateGraphPhysicalBorders();

	
//This piece of code is to keep screen ratio///
	bool keepGraphSizeRatio = true;

	if (keepGraphSizeRatio){
		float ratio = (this->Max.x - this->Min.x) / float(this->Max.y - this->Min.y);
		if ( ratio > L.width() / float(L.height()) ){
			offsetY = L.height(); 
			L.setHeight( L.width() / ratio);
			offsetY = (offsetY - L.height()) / 2;
			//this->moveD.setY(this->drawingBorder.y() + offsetY);
		}
		else{
			offsetX = L.width(); 
			L.setWidth( L.height() * ratio );
			offsetX = (offsetX - L.width()) / 2;
			//this->moveD.setX(this->drawingBorder.x() + offsetX);
		}
	}
//////////////////////////////////////////////


//Check if given features have to be positoned/////
	bool positionFeatures = false;

	if (positionFeatures && this->DrawingFeature == 0){
		L.setWidth(L.width() - 2 * positonFeaturesOffsetX);
		L.setHeight(L.height() - 2 * positonFeaturesOffsetY);
		offsetX += positonFeaturesOffsetX;
		offsetY += positonFeaturesOffsetY;
	}
//////////////////////////////////////////////////

//Calculate Drawing Positions /////////////////////////////////////////////////////////////
	float prodX = L.width()  / (this->Max.x - this->Min.x);
	float prodY = L.height() / (this->Max.y - this->Min.y);

	for (int i = 0; i < this->NoDrawingVertices; i++){
		if (!this->Vertex[i].Draw)
			continue;

		if ((this->mousePressed == true) && (i == this->HitVertex)){
			this->Vertex[i].Pos.x = this->Min.x + (this->Vertex[i].DrawingPos.x - offsetX) * (this->Max.x - this->Min.x) / L.width();
			this->Vertex[i].Pos.y = this->Min.y + (this->Vertex[i].DrawingPos.y - offsetY) * (this->Max.y - this->Min.y) / L.height();
			continue;
		}

		this->Vertex[i].DrawingPos.x = prodX * (this->Vertex[i].Pos.x - this->Min.x) + offsetX;
		this->Vertex[i].DrawingPos.y = prodY * (this->Vertex[i].Pos.y - this->Min.y) + offsetY;
		//painter->drawEllipse(QRectF(this->Vertex[i].Pos.x * L.width(), this->Vertex[i].Pos.y * L.height(), 15, 15));
	}
/////////////////////////////////////////////////////////////////////////////////////////////


//Calculate Drawing Positions  of positioned features////////////////////////////////////////
	offsetX -= positonFeaturesOffsetX;
	offsetY -= positonFeaturesOffsetY;

	if (positionFeatures && this->DrawingFeature == 0){
		int f = 1;
		int N = 0;

		for (int i = 0; i < this->displayGraph->getNoFeatures(); ++i){
			if (i != f)
				N += this->displayGraph->Feature[i]->NoVertices;
			else
				break;
		}
		for (int i = N; i < N + this->displayGraph->Feature[f]->NoVertices; ++i){

			this->Vertex[i].DrawingPos.x = (i - N) * (L.width() + 2 * positonFeaturesOffsetX) / float(this->displayGraph->Feature[f]->NoVertices - 1) + offsetX;
			this->Vertex[i].DrawingPos.y = offsetY;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////////
	
//Calculate VertexSize/////////////
	this->DrawingBox = L;
	this->calculateVertexSize();
///////////////////////////////////
	

	fprintf(stderr, "Drawing Positions Set \n");fflush(stderr);fflush(stderr);
}

void MdiChild::toggleLayoutView(bool enabled){
	if ( !this->Graph->abstractGraph )
		this->Graph->createAbstractGraph();

	emit this->changedLayoutView(enabled);
}

void MdiChild::toggleCuttingThreshold(float v){
	emit this->changedCuttingThreshold(v);
}

void MdiChild::toggleVertexSize(int click){
	emit this->changedVertexSize(click);
}

void MdiChild::toggleVertexTransparency(int click){
	int P = this->VertexOpacity + click * 15;
	P = (P > 0)? P : 0;
	P = (P < 255)? P : 255;

	emit this->changedVertexTransparency(P);
}

void MdiChild::toggleFeatureValuesView(bool enabled){
	//this->displayFeatureValues = enabled;
	emit this->changedFeatureValuesView(enabled);
}

void MdiChild::toggleEdgesView(bool enabled){
	emit this->changedEdgesView(enabled);
}

void MdiChild::toggleEdgeTransparency(int click){
	int P = this->EdgeOpacity + click * 15;
	P = (P > 0)? P : 0;
	P = (P < 255)? P : 255;

	emit this->changedEdgeTransparency(P);
}

void MdiChild::toggleLabelsView(bool enabled){
	//this->displayLabels = enabled;
	emit this->changedLabelsView(enabled);
}

void MdiChild::toggleClustersView(bool enabled){
	emit this->changedClustersView(enabled);
}

void MdiChild::toggleAnomaliesView(bool enabled){
	emit this->changedAnomaliesView(enabled);
}

int MdiChild::translateIDintoGlobalValue(int localID){
	return this->Vertex[localID].Map->getGlobalID();
}

void MdiChild::VertexSelection(int id){
	
	//bool previous = *(this->Vertex[id].Selected);
	//*(this->Vertex[id].Selected) = (true ^ this->Vertex[id].SecondSelection);
	//*(this->Vertex[id].Selected) = *(this->Vertex[id].Selected) ^ this->Vertex[id].SecondSelection;
	//*(this->Vertex[id].Selected) = (true ^ *(this->Vertex[id].Selected));
	//this->Vertex[id].SecondSelection = previous;
	*(this->Vertex[id].Selected) = true;
	this->hasSelectedVertices = true;
	emit refresh();
}

vsGraph* MdiChild::createGraphFromClusters(){

	if (!this->graphFromClustersDialog)
		this->graphFromClustersDialog = new createGraphFromClustersDialog(this);

	this->graphFromClustersDialog->initialize(this->Graph->NoClusters - 1);

	if ( !this->graphFromClustersDialog->exec() )
		return NULL;

	int *cluster = NULL;
	int NoEntries = 0;
	cluster = this->graphFromClustersDialog->getClusters(&NoEntries);

	vsGraph *G = new vsGraph();

	for (int i = 0; i < NoEntries; ++i){
		fprintf(stderr, "CG: %d %d\n", NoEntries, cluster[i]);fflush(stderr);fflush(stderr);
		this->Graph->createGraphFromCluster(G, cluster[i]);
	}
	
	//Graph->printSelf();
	G->createAbstractGraph();

	return G;
}
vsGraph* MdiChild::createGraphFromSelectedVertices(){

	vsGraph *G = new vsGraph();
	if(this->abstractView)
	{
		this->Graph->createGraphFromSelectedAbstractVertices(G);
		//this->Graph->abstractView = enabled;
	}
	else
		this->Graph->createGraphFromSelectedVertices(G);
	//Graph->printSelf();
	G->createAbstractGraph();
	return G;
}

bool MdiChild::importClusteringResults(string ticketurl,string ticket,string dataurl){
	if (!this->Graph)
		return false;
	
	this->loadFile(ticketurl,ticket,dataurl,this->folder_with_timeperiod,"clu");

	return true;
}

bool MdiChild::importClusteringResults_atbeginning(string ticketurl,string ticket,string dataurl){
	/*if (!this->Graph)
		return false;*/

	this->loadFile(ticketurl,ticket,dataurl,this->folder_with_timeperiod,"clu_at_beginning");

	return true;
}
void MdiChild::executeSF(){
	if (!this->Graph)
		return;
	
	bool previousState =  this->hasClusteringResult;
	bool previousState2 =  this->displayClusters;
	this->hasClusteringResult = false;
	this->displayClusters = false;
	emit this->updateApplicationMenus();

	if ( !this->SFDialog )
		this->SFDialog = new executeSFDialog(this);
	
	if ( !this->SFDialog->exec() ){
		this->hasClusteringResult = previousState;
		this->displayClusters = previousState2;
		emit this->updateApplicationMenus();
		return;
	}
	

	float InflationParameter = this->SFDialog->getInflationParameter();
	float Threshold = this->SFDialog->getCuttingThreshold();
	bool AddSelfLoops = this->SFDialog->getSelfLoopsParameter();
	bool Downgrade = this->SFDialog->getNormalizationParameter();
	bool WeightedApproach = this->SFDialog->getWeights((this->Graph->FeatureWeight + 1));
	float ApproximationThreshold =  this->SFDialog->getApproximationThreshold();
	bool SpeedUp = this->SFDialog->getSpeedUp();

	this->Graph->Stochastic_Flows_Algorithm(AddSelfLoops, Downgrade, WeightedApproach, InflationParameter, Threshold, ApproximationThreshold, SpeedUp);

/*
	int il = 500000;
	QProgressDialog progress("Copying files...", "Abort Copy", 0, il, this);
	progress.setWindowModality(Qt::WindowModal);

     for (int i = 0; i < il; i++) {
         progress.setValue(i);

         if (progress.wasCanceled())
             break;
         //... copy one file
     }
     progress.setValue(il);
*/

	this->hasClusteringResult = true;
	this->toggleClustersView(true);
	emit this->updateApplicationMenus();
}

bool MdiChild::hasClusteringResults(){

	return this->hasClusteringResult;
}

void MdiChild::executeStaticAD(){
	if (!this->Graph)
		return;
	
	//bool previousState =  this->hasADResult;
	this->hasADResult = false;
	emit this->updateApplicationMenus();

	this->Graph->Static_AD_Algorithm(true, true, true);

	this->hasADResult = true;
	this->toggleAnomaliesView(true);
	emit this->updateApplicationMenus();
	
}

bool MdiChild::hasADResults(){
	return this->hasADResult;
}

void MdiChild::setDrawnVertices(){
	if (this->DrawingFeature != 0){
		if (this->abstractView){
			//this->calculateEdgeMeanSD();
			this->setAbstractDrawingVertices(false);
			this->update();
		}
		return;
	}
	
	int f1, f2;

	for (int i = 0; i < this->NoEdges; ++i){
		f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
		f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
		//fprintf(stderr, "%d %d %d\n", this->NoDockWins, f1, f2);fflush(stderr);fflush(stderr);
		
		if (f1 >= this->NoDockWins || f2>= this->NoDockWins)
			continue;

		if ((f1 >= 0 && !this->DockWin[f1]->isShownAction->isChecked()) || (f2 >= 0 && !this->DockWin[f2]->isShownAction->isChecked())){
			//this->Edge[i].Source->Draw = false;
			//this->Edge[i].Target->Draw = false;
			this->Edge[i].Draw = false;
		}
		else
			this->Edge[i].Draw = true;

	}

	if (this->abstractView){
		this->calculateEdgeMeanSD(true);
		this->setAbstractDrawingVertices(true);
	}
	else{
		for (int i = 0; i < this->NoDrawingVertices; ++i)
			this->Vertex[i].Draw = false;

		for (int i = 0; i < this->NoEdges; ++i){
			if (this->Edge[i].Draw){
				this->Edge[i].Source->Draw = true;
				this->Edge[i].Target->Draw = true;
			}
		}
	}
	this->update();
}
void MdiChild::dynamic_matp_set_numbers_displayed()
{
	this->DockWin_matp->type_of_labels_displayed=0;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_dates_displayed()
{
	this->DockWin_matp->type_of_labels_displayed=1;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_L1_clicked()
{
	this->DockWin_matp->metric_selected=0;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_L2_clicked()
{
	this->DockWin_matp->metric_selected=1;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_eyclidean_clicked()
{
	this->DockWin_matp->metric_selected=2;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_nodeoperations_clicked()
{
	this->DockWin_matp->metric_selected=3;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_edgeoperations_clicked()
{
	this->DockWin_matp->metric_selected=4;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_set_graphoperations_clicked()
{
	this->DockWin_matp->metric_selected=5;
	this->DockWin_matp->update();
}
void MdiChild::dynamic_matp_show_hide_selected_clicked()
{
	if(this->DockWin_matp->show_selected==true)	
		this->DockWin_matp->show_selected=false;
	else
		this->DockWin_matp->show_selected=true;
	this->DockWin_matp->update();
}


void MdiChild::dynamic_bar_set_numbers_displayed()
{
	this->DockWin_bar->type_of_labels_displayed=0;
	this->DockWin_bar->update();
}
void MdiChild::dynamic_bar_set_dates_displayed()
{
	this->DockWin_bar->type_of_labels_displayed=1;
	this->DockWin_bar->update();
}
void MdiChild::dynamic_bar_show_only_selected_clicked()
{
	if(this->DockWin_bar->show_only_selected==true)	
		this->DockWin_bar->show_only_selected=false;
	else
		this->DockWin_bar->show_only_selected=true;
	this->DockWin_bar->update();
}
/*void MdiChild::keyReleaseEvent(QKeyEvent *event)
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
}*/
