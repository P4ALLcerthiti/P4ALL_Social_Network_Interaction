#include "DynamicAD_GUISelectDataForm.h"
#include "DynamicAD.h"
#include "DynamicADTimeEntry.h"
//#include "DynamicAD_GUIPlot.h"
//#include "DynamicAD_GUIPlot_direct_from_file.h"
#include <iostream>
#include <QtCore/QSize>
#include <QtCore/QVariant>
#include <QtCore/QDir>
#include <QMessageBox>
#include <fstream>

DynamicAD_GUISelectDataForm::DynamicAD_GUISelectDataForm(QWidget* parent,string vs_server_directory_url,string folder_with_timeperiod) : QDialog(parent)
{
	direct_fromfiledialog=true;
	number_of_features=0;
	this->folder_with_timeperiod=folder_with_timeperiod;
	this->vs_server_directory_url=vs_server_directory_url;
	m_dynamicAD = new DynamicAD("");

	//SETUP GUI:
	this->setWindowTitle("Dynamic Anomaly Detection");
	this->setParent(parent, Qt::Window);
	this->resize(800,600);
	this->setSizeGripEnabled(true);

	//setup components:
	QGridLayout* layout = new QGridLayout();
	setLayout(layout);

	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	splitter->setChildrenCollapsible(false);
	layout->addWidget(splitter,0,0,1,4);
	
	m_groupboxRun=new QGroupBox(this); 
	
	layout->addWidget(m_groupboxRun,1,3,4,1);
	QGridLayout* layoutRun=new QGridLayout(m_groupboxRun);

	m_checkbox3D=new QCheckBox("select 3D",this);
	layoutRun->addWidget(m_checkbox3D,0,0);
	
	m_pushbuttonrun_fromfile = new QPushButton(tr("Run_fr_file"), this);
	m_pushbuttonrun_fromfile->setEnabled(true);
	m_pushbuttonrun_fromfile->setToolTip(tr("At least two feature data files\nhave to be loaded in order to\nactivate the \"Run\" button."));
	layoutRun->addWidget(m_pushbuttonrun_fromfile,1,0);
	connect(m_pushbuttonrun_fromfile, SIGNAL(clicked()), this, SLOT(slot_onPressButtonRun_fromfile()));

	m_pushbuttonRun = new QPushButton(tr("Run"), this);
	m_pushbuttonRun->setEnabled(false);
	m_pushbuttonRun->setToolTip(tr("At least two feature data files\nhave to be loaded in order to\nactivate the \"Run\" button."));
	layoutRun->addWidget(m_pushbuttonRun,2,0);
	connect(m_pushbuttonRun, SIGNAL(clicked()), this, SLOT(slot_onPressButtonRun()));

	QPushButton* buttonClose = new QPushButton(tr("Close"), this);
	buttonClose->setEnabled(true);
	layoutRun->addWidget(buttonClose,3,0);
	connect(buttonClose, SIGNAL(clicked()), this, SLOT(reject()));

	//time groupbox:
	m_groupboxTime = new QGroupBox(this);
	m_groupboxTime->setTitle(tr("Time Data"));
	m_groupboxTime->setMaximumWidth(350);
	splitter->addWidget(m_groupboxTime);

	QGridLayout* layoutTime = new QGridLayout(m_groupboxTime);
	m_groupboxTime->setLayout(layoutTime);
	m_Select_time_type=new QLabel("Select time type");
	layoutTime->addWidget(m_Select_time_type, 0, 0);

	m_Select_num_of_char=new QLabel("Select number of characters");
	m_Select_num_of_char->setVisible(false);
	layoutTime->addWidget(m_Select_num_of_char, 0, 0);

	m_combobox_timetype = new QComboBox(m_groupboxTime);
	m_combobox_timetype->setDisabled(false);
	m_combobox_timetype->addItem(tr("Day"),QVariant(tr("day")));
	m_combobox_timetype->addItem(tr("Hour"),QVariant(tr("hour")));
	m_combobox_timetype->addItem(tr("Select number of characters"),QVariant(tr("Select number of characters")));
	connect(m_combobox_timetype, SIGNAL(currentIndexChanged(int)), this, SLOT(combochanged(int)));
	layoutTime->addWidget(m_combobox_timetype, 1, 0);

	m_lineeditNumberOfCharacters = new QLineEdit(this);
	m_lineeditNumberOfCharacters->setText(tr(""));
	m_lineeditNumberOfCharacters->setVisible(false);
	layoutTime->addWidget(m_lineeditNumberOfCharacters, 1, 0);

	m_lineeditTimeDataFile = new QLineEdit(this);
	m_lineeditTimeDataFile->setText(tr(""));
	layoutTime->addWidget(m_lineeditTimeDataFile, 2, 0);

	m_pushbuttonTimeDataLoad = new QPushButton(this);
	m_pushbuttonTimeDataLoad->setText(tr("Load Time Data..."));
	layoutTime->addWidget(m_pushbuttonTimeDataLoad, 2, 1);
	connect(m_pushbuttonTimeDataLoad, SIGNAL(clicked()), this, SLOT(slot_onLoadingTimeData()));

	//create table of time entries:

	m_tablewidgetTimeData = new QTableWidget(0, 2, this);
	layoutTime->addWidget(m_tablewidgetTimeData, 3, 0, 1, 2);
	m_tablewidgetTimeData->setHorizontalHeaderLabels(QStringList() << tr("Unique Time Id") << tr("Number of Entries"));
	m_tablewidgetTimeData->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignCenter);
	m_tablewidgetTimeData->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);
	m_tablewidgetTimeData->resizeColumnsToContents();
	m_tablewidgetTimeData->resizeRowsToContents();
	m_tablewidgetTimeData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_tablewidgetTimeData->setAutoScroll(false);
	m_tablewidgetTimeData->setSelectionBehavior(QAbstractItemView::SelectRows);
	//features groupbox:
	m_groupboxFeatures = new QGroupBox(this);
	m_groupboxFeatures->setTitle(tr("Feature Data"));
	m_groupboxFeatures->setDisabled(true);
	m_groupboxFeatures->setBaseSize(QSize(450,m_groupboxFeatures->height()));
	splitter->addWidget(m_groupboxFeatures);

	QGridLayout* layoutFeatures = new QGridLayout(m_groupboxFeatures);
	m_groupboxFeatures->setLayout(layoutFeatures);

	//button add feature:
	m_pushbuttonFeatureDataLoad = new QPushButton(this);
	m_pushbuttonFeatureDataLoad->setText(tr("Add..."));
	layoutFeatures->addWidget(m_pushbuttonFeatureDataLoad, 0, 0);
	connect(m_pushbuttonFeatureDataLoad, SIGNAL(clicked()), this, SLOT(slot_onLoadingFeatureData()));
	
	/*m_pushbuttonprint = new QPushButton(this);
	m_pushbuttonprint->setText(tr("print..."));
	layoutFeatures->addWidget(m_pushbuttonprint, 0, 2);
	connect(m_pushbuttonprint, SIGNAL(clicked()), this, SLOT(slot_write_data()));
	m_pushbuttonprint->setEnabled(true);
*/
	//button remove feature:
	m_pushbuttonFeatureDataRemove = new QPushButton(this);
	m_pushbuttonFeatureDataRemove->setText(tr("Remove..."));
	layoutFeatures->addWidget(m_pushbuttonFeatureDataRemove, 0, 1);
	connect(m_pushbuttonFeatureDataRemove, SIGNAL(clicked()), this, SLOT(slot_onRemoveFeatureData()));
	m_pushbuttonFeatureDataRemove->setEnabled(false);

	//add tab layout
	m_tabFeatures = new QTabWidget(this);
	m_tabFeatures->setTabsClosable(true);
	layoutFeatures->addWidget(m_tabFeatures,1,0, 1, 2);
	connect(m_tabFeatures, SIGNAL(tabCloseRequested(int)), this, SLOT(slot_onRemoveFeatureData(int)));

	//ALGORITHM OPTIONS
	m_groupboxOptions = new QGroupBox(this);
	m_groupboxOptions->setTitle(tr("Options"));
	m_groupboxOptions->setDisabled(true);
	layout->addWidget(m_groupboxOptions, 1,0,1,3);

	QGridLayout* layoutOptions = new QGridLayout(m_groupboxOptions);

	QPushButton* buttonSelectTmpDir = new QPushButton("Select Temp Directory...");
	layoutOptions->addWidget(buttonSelectTmpDir, 0, 0);
	connect(buttonSelectTmpDir, SIGNAL(clicked()), this, SLOT(slot_onPressButtonSelectTmpDir()));

	m_lineeditTempDir = new QLineEdit("");
	m_lineeditTempDir->setReadOnly(true);
	layoutOptions->addWidget(m_lineeditTempDir, 0, 1);
}

