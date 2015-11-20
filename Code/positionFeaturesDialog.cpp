#include <QtGui>

#include "positionFeaturesDialog.h"


positionFeaturesDialog::positionFeaturesDialog(int NoFeatures, QString *featureNames, QWidget *parent)
	: QDialog(parent)
{
	QStringList features;
	features << "" ;
	for (int i = 0; i < NoFeatures; i++){
		features << featureNames[i];
	}

	this->boxLEFT = new QComboBox();
	this->boxLEFT->addItems(features);
	connect(this->boxLEFT, SIGNAL(currentIndexChanged(int)), this, SLOT(reassignItems(int)));
	this->boxTOP = new QComboBox();
	this->boxTOP->addItems(features);
	this->boxRIGHT = new QComboBox();
	this->boxRIGHT->addItems(features);
	this->boxBOTTOM = new QComboBox();
	this->boxBOTTOM->addItems(features);

	QVBoxLayout *topRightLayout = new QVBoxLayout();
	topRightLayout->addWidget(this->boxTOP);
	topRightLayout->addWidget(this->boxRIGHT);
	topRightLayout->addWidget(this->boxBOTTOM);
	topRightLayout->addWidget(this->boxLEFT);

	this->labelTOP = new QLabel(tr("Top:"));
	this->labelRIGHT = new QLabel(tr("Right:"));
	this->labelBOTTOM = new QLabel(tr("Bottom:"));
	this->labelLEFT = new QLabel(tr("Left:"));

	QVBoxLayout *topLeftLayout = new QVBoxLayout();
	topLeftLayout->addWidget(this->labelTOP);
	topLeftLayout->addWidget(this->labelRIGHT);
	topLeftLayout->addWidget(this->labelBOTTOM);
	topLeftLayout->addWidget(this->labelLEFT);

	//this->lineEdit = new QSpinBox;
	//this->lineEdit->setValue(0);
	//this->recalculateColor(0);
	//this->lineEdit->setRange(0, maxClusterNo);
	//this->label->setBuddy(this->lineEdit);

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->addLayout(topLeftLayout);
	topLayout->addLayout(topRightLayout);
	//vbox->addStretch(1);


	setButton = new QPushButton(tr("&OK"));
	setButton->setDefault(true);
	//setButton->setEnabled(false);
	closeButton = new QPushButton(tr("&Cancel"));

	connect(setButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *bottomLayout = new QHBoxLayout;
	bottomLayout->addWidget(setButton);
	bottomLayout->addWidget(closeButton);


	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(bottomLayout);
	setLayout(mainLayout);

	setWindowTitle(tr("Position Features"));
	setFixedHeight(sizeHint().height());
	setFixedWidth(sizeHint().width());
}

void positionFeaturesDialog::GetFeaturesPosition(int F[4]){

	//fprintf(stderr, "%d\n", this->boxLEFT->currentIndex());fflush(stderr);
	//fprintf(stderr, "%d\n", this->boxTOP->currentIndex());fflush(stderr);
	//fprintf(stderr, "%d\n", this->boxRIGHT->currentIndex());fflush(stderr);
	//fprintf(stderr, "%d\n", this->boxBOTTOM->currentIndex());fflush(stderr);
	F[0] = this->boxLEFT->currentIndex();
	F[1] = this->boxTOP->currentIndex();
	if ( (F[1] > 0) && (F[1] == F[0]) )
		F[1] = 0;
	F[2] = this->boxRIGHT->currentIndex();
	if ( (F[2] > 0) && ((F[2] == F[0]) || (F[2] == F[1])) )
		F[2] = 0;
	F[3] = this->boxBOTTOM->currentIndex();
	if ( (F[3] > 0) && ((F[3] == F[0]) || (F[3] == F[1]) || (F[3] == F[2])) )
		F[3] = 0;
}

void positionFeaturesDialog::reassignItems(int N){
	//return this->lineEdit->value();
}

