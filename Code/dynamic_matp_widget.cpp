#include "dynamic_matp_widget.h"
#include "mdichild.h"
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
#include <fstream>
//VIS-SENSE Qt includes

#ifndef GL_MULITSAMPLE
#define GL_MULITSAMPLE 0x809D
#endif

dynamic_matp_widget::dynamic_matp_widget(QWidget *parent,MdiChild* parentmdich, std::vector<Eigen::MatrixXd> MatP,int dynamic_numberoffeatures,std::vector<std::string> names_of_files) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
//dynamic_matp_widget::dynamic_matp_widget(QWidget *parent, vsGraph *graph, int drawingfeature) : QWidget(parent)
{
    //setFixedSize(400, 400);
	//setMinimumSize(300, 300);
    this->dynamic_numberoffeatures=dynamic_numberoffeatures;
	this->names_of_files=names_of_files;
	this->parent_dmatp=parentmdich;
	connect(this, SIGNAL(clearSelected_dmatp()), this, SLOT(unsetSelected_dmatp()));
	this->setupColors_dmatp();
	setAutoFillBackground(false);

    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    //background = QBrush(QColor(64, 32, 64));
    //this->background = QBrush(QColor(164, 132, 164));
    this->background_dmatp = QBrush(QColor(255, 255, 255));
    //this->circleBrush = QBrush(gradient);
    this->circleBrush_dmatp = QBrush(QColor(0,0,0));
    this->circlePen_dmatp = QPen(Qt::black);
    //this->circlePen.setWidth(1);
    //this->textPen = QPen(Qt::white);
    this->textPen_dmatp = QPen(Qt::black);
    this->textFont_dmatp.setPixelSize(12);
    this->selectedBrush_dmatp = QBrush(QColor(255,0,0));
    this->selectedPen_dmatp = QPen(QColor(255,0,0));

	

	

	this->DrawingBox_dmatp.setWidth(0);
	this->DrawingBox_dmatp.setHeight(0);

    this->mousePressed_dmatp = false;
    this->drawSelectionBox_dmatp = false;
	this->scaleFactor_dmatp = 0.9;
	this->drawingBorder_dmatp.setX(0);
	this->drawingBorder_dmatp.setY(0);
	this->moveD_dmatp.setX(this->drawingBorder_dmatp.x());
	this->moveD_dmatp.setY(this->drawingBorder_dmatp.y());

	//this->setupColors();

	this->PositionsSet_dmatp = false;
	

	

	
	//this->Layout = Random;

	/*this->timer_dmatp = new QTimer(this);
	connect(this->timer_dmatp, SIGNAL(timeout_dmatp()), this, SLOT(animate_dmatp()));
	this->timer_dmatp->setSingleShot(true);
*/

	this->isVisible_dmatp = true;

	this->CuttingThreshold_dmatp = -3.0f;
	//this->EdgeMean = 0;
	//this->EdgeSD = 0;
	

	this->isShownAction_dmatp = new QAction(tr(""), this);
	this->isShownAction_dmatp->setCheckable(true);
	this->isShownAction_dmatp->setChecked(true);
	this->MatP=MatP;
	this->NoDrawingVertices_dmatp=MatP[0].rows();
	this->HitVertex_dmatp=NoDrawingVertices_dmatp;
	for(int i=0;i<MatP.size();i++)
	{
		matp_points.push_back(vector<matp_point* >());
		for(int j=0;j<this->NoDrawingVertices_dmatp;j++)
		{
			this->matp_points[i].push_back(new matp_point(MatP[i].coeff(j,0),MatP[i].coeff(j,1),6.0,false));
			this->matp_drawing_positions.push_back(new matp_point(MatP[i].coeff(j,0),MatP[i].coeff(j,1),6.0,false));
			
		}
	}
	this->hasSelectedVertices_dmatp = false;
	this->type_of_labels_displayed=0;
	this->metric_selected=0;
	this->ctrl_clicked=false;
	this->show_selected=false;
	this->is_vector_i_selected_global=vector<bool>();
	for(int i=0;i<this->NoDrawingVertices_dmatp;i++)
	{
		this->is_vector_i_selected_global.push_back(false);
	}
	this->is_vector_i_selected_global[0]=true;
	colorInit_vertice=Qt::yellow;
	colorTarget_vertice=QColor(255, 127,0);
	//this->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowMatpChoices(const QPoint&)));
	//connect(this->isShownAction, SIGNAL(triggered(bool)), this, SLOT(setView(bool)));
}
void dynamic_matp_widget::unsetSelected_dmatp(){

	for (int i = 0; i < this->NoDrawingVertices_dmatp; ++i){
		this->matp_drawing_positions[i]->selected= false;
	}

	this->hasSelectedVertices_dmatp = false;

	update();
}
void dynamic_matp_widget::animate(){
    //repaint();
    update();
    //updateGL();
//fprintf(stderr, "Animated ...-\n");fflush(stderr);
}