DynamicAD_GUISelectDataForm::~DynamicAD_GUISelectDataForm()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	delete m_dynamicAD;
	QApplication::restoreOverrideCursor();
}

void DynamicAD_GUISelectDataForm::slot_onLoadingTimeData()
{
	if (m_dynamicAD->GetNumFeatures() > 0)
	{
		QMessageBox msgBox(QMessageBox::Warning, tr("Empty Feature data"), tr("Feature data have been already assigned to the current time data. This action will clear all current feature data. Do you want to proceed?"), QMessageBox::Yes | QMessageBox::No, this);
		msgBox.exec();

		if (msgBox.result() == QMessageBox::No ||
			msgBox.result() == QMessageBox::NoButton)
		{
			return;
		}
	}
	
	string filename;
	if(direct_fromfiledialog==true)
	{
		filename = QFileDialog::getOpenFileName(
			this,
			tr("Open Time Data File"), 
			tr("./Data/SpamCloud/March"),
			tr("CSV file (*.csv)")
			).toLatin1().data();
	}
	else
	{
		sel_feat=new DynamicAD_GUISelect_Feature("dynamic_time","","","","","",this->folder_with_timeperiod,this);
		if (sel_feat->allfeatures.size()==0||sel_feat->get_complete_clicked()==0) return;
		filename=this->folder_with_timeperiod+"/"+sel_feat->allfeatures[0][0];

	}
	DynamicADTimeEntry::TimeType timetype;
	if (!(filename.length()==0))
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		this->repaint();
		m_lineeditTimeDataFile->setText(QString::fromUtf8(filename.c_str()));

		std::string strName = filename.substr(filename.find_last_of('/')+1);
		strName = strName.substr(0, strName.find_last_of('.'));
		//mono klisi me vasi to id
		string time_type_str;
		int number_of_semantic_characters=0;
		if(m_combobox_timetype->isVisible())
		{
			time_type_str=m_combobox_timetype->itemData(m_combobox_timetype->currentIndex()).toString().toLatin1();
			m_dynamicAD->LoadTimeCSV(filename, strName, number_of_semantic_characters,time_type_str);
		}
		else
		{
			number_of_semantic_characters=atoi(m_lineeditNumberOfCharacters->text().toLatin1());
			m_dynamicAD->LoadTimeCSV(filename, strName, number_of_semantic_characters,"cut a number of characters");
		}
		m_tabFeatures->clear();
		updateTimeDataTable();
		updateComponentsState();
		QApplication::restoreOverrideCursor();
	}
}

