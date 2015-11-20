#include "dynamic_bar_widget.h"
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

dynamic_bar_widget::dynamic_bar_widget(QWidget *parent,MdiChild* parentmdich, std::vector<Eigen::MatrixXd> bar,int dynamic_numberoffeatures,std::vector<std::string> names_of_files) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
//dynamic_bar_widget::dynamic_bar_widget(QWidget *parent, vsGraph *graph, int drawingfeature) : QWidget(parent)
{
    //setFixedSize(400, 400);
	//setMinimumSize(300, 300);
    this->dynamic_numberoffeatures=dynamic_numberoffeatures;
	this->names_of_files=names_of_files;
	this->parent_dbar=parentmdich;
	connect(this, SIGNAL(clearSelected_dbar()), this, SLOT(unsetSelected_dbar()));
	this->setupColors_dbar();
	setAutoFillBackground(false);

    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    //background = QBrush(QColor(64, 32, 64));
    //this->background = QBrush(QColor(164, 132, 164));
    this->background_dbar = QBrush(QColor(255, 255, 255));
    //this->circleBrush = QBrush(gradient);
    this->circleBrush_dbar = QBrush(QColor(0,0,0));
    this->circlePen_dbar = QPen(Qt::black);
    //this->circlePen.setWidth(1);
    //this->textPen = QPen(Qt::white);
    this->textPen_dbar = QPen(Qt::black);
    this->textFont_dbar.setPixelSize(12);
    this->selectedBrush_dbar = QBrush(QColor(255,0,0));
    this->selectedPen_dbar = QPen(QColor(255,0,0));

	

	

	this->DrawingBox_dbar.setWidth(0);
	this->DrawingBox_dbar.setHeight(0);

    this->mousePressed_dbar = false;
    this->drawSelectionBox_dbar = false;
	this->scaleFactor_dbar = 1;
	this->drawingBorder_dbar.setX(0);
	this->drawingBorder_dbar.setY(0);
	this->moveD_dbar.setX(this->drawingBorder_dbar.x());
	this->moveD_dbar.setY(this->drawingBorder_dbar.y());

	//this->setupColors();

	this->PositionsSet_dbar = false;
	

	

	
	

	/*this->timer_dbar = new QTimer(this);
	connect(this->timer_dbar, SIGNAL(timeout_dbar()), this, SLOT(animate_dbar()));
	this->timer_dbar->setSingleShot(true);*/

	this->isVisible_dbar = true;

	this->CuttingThreshold_dbar = -3.0f;
	

	this->isShownAction_dbar = new QAction(tr(""), this);
	this->isShownAction_dbar->setCheckable(true);
	this->isShownAction_dbar->setChecked(true);
	this->bar=bar;
	this->NoDrawingVertices_dbar=bar[0].rows();
	this->HitVertex_dbar=NoDrawingVertices_dbar;
	for(int i=0;i<bar.size();i++)
	{
		bar_points.push_back(vector<bar_point* >());
		for(int j=0;j<this->NoDrawingVertices_dbar;j++)
		{
			this->bar_points[i].push_back(new bar_point(bar[i].coeff(j,0),bar[i].coeff(j,1),6.0,false));
			this->bar_drawing_positions.push_back(new bar_point(bar[i].coeff(j,0),bar[i].coeff(j,1),6.0,false));
			
		}
	}
	this->hasSelectedVertices_dbar = false;
	this->type_of_labels_displayed=0;
	this->metric_selected=0;
	this->ctrl_clicked=false;
	this->show_only_selected=false;
	this->is_vector_i_selected_global=vector<bool>();
	for(int i=0;i<this->NoDrawingVertices_dbar;i++)
	{
		this->is_vector_i_selected_global.push_back(false);
	}
	this->is_vector_i_selected_global[0]=true;
	colorInit_vertice=Qt::yellow;
	colorTarget_vertice=QColor(255, 127,0);



	this->number_of_dots=(double)this->names_of_files.size()/dynamic_numberoffeatures;
	

	this->selected_point=0;
	this->selected_point_hitted=false;
	this->has_moved=false;
	this->point_h=20;
	this->point_width=6;
	this->fontsize=15;
	this->which_point_hitted=this->number_of_dots;
	this->dotted_selection=false;
	this->h_point=20;
	this->height_upgrade_for_red_points=10;
	this->height_upgrade=0;
	//this->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowbarChoices(const QPoint&)));
	//connect(this->isShownAction, SIGNAL(triggered(bool)), this, SLOT(setView(bool)));
}
void dynamic_bar_widget::unsetSelected_dbar(){
	for (int i = 0; i < this->NoDrawingVertices_dbar; ++i){
		this->bar_drawing_positions[i]->selected= false;
	}

	this->hasSelectedVertices_dbar = false;
	update();
}
void dynamic_bar_widget::animate(){
    //repaint();
	update();
    //updateGL();
//fprintf(stderr, "Animated ...-\n");fflush(stderr);
}

void dynamic_bar_widget::paintEvent(QPaintEvent *event){
	/*if (this->timer_dbar->isActive())
		this->timer_dbar->stop();
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
    /*if ( !this->PositionsSet_dbar )
		this->timer_dbar->start(10);*/
	
}

