#ifndef CREATEGRAPHFROMCLUSTERSDIALOG_H
#define CREATEGRAPHFROMCLUSTERSDIALOG_H

#include <QDialog>
#include<QSpinBox>
#include<QDialogButtonBox>

//Forward Qt classes declaration
class QLabel;
class QLineEdit;
class QSpinBox;
class QVBoxLayout;
class QHBoxLayout;
class QDialogButtonBox;
class QSignalMapper;

//Forward VIS-SENSE Qt declarations
class vsColor;
class MdiChild;

class createGraphFromClustersDialog : public QDialog
{
	Q_OBJECT

public:
	createGraphFromClustersDialog(QWidget *parent = 0);

	int* getClusters(int*);

	void initialize(int);

private slots:
	void recalculateColor(int);
	void createLayout();
	void createEntry();
	void removeEntry();

private:
	QLabel **label;
	QSpinBox **lineEdit;
	int NoEntries;

	QPushButton *addButton;
	QPushButton *removeButton;

	QPushButton *setButton;
	QPushButton *closeButton;
	QDialogButtonBox *buttonBox;

	//QVBoxLayout *lefttopLayout, *righttopLayout;
	//QVBoxLayout *lefttopLayout, *righttopLayout, *mainLayout;
	//QHBoxLayout *topLayout;
	//*bottomLayout;

	int NoColors;
	vsColor** Color;
	int maxClusterNo;

	QSignalMapper *spinboxMapper;

	MdiChild *child;

};

#endif