void DynamicAD_GUISelectDataForm::slot_onLoadingFeatureData()
{
	if (m_dynamicAD->GetNumTimeGroups() < 2) return;
	if(direct_fromfiledialog==true)
	{
		QFileDialog* fileDialog = new QFileDialog(this, tr("Load Feature Data File"));
		fileDialog->setDirectory("./Data/SpamCloud/March");
//		fileDialog->setFilter(tr("CSV file (*.csv)"));
		fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
		fileDialog->setFileMode(QFileDialog::ExistingFiles);
		fileDialog->setReadOnly(true);
		fileDialog->setOption(QFileDialog::Option::DontUseNativeDialog, false);

		//add extra components to the layout:
		QGridLayout* layout = (QGridLayout*)fileDialog->layout();

		QGroupBox* groupboxOptions = new QGroupBox(tr("Parsing Options"), fileDialog);
		layout->addWidget(groupboxOptions, layout->rowCount(), 0, 1, 3);

		QGridLayout* layoutOptions = new QGridLayout(groupboxOptions);

		//Split Data
		QCheckBox* checkSplitFeatureData = new QCheckBox(tr("Split Data"), groupboxOptions);
		checkSplitFeatureData->setChecked(true);
		layoutOptions->addWidget(checkSplitFeatureData, 0, 0);

		QLabel* labelSplitDataDelimiter = new QLabel(tr("Delimiter for splitting words:"), groupboxOptions);
		labelSplitDataDelimiter->setDisabled(false);
		layoutOptions->addWidget(labelSplitDataDelimiter, 0, 1);

		QComboBox* comboboxDelimiter = new QComboBox(groupboxOptions);
		comboboxDelimiter->setDisabled(false);
		comboboxDelimiter->addItem(tr(","),QVariant(tr(",")));
		comboboxDelimiter->addItem(tr("|"),QVariant(tr("|")));
		comboboxDelimiter->addItem(tr("/"),QVariant(tr("/")));
		comboboxDelimiter->addItem(tr("\\"),QVariant(tr("\\")));
		comboboxDelimiter->addItem(tr("Space"), QVariant(tr(" ")));
		comboboxDelimiter->addItem(tr("Tab"), QVariant(tr("\t")));
		layoutOptions->addWidget(comboboxDelimiter, 0, 2);

		connect(checkSplitFeatureData, SIGNAL(toggled(bool)), labelSplitDataDelimiter, SLOT(setEnabled(bool)));
		connect(checkSplitFeatureData, SIGNAL(toggled(bool)), comboboxDelimiter, SLOT(setEnabled(bool)));

		//Discard Words:
		QCheckBox* checkDiscardSmallWords = new QCheckBox(tr("Discard Small Words"), groupboxOptions);
		checkDiscardSmallWords->setTristate(false);
		checkDiscardSmallWords->setChecked(true);
		layoutOptions->addWidget(checkDiscardSmallWords, 1, 0);

		QLabel* labelSmallWordSize = new QLabel(tr("Words will be ignored if size less than:"), groupboxOptions);
		labelSmallWordSize->setDisabled(false);
		layoutOptions->addWidget(labelSmallWordSize, 1, 1);

		QLineEdit* lineEditWordSize = new QLineEdit(tr("3"),groupboxOptions);
		lineEditWordSize->setDisabled(false);
		lineEditWordSize->setMaxLength(3);
		lineEditWordSize->setMaximumWidth(lineEditWordSize->minimumSizeHint().width()/1.5);
		layoutOptions->addWidget(lineEditWordSize, 1, 2);

		connect(checkDiscardSmallWords, SIGNAL(toggled(bool)), labelSmallWordSize, SLOT(setEnabled(bool)));
		connect(checkDiscardSmallWords, SIGNAL(toggled(bool)), lineEditWordSize, SLOT(setEnabled(bool)));
		//layoutOptions->addWidget(comboboxDelimiter, 1, 1,Qt::AlignCenter);
		QPushButton* complete_selection_button = new QPushButton("&complete");

		connect(complete_selection_button, SIGNAL(clicked()), this, SLOT(slot_on_list_selections()));
		//connect(complete_selection_button, SIGNAL(clicked()), selectfeatures, SLOT(reject()));
		layoutOptions->addWidget(complete_selection_button, 2, 1,Qt::AlignCenter);

		//selectfeatures->exec();

		//Start:
		if (fileDialog->exec())
		{
			QStringList vFileName = fileDialog->selectedFiles();

			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			this->repaint();

			char chDelimiter = QVariant(comboboxDelimiter->itemData(comboboxDelimiter->currentIndex())).toString().toStdString().c_str()[0];
			unsigned int numCharacters = QVariant(lineEditWordSize->text()).toUInt();

			for (unsigned int i=0; i<vFileName.size(); i++)
			{
				addNewFeature(vFileName.at(i).toLatin1().data(), checkSplitFeatureData->isChecked(), chDelimiter, checkDiscardSmallWords->isChecked(), numCharacters);
			}
			number_of_features+=vFileName.size();
			QApplication::restoreOverrideCursor();
		}
		else
		{
			return;
		}
	}
	else
	{
		sel_feat=new DynamicAD_GUISelect_Feature("dynamic","","","","","",this->folder_with_timeperiod,this);
		if (sel_feat->allfeatures.size()==0||sel_feat->get_complete_clicked()==0) 
			return;
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		this->repaint();

		for (unsigned int i=0; i<sel_feat->allfeatures.size(); i++)
		{
			string url="";
			//if(sel_feat->allfeatures[i][1]=="spamcloud") url="Data/SpamCloud/March/33692/";
			url=this->folder_with_timeperiod;
			this->addNewFeature(url+"/"+sel_feat->allfeatures[i][0], true, ',', true, 2);
		}
		//number_of_features=sel_feat->allfeatures.size();
		
		QApplication::restoreOverrideCursor();
		delete sel_feat;
	}
}
void DynamicAD_GUISelectDataForm::slot_on_list_selections()
{
	//this->selectfeatures->reject();
}
void DynamicAD_GUISelectDataForm::slot_onRemoveFeatureData()
{
	QMessageBox msgBox(QMessageBox::Question, tr("Feature data removal"), tr("Are you sure you want to remove the selected feature data?"), QMessageBox::Yes | QMessageBox::No, this);
	msgBox.exec();

	if (msgBox.result() == QMessageBox::No ||
		msgBox.result() == QMessageBox::NoButton)
	{
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QVariant varName = m_tabFeatures->currentWidget()->property("feature_name");
	DynamicADFeature* featureToBeRemoved = m_dynamicAD->GetFeature(varName.toString().toStdString());
	m_tabFeatures->removeTab(m_tabFeatures->currentIndex());
	updateComponentsState();
	this->repaint();
	m_dynamicAD->RemoveFeatureCSV(featureToBeRemoved);
	updateComponentsState();
	QApplication::restoreOverrideCursor();
}

void DynamicAD_GUISelectDataForm::slot_onRemoveFeatureData(int idx)
{
	QMessageBox msgBox(QMessageBox::Question, tr("Feature data removal"), tr("Are you sure you want to remove the selected feature data?"), QMessageBox::Yes | QMessageBox::No, this);
	msgBox.exec();

	if (msgBox.result() == QMessageBox::No ||
		msgBox.result() == QMessageBox::NoButton)
	{
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QVariant varName = m_tabFeatures->widget(idx)->property("feature_name");
	DynamicADFeature* featureToBeRemoved = m_dynamicAD->GetFeature(varName.toString().toStdString());
	m_tabFeatures->removeTab(idx);
	updateComponentsState();
	this->repaint();
	m_dynamicAD->RemoveFeatureCSV(featureToBeRemoved);
	number_of_features--;
	updateComponentsState();
	QApplication::restoreOverrideCursor();
}
void DynamicAD_GUISelectDataForm::slot_onPressButtonRun_fromfile()
{
	
//	if(m_checkbox3D->isChecked())
//	{
//		DynamicAD_GUIPlot_direct_from_fileDialog_3D* plotDialog = new DynamicAD_GUIPlot_direct_from_fileDialog_3D(m_dynamicAD,this->folder_with_timeperiod, this);
//		plotDialog->exec();
//	}
//	else
//	{
//		DynamicAD_GUIPlot_direct_from_fileDialog* plotDialog = new DynamicAD_GUIPlot_direct_from_fileDialog(m_dynamicAD,this->folder_with_timeperiod, this);
//		plotDialog->exec();
//	}
}
void DynamicAD_GUISelectDataForm::slot_onPressButtonRun()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	this->repaint();
	std::ofstream ofs("test_m_matrix.txt");
	m_dynamicAD->m_vResult_clear();
	m_dynamicAD->CreateGraphs();
	/*for(int graph_id=0;graph_id<m_dynamicAD->m_vGraph.size();graph_id++)
	{

		for(int i=0;i<m_dynamicAD->m_vGraph[graph_id]->m_matrix.rows();i++)
		{
			for(int j=0;j<m_dynamicAD->m_vGraph[graph_id]->m_matrix.cols();j++)
			{
				ofs<<m_dynamicAD->m_vGraph[graph_id]->m_matrix.coeffRef(i, j)<<" ";
		
			}
			ofs<<"\n";
		}
		ofs<<"\n\n";
	}
	ofs.close();*/
	/*std::ofstream nodesstream("nodes.txt",std::fstream::app);
	for(int i=0;i<m_dynamicAD->GetGraph().size();i++)
	{
		for(int f=0;f<m_dynamicAD->GetGraph()[i]->Get_numfeatures();f++)
		{
			nodesstream<<"\n"<<m_dynamicAD->GetGraph()[i]->Get_mstrname().c_str()<<" "<<f<<" "<<"\n";
			for(std::set<std::string>::iterator it=m_dynamicAD->GetGraph()[i]->Get_m_vnode()[f].begin();it!=m_dynamicAD->GetGraph()[i]->Get_m_vnode()[f].end();it++)
			{
				nodesstream<<(*it)<<"\n";
			}
		
		}
	}
	nodesstream.close();*/
	std::string strDir(m_lineeditTempDir->text().toStdString());


	if (strDir == "")
	{
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L1);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L2);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EUCLIDEAN);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_NodeOperations);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EdgeOperations);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_GraphOperations);
	}
	else
	{
		QDir qDir(m_lineeditTempDir->text());

		qDir.mkdir("L1");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L1, strDir+"/L1");

		qDir.mkdir("L2");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L2, strDir+"/L2");

		qDir.mkdir("EUCLIDEAN");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EUCLIDEAN, strDir+"/EUCLIDEAN");

		qDir.mkdir("Nodes");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_NodeOperations, strDir+"/Nodes");

		qDir.mkdir("Edges");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EdgeOperations, strDir+"/Edges");

		qDir.mkdir("NodesEdges");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_GraphOperations, strDir+"/NodesEdges");
	}

	QApplication::restoreOverrideCursor();
