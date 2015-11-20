#ifndef EXECUTESFDIALOG_H
#define EXECUTESFDIALOG_H

#include <QDialog>
#include<QDoubleSpinBox>

//Forward Qt classes declaration
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QDoubleSpinBox;
class QGroupBox;
class QRadioButton;


class executeSFDialog : public QDialog
{
	Q_OBJECT

public:
	executeSFDialog(QWidget *parent = 0);

	void createFeatureWeightEntries();

	float getInflationParameter();
	float getCuttingThreshold();
	float getApproximationThreshold();
	bool getSelfLoopsParameter();
	bool getNormalizationParameter();
	bool getWeights(float*);
	bool getSpeedUp();

public slots:
	void recalculateMemoryRequirements(double v = 0);
	//void recalculateWeights();

private:
	QLabel *label1;
	//QSpinBox *lineEdit1;
	QDoubleSpinBox *lineEdit1;
	QLabel *label2;
	QDoubleSpinBox *lineEdit2;
	QLabel *label3;
	QDoubleSpinBox *lineEdit3;
	QPushButton *setButton;
	QPushButton *closeButton;
	QCheckBox *addSelfLoops;
	QCheckBox *downgrade;
	QCheckBox *speedup;
	
	QLabel **featureLabel;
	QDoubleSpinBox **weightBox;

	QGroupBox* approximationBox;
	QGroupBox* featureBox;

	int NoFeatures;
	float* Weights; 

};

#endif