void dynamic_matp_widget::paintEvent(QPaintEvent *event){
	/*if (this->timer_dmatp->isActive())
		this->timer_dmatp->stop();
*/

	//QPrinter printer(QPrinter::HighResolution);
    //printer.setOutputFileName("print.pdf");
	QRect alph=event->rect();
	QRegion bit=event->region();
	QPainter painter;
	painter.begin(this);
	//painter.begin(&printer);
	painter.setRenderHint(QPainter::Antialiasing);
	paint(&painter, event);
	painter.end();
    
	/*if ( !this->PositionsSet_dmatp )
		this->timer_dmatp->start(10);*/
}

void dynamic_matp_widget::paint(QPainter* painter, QPaintEvent* event)
{
	
	
	//if(this->width_of_last_matp!=this->size().width()||this->height_of_last_matp!=this->size().height())
	//{
		/*if (this->HitVertex!=NoDrawingVertices)
		{
			for(int i=0;i<MatP[0].rows();i++)
			{
				double drawpointx=matp_drawing_positions[i]->X - matp_drawing_positions[i]->size_of_point/2;
				double drawpointy=matp_drawing_positions[i]->Y - matp_drawing_positions[i]->size_of_point/2;
				painter->drawEllipse(QRectF(drawpointx,drawpointy , matp_points[i]->size_of_point, matp_points[i]->size_of_point));
			}
			return;
		}*/
		//this->selectedBrush_dmatp.setColor(QColor(this->brushColor_dmatp[12]->R, this->brushColor_dmatp[12]->G, this->brushColor_dmatp[12]->B, 30));
		this->selectedBrush_dmatp.setColor(QColor(this->penColor_dmatp[12]->R, this->penColor_dmatp[12]->G, this->penColor_dmatp[12]->B, 210));
		this->selectedPen_dmatp.setColor(QColor(this->penColor_dmatp[12]->R, this->penColor_dmatp[12]->G, this->penColor_dmatp[12]->B, 210));
		painter->fillRect(event->rect(), background_dmatp);
		painter->setBrush(this->selectedBrush_dmatp);
		painter->setPen(this->selectedPen_dmatp);
		//paintVertices_dmatp(painter);
		paintedges_dmatp(painter);
		paintVertices_dmatp(painter);
	//}
	if (this->drawSelectionBox_dmatp){
		this->circleBrush_dmatp.setColor(QColor(this->brushColor_dmatp[12]->R, this->brushColor_dmatp[12]->G, this->brushColor_dmatp[12]->B, 30));
		this->circlePen_dmatp.setColor(QColor(this->penColor_dmatp[12]->R, this->penColor_dmatp[12]->G, this->penColor_dmatp[12]->B, 210));
		painter->setBrush(this->circleBrush_dmatp);
		painter->setPen(this->circlePen_dmatp);

		painter->drawRect(this->selectionBox_dmatp);
		//painter->drawPolygon(this->selectionBox, Qt::OddEvenFill);
		//painter->drawPolyline(this->selectionBox);
	}
}
void dynamic_matp_widget::paintVertices_dmatp(QPainter *painter)
{
	double min_x_matp=0;
	double max_x_matp=0;
	double min_y_matp=0;
	double max_y_matp=0;
	this->width_of_last_matp=this->size().width();
	this->height_of_last_matp=this->size().height();

	for(int i=0;i<MatP[0].rows();i++)
	{
		if(MatP[metric_selected].coeff(i,0)<min_x_matp) min_x_matp=MatP[metric_selected].coeff(i,0);
		if(MatP[metric_selected].coeff(i,0)>max_x_matp) max_x_matp=MatP[metric_selected].coeff(i,0);
		if(MatP[metric_selected].coeff(i,1)<min_y_matp) min_y_matp=MatP[metric_selected].coeff(i,1);
		if(MatP[metric_selected].coeff(i,1)>max_y_matp) max_y_matp=MatP[metric_selected].coeff(i,1);
	}

	double matp_width=max_x_matp-min_x_matp;
	double matp_height=max_y_matp-min_y_matp;
	ratio_width_dmatp=this->size().width()*0.8/matp_width;
	ratio_height_dmatp=this->size().height()*0.8/matp_width;

	double ratio=ratio_width_dmatp<ratio_height_dmatp?ratio_width_dmatp:ratio_height_dmatp;
	double sizes=this->size().width();
	
	//std::ofstream ofs("mATPPONTS.txt");
	for(int i=0;i<MatP[0].rows();i++)
	{
		if (matp_drawing_positions[i]->selected){
			painter->setBrush(this->selectedBrush_dmatp);
			painter->setPen(this->selectedPen_dmatp);
		}
		else
		{
			QColor color;
			double alpha = ((double)i+1)/((double)MatP[0].rows());
			color.setRedF((1.0-alpha) * this->colorInit_vertice.redF() + alpha * this->colorTarget_vertice.redF());
			color.setGreenF((1.0-alpha) * this->colorInit_vertice.greenF() + alpha * this->colorTarget_vertice.greenF());
			color.setBlueF((1.0-alpha) * this->colorInit_vertice.blueF() + alpha * this->colorTarget_vertice.blueF());
			this->circleBrush_dmatp.setColor(color);
			this->circlePen_dmatp.setColor(color);
			/*double color_steps=250/MatP[0].rows();
			this->circleBrush_dmatp.setColor(QColor(i*color_steps, 0, 0));
			this->circlePen_dmatp.setColor(QColor(i*color_steps, 0, 0));*/
			painter->setBrush(this->circleBrush_dmatp);
			painter->setPen(this->circlePen_dmatp);
		}
		if(this->show_selected==true&&is_vector_i_selected_global.size()>0)
		{
			if(is_vector_i_selected_global[i]==true)
			{
				painter->setBrush(this->selectedBrush_dmatp);
				painter->setPen(this->selectedPen_dmatp);
			}
		}
		double testx=(matp_points[metric_selected][i]->X - min_x_matp)*ratio_width_dmatp+this->size().width()*0.1;
		matp_drawing_positions[i]->X=(testx*this->scaleFactor_dmatp)+ this->moveD_dmatp.x() ;
		double testy=(matp_points[metric_selected][i]->Y - min_y_matp)*ratio_height_dmatp+this->size().height()*0.1;
		matp_drawing_positions[i]->Y=this->height()-(testy*this->scaleFactor_dmatp)- this->moveD_dmatp.y();
		
		matp_drawing_positions[i]->size_of_point=matp_points[metric_selected][i]->size_of_point*this->scaleFactor_dmatp;
		//matp_points[i]->X=matp_points[i]->X/ratio;
		//matp_points[i]->Y=matp_points[i]->Y/ratio;
		double drawpointx=matp_drawing_positions[i]->X - matp_drawing_positions[i]->size_of_point/2;
		double drawpointy=matp_drawing_positions[i]->Y - matp_drawing_positions[i]->size_of_point/2;
		painter->drawEllipse(QRectF(drawpointx,drawpointy , matp_drawing_positions[i]->size_of_point, matp_drawing_positions[i]->size_of_point));
		//ofs<<matp_points[metric_selected][i]->X<<" "<<matp_points[metric_selected][i]->Y<<"\n";

		//painter->setPen(this->textPen_dmatp);
		this->textFont_dmatp.setPixelSize(matp_drawing_positions[i]->size_of_point*2);     // kai vale afto
		painter->setFont(this->textFont_dmatp); 
		if(type_of_labels_displayed==0)
			
			painter->drawText(matp_drawing_positions[i]->X - matp_drawing_positions[i]->size_of_point/2, matp_drawing_positions[i]->Y - matp_drawing_positions[i]->size_of_point/2, QString::number(i+1));
		else
		{
			std::string name_of_file=this->names_of_files[i*dynamic_numberoffeatures];
			int first_pos=name_of_file.find_last_of( "(" )+1;
			int last_pos=name_of_file.find_last_of( ")" );
			int len=last_pos-first_pos;
			painter->drawText(matp_drawing_positions[i]->X - len*matp_drawing_positions[i]->size_of_point/2, matp_drawing_positions[i]->Y - matp_drawing_positions[i]->size_of_point/2, QString::fromStdString(name_of_file.substr(first_pos,len)));
		}
		painter->setPen(this->circlePen_dmatp);

	}
	//ofs.close();
}
void dynamic_matp_widget::paintedges_dmatp(QPainter *painter)
{
	
	double min_x_matp=0;
	double max_x_matp=0;
	double min_y_matp=0;
	double max_y_matp=0;
	this->width_of_last_matp=this->size().width();
	this->height_of_last_matp=this->size().height();

	for(int i=0;i<MatP[0].rows();i++)
	{
		if(MatP[metric_selected].coeff(i,0)<min_x_matp) min_x_matp=MatP[metric_selected].coeff(i,0);
		if(MatP[metric_selected].coeff(i,0)>max_x_matp) max_x_matp=MatP[metric_selected].coeff(i,0);
		if(MatP[metric_selected].coeff(i,1)<min_y_matp) min_y_matp=MatP[metric_selected].coeff(i,1);
		if(MatP[metric_selected].coeff(i,1)>max_y_matp) max_y_matp=MatP[metric_selected].coeff(i,1);
	}

	double matp_width=max_x_matp-min_x_matp;
	double matp_height=max_y_matp-min_y_matp;
	ratio_width_dmatp=this->size().width()*0.8/matp_width;
	ratio_height_dmatp=this->size().height()*0.8/matp_width;

	double ratio=ratio_width_dmatp<ratio_height_dmatp?ratio_width_dmatp:ratio_height_dmatp;
	double sizes=this->size().width();
	
	for(int i=0;i<MatP[0].rows();i++)
	{
		double testx=(matp_points[metric_selected][i]->X - min_x_matp)*ratio_width_dmatp+this->size().width()*0.1;
		matp_drawing_positions[i]->X=(testx*this->scaleFactor_dmatp)+ this->moveD_dmatp.x() ;
		double testy=(matp_points[metric_selected][i]->Y - min_y_matp)*ratio_height_dmatp+this->size().height()*0.1;
		matp_drawing_positions[i]->Y=this->height()-(testy*this->scaleFactor_dmatp)- this->moveD_dmatp.y();
		
		matp_drawing_positions[i]->size_of_point=matp_points[metric_selected][i]->size_of_point*this->scaleFactor_dmatp;
		
	}
	



	
	
	
	
	
	
	float x1, x2, y1, y2;
	int j, C;
	int lastColor = this->NoColors_dmatp;
	QPen edgepen;
	QBrush edgebrush;
	edgepen.setColor(QColor(0,0,0));
	edgebrush.setColor(QColor(0,0,0));
	for(int i=0;i<MatP[0].rows()-1;i++)
	{
		x1 = matp_drawing_positions[i]->X;
		y1 = matp_drawing_positions[i]->Y;
		x2 = matp_drawing_positions[i+1]->X;
		y2 = matp_drawing_positions[i+1]->Y;
		if (matp_drawing_positions[i]->selected||matp_drawing_positions[i+1]->selected){
			this->selectedPen_dmatp.setWidth(2);
			painter->setBrush(this->selectedBrush_dmatp);
			painter->setPen(this->selectedPen_dmatp);
			
		}
		else
		{
			this->circlePen_dmatp.setWidth(1);
			
			painter->setBrush(edgebrush);
			painter->setPen(edgepen);
		}
		
		painter->drawLine(x1, y1, x2, y2);
	}

	this->circlePen_dmatp.setWidth(1);
	this->selectedPen_dmatp.setWidth(1);
}

