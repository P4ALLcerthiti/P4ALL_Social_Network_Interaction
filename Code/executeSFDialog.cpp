#include <QtGui>

#include "executeSFDialog.h"

#include "vsGraph.h"
#include "mdichild.h"

executeSFDialog::executeSFDialog(QWidget *parent) : QDialog(parent)
{
	this->label1 = new QLabel(tr("Inflation Parameter:"));
	//lineEdit1 = new QSpinBox;
	this->lineEdit1 = new QDoubleSpinBox;
	this->lineEdit1->setDecimals(2);
	this->lineEdit1->setValue(2);
	this->label1->setBuddy(this->lineEdit1);

	this->label2 = new QLabel(tr("Cutting &Threshold:"));
	/*lineEdit2 = new QLineEdit;
	lineEdit2->setText("0.001");*/
	this->lineEdit2 = new QDoubleSpinBox;
	this->lineEdit2->setDecimals(5);
	this->lineEdit2->setValue(0.001);
	this->label2->setBuddy(this->lineEdit2);

	QVBoxLayout *lefttopLayout = new QVBoxLayout;
	lefttopLayout->addWidget(this->label1);
	lefttopLayout->addWidget(this->label2);

	QVBoxLayout *righttopLayout = new QVBoxLayout;
	righttopLayout->addWidget(this->lineEdit1);
	righttopLayout->addWidget(this->lineEdit2);

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->addLayout(lefttopLayout);
	topLayout->addLayout(righttopLayout);
	//vbox->addStretch(1);

	QGroupBox* parametersBox = new QGroupBox(tr("Parameters"));
	parametersBox->setLayout(topLayout);


	this->label3 = new QLabel(tr("Approximation Threshold (%):"));
	//lineEdit1 = new QSpinBox;
	this->lineEdit3 = new QDoubleSpinBox;
	this->lineEdit3->setDecimals(2);
	this->lineEdit3->setValue(1);
	this->lineEdit3->setMaximum(100);
	this->label3->setBuddy(this->lineEdit3);
	QVBoxLayout *leftmiddleLayout = new QVBoxLayout;
	leftmiddleLayout->addWidget(this->label3);
	QVBoxLayout *rightmiddleLayout = new QVBoxLayout;
	rightmiddleLayout->addWidget(this->lineEdit3);
	QHBoxLayout *middleLayout = new QHBoxLayout();
	middleLayout->addLayout(leftmiddleLayout);
	middleLayout->addLayout(rightmiddleLayout);

	this->approximationBox = new QGroupBox(tr("Approximation"));
	this->approximationBox->setCheckable(true);
	this->approximationBox->setChecked(true);
	this->approximationBox->setLayout(middleLayout);


	/*QRadioButton *radio1 = new QRadioButton(tr("Do no&t downgrade hub nodes"));
	QRadioButton *radio2 = new QRadioButton(tr("&Downgrade hub nodes"));
	radio1->setChecked(true);*/
	this->addSelfLoops = new QCheckBox(tr("&Add self loops"));
	this->addSelfLoops->setChecked(true);
	this->downgrade = new QCheckBox(tr("&Downgrade hub nodes"));
	this->downgrade->setChecked(true);

	QVBoxLayout *middleLayout2 = new QVBoxLayout;
	//middleLayout->addWidget(radio1);
	//middleLayout->addWidget(radio2);
	middleLayout2->addWidget(this->addSelfLoops);
	middleLayout2->addWidget(this->downgrade);
	middleLayout2->addStretch(1);

	QGroupBox* normalizationBox = new QGroupBox(tr("Normalization"));
	normalizationBox->setLayout(middleLayout2);


	this->speedup = new QCheckBox(tr("&Use NxN Matrix"));
	this->speedup->setChecked(false);

	QVBoxLayout *middleLayout3 = new QVBoxLayout;
	middleLayout3->addWidget(this->speedup);
	middleLayout3->addStretch(1);

	QGroupBox* speedUpBox = new QGroupBox(tr("Speed Up"));
	speedUpBox->setLayout(middleLayout3);


	this->createFeatureWeightEntries();
	QVBoxLayout *featureLabelsLayout = new QVBoxLayout;
	QVBoxLayout *featureWeightsLayout = new QVBoxLayout;
	for (int i = 0; i < this->NoFeatures; i++){
		featureLabelsLayout->addWidget(this->featureLabel[i]);
		featureWeightsLayout->addWidget(this->weightBox[i]);
	}

	QHBoxLayout *featureLayout = new QHBoxLayout();
	featureLayout->addLayout(featureLabelsLayout);
	featureLayout->addLayout(featureWeightsLayout);

	this->featureBox = new QGroupBox(tr("Feature Weights"));
	this->featureBox->setCheckable(true);
	this->featureBox->setChecked(false);
	this->featureBox->setLayout(featureLayout);


	this->setButton = new QPushButton(tr("&Start"));
	this->setButton->setDefault(true);
	//setButton->setEnabled(false);
	closeButton = new QPushButton(tr("Cancel"));

	//connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString &)));
	connect(this->setButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(this->closeButton, SIGNAL(clicked()), this, SLOT(reject()));
	//connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottomLayout = new QHBoxLayout;
	bottomLayout->addWidget(setButton);
	bottomLayout->addWidget(closeButton);
	//bottomLayout->addStretch();


	QVBoxLayout *mainLayout = new QVBoxLayout;
	//mainLayout->addLayout(topLayout);
	mainLayout->addWidget(parametersBox);
	mainLayout->addWidget(normalizationBox);
	mainLayout->addWidget(speedUpBox);
	mainLayout->addWidget(approximationBox);
	if (this->NoFeatures > 1){
		mainLayout->addWidget(featureBox);
		this->featureBox->setChecked(true);
	}
	mainLayout->addLayout(bottomLayout);
	setLayout(mainLayout);

	setWindowTitle(tr("Set Stochastic Flows parametres"));
	setFixedHeight(sizeHint().height());
}

float executeSFDialog::getInflationParameter(){
	return this->lineEdit1->value();
}

float executeSFDialog::getCuttingThreshold(){
	//return (this->lineEdit2->text()).toFloat();
	return (float)this->lineEdit2->value();
}

float executeSFDialog::getApproximationThreshold(){
	if (this->approximationBox->isChecked())
		return (float)this->lineEdit3->value() / 100;
	else 
		return 0;
}

bool executeSFDialog::getSelfLoopsParameter(){
	return this->addSelfLoops->isChecked();
}

bool executeSFDialog::getNormalizationParameter(){
	return this->downgrade->isChecked();
}

bool executeSFDialog::getWeights(float* w ){
	if ( this->featureBox->isChecked() ){
		float sum = 0;
		for (int i = 0; i < this->NoFeatures; i++)
			sum += this->weightBox[i]->value();
		for (int i = 0; i < this->NoFeatures; i++)
			w[i] =  this->weightBox[i]->value() / sum;

		return true;
	}
	else return false;
}

bool executeSFDialog::getSpeedUp(){
	return this->speedup->isChecked();
}

void executeSFDialog::createFeatureWeightEntries(){

	vsGraph *g = qobject_cast<MdiChild *>(this->parent())->getGraph();

	this->NoFeatures = g->getNoFeatures() - 1;
	int N = this->NoFeatures;

	this->featureLabel = (QLabel**) malloc(N * sizeof(QLabel*));
	this->weightBox = (QDoubleSpinBox**) malloc(N * sizeof(QDoubleSpinBox*));
	this->Weights = (float*) malloc(N * sizeof(float));

	if ( !this->weightBox || !this->featureLabel || !this->Weights){
		fprintf(stderr, "Cannot allocate memory in function \"createFeatureWeights\". Exiting ...\n");fflush(stderr);
		exit(1);
	}



	for (int i = 0; i < N; i++){
		//this->featureLabel[i] = new QLabel(tr("<font color=%1  style=\"font-weight:bold;\">%2</font>") .arg("#5F9EA0") .arg(featureNames[i]));
		QString fname = QString(g->getFeatureLabel(i + 1));
		fname.replace("\\", "/");
		int x = fname.lastIndexOf(QRegExp("[\\/]"));
		fname = fname.remove(0, x + 1);
		x = fname.lastIndexOf(QRegExp("[.]"));
		fname = fname.left(x);

		//this->featureLabel[i] = new QLabel(tr("%1") .arg(g->getFeatureLabel(i + 1)));
		this->featureLabel[i] = new QLabel(tr("%1") .arg(fname));
		this->weightBox[i] = new QDoubleSpinBox;
		this->weightBox[i]->setDecimals(5);
		this->weightBox[i]->setRange(0.0, 1.0);
		this->weightBox[i]->setValue(1.0f / N);
		this->Weights[i] = this->weightBox[i]->value();
		//connect(this->weightBox[i], SIGNAL(editingFinished()), this, SLOT(recalculateMemoryRequirements()));
		connect(this->weightBox[i], SIGNAL(valueChanged(double)), this, SLOT(recalculateMemoryRequirements(double)));
	}

	this->recalculateMemoryRequirements();
}

void executeSFDialog::recalculateMemoryRequirements(double){
	
	vsGraph *g = qobject_cast<MdiChild *>(this->parent())->getGraph();
	int N = g->getNoFeatures() - 1;

	long int M = g->getNoVertices(0);
	int K = 0;
	char C = ' ';

	for (int i = 0; i < N; i++){
		if (this->weightBox[i]->value() > 0)
			M += g->getNoVertices(i + 1);
	}

	M = M * M * 4.0f;
	K = M;

	if ( K > 1024 ){
		K = M / 1024;
		C = 'K';
		if ( K > 1024 ){
			K = M / (1024 * 1024);
			C = 'M';
			if ( K > 1024 ){
				K = M / (1024 * 1024 * 1024);
				C = 'G';
			}
		}
	}

	this->speedup->setText(tr("NxN Matrix (%1 %2B)") .arg(K) .arg(C));
}

/*
void executeSFDialog::recalculateWeights(){

	float sum = 0;

	int n;

	for (int i = 0; i < NoFeature; i++){
		if (this->Weights[i] != this->weightBox[i]->value())
			n = i;
		fprintf(stderr, "%d\n", n);fflush(stderr);
	}
	
	sum += 1 - this->weightBox[n]->value();

	for (int i = 0; i < NoFeature; i++){
		if (i != n)	
			this->weightBox[i]->setValue( this->weightBox[i]->value() * sum / (1 - sum));

		this->Weights[i] = this->weightBox[i]->value();
	}

}
*/
