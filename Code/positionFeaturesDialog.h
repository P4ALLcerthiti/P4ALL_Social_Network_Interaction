#ifndef POSITIONFEATURESDIALOG_H
#define POSITIONFEATURESDIALOG_H

#include <QDialog>
#include<QComboBox>
#include<QVBoxLayout>
#include<QLabel>
#include<QPushButton>

//Forward Qt classes declaration
class QLabel;
class QLineEdit;
class QComboBox;


class positionFeaturesDialog : public QDialog
{
	Q_OBJECT

public:
	positionFeaturesDialog(int NoFeatures, QString *featureNames, QWidget *parent = 0);

	void GetFeaturesPosition(int F[4]);

private slots:
	void reassignItems(int N);

private:
	QLabel *labelLEFT;
	QLabel *labelTOP;
	QLabel *labelRIGHT;
	QLabel *labelBOTTOM;
	QComboBox *boxLEFT;
	QComboBox *boxTOP;
	QComboBox *boxRIGHT;
	QComboBox *boxBOTTOM;
	QPushButton *setButton;
	QPushButton *closeButton;
	//int NoColors;
	//double* Colors;
};

#endif
