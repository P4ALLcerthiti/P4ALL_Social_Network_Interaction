//Qt includes
#include <QtGui>

//C++ includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//VIS-SENSE includes
#include "vsCSVFile.h"
#include "vsGraph.h"
#include "vsFeature.h"
#include "vsNode.h"
#include <exception>

//VIS-SENSE Qt includes
#include "glwidget.h"
#include <fstream>
#ifndef GL_MULITSAMPLE
#define GL_MULITSAMPLE 0x809D
#endif


GLWidget::GLWidget(QWidget *parent,  vsGraph *graph, int drawingfeature) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
//GLWidget::GLWidget(QWidget *parent, vsGraph *graph, int drawingfeature) : QWidget(parent)
{
    //setFixedSize(400, 400);
	//setMinimumSize(300, 300);
    setAutoFillBackground(false);

    
	QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    //background = QBrush(QColor(64, 32, 64));
    //this->background = QBrush(QColor(164, 132, 164));
    this->background = QBrush(QColor(255, 255, 255));
    //this->circleBrush = QBrush(gradient);
    this->circleBrush = QBrush(QColor(0,0,0));
    this->circlePen = QPen(Qt::black);
    //this->circlePen.setWidth(1);
    //this->textPen = QPen(Qt::white);
    this->textPen = QPen(Qt::black);
    this->textFont.setPixelSize(12);
    this->selectedBrush = QBrush(QColor(255,0,0));
    this->selectedPen = QPen(QColor(255,0,0));

	this->Graph = graph;
	this->DrawingFeature = drawingfeature;
	this->displayGraph = NULL;
	this->setNewGraph = graph;

	this->VertexOpacity = 180;
	this->displayFeatureValues = true;
	this->EdgeWidth = 1;
	this->EdgeOpacity = 105;
	this->abstractView = false;
	this->displayEdges = true;
	this->displayLabels = false;
	this->dVertexSize = 0;
	this->VertexSize = 10;
	this->ReferenceVertexSize = 10;
	this->minVertexSize = 2;
	this->maxVertexSize = 30;

	this->displayClusters = false;
	this->displayAnomalies = false;

	this->DrawingBox.setWidth(0);
	this->DrawingBox.setHeight(0);

    this->mousePressed = false;
    this->drawSelectionBox = false;
	this->scaleFactor = 1;
	this->drawingBorder.setX(10);
	this->drawingBorder.setY(10);
	this->moveD.setX(this->drawingBorder.x());
	this->moveD.setY(this->drawingBorder.y());

	this->setupColors();

	this->PositionsSet = false;
	this->Vertex = NULL;
	this->NoDrawingVertices = 0;
	this->NoDrawingEvents = 0;
	this->NoDrawingFeatureValues = 0;
	this->NoShownVertices = 0;

	this->Edge = NULL;
	this->NoEdges = 0;

	this->hasSelectedVertices = false;
	this->hasClusteringResult = false;
	this->hasADResult = false;

	this->writePositionsToFile = true;

	if (this->DrawingFeature == 0)
		//this->Layout = Geometric;
		this->Layout = FR;
	else
		this->Layout = FromFile;
	//this->Layout = Random;

	this->timer = new QTimer(this);
	connect(this->timer, SIGNAL(timeout()), this, SLOT(animate()));
	this->timer->setSingleShot(true);

	this->isVisible = true;

	this->CuttingThreshold = -3.0f;
	//this->EdgeMean = 0;
	//this->EdgeSD = 0;
	this->edgeStatistics = NULL;

	this->isShownAction = new QAction(tr(""), this);
	this->isShownAction->setCheckable(true);
	this->isShownAction->setChecked(true);
	//connect(this->isShownAction, SIGNAL(triggered(bool)), this, SLOT(setView(bool)));
}

void GLWidget::animate(){
    //repaint();
	update();
    //updateGL();
//fprintf(stderr, "Animated ...-\n");fflush(stderr);
}

