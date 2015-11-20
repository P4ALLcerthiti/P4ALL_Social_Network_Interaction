#include "DynamicAD_GUISelect_Feature.h"
#include <sstream>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <iostream>
using namespace std;
DynamicAD_GUISelect_Feature::DynamicAD_GUISelect_Feature(string dynamic_or_static,string ticketurl,string ticket,string dataurl,string data_to_display,string session_id,string folder,QWidget *parent): QDialog(parent)
{
	//parent_dyn_s_d=parent;
	complete_clicked=0;
	resize(800,600);
	QGridLayout* layout = new QGridLayout();
	setLayout(layout);
	QGroupBox* groupboxOptions = new QGroupBox(tr("Parsing Options"), this);
	layout->addWidget(groupboxOptions, 0, 0, 3, 3);

	QGridLayout* layoutOptions = new QGridLayout(groupboxOptions);
	
	QLabel* labelSplitDataDelimiter = new QLabel(tr("Select features:"), groupboxOptions);
	labelSplitDataDelimiter->setDisabled(false);
	layoutOptions->addWidget(labelSplitDataDelimiter, 0,0,Qt::AlignCenter);
	//Split Data
	this->comboboxFeature = new QComboBox(groupboxOptions);
	comboboxFeature->setDisabled(false);
	
	
	
	
	QDir dir(folder.c_str());
	//dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	//dir.setSorting(QDir::Size | QDir::Reversed);
	QFileInfoList list = dir.entryInfoList();
	std::vector<QString> list_of_filenames;
	//std::cout << "     Bytes Filename" << std::endl;
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
			.arg(fileInfo.fileName()));
		std::cout << std::endl;
		if(fileInfo.size()>0)
			list_of_filenames.push_back(fileInfo.fileName());

	}

	if(dynamic_or_static=="dynamic_time")
	{
		for(int i=0;i<list_of_filenames.size();i++)
		{
			QString filename=list_of_filenames.at(i);
			//if(filename.indexOf("date")>0)
			if(filename.indexOf(".csv")>0)
			{
				int to=filename.indexOf(".csv");
				QString featurename=filename.midRef(0,to).toString();
				int firstdelimeter=filename.indexOf("_");
				QString database_name=filename.midRef(0,firstdelimeter).toString();
				comboboxFeature->addItem(featurename,QVariant(filename+" " +database_name));
			}

		}
	}
	else if(dynamic_or_static=="static_cluster")
	{
		for(int i=0;i<list_of_filenames.size();i++)
		{
			QString filename=list_of_filenames.at(i);
			if(filename.indexOf(".clu")>0)
			{
				comboboxFeature->addItem("cluster",QVariant("clusters.clu harmur"));
			}

		}
	}
	else
	{
		for(int i=0;i<list_of_filenames.size();i++)
		{
			QString filename=list_of_filenames.at(i);
			//if(filename.indexOf("date")<0&&filename.indexOf(".csv")>0)
			if(filename.indexOf(".csv")>0)
			{
				int to=filename.indexOf(".csv");
				QString featurename=filename.midRef(0,to).toString();
				int firstdelimeter=filename.indexOf("_");
				QString database_name=filename.midRef(0,firstdelimeter).toString();
				comboboxFeature->addItem(featurename,QVariant(filename+" "+ database_name));
				int a=0;
			}

		}
	}
	/*if (dynamic_or_static=="dynamic")
	{
		
		comboboxFeature->addItem(tr("bot-spamcloud"),QVariant(tr("Rspams_bot.csv spamcloud")));
		comboboxFeature->addItem(tr("country-spamcloud"),QVariant(tr("Rspams_country.csv spamcloud")));
		comboboxFeature->addItem(tr("classA-spamcloud"),QVariant(tr("Rspams_classA.csv spamcloud")));
		comboboxFeature->addItem(tr("lang-spamcloud"),QVariant(tr("Rspams_lang.csv spamcloud")));
		comboboxFeature->addItem(tr("host-spamcloud"),QVariant(tr("Rspams_host.csv spamcloud")));
		comboboxFeature->addItem(tr("IP-spamcloud"),QVariant(tr("Rspams_IP.csv spamcloud")));
		comboboxFeature->addItem(tr("mail-from-spamcloud"),QVariant(tr("Rspams_mail_from.csv spamcloud")));
		comboboxFeature->addItem(tr("subject-spamcloud"),QVariant(tr("Rspams_subject.csv spamcloud")));

		comboboxFeature->addItem(tr("cell_tower"),QVariant(tr("Rspams_Cell_Tower.csv spamcloud")));
		comboboxFeature->addItem(tr("duration"),QVariant(tr("Duration.csv spamcloud")));
		comboboxFeature->addItem(tr("From"),QVariant(tr("Rspams_From.csv spamcloud")));
		comboboxFeature->addItem(tr("to"),QVariant(tr("Rspams_To.csv spamcloud")));
		

	}
	else if(dynamic_or_static=="dynamic_time")
	{
		comboboxFeature->addItem(tr("day-spamcloud"),QVariant(tr("Rspams_day.csv spamcloud")));
		comboboxFeature->addItem(tr("hour-spamcloud"),QVariant(tr("Rspams_hour.csv spamcloud")));
	}
	else if(dynamic_or_static=="static")
	{
		comboboxFeature->addItem(tr("ASN-harmur"),QVariant(tr("feature_asn.csv harmur")));
		comboboxFeature->addItem(tr("Domain-harmur"),QVariant(tr("feature_domain.csv harmur")));
		comboboxFeature->addItem(tr("address-harmur"),QVariant(tr("feature_address.csv harmur")));
		comboboxFeature->addItem(tr("nameserver-harmur"),QVariant(tr("feature_nameserver.csv harmur")));
		
		comboboxFeature->addItem(tr("path-sgnet"),QVariant(tr("SGNET_feature1_ex_path.csv sgnet")));
		comboboxFeature->addItem(tr("source-sgnet"),QVariant(tr("SGNET_feature3_ex_source.csv sgnet")));
		comboboxFeature->addItem(tr("source-sgnet"),QVariant(tr("SGNET_feature3_ex_source.csv sgnet")));
		comboboxFeature->addItem(tr("md5-sgnet"),QVariant(tr("SGNET_feature16_mw_md5.csv sgnet")));
		comboboxFeature->addItem(tr("bot-spamcloud"),QVariant(tr("Rspams_bot.csv spamcloud")));
		comboboxFeature->addItem(tr("country-spamcloud"),QVariant(tr("Rspams_country.csv spamcloud")));
	}
	else if(dynamic_or_static=="static_cluster")
	{
		comboboxFeature->addItem(tr("cluster"),QVariant(tr("clusters.clu harmur")));
	}*/
	/*comboboxFeature->addItem(tr("/"),QVariant(tr("/")));
	comboboxFeature->addItem(tr("\\"),QVariant(tr("\\")));
	comboboxFeature->addItem(tr("Space"), QVariant(tr(" ")));
	comboboxFeature->addItem(tr("Tab"), QVariant(tr("\t")));*/
	layoutOptions->addWidget(comboboxFeature, 1, 0,Qt::AlignCenter);
	
	QPushButton* add_feature = new QPushButton("&add");
	connect(add_feature, SIGNAL(clicked()), this, SLOT(slot_on_add_feature()));
	layoutOptions->addWidget(add_feature, 1, 1,Qt::AlignCenter);
	
	labelallfeatures = new QLabel(tr("You have chosen: "), groupboxOptions);
	labelallfeatures->setDisabled(false);
	layoutOptions->addWidget(labelallfeatures, 1,2,Qt::AlignCenter);

	QPushButton* complete_selection_button = new QPushButton("&complete");
	connect(complete_selection_button, SIGNAL(clicked()), this, SLOT(slot_on_list_selections()));
	layoutOptions->addWidget(complete_selection_button, 2,0,Qt::AlignCenter);
	exec();
}

