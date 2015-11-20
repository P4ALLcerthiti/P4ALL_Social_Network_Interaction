#ifndef _DYNAMIC_AD_GUI_SELECT_FEATURE_H_
#define _DYNAMIC_AD_GUI_SELECT_FEATURE_H_

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


//class DynamicAD_GUISelectDataForm;
using namespace std;

//class DynamicAD_GUISelectDataForm;
class DynamicAD_GUISelect_Feature : public QDialog
{
	Q_OBJECT
public:
	
	DynamicAD_GUISelect_Feature(string dynamic_or_static,string ticketurl,string ticket,string dataurl,string data_to_display,string session_id,string folder,QWidget *parent=0) ;
	vector<vector<string>> allfeatures;
	int get_complete_clicked(){return complete_clicked;}
private slots:
	void slot_on_list_selections() ;
	void slot_on_add_feature() ;
private:
	QComboBox* comboboxFeature;
	int complete_clicked;
	QLabel* labelallfeatures;
	//DynamicAD_GUISelectDataForm parent_dyn_s_d;
};
#endif