void dynamic_matp_widget::mousePressEvent(QMouseEvent *event)
{
	double eventx=event->x();
	double eventy=event->y();
	this->mousePressed_dmatp = true;

	if ((event->buttons() & Qt::LeftButton) )
		emit clearSelected_dmatp();
	if(QApplication::keyboardModifiers() & 0x04000000)
		this->ctrl_clicked=true;
	this->lastPos_dmatp = event->pos();
	this->lastClickedPos_dmatp = event->pos();

	//this->lastClickedPos.setX((event->x() - this->moveD.x() - this->drawingBorder.x() * this->scaleFactor) / this->scaleFactor);
	//this->lastClickedPos.setY((event->y() - this->moveD.y() - this->drawingBorder.y() * this->scaleFactor) / this->scaleFactor);

	this->HitVertex_dmatp = MatP[0].rows();
	//float Mx = (event->x() - this->moveD.x()) / this->scaleFactor;
	//float My = (event->y() - this->moveD.y()) / this->scaleFactor;
	float Mx = event->x();
	float My = event->y(); 
	/*float Mx = (event->x() - this->moveD_dmatp.x()) / this->scaleFactor_dmatp;
	float My = (event->y() - this->moveD_dmatp.y()) / this->scaleFactor_dmatp;*/
	fprintf(stderr, "%d %d %d %d\n", lastPos_dmatp.x(), lastPos_dmatp.y(), event->globalX(), event->globalY());fflush(stderr);

	int N = MatP[0].rows();
	
	float rX, rY;
	for (int i = N - 1; i >= 0; --i){
		
		rX = matp_drawing_positions[i]->X - Mx;
		rY = matp_drawing_positions[i]->Y - My;
    
		if (sqrt(rX * rX + rY * rY) < this->matp_drawing_positions[i]->size_of_point / 2.0f){
			fprintf(stderr, "%d -> %d %d\n", i, matp_drawing_positions[i]->X, this->matp_drawing_positions[i]->Y);fflush(stderr);
			this->HitVertex_dmatp = i;
			break;
		}   
    
	} 
}
void dynamic_matp_widget::mouseMoveEvent(QMouseEvent *event){
	int dx = event->x() - lastPos_dmatp.x();
	int dy = event->y() - lastPos_dmatp.y();

	if (event->buttons() & Qt::LeftButton) {

		if (this->HitVertex_dmatp != this->NoDrawingVertices_dmatp){
			if (this->scaleFactor_dmatp == 1){
				matp_drawing_positions[HitVertex_dmatp]->X  += dx;
				matp_drawing_positions[HitVertex_dmatp]->Y += dy;
				matp_points[metric_selected][HitVertex_dmatp]->X+=dx/ratio_width_dmatp;
				matp_points[metric_selected][HitVertex_dmatp]->Y-=dy/ratio_height_dmatp;
			}
			else{
				matp_drawing_positions[HitVertex_dmatp]->X += dx / this->scaleFactor_dmatp ; //Add 0.5 so that the pointer doesn't lose focus due to multiplication effect
				matp_drawing_positions[HitVertex_dmatp]->Y += dy / this->scaleFactor_dmatp;
				matp_points[metric_selected][HitVertex_dmatp]->X+=dx/(ratio_width_dmatp*this->scaleFactor_dmatp);
				matp_points[metric_selected][HitVertex_dmatp]->Y-=dy/(ratio_height_dmatp*this->scaleFactor_dmatp);
			}
			//this->DrawingPos[this->HitVertex].x = (event->x() - this->moveD.x() - this->drawingBorder.x() * this->scaleFactor) / this->scaleFactor;
			repaint();
			//update();
		}
		else{
			this->drawSelectionBox_dmatp = true;
			//this->selectionBoxPos.setX();
			int Mx = event->x();
			int My = event->y();

			int x = (this->lastClickedPos_dmatp.x() < Mx)? this->lastClickedPos_dmatp.x() : Mx;
			//x = (x - this->moveD_dmatp.x()) / this->scaleFactor_dmatp;
			int y = (this->lastClickedPos_dmatp.y() < My)? this->lastClickedPos_dmatp.y() : My;
			//y = (y - this->moveD_dmatp.y()) / this->scaleFactor_dmatp;
			//int w = fabs(float(this->lastClickedPos_dmatp.x() - event->x())) / this->scaleFactor_dmatp;
			//int h = fabs(float(this->lastClickedPos_dmatp.y() - event->y())) / this->scaleFactor_dmatp;
			int w = fabs(float(this->lastClickedPos_dmatp.x() - event->x())) ;
			int h = fabs(float(this->lastClickedPos_dmatp.y() - event->y()));
			this->selectionBox_dmatp.setRect(x, y, w, h);

			//if (!(QApplication::keyboardModifiers() & 0x04000000))
				emit clearSelected_dmatp();

			for (int i = this->NoDrawingVertices_dmatp - 1; i >= 0; --i) {
				//this->Vertex[i].SecondSelection = false;

				if ( this->selectionBox_dmatp.contains(matp_drawing_positions[i]->X, matp_drawing_positions[i]->Y) ){
					//this->Vertex[i].SecondSelection = true;
					//emit this->selectedVertex(i);
					matp_points[metric_selected][i]->selected=true;
					matp_drawing_positions[i]->selected=true;
					this->hasSelectedVertices_dmatp=true;
					update();

				}
			}

			update();
		}
		
	}
	else if (event->buttons() & Qt::MiddleButton)
	{
		this->moveD_dmatp.setX(this->moveD_dmatp.x() + dx);
		this->moveD_dmatp.setY(this->moveD_dmatp.y() - dy);
		//fprintf(stderr, "yes\n");fflush(stderr);
		update();
	}
	

	this->lastPos_dmatp = event->pos();
	fprintf(stderr, "%d %d\n", lastPos_dmatp.x(), lastPos_dmatp.y());fflush(stderr);
}