//DynamicAD_GUIPlotDialog* plotDialog;
//if(m_checkbox3D->isChecked())
//{
//	//edo mporoume na steiloume anti gia m_dynamicad to id tou client kai na xeiristei o DynamicAD_GUIPlotDialog_3D
//	//ayta pou thelei
//	DynamicAD_GUIPlotDialog_3D* plotDialog = new DynamicAD_GUIPlotDialog_3D(m_dynamicAD, this);
//	plotDialog->exec();
//}
//else
//{
	
//	DynamicAD_GUIPlotDialog* plotDialog = new DynamicAD_GUIPlotDialog(m_dynamicAD, this);
//	plotDialog->exec();
//}
}


void DynamicAD_GUISelectDataForm::slot_onPressButtonRun3D()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	this->repaint();

	m_dynamicAD->CreateGraphs();

	std::string strDir(m_lineeditTempDir->text().toStdString());


	if (strDir == "")
	{
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L1);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L2);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EUCLIDEAN);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_NodeOperations);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EdgeOperations);
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_GraphOperations);
	}
	else
	{
		QDir qDir(m_lineeditTempDir->text());

		qDir.mkdir("L1");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L1, strDir+"/L1");

		qDir.mkdir("L2");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_L2, strDir+"/L2");

		qDir.mkdir("EUCLIDEAN");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EUCLIDEAN, strDir+"/EUCLIDEAN");

		qDir.mkdir("Nodes");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_NodeOperations, strDir+"/Nodes");
  
		qDir.mkdir("Edges");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_EdgeOperations, strDir+"/Edges");

		qDir.mkdir("NodesEdges");
		m_dynamicAD->PerformMDS(DynamicADGraph::DistanceType_GraphOperations, strDir+"/NodesEdges");
	}

	QApplication::restoreOverrideCursor();