void dynamic_bar_widget::paint(QPainter* painter, QPaintEvent* event)
{
	
	
	//if(this->width_of_last_bar!=this->size().width()||this->height_of_last_bar!=this->size().height())
	//{
		/*if (this->HitVertex!=NoDrawingVertices)
		{
			for(int i=0;i<bar[0].rows();i++)
			{
				double drawpointx=bar_drawing_positions[i]->X - bar_drawing_positions[i]->size_of_point/2;
				double drawpointy=bar_drawing_positions[i]->Y - bar_drawing_positions[i]->size_of_point/2;
				painter->drawEllipse(QRectF(drawpointx,drawpointy , bar_points[i]->size_of_point, bar_points[i]->size_of_point));
			}
			return;
		}*/
		//this->selectedBrush_dbar.setColor(QColor(this->brushColor_dbar[12]->R, this->brushColor_dbar[12]->G, this->brushColor_dbar[12]->B, 30));
		this->selectedBrush_dbar.setColor(QColor(this->penColor_dbar[12]->R, this->penColor_dbar[12]->G, this->penColor_dbar[12]->B, 210));
		this->selectedPen_dbar.setColor(QColor(this->penColor_dbar[12]->R, this->penColor_dbar[12]->G, this->penColor_dbar[12]->B, 210));
		painter->fillRect(event->rect(), background_dbar);
		painter->setBrush(this->selectedBrush_dbar);
		painter->setPen(this->selectedPen_dbar);
		paintline_dbar(painter);
		//paintedges_dbar(painter);
	//}

	if (this->drawSelectionBox_dbar){
		this->circleBrush_dbar.setColor(QColor(this->brushColor_dbar[12]->R, this->brushColor_dbar[12]->G, this->brushColor_dbar[12]->B, 30));
		this->circlePen_dbar.setColor(QColor(this->penColor_dbar[12]->R, this->penColor_dbar[12]->G, this->penColor_dbar[12]->B, 210));
		//this->circleBrush_dbar.setColor(QColor(Qt::red));
		//this->circlePen_dbar.setColor(QColor(Qt::red));
		painter->setBrush(this->circleBrush_dbar);
		painter->setPen(this->circlePen_dbar);
		painter->drawRect(this->selectionBox_dbar);
		//painter->drawPolygon(this->selectionBox, Qt::OddEvenFill);
		//painter->drawPolyline(this->selectionBox);
	}
}
void dynamic_bar_widget::paintline_dbar(QPainter *painter)
{
	point_width=0.8*this->size().width()/(this->number_of_dots*2);
	if(point_width>6) point_width=6; 
	if(point_width<3) point_width=3; 
	
	//point_width=0.1;
	this->line_distances=(this->size().width()*0.8/((double)this->number_of_dots-1))*this->scaleFactor_dbar;
	this->x1=(0.1*this->size().width()*this->scaleFactor_dbar) +moveD_dbar.x();
	this->x2=(0.9*this->size().width()*this->scaleFactor_dbar) +moveD_dbar.x();
	this->bar_position_y=this->size().height()/2;
	painter->drawLine(x1, this->bar_position_y, x2, this->bar_position_y);
	std::vector<int> selected_points_from_previous_selection;
	int previous_number_of_selection;
	for(int i=0;i<this->is_vector_i_selected_global.size();i++)
	{
		if(is_vector_i_selected_global[i]==true)
		{
			if(selected_points_from_previous_selection.size()>0&&(i-previous_number_of_selection)>1)
				this->dotted_selection=true;
			previous_number_of_selection=i;
			selected_points_from_previous_selection.push_back(i);
		}
	}
	//this->number_of_points=this->names_of_files.size();
	points_location_x.clear();
	this->points_location_x_after_scale_moved.clear();
	for(int i=0;i<number_of_dots;i++)
	{
		
		this->height_upgrade=0;
		double x = x1+i*this->line_distances-(point_width/2);
		double y = this->bar_position_y-(this->h_point/2);
		double w = point_width ;
		double h = this->h_point;
		this->pointBox_dbar.setRect(x, this->bar_position_y-point_h/2, w, h);

		this->circleBrush_dbar.setColor(QColor(this->brushColor_dbar[12]->R, this->brushColor_dbar[12]->G, this->brushColor_dbar[12]->B, 30));
		this->circlePen_dbar.setColor(QColor(this->penColor_dbar[12]->R, this->penColor_dbar[12]->G, this->penColor_dbar[12]->B, 210));
		painter->setBrush(this->circleBrush_dbar);
		painter->setPen(this->circlePen_dbar);

		painter->drawRect(this->pointBox_dbar);
		int x_inside,y_inside,w_inside,h_inside;
		this->pointBox_dbar.getRect(&x_inside,&y_inside,&w_inside,&h_inside);
		
		points_location_x.push_back(x_inside);
		
		//this->points_location_x_after_scale_moved.push_back(x_inside);
		
		
		this->textFont_dbar.setPixelSize(this->fontsize);     
		this->circleBrush_dbar.setColor(QColor(0,0,0));
		this->circlePen_dbar.setColor(QColor(0,0,0));
		painter->setBrush(this->circleBrush_dbar);
		painter->setPen(this->circlePen_dbar);

		painter->setFont(this->textFont_dbar); 
		double distance_of_points=abs(this->moving_point_location_x+point_width/2-points_location_x[i]);
		double biggest_distance_to_match=(double)this->line_distances/2;
		if(distance_of_points<biggest_distance_to_match&&this->has_moved==true)
		{
			painter->setBrush(Qt::red);
			painter->setPen(Qt::red);
			this->textFont_dbar.setPixelSize(18);
			painter->setFont(this->textFont_dbar);
			height_upgrade=height_upgrade_for_red_points;
			//painter->drawText(this->points_location_x[i] , bar_position_y-(this->h_point/2+height_upgrade), QString::number(i+1));
		}
		else if(this->drawSelectionBox_dbar == true)
		{
			
			if(selected_vertices.size()>0&&(i==selected_vertices[0]||i==selected_vertices[selected_vertices.size()-1]))
			{
				painter->setBrush(Qt::red);
				painter->setPen(Qt::red);
				this->textFont_dbar.setPixelSize(18);
				painter->setFont(this->textFont_dbar);
				height_upgrade=height_upgrade_for_red_points;
			}
			//painter->drawText(this->points_location_x[i], bar_position_y-(this->h_point/2+height_upgrade), QString::number(i+1));
			

		}

		else if(this->show_only_selected&&this->dotted_selection==true&&this->drawSelectionBox_dbar == false&&!(QApplication::keyboardModifiers() & 0x08000000))
		{
			if(!this->has_moved==true)
			{
				painter->setBrush(Qt::red);
				painter->setPen(Qt::red);
				this->textFont_dbar.setPixelSize(18);
				painter->setFont(this->textFont_dbar); 
				height_upgrade=height_upgrade_for_red_points;
			}
			else continue;
			if(this->is_vector_i_selected_global.size()>0&&this->is_vector_i_selected_global[i]==true)
			{

				
			}
			else
				continue;
		}
		else if(this->show_only_selected&&this->dotted_selection==false&&this->drawSelectionBox_dbar == false&&!(QApplication::keyboardModifiers() & 0x08000000))
		{
			if(this->is_vector_i_selected_global.size()>0&&(selected_points_from_previous_selection[0]==i||selected_points_from_previous_selection[selected_points_from_previous_selection.size()-1]==i))
			{
				if(!this->has_moved==true)
				{
					painter->setBrush(Qt::red);
					painter->setPen(Qt::red);
					this->textFont_dbar.setPixelSize(18);
					painter->setFont(this->textFont_dbar); 
					height_upgrade=height_upgrade_for_red_points;
				}
				else continue;
				
			}
			else 
				continue;
		}
		else
		{
			if(this->has_many_selected_points&&(i==selected_points_from_previous_selection[0]||i==selected_points_from_previous_selection[selected_points_from_previous_selection.size()-1])&&this->drawSelectionBox_dbar == false&&!(QApplication::keyboardModifiers() & 0x08000000))
			{
				
				if(!this->has_moved==true)
				{
					painter->setBrush(Qt::red);
					painter->setPen(Qt::red);
					this->textFont_dbar.setPixelSize(18);
					painter->setFont(this->textFont_dbar); 
					height_upgrade=height_upgrade_for_red_points;
				}
				//painter->drawText(this->points_location_x[i], bar_position_y-(this->h_point/2+height_upgrade), QString::number(i+1));
			}
			else if(type_of_labels_displayed==0&&!(QApplication::keyboardModifiers() & 0x08000000));

				//painter->drawText(this->points_location_x[i] , bar_position_y-(this->h_point/2), QString::number(i+1));
			else if(QApplication::keyboardModifiers() & 0x08000000)
			{
				if(selected_vertices.size()>0&&(i==selected_vertices[selected_vertices.size()-1]||i==selected_vertices[0]))
				{
					painter->setBrush(Qt::red);
					painter->setPen(Qt::red);
					this->textFont_dbar.setPixelSize(18);
					painter->setFont(this->textFont_dbar);
					height_upgrade=height_upgrade_for_red_points;
					//painter->drawText(this->points_location_x[i] , bar_position_y-(this->h_point/2+height_upgrade), QString::number(i+1));
				}
				else;
					//painter->drawText(this->points_location_x[i] , bar_position_y-(this->h_point/2), QString::number(i+1));
			}
			else
			{
				std::string name_of_file=this->names_of_files[i*2];
				int first_pos=name_of_file.find_last_of( "(" )+1;
				int last_pos=name_of_file.find_last_of( ")" );
				int len=last_pos-first_pos;
				this->textFont_dbar.setPixelSize(12);
				painter->setFont(this->textFont_dbar); 
				//painter->drawText(this->points_location_x[i], bar_position_y-(this->h_point/2+height_upgrade), QString::fromStdString(name_of_file.substr(first_pos,len)));
			}
			//painter->setPen(this->circlePen_dbar);
		}


		if(type_of_labels_displayed==0)

			painter->drawText(this->points_location_x[i], bar_position_y-(this->h_point/2+height_upgrade), QString::number(i+1));
		else
		{
			std::string name_of_file=this->names_of_files[i*dynamic_numberoffeatures];
			int first_pos=name_of_file.find_last_of( "(" )+1;
			int last_pos=name_of_file.find_last_of( ")" );
			int len=last_pos-first_pos;
			this->textFont_dbar.setPixelSize(12);
			painter->setFont(this->textFont_dbar); 
			painter->drawText(this->points_location_x[i], bar_position_y-(this->h_point/2+height_upgrade), QString::fromStdString(name_of_file.substr(first_pos,len)));
		}
		
	}
	if(this->drawSelectionBox_dbar == true)
	{
		for(int i=0;i<selected_vertices.size();i++)
		{
			QColor suspiciouscolor(Qt::red);
			painter->setBrush(suspiciouscolor);
			painter->setPen(suspiciouscolor);
			double x = x1+selected_vertices[i]*this->line_distances-(point_width/2) ;
			double y = this->bar_position_y-(this->h_point/2);
			double w = point_width ;
			double h = this->h_point;
			this->pointBox_dbar.setRect(x, y, w, h);
			painter->drawRect(this->pointBox_dbar);
		}
	}
	else if (this->has_many_selected_points == true&&!this->has_moved&&!(QApplication::keyboardModifiers() & 0x08000000))
	{
		for(int i=0;i<this->is_vector_i_selected_global.size();i++)
		{
			if(is_vector_i_selected_global[i]==true)
			{
				QColor suspiciouscolor(Qt::red);
				painter->setBrush(suspiciouscolor);
				painter->setPen(suspiciouscolor);
				double x = x1+i*this->line_distances-(point_width/2) ;
				double y = this->bar_position_y-(this->h_point/2);
				double w = point_width ;
				double h = this->h_point;
				this->pointBox_dbar.setRect(x, y, w, h);
				painter->drawRect(this->pointBox_dbar);
			}
		}
	}
	else if(!this->selected_point_hitted&&this->which_point_hitted<this->number_of_dots)
	{
		QColor suspiciouscolor(Qt::red);
		painter->setBrush(suspiciouscolor);
		painter->setPen(suspiciouscolor);
		double x = x1+this->which_point_hitted*this->line_distances-(point_width/2) ;
		double y = this->bar_position_y-(this->h_point/2);
		double w = point_width ;
		double h = this->h_point;
		this->pointBox_dbar.setRect(x, y, w, h);
		painter->drawRect(this->pointBox_dbar);
	}
	else
	{	
		if((QApplication::keyboardModifiers() & 0x08000000))
		{
			for(int i=0;i<this->selected_vertices.size();i++)
			{
				QColor suspiciouscolor(Qt::red);
				painter->setBrush(suspiciouscolor);
				painter->setPen(suspiciouscolor);
				double x = x1+selected_vertices[i]*this->line_distances-(point_width/2) ;
				double y = this->bar_position_y-(this->h_point/2);
				double w = point_width ;
				double h = this->h_point;
				this->pointBox_dbar.setRect(x, y, w, h);
				painter->drawRect(this->pointBox_dbar);
			}
				
		}
		else if(this->selected_point_hitted&&this->has_moved)
		{
			QColor suspiciouscolor(Qt::red);
			painter->setBrush(suspiciouscolor);
			painter->setPen(suspiciouscolor);
			this->pointBox_dbar.setRect(moving_point_location_x, bar_position_y-this->point_h/2, this->point_width, this->point_h);
			painter->drawRect(this->pointBox_dbar);
		}
		else
		{
			//if(!this->selected_point_hitted&&this->which_point_hitted==this->number_of_dots)
			//{
				QColor suspiciouscolor(Qt::red);
				painter->setBrush(suspiciouscolor);
				painter->setPen(suspiciouscolor);
				double x = x1+selected_point*this->line_distances-(point_width/2) ;
				double y = this->bar_position_y-(this->h_point/2);
				double w = point_width ;
				double h = this->h_point;
				this->pointBox_dbar.setRect(x, y, w, h);
				painter->drawRect(this->pointBox_dbar);
			//}
		}
	
	}
	
	
	
	/*double min_x_bar=0;
	double max_x_bar=0;
	double min_y_bar=0;
	double max_y_bar=0;
	this->width_of_last_bar=this->size().width();
	this->height_of_last_bar=this->size().height();

	for(int i=0;i<bar[0].rows();i++)
	{
		if(bar[metric_selected].coeff(i,0)<min_x_bar) min_x_bar=bar[metric_selected].coeff(i,0);
		if(bar[metric_selected].coeff(i,0)>max_x_bar) max_x_bar=bar[metric_selected].coeff(i,0);
		if(bar[metric_selected].coeff(i,1)<min_y_bar) min_y_bar=bar[metric_selected].coeff(i,1);
		if(bar[metric_selected].coeff(i,1)>max_y_bar) max_y_bar=bar[metric_selected].coeff(i,1);
	}

	double bar_width=max_x_bar-min_x_bar;
	double bar_height=max_y_bar-min_y_bar;
	ratio_width_dbar=this->size().width()*0.8/bar_width;
	ratio_height_dbar=this->size().height()*0.8/bar_width;

	double ratio=ratio_width_dbar<ratio_height_dbar?ratio_width_dbar:ratio_height_dbar;
	double sizes=this->size().width();
	
	std::ofstream ofs("barPONTS.txt");
	for(int i=0;i<bar[0].rows();i++)
	{
		if (bar_drawing_positions[i]->selected){
			painter->setBrush(this->selectedBrush_dbar);
			painter->setPen(this->selectedPen_dbar);
		}
		else
		{
			QColor color;
			double alpha = ((double)i+1)/((double)bar[0].rows());
			color.setRedF((1.0-alpha) * this->colorInit_vertice.redF() + alpha * this->colorTarget_vertice.redF());
			color.setGreenF((1.0-alpha) * this->colorInit_vertice.greenF() + alpha * this->colorTarget_vertice.greenF());
			color.setBlueF((1.0-alpha) * this->colorInit_vertice.blueF() + alpha * this->colorTarget_vertice.blueF());
			this->circleBrush_dbar.setColor(color);
			this->circlePen_dbar.setColor(color);
			
			painter->setBrush(this->circleBrush_dbar);
			painter->setPen(this->circlePen_dbar);
		}
		if(this->show_selected==true&&is_vector_i_selected_global.size()>0)
		{
			if(is_vector_i_selected_global[i]==true)
			{
				painter->setBrush(this->selectedBrush_dbar);
				painter->setPen(this->selectedPen_dbar);
			}
		}
		double testx=(bar_points[metric_selected][i]->X - min_x_bar)*ratio_width_dbar+this->size().width()*0.1;
		bar_drawing_positions[i]->X=(testx*this->scaleFactor_dbar)+ this->moveD_dbar.x() ;
		double testy=(bar_points[metric_selected][i]->Y - min_y_bar)*ratio_height_dbar+this->size().height()*0.1;
		bar_drawing_positions[i]->Y=this->height()-(testy*this->scaleFactor_dbar)+ this->moveD_dbar.y();
		
		bar_drawing_positions[i]->size_of_point=bar_points[metric_selected][i]->size_of_point*this->scaleFactor_dbar;
		//bar_points[i]->X=bar_points[i]->X/ratio;
		//bar_points[i]->Y=bar_points[i]->Y/ratio;
		double drawpointx=bar_drawing_positions[i]->X - bar_drawing_positions[i]->size_of_point/2;
		double drawpointy=bar_drawing_positions[i]->Y - bar_drawing_positions[i]->size_of_point/2;
		painter->drawEllipse(QRectF(drawpointx,drawpointy , bar_drawing_positions[i]->size_of_point, bar_drawing_positions[i]->size_of_point));
		ofs<<bar_points[metric_selected][i]->X<<" "<<bar_points[metric_selected][i]->Y<<"\n";

		//painter->setPen(this->textPen_dbar);
		

	}
	ofs.close();*/
}