void GLWidget::paintEvent(QPaintEvent *event){
	fprintf(stderr,"glwidget paint event\n");fflush(stderr);
	if (this->timer->isActive())
	{
		this->timer->stop();
	}

	//QPrinter printer(QPrinter::HighResolution);
    //printer.setOutputFileName("print.pdf");
	QRect alph=event->rect();
	QRegion bit=event->region();
	QPainter painter;
	try
	{
		painter.begin(this);
	}
	
	catch (...)
	{
		fprintf(stderr,"exception");fflush(stderr);
		//fprintf(stderr,e.what());fflush(stderr);
	}

	
	
	//painter.begin(&printer);
	painter.setRenderHint(QPainter::Antialiasing);
	paint(&painter, event);
	painter.end();
    
	if ( !this->PositionsSet )
		this->timer->start(10);
}

void GLWidget::paint(QPainter*, QPaintEvent*){
	
}

void GLWidget::paintVertices(QPainter *painter){
	int j, C;
	int lastColor = this->NoColors;

	for (int i = 0; i < this->NoDrawingVertices; ++i){
		if (!this->Vertex[i].Draw){
			continue;
		}

		if (!this->displayFeatureValues && i >= this->NoDrawingEvents)
			break;

/*
		if (this->displayAnomalies && this->Graph->NormalityScore[i] < 0){
			painter->setBrush(this->selectedBrush);
			painter->setPen(this->selectedPen);
		}
*/
		if(this->displayAnomalies == true)
		{
			if(*(this->Vertex[i].is_suspicious))
			{
				QColor suspiciouscolor(Qt::red);
				painter->setBrush(suspiciouscolor);
				painter->setPen(suspiciouscolor);
			}
			else
			{
				QColor unsuspiciouscolor_brush(255,255,255);
				QColor unsuspiciouscolor_pen(200,200,200);
				painter->setBrush(unsuspiciouscolor_brush);
				painter->setPen(unsuspiciouscolor_pen);
			}
		}
		else
		{
			if (*(this->Vertex[i].Selected)){
				painter->setBrush(this->selectedBrush);
				painter->setPen(this->selectedPen);
			}
			else if (this->displayClusters){
				int cl = *(this->Vertex[i].ClusterID);
				if (cl >= this->NoColors)
					C = 0;
				else
					C = (cl - 1) % (this->NoColors - 1) + 1;
				this->circleBrush.setColor(QColor(this->brushColor[C]->R, this->brushColor[C]->G, this->brushColor[C]->B, this->VertexOpacity));
				this->circlePen.setColor(QColor(this->penColor[C]->R, this->penColor[C]->G, this->penColor[C]->B, this->VertexOpacity));
				painter->setBrush(this->circleBrush);
				painter->setPen(this->circlePen);
			}
			else{
				j = this->Vertex[i].Map->Feature->getID();
				C = (j - 1) % (this->NoColors - 1) + 1;
				if (C != lastColor){
					this->circleBrush.setColor(QColor(this->brushColor[C]->R, this->brushColor[C]->G, this->brushColor[C]->B, this->VertexOpacity));
					this->circlePen.setColor(QColor(this->penColor[C]->R, this->penColor[C]->G, this->penColor[C]->B, this->VertexOpacity));
					lastColor = C;
				}
				painter->setBrush(this->circleBrush);
				painter->setPen(this->circlePen);
			}
		}

		painter->drawEllipse(QRectF(this->Vertex[i].DrawingPos.x - this->Vertex[i].Size/2, this->Vertex[i].DrawingPos.y - this->Vertex[i].Size/2, this->Vertex[i].Size, this->Vertex[i].Size));

		if ((this->displayLabels) && (i >= this->NoDrawingEvents)){
			painter->setPen(this->textPen);
			this->textFont.setPixelSize(this->Vertex[i].Size);     // kai vale afto
			painter->setFont(this->textFont); 

			painter->drawText(this->Vertex[i].DrawingPos.x - this->Vertex[i].Size/2, this->Vertex[i].DrawingPos.y - this->Vertex[i].Size/2, this->Vertex[i].Map->getLabel());
			painter->setPen(this->circlePen);
		}
	}
}

