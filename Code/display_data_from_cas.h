#ifndef _DISPLAY_DATA_FROM_CAS_H_
#define _DISPLAY_DATA_FROM_CAS_H_

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
class display_data_from_cas : public QDialog
{
	
public:
	
	display_data_from_cas(string ticketurl,string ticket,string dataurl,string data_to_display,string session_id,QWidget *parent=0) ;
	vector<vector<string>> allfeatures;

private:
	QComboBox* comboboxFeature;
	
	QLabel* labelallfeatures;
	//DynamicAD_GUISelectDataForm parent_dyn_s_d;
};
#endif