void dynamic_bar_widget::mousePressEvent(QMouseEvent *event)
{
	fprintf(stderr,"dynamicbar mousepressevent\n");fflush(stderr);
	double eventx=event->x();
	double eventy=event->y();
	this->lastPos_dbar = event->pos();
	this->lastClickedPos_dbar = event->pos();
	if((QApplication::keyboardModifiers() & 0x08000000))
	{
		for(int i=0;i<points_location_x.size();i++)
		{
			if((eventx>points_location_x[i]-point_width)&&(eventx<points_location_x[i]+point_width)&&(eventy>bar_position_y-point_h/2)&&eventy<(bar_position_y+point_h/2))
				this->selected_vertices.push_back(i);
		}
		//if(this->selected_vertices.size()>0)
		//	this->has_many_selected_points=true;
		
	}
	else
	{
		if((eventx>points_location_x[selected_point]-point_width)&&(eventx<points_location_x[selected_point]+point_width)&&(eventy>bar_position_y-point_h/2)&&eventy<(bar_position_y+point_h/2))
			this->selected_point_hitted=true;
		if (this->has_many_selected_points == true)
		{
			for(int i=0;i<this->is_vector_i_selected_global.size();i++)
			{
				if(is_vector_i_selected_global[i]==true)
				{
					if((eventx>points_location_x[i]-point_width)&&(eventx<points_location_x[i]+point_width)&&(eventy>bar_position_y-point_h/2)&&eventy<(bar_position_y+point_h/2))
					{
						this->selected_point_hitted=true;
						this->selected_point=i;
					}

				}
			}
		}
		for(int i=0;i<points_location_x.size();i++)
		{
			if((eventx>points_location_x[i]-point_width)&&(eventx<points_location_x[i]+point_width)&&(eventy>bar_position_y-point_h/2)&&eventy<(bar_position_y+point_h/2))
				this->which_point_hitted=i;
		}
		 
		
		if(QApplication::keyboardModifiers() & 0x04000000)
			this->ctrl_clicked=true;
	}
	repaint();
	
	/*this->mousePressed_dbar = true;

	if ((event->buttons() & Qt::LeftButton) )
		emit clearSelected_dbar();
	
	

	this->HitVertex_dbar = bar[0].rows();
	float Mx = event->x();
	float My = event->y(); 
	
	int N = bar[0].rows();
	
	float rX, rY;
	for (int i = N - 1; i >= 0; --i){
		
		rX = bar_drawing_positions[i]->X - Mx;
		rY = bar_drawing_positions[i]->Y - My;
    
		if (sqrt(rX * rX + rY * rY) < this->bar_drawing_positions[i]->size_of_point / 2.0f){
			fprintf(stderr, "%d -> %d %d\n", i, bar_drawing_positions[i]->X, this->bar_drawing_positions[i]->Y);fflush(stderr);
			this->HitVertex_dbar = i;
			break;
		}   
    
	} */
}
void dynamic_bar_widget::mouseMoveEvent(QMouseEvent *event){
	
	fprintf(stderr,"dynamicbar mousemoveevent\n");fflush(stderr);
	int dx = event->x() - lastPos_dbar.x();
	if (event->buttons() & Qt::LeftButton) 
	{

		if (this->which_point_hitted==this->number_of_dots)
		{
			this->drawSelectionBox_dbar = true;
			int Mx = event->x();
			int My = event->y();

			int x = (this->lastClickedPos_dbar.x() < Mx)? this->lastClickedPos_dbar.x() : Mx;
			int y = (this->lastClickedPos_dbar.y() < My)? this->lastClickedPos_dbar.y() : My;
			int w = fabs(float(this->lastClickedPos_dbar.x() - event->x())) ;
			int h = fabs(float(this->lastClickedPos_dbar.y() - event->y()));
			this->selectionBox_dbar.setRect(x, y, w, h);

			//if (!(QApplication::keyboardModifiers() & 0x04000000))
			//emit clearSelected_dbar();
			this->selected_vertices.clear();
			this->is_vector_i_selected.clear();
			for (int i = 0; i < this->points_location_x.size(); ++i) {
				if ( this->selectionBox_dbar.contains(points_location_x[i], bar_position_y) )
				{
					this->selected_vertices.push_back(i);
					this->is_vector_i_selected.push_back(true);
					update();

				}
				else
					this->is_vector_i_selected.push_back(false);
			}

			update();
		}
		else if(this->selected_point_hitted)
		{
			dx=event->x() - lastClickedPos_dbar.x();
			int x = x1+selected_point*this->line_distances+dx-point_width/2;
			int y = this->bar_position_y-(this->h_point/2);
			int w = point_width ;
			int h = this->h_point;
			if(x<this->x1-point_width/2) x=this->x1-point_width/2;
			if(x>this->x2-point_width/2) x=this->x2-point_width/2;
			this->moving_point_location_x=x;
			this->has_moved=true;
			repaint();

		}
	}
	else if (event->buttons() & Qt::MiddleButton)
	{
		this->moveD_dbar.setX(this->moveD_dbar.x() + dx);
		//this->moveD_dbar.setY(this->moveD_dbar.y() + dy);
		update();
	}
	this->lastPos_dbar = event->pos();

	/*int dx = event->x() - lastPos_dbar.x();
	int dy = event->y() - lastPos_dbar.y();

	if (event->buttons() & Qt::LeftButton) {

		if (this->HitVertex_dbar != this->NoDrawingVertices_dbar){
			if (this->scaleFactor_dbar == 1){
				bar_drawing_positions[HitVertex_dbar]->X  += dx;
				bar_drawing_positions[HitVertex_dbar]->Y += dy;
				bar_points[metric_selected][HitVertex_dbar]->X+=dx/ratio_width_dbar;
				bar_points[metric_selected][HitVertex_dbar]->Y-=dy/ratio_height_dbar;
			}
			else{
				bar_drawing_positions[HitVertex_dbar]->X += dx / this->scaleFactor_dbar ; //Add 0.5 so that the pointer doesn't lose focus due to multiplication effect
				bar_drawing_positions[HitVertex_dbar]->Y += dy / this->scaleFactor_dbar;
				bar_points[metric_selected][HitVertex_dbar]->X+=dx/(ratio_width_dbar*this->scaleFactor_dbar);
				bar_points[metric_selected][HitVertex_dbar]->Y-=dy/(ratio_height_dbar*this->scaleFactor_dbar);
			}
			//this->DrawingPos[this->HitVertex].x = (event->x() - this->moveD.x() - this->drawingBorder.x() * this->scaleFactor) / this->scaleFactor;
			repaint();
			//update();
		}
		else{
			this->drawSelectionBox_dbar = true;
			//this->selectionBoxPos.setX();
			int Mx = event->x();
			int My = event->y();

			int x = (this->lastClickedPos_dbar.x() < Mx)? this->lastClickedPos_dbar.x() : Mx;
			//x = (x - this->moveD_dbar.x()) / this->scaleFactor_dbar;
			int y = (this->lastClickedPos_dbar.y() < My)? this->lastClickedPos_dbar.y() : My;
			//y = (y - this->moveD_dbar.y()) / this->scaleFactor_dbar;
			//int w = fabs(float(this->lastClickedPos_dbar.x() - event->x())) / this->scaleFactor_dbar;
			//int h = fabs(float(this->lastClickedPos_dbar.y() - event->y())) / this->scaleFactor_dbar;
			int w = fabs(float(this->lastClickedPos_dbar.x() - event->x())) ;
			int h = fabs(float(this->lastClickedPos_dbar.y() - event->y()));
			this->selectionBox_dbar.setRect(x, y, w, h);

			//if (!(QApplication::keyboardModifiers() & 0x04000000))
				emit clearSelected_dbar();

			for (int i = this->NoDrawingVertices_dbar - 1; i >= 0; --i) {
				//this->Vertex[i].SecondSelection = false;

				if ( this->selectionBox_dbar.contains(bar_drawing_positions[i]->X, bar_drawing_positions[i]->Y) ){
					//this->Vertex[i].SecondSelection = true;
					//emit this->selectedVertex(i);
					bar_points[metric_selected][i]->selected=true;
					bar_drawing_positions[i]->selected=true;
					this->hasSelectedVertices_dbar=true;
					fprintf(stderr,"prin to update 22\n");fflush(stderr);
					update();

				}
			}

			fprintf(stderr,"prin to update 1\n");fflush(stderr);
			update();
		}
		
	}
	else if (event->buttons() & Qt::MiddleButton)
	{
		this->moveD_dbar.setX(this->moveD_dbar.x() + dx);
		this->moveD_dbar.setY(this->moveD_dbar.y() + dy);
		//fprintf(stderr, "yes\n");fflush(stderr);
		update();
	}
	

	this->lastPos_dbar = event->pos();
	fprintf(stderr, "%d %d\n", lastPos_dbar.x(), lastPos_dbar.y());fflush(stderr);*/
}