//	DynamicAD_GUIPlotDialog_3D* plotDialog = new DynamicAD_GUIPlotDialog_3D(m_dynamicAD, this);
//	plotDialog->exec();
}

void DynamicAD_GUISelectDataForm::slot_onPressButtonSelectTmpDir()
{
	QString strDir = QFileDialog::getExistingDirectory(this, tr("Select Temporary Directory for Results"), tr("./RemTest"));

	if (!strDir.isEmpty())
	{
		m_lineeditTempDir->setText(strDir);
	}
}

void DynamicAD_GUISelectDataForm::updateTimeDataTable()
{
	//edo kaloume na mas girisei vector< std::pair<std::string, unsigned int> >
	std::vector< std::pair<std::string, unsigned int> > vDataPerTime = m_dynamicAD->GetDataDistributionPerTime();

	if (vDataPerTime.size() == 0) return;

	m_tablewidgetTimeData->clearContents();

	m_tablewidgetTimeData->setRowCount(vDataPerTime.size()+1); //+1 for the total...

	QTableWidgetItem* tableItem;
	for (unsigned int i=0; i<vDataPerTime.size(); i++)
	{
		tableItem = new QTableWidgetItem();
		tableItem->setText(QString(vDataPerTime[i].first.c_str()));
		tableItem->setTextAlignment(Qt::AlignCenter);
		m_tablewidgetTimeData->setItem(i,0, tableItem);

		tableItem = new QTableWidgetItem();
		tableItem->setText(QVariant(vDataPerTime[i].second).toString());
		tableItem->setTextAlignment(Qt::AlignCenter);
		m_tablewidgetTimeData->setItem(i,1, tableItem);
	}

	//add total:
	tableItem = new QTableWidgetItem();
	tableItem->setText(tr("Total:"));
	tableItem->setTextAlignment(Qt::AlignCenter);
	tableItem->setBackgroundColor(QColor(154,205,50));
	m_tablewidgetTimeData->setItem(vDataPerTime.size(), 0, tableItem);

	tableItem = new QTableWidgetItem();
	/*
	unsigned int nTotal = 0;
	for (unsigned int i=0; i<vDataPerTime.size(); i++)
	{
		nTotal += vDataPerTime[i].second;
	}
	*/
	tableItem->setText(QVariant(m_dynamicAD->GetNumTimeEntries()).toString());
	tableItem->setTextAlignment(Qt::AlignCenter);
	tableItem->setBackgroundColor(QColor(154,205,50));
	m_tablewidgetTimeData->setItem(vDataPerTime.size(), 1, tableItem);

	m_tablewidgetTimeData->resizeColumnsToContents();
	m_tablewidgetTimeData->resizeRowsToContents();
}

