#ifndef _DYNAMIC_AD_GUI_SELECT_DATA_FORM2_H_
#define _DYNAMIC_AD_GUI_SELECT_DATA_FORM2_H_

#include <QDialog>
#include <QWidget>
#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <QTableWidget>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>
#include <QtCore/QStringList>
#include <string>
#include "DynamicAD_GUISelect_Feature.h"
#include <Eigen/Eigenvalues> 
using namespace std;
class DynamicAD;
//class DynamicAD_GUISelect_Feature;
class DynamicAD_GUISelectDataForm2 : public QDialog
{
	Q_OBJECT
public:
	DynamicAD_GUISelectDataForm2(QWidget* parent,string vs_server_directory_url,string folder_with_timeperiod);
	~DynamicAD_GUISelectDataForm2();
	vector<std::string> Getnames_of_files(){return names_of_files;}
	int Getcomplete_clicked(){return complete_clicked;}
	vector<Eigen::MatrixXd> getMatP(){return MatP;}
	int getnumber_of_features(){return number_of_features;}
	void LoadTimeData_atbeginning(string dataset_name,QString timeperiod);
	void LoadFeatureData_atbeginning();
	void Run_atbeginning();
	private slots:
		void slot_onLoadingTimeData();
		void slot_onLoadingFeatureData();
		void slot_onRemoveFeatureData();
		void slot_onRemoveFeatureData(int idx);
		void slot_onPressButtonRun();
		void slot_onPressButtonRun_fromfile();
		void slot_onPressButtonRun3D();
		void slot_onPressButtonSelectTmpDir();
		void slot_on_list_selections();
		void slot_write_data();
		void combochanged(int idx);

private:
	void updateTimeDataTable();
	void addNewFeature(const std::string& strFeatureFile, const bool& bSplit, const char& strDelimiter, const bool& bDiscard, const unsigned int& minCharacters);
	void updateComponentsState();

private:
	//algorithmic:
	DynamicAD* m_dynamicAD;

	//Time gui:
	QGroupBox* m_groupboxTime;
	QLineEdit* m_lineeditTimeDataFile;
	QPushButton* m_pushbuttonTimeDataLoad;
	QTableWidget* m_tablewidgetTimeData;

	//Features gui:
	QGroupBox* m_groupboxFeatures;
	QPushButton* m_pushbuttonFeatureDataLoad;
	QPushButton* m_pushbuttonFeatureDataRemove;
	QPushButton* m_pushbuttonprint;
	QTabWidget* m_tabFeatures;

	//Options gui:
	QGroupBox* m_groupboxOptions;
	QPushButton* m_pushbuttonRun;
	QPushButton* m_pushbuttonrun_fromfile;
	QLineEdit* m_lineeditTempDir;

	QGroupBox* m_groupboxRun;
	QCheckBox* m_checkbox3D;
	QLineEdit* m_lineeditNumberOfCharacters;
	QLabel* m_Select_time_type;
	QLabel* m_Select_num_of_char;
	QComboBox* m_combobox_timetype;
	string vs_server_directory_url;
	QDialog* selectfeatures;
	DynamicAD_GUISelect_Feature* sel_feat;
	vector<std::string> names_of_files;
	int complete_clicked;
	vector<Eigen::MatrixXd> MatP;
	int number_of_features;
	string folder_with_timeperiod;
	bool direct_fromfiledialog;
};

#endif