void dynamic_matp_widget::mouseDoubleClickEvent(QMouseEvent *event){
	/*if ( event->button() == Qt::LeftButton ){
		if (this->HitVertex != this->NoDrawingVertices)
			emit selectedVertex(this->HitVertex);
	}*/
}



void dynamic_matp_widget::mouseReleaseEvent(QMouseEvent*){
	this->mousePressed_dmatp = false;
	vector<int> selected_vertices;
	vector<bool> is_vector_i_selected;
	//this->HitVertex=NoDrawingVertices;
	if (this->drawSelectionBox_dmatp){
		std::ofstream ofs("simeia_pou_epilextikan.txt");
		for(int i=0;i<this->NoDrawingVertices_dmatp;i++)
		{
			if(matp_drawing_positions[i]->selected==true)
			{	
				is_vector_i_selected.push_back(true);
				selected_vertices.push_back(i);
				ofs<<i<<" "<<this->matp_drawing_positions[i]->X<<" "<<this->matp_drawing_positions[i]->Y<<"\n";
			}
			else
				is_vector_i_selected.push_back(false);
		}
		ofs.close();
		this->drawSelectionBox_dmatp = false;
		//update();
	}
	if(this->HitVertex_dmatp!=NoDrawingVertices_dmatp)
	{
		for(int i=0;i<this->NoDrawingVertices_dmatp;i++)
		{
			if(i==this->HitVertex_dmatp)
				is_vector_i_selected.push_back(true);
			else
				is_vector_i_selected.push_back(false);
		}
	}
	if(this->HitVertex_dmatp!=NoDrawingVertices_dmatp||selected_vertices.size()>0)
	this->parent_dmatp->update_parent_mainwindow(this->HitVertex_dmatp,selected_vertices,this->ctrl_clicked,is_vector_i_selected);
	this->ctrl_clicked=false;
	for (int i = 0; i < this->NoDrawingVertices_dmatp; ++i){
		this->matp_drawing_positions[i]->selected= false;
	}

	this->hasSelectedVertices_dmatp = false;

	update();
	//qobject_cast<MdiChild *>(this->parent);
}