void DynamicAD_GUISelectDataForm::addNewFeature(const std::string& strFeatureFile, const bool& bSplit, const char& chDelimiter, const bool& bDiscard, const unsigned int& minCharacters)
{
	//LoadFeatureCSV
	std::string strFeatureName = strFeatureFile.substr(strFeatureFile.find_last_of('/')+1);
	strFeatureName = strFeatureName.substr(0, strFeatureName.find_last_of('.'));
	
	//load to the model:
	m_dynamicAD->LoadFeatureCSV(strFeatureFile, strFeatureName, bDiscard?minCharacters:0, bSplit, chDelimiter);
	DynamicADFeature* feature = m_dynamicAD->GetFeature(strFeatureName);
	//edo tha kalesoume na girisei ayto pou theloume map<std::string, unsigned int>
	const std::map<std::string, unsigned int> mapWord2Times = feature->GetTimesPerDataWord();

	//create a new card
	QTableWidget* featureTable = new QTableWidget(mapWord2Times.size()+1, 2);
	featureTable->setHorizontalHeaderLabels(QStringList() << tr("Feature Id") << tr("Number of Entries"));
	featureTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignCenter);
	featureTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);
	featureTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	featureTable->setAutoScroll(false);
	featureTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	std::map<std::string, unsigned int>::const_iterator it;
	QTableWidgetItem* item;
	unsigned int row=0;
	unsigned int nTotal=0;
	for (it=mapWord2Times.begin(); it!=mapWord2Times.end(); it++)
	{
		item = new QTableWidgetItem(QVariant(it->first.c_str()).toString());
		item->setTextAlignment(Qt::AlignCenter);
		featureTable->setItem(row, 0, item);

		item = new QTableWidgetItem(QVariant(it->second).toString());
		item->setTextAlignment(Qt::AlignCenter);
		featureTable->setItem(row, 1, item);

		nTotal += it->second;

		row++;
	}
	
	//add total:
	item = new QTableWidgetItem();
	item->setText(tr("Total:"));
	item->setTextAlignment(Qt::AlignCenter);
	item->setBackgroundColor(QColor(154,205,50));
	featureTable->setItem(row, 0, item);

	item = new QTableWidgetItem();
	item->setText(QVariant(nTotal).toString());
	item->setTextAlignment(Qt::AlignCenter);
	item->setBackgroundColor(QColor(154,205,50));
	featureTable->setItem(row, 1, item);

	featureTable->resizeColumnsToContents();
	featureTable->resizeRowsToContents();

	featureTable->setProperty("feature_name", QVariant(QString(strFeatureName.c_str())));

	m_tabFeatures->setCurrentIndex(m_tabFeatures->addTab(featureTable, strFeatureName.c_str()));
	m_tabFeatures->setTabToolTip(m_tabFeatures->currentIndex(), QString(strFeatureFile.c_str()));

	this->updateComponentsState();
}