void dynamic_bar_widget::mouseDoubleClickEvent(QMouseEvent *event){
	/*if ( event->button() == Qt::LeftButton ){
		if (this->HitVertex != this->NoDrawingVertices)
			emit selectedVertex(this->HitVertex);
	}*/
}



void dynamic_bar_widget::mouseReleaseEvent(QMouseEvent*){
	
	fprintf(stderr,"dynamicbar mousereleaseevent\n");fflush(stderr);
	this->setFocus();
	if((QApplication::keyboardModifiers() & 0x08000000))
	{
		return;
	}
	int selected_point_for_new_graph=0;
	for(int i=0;i<this->number_of_dots;i++)
	{
		if(this->drawSelectionBox_dbar==true)
		{
			//if(this->is_vector_i_selected[i]==true)
				//selected_vertices.push_back(i);
		}

		else
		{
			double distance_of_points=abs(this->moving_point_location_x+point_width/2-points_location_x[i]);
			double biggest_distance_to_match=(double)this->line_distances/2;
			if(distance_of_points<biggest_distance_to_match&&this->has_moved==true)
			{
				this->is_vector_i_selected.push_back(true);
				selected_point_for_new_graph=i;
			}
			else
			{
				if(this->which_point_hitted==i&&this->has_moved==false)
				{
					this->is_vector_i_selected.push_back(true);
					selected_point_for_new_graph=this->which_point_hitted;
				}
				else
					this->is_vector_i_selected.push_back(false);
			}
		}
	}
	if(this->selected_point_hitted==true||this->which_point_hitted<this->number_of_dots||selected_vertices.size()>0)
		this->parent_dbar->update_parent_mainwindow(selected_point_for_new_graph,selected_vertices,this->ctrl_clicked,is_vector_i_selected);
	this->selected_point_hitted=false;
	this->has_moved=false;
	this->which_point_hitted=this->number_of_dots;
	this->ctrl_clicked=false;
	this->selected_vertices.clear();
	this->is_vector_i_selected.clear();
	this->drawSelectionBox_dbar=false;
	repaint();
	
	
	/*this->mousePressed_dbar = false;
	vector<int> selected_vertices;
	vector<bool> is_vector_i_selected;
	//this->HitVertex=NoDrawingVertices;
	if (this->drawSelectionBox_dbar){
		std::ofstream ofs("simeia_pou_epilextikan.txt");
		for(int i=0;i<this->NoDrawingVertices_dbar;i++)
		{
			if(bar_drawing_positions[i]->selected==true)
			{	
				is_vector_i_selected.push_back(true);
				selected_vertices.push_back(i);
				ofs<<i<<" "<<this->bar_drawing_positions[i]->X<<" "<<this->bar_drawing_positions[i]->Y<<"\n";
			}
			else
				is_vector_i_selected.push_back(false);
		}
		ofs.close();
		this->drawSelectionBox_dbar = false;
		//update();
	}
	if(this->HitVertex_dbar!=NoDrawingVertices_dbar)
	{
		for(int i=0;i<this->NoDrawingVertices_dbar;i++)
		{
			if(i==this->HitVertex_dbar)
				is_vector_i_selected.push_back(true);
			else
				is_vector_i_selected.push_back(false);
		}
	}
	if(this->HitVertex_dbar!=NoDrawingVertices_dbar||selected_vertices.size()>0)
	this->parent_dbar->update_parent_mainwindow(this->HitVertex_dbar,selected_vertices,this->ctrl_clicked,is_vector_i_selected);
	this->ctrl_clicked=false;
	for (int i = 0; i < this->NoDrawingVertices_dbar; ++i){
		this->bar_drawing_positions[i]->selected= false;
	}

	this->hasSelectedVertices_dbar = false;

	update();
	//qobject_cast<MdiChild *>(this->parent);*/
}