void GLWidget::paintEdges(QPainter *painter){
	float x1, x2, y1, y2;
	vsDrawingVertex *source, *target;
	int j, C;
	int lastColor = this->NoColors;

	for (int i = 0; i < this->NoEdges; i++){
		if ( !this->Edge[i].Draw )
			continue;

		//if (this->abstractView && i%2 == 0)
			//continue;

		source =  this->Edge[i].Source;
		target = this->Edge[i].Target;

		x1 = source->DrawingPos.x;
		y1 = source->DrawingPos.y;
		x2 = target->DrawingPos.x;
		y2 = target->DrawingPos.y;

		j = source->Map->Feature->getID();
		C = (j - 1) % (this->NoColors - 1) + 1;
		if (C != lastColor){
			this->circlePen.setColor(QColor(this->penColor[C]->R, this->penColor[C]->G, this->penColor[C]->B, this->EdgeOpacity));
			//painter->setPen(this->circlePen);
			lastColor = C;
		}

		//if (this->displayAnomalies && (this->Graph->NormalityScore[N] < 0 || this->Graph->NormalityScore[id] < 0)) 
			//painter->setPen(this->selectedPen);
		if (*(source->Selected) || *(target->Selected)){
			this->selectedPen.setWidth(this->Edge[i].Weight);
			painter->setPen(this->selectedPen);
		}
		else{
			this->circlePen.setWidth(this->Edge[i].Weight);
			painter->setPen(this->circlePen);
		}
	
		painter->drawLine(x1, y1, x2, y2);
	}

	this->circlePen.setWidth(1);
	this->selectedPen.setWidth(1);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	this->mousePressed = true;

	if ((event->buttons() & Qt::LeftButton) && !(QApplication::keyboardModifiers() & 0x04000000))
		emit clearSelected();

	this->lastPos = event->pos();
	this->lastClickedPos = event->pos();
	//this->lastClickedPos.setX((event->x() - this->moveD.x() - this->drawingBorder.x() * this->scaleFactor) / this->scaleFactor);
	//this->lastClickedPos.setY((event->y() - this->moveD.y() - this->drawingBorder.y() * this->scaleFactor) / this->scaleFactor);

	this->HitVertex = this->NoDrawingVertices;
	float Mx = (event->x() - this->moveD.x()) / this->scaleFactor;
	float My = (event->y() - this->moveD.y()) / this->scaleFactor;

	fprintf(stderr, "%d %d %d %d\n", lastPos.x(), lastPos.y(), event->globalX(), event->globalY());fflush(stderr);

	int N;
	if (this->displayFeatureValues)
		N = this->NoDrawingVertices;
	else
		N = this->NoDrawingEvents;
	
	float rX, rY;
	for (int i = N - 1; i >= 0; --i){
		if	 (!this->Vertex[i].Draw)
			continue;
    
		rX = this->Vertex[i].DrawingPos.x - Mx;
		rY = this->Vertex[i].DrawingPos.y - My;
    
		if (sqrt(rX * rX + rY * rY) < this->Vertex[i].Size / 2.0f){
			fprintf(stderr, "%d -> %d %d\n", i, this->Vertex[i].DrawingPos.x, this->Vertex[i].DrawingPos.y);fflush(stderr);
			this->HitVertex = i;
			break;
		}   
    
	} 
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event){
	if ( event->button() == Qt::LeftButton )
	{
		if (this->HitVertex != this->NoDrawingVertices)
		{
			emit selectedVertex(this->HitVertex);
			update();
			emitupdateApplicationMenus();
		}
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent*){
}

void GLWidget::mouseReleaseEvent(QMouseEvent*){
	this->mousePressed = false;
	if (this->drawSelectionBox){
		this->drawSelectionBox = false;
		update();
		emitupdateApplicationMenus();
	}
}

void GLWidget::wheelEvent(QWheelEvent * event){
	//return;

	float old = this->scaleFactor;
	this->scaleFactor += (event->delta() / 480.0f); //or use any other step for zooming 

	if (this->scaleFactor < 0.5)
		this->scaleFactor  = 0.5;

	int dx = event->x() - this->scaleFactor *((event->x() - this->moveD.x()) / old);
	int dy = event->y() - this->scaleFactor *((event->y() - this->moveD.y()) / old);

	if (this->scaleFactor == 1){
		dx = this->drawingBorder.x();
		dy = this->drawingBorder.y();
	}

	this->moveD.setX(dx);
	this->moveD.setY(dy);

	fprintf(stderr, "%f %d %d\n", this->scaleFactor, this->size().width(), this->size().width());fflush(stderr);
	update();
	
}

void GLWidget::setGraphDisplayOptions(bool av, float ct, bool dfv, bool de, bool dl){
	this->abstractView = av;
	if (av){
		this->setNewGraph = this->Graph->abstractGraph;
		this->Layout = FR;
	}
	this->CuttingThreshold = ct;
	this->displayFeatureValues = dfv;
	this->displayEdges = de;
	this->displayLabels = dl;
}

void GLWidget::setupColors(){
	this->NoColors = 21;

	this->brushColor = (vsColor**) malloc(this->NoColors * sizeof(vsColor*));
	this->penColor = (vsColor**) malloc(this->NoColors * sizeof(vsColor*));
	
	this->brushColor[0] = new vsColor(255, 255, 255);
	this->brushColor[1] = new vsColor(204, 204, 255);
	this->brushColor[2] = new vsColor(204, 204, 26);
	this->brushColor[3] = new vsColor(161, 13, 89);
	this->brushColor[4] = new vsColor(108, 170, 255);
	this->brushColor[5] = new vsColor(148, 255, 148);
	this->brushColor[6] = new vsColor(244, 255, 98);
	this->brushColor[7] = new vsColor(255, 98, 190);
	this->brushColor[8] = new vsColor(156, 130, 28);
	this->brushColor[9] = new vsColor(50, 205, 50);			//Lime Green
	this->brushColor[10] = new vsColor(0, 0, 255);			//Blue
	this->brushColor[11] = new vsColor(65, 105, 225);		//Royal Blue
	this->brushColor[12] = new vsColor(25, 25, 112);		//Midnight Blue
	this->brushColor[13] = new vsColor(217, 247, 255);		//Very Light Blue
	this->brushColor[14] = new vsColor(124, 252, 0);		//Lawn Green
	this->brushColor[15] = new vsColor(0, 100, 0);			//Dark Green
	this->brushColor[16] = new vsColor(205, 92, 92);		//Indian Red
	this->brushColor[17] = new vsColor(210, 105, 30);		//Chocolate
	this->brushColor[18] = new vsColor(255, 165, 0);		//Orange
	this->brushColor[19] = new vsColor(176, 48, 96);		//Maroon
	this->brushColor[20] = new vsColor(153, 50, 204);		//Dark Orchid

	//this->brushColor[2] = new vsColor(166, 223, 57);
	
	float darken = 0.85f;

	this->penColor[0] = new vsColor(180, 180, 180);
	for (int i = 1; i < this->NoColors; i++){
		this->penColor[i] = new vsColor(int(this->brushColor[i]->R * darken), int(this->brushColor[i]->G * darken), int(this->brushColor[i]->B * darken));
	}
}

void GLWidget::setCuttingThreshold(float v){
	this->CuttingThreshold += v;
	//this->setAbstractDrawingVertices();
	this->setDrawnVertices();
	update();
}

void GLWidget::setVertexTransparency(int P){
	this->VertexOpacity = P;
	update();
}

void GLWidget::setFeatureValuesView(bool enabled){
	this->displayFeatureValues = enabled;
	update();
}

void GLWidget::setEdgesView(bool enabled){
	this->displayEdges = enabled;
	update();
}

void GLWidget::setEdgeTransparency(int P){
	this->EdgeOpacity = P;
	update();
}

void GLWidget::setLabelsView(bool enabled){
	this->displayLabels = enabled;
	update();
}

void GLWidget::setClustersView(bool enabled){
	this->displayClusters = enabled;
	update();
}

void GLWidget::setAnomaliesView(bool enabled){
	this->displayAnomalies = enabled;
	update();
}

void GLWidget::setLayoutView(bool enabled){
	if (enabled)
		this->setNewGraph = this->Graph->abstractGraph;
	else
		this->setNewGraph = this->Graph;

	this->abstractView = enabled;
	update();
}

void GLWidget::setVertexSize(int click){
	this->dVertexSize += click;

	//float size = sqrt(this->DrawingBox.width() * this->DrawingBox.height() / (8 * (float)this->NoDrawingVertices));
	float size = sqrt(this->DrawingBox.width() * this->DrawingBox.height() / (12 * (float)this->NoShownVertices));

	int minSize = this->minVertexSize;
	int maxSize = this->maxVertexSize;

	size = (size < maxSize) ? size : maxSize;
	size = (size > minSize) ? size : minSize;

	float old_size = size;
	size += this->dVertexSize;

	size = (size < maxSize) ? size : maxSize;
	size = (size > minSize) ? size : minSize;

	this->dVertexSize = size - old_size;

	this->calculateVertexSize();
	update();
	fprintf(stderr, "Vertex Size Set!!! New Size: %f, Old Size: %f, D:%d\n", old_size, size, this->dVertexSize);fflush(stderr);
}

void GLWidget::calculateVertexSize(){
	 
	//float size = sqrt(this->DrawingBox.width() * this->DrawingBox.height() / (7 * (float)this->NoDrawingVertices));
	float size = sqrt(this->DrawingBox.width() * this->DrawingBox.height() / (12 * (float)this->NoShownVertices));
	fprintf(stderr, "NoShownVertices:%d NoVertices:%d\n", this->NoShownVertices, this->NoDrawingVertices);fflush(stderr);
	fprintf(stderr, "D:%d VertexSize:%d CalculatedSize:%f\n", this->dVertexSize, this->VertexSize, size);fflush(stderr);
	fprintf(stderr, "Width:%d - Height: %d\n", this->DrawingBox.width(), this->DrawingBox.height());fflush(stderr);

	int minSize = this->minVertexSize;
	int maxSize = this->maxVertexSize;
 
	size = (size < maxSize) ? size : maxSize;
	size = (size > minSize) ? size : minSize;

	size += this->dVertexSize;

	size = (size < maxSize) ? size : maxSize;
	size = (size > minSize) ? size : minSize;
	
	this->VertexSize = size;

	float ratio = (float)this->VertexSize / this->ReferenceVertexSize;

	for (int i = 0; i < this->NoDrawingVertices; ++i){
		if ( !this->Vertex[i].Draw )
			continue;
		
		this->Vertex[i].Size = this->Vertex[i].NormalSize * ratio;
	}

	for (int i = 0; i < this->NoEdges; ++i){
		this->Edge[i].Weight = this->Edge[i].NormalWeight * ratio;
	}

	//qDebug() << this->windowTitle();
	//fprintf(stderr, "New: %d %d\n", this->dVertexSize, this->VertexSize);fflush(stderr);
}

void GLWidget::unsetSelected(){

	for (int i = 0; i < this->NoDrawingVertices; ++i){
		this->Vertex[i].SecondSelection = false;
		*(this->Vertex[i].Selected) = false;
	}

	this->hasSelectedVertices = false;

	update();
}

void GLWidget::copySelected(){
	for (int i = 0; i < this->NoDrawingVertices; ++i){
		this->Vertex[i].SecondSelection = *(this->Vertex[i].Selected);
	}
}

void GLWidget::setView(bool shown){
	emit this->changedDrawnVertices(shown, this->DrawingFeature);
}

void GLWidget::setDrawnVertices(){
}

void GLWidget::calculateEdgeMeanSD(bool onlyDrawn){
	float f;

	int E = 0;

	if (this->DrawingFeature == 0)
		E = this->getNoFeatures() - 1; 
	else
		E = 1;

	free(this->edgeStatistics);
	this->edgeStatistics = (vsSD*) malloc(E * E * sizeof(vsSD));

	for	(int i = 0; i < E * E; ++i){
		this->edgeStatistics[i].N = 0;
		this->edgeStatistics[i].mean = 0;
		this->edgeStatistics[i].SD = 0;
	}

	for (int i = 0; i < this->NoEdges; ++i){
		if (onlyDrawn && !this->Edge[i].Draw)
			continue;

		int f1, f2;
		if (this->DrawingFeature == 0){
			f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
			f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
		}
		else{
			f1 = 0;
			f2 = 0;
		}

		this->edgeStatistics[f1 * E + f2].mean += this->Edge[i].ActualWeight;
		this->edgeStatistics[f1 * E + f2].N++;
	}
	
	for	(int i = 0; i < E; ++i){
		for	(int j = i + 1; j < E; ++j){
			this->edgeStatistics[i * E + j].mean += this->edgeStatistics[j * E + i].mean;
			this->edgeStatistics[j * E + i].mean = this->edgeStatistics[i * E + j].mean;
			this->edgeStatistics[i * E + j].N += this->edgeStatistics[j * E + i].N;
			this->edgeStatistics[j * E + i].N = this->edgeStatistics[i * E + j].N;
		}
	}
	for	(int i = 0; i < E * E; ++i){
		this->edgeStatistics[i].mean /= this->edgeStatistics[i].N;
	}

	for (int i = 0; i < this->NoEdges; ++i){
		if (onlyDrawn && !this->Edge[i].Draw)
			continue;

		int f1, f2;
		if (this->DrawingFeature == 0){
			f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
			f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
		}
		else{
			f1 = 0;
			f2 = 0;
		}

		f = (this->Edge[i].ActualWeight - this->edgeStatistics[f1 * E + f2].mean);

		this->edgeStatistics[f1 * E + f2].SD += f * f;
	}

	for	(int i = 0; i < E; ++i){
		for	(int j = i + 1; j < E; ++j){
			this->edgeStatistics[i * E + j].SD += this->edgeStatistics[j * E + i].SD;
			this->edgeStatistics[j * E + i].SD = this->edgeStatistics[i * E + j].SD;
		}
	}
	for	(int i = 0; i < E * E; ++i){
		this->edgeStatistics[i].SD /= this->edgeStatistics[i].N;
		this->edgeStatistics[i].SD /= sqrt(this->edgeStatistics[i].SD);
	}

	//this->EdgeMean = mean;
	//this->EdgeSD = var;
}

void GLWidget::setAbstractDrawingVertices(bool onlyDrawn){
	this->NoShownVertices = 0;

	//if (this->EdgeMean == 0 && this->EdgeSD == 0){
	//if (this->edgeStatistics == NULL)
		this->calculateEdgeMeanSD(onlyDrawn);

	//double mean = this->EdgeMean;
	//double var = this->EdgeSD;

	float z = this->CuttingThreshold;

	for (int i = 0; i < this->NoDrawingVertices; i++){
		this->Vertex[i].Draw = false;
	}

	for (int i = 0; i < this->NoEdges; ++i){
		if (onlyDrawn && !this->Edge[i].Draw)
			continue;

		int f1, f2, E;
		if (this->DrawingFeature == 0){
			f1 = this->Edge[i].Source->Map->Feature->getID() - 1;
			f2 = this->Edge[i].Target->Map->Feature->getID() - 1;
			E = this->getNoFeatures() - 1;
		}
		else{
			f1 = 0;
			f2 = 0;
			E = 0;
		}

		if ( (this->Edge[i].ActualWeight - this->edgeStatistics[f1 * E + f2].mean) / this->edgeStatistics[f1 * E + f2].SD < z ){
			this->Edge[i].Draw = false;
		}
		else{
			this->Edge[i].Draw = true;

			this->Edge[i].Source->Draw = true;
			this->Edge[i].Target->Draw = true;
		}
	}

	for (int i = 0; i < this->NoDrawingVertices; i++)
		if (this->Vertex[i].Draw)
			this->NoShownVertices++;
	fprintf(stderr, "NoShownVertices:%d NoVertices:%d, CutThres: %f\n", this->NoShownVertices, this->NoDrawingVertices, this->CuttingThreshold);fflush(stderr);
	//fprintf(stderr, "CutThres: %f, Mean: %f, SD: %f\n", this->CuttingThreshold, this->EdgeMean, this->EdgeSD);fflush(stderr);
}


vsGraph* GLWidget::getGraph(){
	return this->Graph;
}

int GLWidget::getNoFeatures(){
	if (this->Graph)
		return this->Graph->getNoFeatures();

	return 0;
}


GLWidget::~GLWidget()
{

}