void DynamicAD_GUISelectDataForm::updateComponentsState()
{
	//mas girnaei int
	//push button run:
	m_pushbuttonRun->setEnabled(m_dynamicAD->GetNumFeatures() >= 2);

	//group box options:
	m_groupboxOptions->setEnabled(m_dynamicAD->GetNumFeatures() >= 2);

	//group box features:
	m_groupboxFeatures->setEnabled(m_dynamicAD->GetNumTimeGroups() > 2);

	//button remove features:
	m_pushbuttonFeatureDataRemove->setEnabled(m_dynamicAD->GetNumFeatures() > 0);
}
void DynamicAD_GUISelectDataForm::slot_write_data()
{
	std::ofstream ofs("dynamic_rawdata.txt");
	ofs<<m_dynamicAD->GetTimeEntry(0)->GeTimeType()<<"\n";
	for(int i=0;i<m_dynamicAD->GetNumTimeEntries();i++)
	{
		ofs<<(m_dynamicAD->GetTimeEntry(i)->GetKeyData()).c_str()<<","<<(m_dynamicAD->GetTimeEntry(i)->GetTimeData()).c_str();
	}
	for(int i=0;i<m_dynamicAD->GetNumFeatures();i++)
	{
		ofs<<(m_dynamicAD->GetFeature(i)->GetName()).c_str()<<"\n";
		for(int j=0;j<m_dynamicAD->GetFeature(i)->GetNumEntries();j++)
		{
			ofs<<(m_dynamicAD->GetFeature(i)->GetEntry(j)->GetKeyData()).c_str()<<","<<(m_dynamicAD->GetFeature(i)->GetEntry(j)->GetFeatureDataSingle()).c_str()<<"\n";
		}
	}
}
void DynamicAD_GUISelectDataForm::combochanged(int idx)
{
	if(idx==2)
	{
		m_lineeditNumberOfCharacters->setVisible(true);
		m_Select_num_of_char->setVisible(true);
		m_Select_time_type->setVisible(false);
		m_combobox_timetype->setVisible(false);
	}
}