void dynamic_bar_widget::wheelEvent(QWheelEvent * event){
	//return;
	fprintf(stderr,"dynamicbar wheelevent\n");fflush(stderr);
	float old = this->scaleFactor_dbar;
	this->scaleFactor_dbar += (event->delta() / 480.0f); //or use any other step for zooming 

	if (this->scaleFactor_dbar < 1)
		this->scaleFactor_dbar  = 1;

	int dx = event->x() - this->scaleFactor_dbar *((event->x() - this->moveD_dbar.x()) / old);
	int dy = event->y() - this->scaleFactor_dbar *((event->y() - this->moveD_dbar.y()) / old);

	if (this->scaleFactor_dbar == 1){
		dx = this->drawingBorder_dbar.x();
		dy = this->drawingBorder_dbar.y();
	}
	
	this->moveD_dbar.setX(dx);
	this->moveD_dbar.setY(dy);

	fprintf(stderr, "%f %d %d\n", this->scaleFactor_dbar, this->size().width(), this->size().width());fflush(stderr);
	update();
	
}
 void dynamic_bar_widget::keyReleaseEvent(QKeyEvent *event)
 {
	fprintf(stderr,"dynamicbar keyreleaseevent\n");fflush(stderr);
	 int selected_point_for_new_graph=0;

	if((event->key()==Qt::Key_Alt))
	{
		for(int i=0;i<this->points_location_x.size();i++)
			this->is_vector_i_selected.push_back(false);
		for(int i=0;i<this->selected_vertices.size();i++)
			this->is_vector_i_selected[selected_vertices[i]]=true;
		if(this->selected_point_hitted==true||this->which_point_hitted<this->number_of_dots||selected_vertices.size()>0)
			this->parent_dbar->update_parent_mainwindow(selected_point_for_new_graph,selected_vertices,this->ctrl_clicked,is_vector_i_selected);
		this->selected_vertices.clear();
		this->is_vector_i_selected.clear();
	}
 }