void dynamic_matp_widget::wheelEvent(QWheelEvent * event){
	//return;
	double eventx=event->x();
	double eventy=event->y();
	float old = this->scaleFactor_dmatp;
	this->scaleFactor_dmatp += (event->delta() / 480.0f); //or use any other step for zooming 

	if (this->scaleFactor_dmatp < 0.9)
		this->scaleFactor_dmatp  = 0.9;

	double dx = (event->x() - this->scaleFactor_dmatp *((event->x() - this->moveD_dmatp.x()) / old));
	double dy = ((this->height()-event->y()) - this->scaleFactor_dmatp *(((this->height()-event->y()) - this->moveD_dmatp.y()) / old));

	if (this->scaleFactor_dmatp == 1){
		dx = this->drawingBorder_dmatp.x();
		dy = this->drawingBorder_dmatp.y();
	}
	
	this->moveD_dmatp.setX(dx);
	this->moveD_dmatp.setY(dy);

	fprintf(stderr, "%f %d %d\n", this->scaleFactor_dmatp, this->size().width(), this->size().width());fflush(stderr);
	update();
	
}
/*
void dynamic_matp_widget::setGraphDisplayOptions(bool av, float ct, bool dfv, bool de, bool dl){
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
*/
void dynamic_matp_widget::setupColors_dmatp(){
	this->NoColors_dmatp = 21;

	this->brushColor_dmatp = (vsColor**) malloc(this->NoColors_dmatp * sizeof(vsColor*));
	this->penColor_dmatp = (vsColor**) malloc(this->NoColors_dmatp * sizeof(vsColor*));
	
	this->brushColor_dmatp[0] = new vsColor(255, 255, 255);
	this->brushColor_dmatp[1] = new vsColor(204, 204, 255);
	this->brushColor_dmatp[2] = new vsColor(204, 204, 26);
	this->brushColor_dmatp[3] = new vsColor(161, 13, 89);
	this->brushColor_dmatp[4] = new vsColor(108, 170, 255);
	this->brushColor_dmatp[5] = new vsColor(148, 255, 148);
	this->brushColor_dmatp[6] = new vsColor(244, 255, 98);
	this->brushColor_dmatp[7] = new vsColor(255, 98, 190);
	this->brushColor_dmatp[8] = new vsColor(156, 130, 28);
	this->brushColor_dmatp[9] = new vsColor(50, 205, 50);			//Lime Green
	this->brushColor_dmatp[10] = new vsColor(0, 0, 255);			//Blue
	this->brushColor_dmatp[11] = new vsColor(65, 105, 225);		//Royal Blue
	this->brushColor_dmatp[12] = new vsColor(25, 25, 112);		//Midnight Blue
	this->brushColor_dmatp[13] = new vsColor(217, 247, 255);		//Very Light Blue
	this->brushColor_dmatp[14] = new vsColor(124, 252, 0);		//Lawn Green
	this->brushColor_dmatp[15] = new vsColor(0, 100, 0);			//Dark Green
	this->brushColor_dmatp[16] = new vsColor(205, 92, 92);		//Indian Red
	this->brushColor_dmatp[17] = new vsColor(210, 105, 30);		//Chocolate
	this->brushColor_dmatp[18] = new vsColor(255, 165, 0);		//Orange
	this->brushColor_dmatp[19] = new vsColor(176, 48, 96);		//Maroon
	this->brushColor_dmatp[20] = new vsColor(153, 50, 204);		//Dark Orchid

	//this->brushColor_dmatp[2] = new vsColor(166, 223, 57);
	
	float darken = 0.85f;

	this->penColor_dmatp[0] = new vsColor(180, 180, 180);
	for (int i = 1; i < this->NoColors_dmatp; i++){
		this->penColor_dmatp[i] = new vsColor(int(this->brushColor_dmatp[i]->R * darken), int(this->brushColor_dmatp[i]->G * darken), int(this->brushColor_dmatp[i]->B * darken));
	}
}


void dynamic_matp_widget::ShowMatpChoices(const QPoint& pos)
{
	QPoint globalPos = this->mapToGlobal(pos);
	QMenu myMenu(this);
	this->dynamic_numbers_displayed = new QAction(tr("display numbers"), this);
	this->dynamic_numbers_displayed->setCheckable(true);
	this->dynamic_numbers_displayed->setChecked(true);

	this->dynamic_dates_displayed = new QAction(tr("display dates"), this);
	this->dynamic_dates_displayed->setCheckable(true);
	this->dynamic_dates_displayed->setChecked(true);
	myMenu.addAction(this->dynamic_numbers_displayed);
	myMenu.addAction(this->dynamic_dates_displayed);

	QAction* selectedItem = myMenu.exec(globalPos);
	if (selectedItem){
		fprintf(stderr, "Executed\n");fflush(stderr);
	}
	else{
		fprintf(stderr, "Not Executed\n");fflush(stderr);
	}
}

dynamic_matp_widget::~dynamic_matp_widget()
{

}