/*
select_features::select_features(QWidget* parent):QDialog(parent)
{
	/*selectfeatures=new QDialog();
	selectfeatures->resize(800,600);
	QGridLayout* layout = new QGridLayout();
	selectfeatures->setLayout(layout);
	QGroupBox* groupboxOptions = new QGroupBox(tr("Parsing Options"), selectfeatures);
	layout->addWidget(groupboxOptions, 0, 0, 3, 1);

	QGridLayout* layoutOptions = new QGridLayout(groupboxOptions);
	
	QLabel* labelSplitDataDelimiter = new QLabel(tr("Select features:"), groupboxOptions);
	labelSplitDataDelimiter->setDisabled(false);
	layoutOptions->addWidget(labelSplitDataDelimiter, 0, 0,Qt::AlignCenter);
	//Split Data
	QComboBox* comboboxDelimiter = new QComboBox(groupboxOptions);
	comboboxDelimiter->setDisabled(false);
	comboboxDelimiter->addItem(tr(","),QVariant(tr(",")));
	comboboxDelimiter->addItem(tr("|"),QVariant(tr("|")));
	comboboxDelimiter->addItem(tr("/"),QVariant(tr("/")));
	comboboxDelimiter->addItem(tr("\\"),QVariant(tr("\\")));
	comboboxDelimiter->addItem(tr("Space"), QVariant(tr(" ")));
	comboboxDelimiter->addItem(tr("Tab"), QVariant(tr("\t")));
	layoutOptions->addWidget(comboboxDelimiter, 1, 1,Qt::AlignCenter);
	QPushButton* complete_selection_button = new QPushButton("&complete");
	connect(complete_selection_button, SIGNAL(clicked()), this, SLOT(slot_on_list_selections()));
	//connect(complete_selection_button, SIGNAL(clicked()), selectfeatures, SLOT(reject()));
	layoutOptions->addWidget(complete_selection_button, 2, 1,Qt::AlignCenter);
	
	selectfeatures->exec();
}*/