/*
void dynamic_bar_widget::setGraphDisplayOptions(bool av, float ct, bool dfv, bool de, bool dl){
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
void dynamic_bar_widget::setupColors_dbar(){
	fprintf(stderr,"dynamicbar setupcolors\n");fflush(stderr);
	this->NoColors_dbar = 21;

	this->brushColor_dbar = (vsColor**) malloc(this->NoColors_dbar * sizeof(vsColor*));
	this->penColor_dbar = (vsColor**) malloc(this->NoColors_dbar * sizeof(vsColor*));
	
	this->brushColor_dbar[0] = new vsColor(255, 255, 255);
	this->brushColor_dbar[1] = new vsColor(204, 204, 255);
	this->brushColor_dbar[2] = new vsColor(204, 204, 26);
	this->brushColor_dbar[3] = new vsColor(161, 13, 89);
	this->brushColor_dbar[4] = new vsColor(108, 170, 255);
	this->brushColor_dbar[5] = new vsColor(148, 255, 148);
	this->brushColor_dbar[6] = new vsColor(244, 255, 98);
	this->brushColor_dbar[7] = new vsColor(255, 98, 190);
	this->brushColor_dbar[8] = new vsColor(156, 130, 28);
	this->brushColor_dbar[9] = new vsColor(50, 205, 50);			//Lime Green
	this->brushColor_dbar[10] = new vsColor(0, 0, 255);			//Blue
	this->brushColor_dbar[11] = new vsColor(65, 105, 225);		//Royal Blue
	this->brushColor_dbar[12] = new vsColor(25, 25, 112);		//Midnight Blue
	this->brushColor_dbar[13] = new vsColor(217, 247, 255);		//Very Light Blue
	this->brushColor_dbar[14] = new vsColor(124, 252, 0);		//Lawn Green
	this->brushColor_dbar[15] = new vsColor(0, 100, 0);			//Dark Green
	this->brushColor_dbar[16] = new vsColor(205, 92, 92);		//Indian Red
	this->brushColor_dbar[17] = new vsColor(210, 105, 30);		//Chocolate
	this->brushColor_dbar[18] = new vsColor(255, 165, 0);		//Orange
	this->brushColor_dbar[19] = new vsColor(176, 48, 96);		//Maroon
	this->brushColor_dbar[20] = new vsColor(153, 50, 204);		//Dark Orchid

	//this->brushColor_dbar[2] = new vsColor(166, 223, 57);
	
	float darken = 0.85f;

	this->penColor_dbar[0] = new vsColor(180, 180, 180);
	for (int i = 1; i < this->NoColors_dbar; i++){
		this->penColor_dbar[i] = new vsColor(int(this->brushColor_dbar[i]->R * darken), int(this->brushColor_dbar[i]->G * darken), int(this->brushColor_dbar[i]->B * darken));
	}
}


void dynamic_bar_widget::ShowbarChoices(const QPoint& pos)
{
	fprintf(stderr,"dynamicbar showbarchoices\n");fflush(stderr);
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

dynamic_bar_widget::~dynamic_bar_widget()
{

}
