#include <QtGui>

#include <iostream>
#include <sstream>

#include "createGraphFromClustersDialog.h"
#include "mdichild.h"
#include "vsGraph.h"

using namespace std;


//printClustersDialog::printClustersDialog(int maxClusterNo, double* Colours, int NoColours, QWidget *parent)
createGraphFromClustersDialog::createGraphFromClustersDialog(QWidget *parent) : QDialog(parent)
{
	fprintf(stderr, "So far so good1\n");fflush(stderr);

	this->label = NULL;
	this->lineEdit = NULL;
	this->NoEntries = 0;
	this->maxClusterNo = 0;

	this->child = qobject_cast<MdiChild *>(this->parent());
	//vsGraph *g = this->child->getGraph();
	//this->maxClusterNo = g->NoClusters - 1;
	this->NoColors = this->child->NoColors;
	this->Color = this->child->brushColor;

	//this->calculateNoClusters();

	this->spinboxMapper = new QSignalMapper(this);
	connect(this->spinboxMapper, SIGNAL(mapped(int)), this, SLOT(recalculateColor(int)));

	this->createLayout();
	this->createEntry();

	setWindowTitle(tr("Create Graph from Cluster(s)"));
	setFixedHeight(sizeHint().height());
	setFixedWidth(sizeHint().width());
}

void createGraphFromClustersDialog::initialize(int N){
	this->maxClusterNo = N;
	
	for (int i = 0; i < NoEntries; ++i){
		this->lineEdit[i]->setRange(0, this->maxClusterNo);
		
	}
}

int* createGraphFromClustersDialog::getClusters(int *N){
	
	int *v = (int*) malloc(this->NoEntries * sizeof(int));

	for (int i = 0; i < this->NoEntries; i++){
		v[i] = this->lineEdit[i]->value();
		fprintf(stderr, "%d %d\n", v[i], this->NoEntries);fflush(stderr);
	}
	N[0] = this->NoEntries;

	return v;
}

void createGraphFromClustersDialog::recalculateColor(int i){
	int No = this->lineEdit[i]->value();

	stringstream ss;

	if (No + 1 < this->NoColors){
		ss << "#";
		if ( this->Color[No + 1]->R < 16 )
			ss << hex << 0;
		ss << hex << this->Color[No + 1]->R;
		if ( this->Color[No + 1]->G < 16 )
			ss << hex << 0;
		ss << hex << this->Color[No + 1]->G;
		if ( this->Color[No + 1]->B < 16 )
			ss << hex << 0;
		ss << hex << this->Color[No + 1]->B;
	}
	else
		ss << "#" << hex << (int)(0) << (int)(0) << (int)(0);

	string result = ss.str();

	char c[8];
	strcpy(c, result.c_str());
	//fprintf(stderr, "%s\n", c);fflush(stderr);

	this->label[i]->setText(tr("<font color=%1  style=\"font-weight:bold;\">Cluster No:</font>") .arg(c));

}

void createGraphFromClustersDialog::removeEntry(){
	QLayoutItem *LI = qobject_cast<QGridLayout *>(this->layout())->itemAtPosition(this->NoEntries -1, 0);
	//qobject_cast<QGridLayout *>(this->layout())->removeItem(LI);

	this->NoEntries--;
	int N = this->NoEntries;

	delete this->label[N];
	delete this->lineEdit[N];

	this->label = (QLabel**) realloc(this->label, N * sizeof(QLabel*));
	this->lineEdit = (QSpinBox**) realloc(this->lineEdit, N * sizeof(QSpinBox*));

	delete LI;

	if (this->NoEntries == 1)
		this->removeButton->setEnabled(false);
		//this->removeButton->setVisible(false);
	//this->update();
}

void createGraphFromClustersDialog::createEntry(){
	this->NoEntries++;
	int N = this->NoEntries;

	this->label = (QLabel**) realloc(this->label, N * sizeof(QLabel*));
	this->lineEdit = (QSpinBox**) realloc(this->lineEdit, N * sizeof(QSpinBox*));

	this->label[N - 1] = new QLabel(tr("&Cluster No:"));
	this->lineEdit[N - 1] = new QSpinBox;
	this->lineEdit[N - 1]->setValue(0);
	this->lineEdit[N - 1]->setRange(0, this->maxClusterNo);
	this->label[N - 1]->setBuddy(this->lineEdit[N - 1]);
	connect(this->lineEdit[N - 1], SIGNAL(valueChanged(int)), this->spinboxMapper, SLOT(map()));
	this->spinboxMapper->setMapping(this->lineEdit[N - 1], N - 1);

	this->recalculateColor(N - 1);

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->addWidget(this->label[N - 1]);
	topLayout->addWidget(this->lineEdit[N - 1]);
	
	qobject_cast<QGridLayout *>(this->layout())->addLayout(topLayout, N - 1, 0);

	if (this->NoEntries > 1)
		this->removeButton->setEnabled(true);
	//this->update();
}


void createGraphFromClustersDialog::createLayout(){
	this->addButton = new QPushButton(tr("&Add"));
	this->addButton->setDefault(true);
	//setButton->setEnabled(false);
	connect(this->addButton, SIGNAL(clicked()), this, SLOT(createEntry()));

	this->removeButton = new QPushButton(tr("&Remove"));
	this->removeButton->setDefault(true);
	this->removeButton->setEnabled(false);
	connect(this->removeButton, SIGNAL(clicked()), this, SLOT(removeEntry()));

	buttonBox = new QDialogButtonBox(Qt::Vertical);
	buttonBox->addButton(this->addButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(this->removeButton, QDialogButtonBox::ActionRole);


	QDialogButtonBox *bottombuttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical);
	//bottombuttonBox->insertSpacing(0, 10);
    connect(bottombuttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bottombuttonBox, SIGNAL(rejected()), this, SLOT(reject()));


	QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	int N = 25;
	mainLayout->setRowMinimumHeight(0, N);
	mainLayout->setRowMinimumHeight(1, N);
	mainLayout->setRowMinimumHeight(2, N);
	mainLayout->setRowMinimumHeight(3, N);
	mainLayout->setRowMinimumHeight(4, N);
    mainLayout->addWidget(bottombuttonBox, 0, 1, 3, 1);
	mainLayout->addWidget(buttonBox, 3, 1, 2, 1);
    //mainLayout->addWidget(bottombuttonBox, 1, 0, 1, 2, Qt::AlignJustify);
    setLayout(mainLayout);
}