void DynamicAD_GUISelect_Feature::slot_on_list_selections() 
{
	complete_clicked=1;
	this->reject();
}

void DynamicAD_GUISelect_Feature::slot_on_add_feature() 
{
	/*int a=this->comboboxFeature->currentIndex();
	QVariant b=this->comboboxFeature->itemData(a);
	QString feature_i=b.toString();
	feature_i.toAscii();*/
    string feature=(this->comboboxFeature->itemData(this->comboboxFeature->currentIndex())).toString().toUtf8();
	vector<string> one_feature;
	
	
	string buf; // Have a buffer string
    stringstream ss(feature); // Insert the string into a stream

    

    while (ss >> buf)
        one_feature.push_back(buf);

	/*one_feature.push_back("ASN");
	one_feature.push_back("feature_asn.csv harmur");*/
	allfeatures.push_back(one_feature);
	if(allfeatures.size()==1) 
	{
		QString label=labelallfeatures->text();
        string existing_label=label.toUtf8();
		string newlabel=existing_label+one_feature[0];
		labelallfeatures->setText(newlabel.c_str());
	}
	else 
	{
        string existing_label=labelallfeatures->text().toUtf8();
		labelallfeatures->setText(tr((existing_label+","+one_feature[0]).c_str()));
	}
	//int aB=9;
}

