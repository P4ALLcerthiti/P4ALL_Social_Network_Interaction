#ifndef DYNAMIC_BAR_WIDGET_H
#define DYNAMIC_BAR_WIDGET_H

//#include </usr/include/qt4/QtOpenGL/QGLWidget>
#include <QGLWidget>

#include <QBrush>
#include <QFont>
#include <QPen>

#include<QAction>
#include <math.h>
#include "vsNode.h"
#include "vsFeature.h"
#include "glwidget.h"
#include <Eigen/SparseCore>
class MdiChild;
//Forward Qt declarations
QT_BEGIN_NAMESPACE
class QPaintEvent;
class QWidget;
QT_END_NAMESPACE

//Forward VIS-SENSE declarations

#include<QMenu>

class bar_point{
	public:
		bar_point(double xvalue,double yvalue,double size,bool selected)
		{
			X=xvalue; 
			Y=yvalue;
			size_of_point=size;
			this->selected=selected;
		}
		double X;
		double Y;
		double size_of_point;
		bool selected;
};
class dynamic_bar_widget : public QGLWidget
	//class GLWidget : public QWidget
{
	Q_OBJECT

public:
	//explicit GLWidget(QWidget *parent = NULL, vsGraph *graph = NULL, int drawingfeature = 0);
	dynamic_bar_widget(QWidget *parent,MdiChild* parentmdich,  std::vector<Eigen::MatrixXd> bar,int dynamic_numberoffeatures,std::vector<std::string> names_of_files=std::vector<std::string>());
	~dynamic_bar_widget();
	int get_HitVertex(){return HitVertex_dbar;}
	void setPathName(QString name){PathName_dbar=name;}
	/*void setGraphDisplayOptions(bool, float, bool, bool, bool);
	
	bool abstractView;
	bool displayFeatureValues;
	bool displayEdges;
	bool displayLabels;
	bool displayClusters;
	bool displayAnomalies;
*/
	
	//bool hasClusteringResult;
	//bool hasADResult;

	//bool writePositionsToFile;

	virtual void update_parent(){}
	
	int getNoDrawingVertices(){return NoDrawingVertices_dbar;}
	int type_of_labels_displayed;
	int metric_selected;
	bool hasSelectedVertices_dbar;
	bool show_only_selected;
	std::vector<bool> is_vector_i_selected_global;
	int selected_point;
	bool has_many_selected_points;
	bool dotted_selection;
	int dynamic_numberoffeatures;
	public slots:
		void animate();
		void ShowbarChoices(const QPoint&);
		//void setLayoutView(bool);
		//void setCuttingThreshold(float);

		//void setVertexSize(int);
		//void setVertexTransparency(int);
		//void setFeatureValuesView(bool);
		//void setEdgesView(bool);
		//void setEdgeTransparency(int);
		//void setLabelsView(bool);

		//void setClustersView(bool);
		//void setAnomaliesView(bool);
		
		//void setView(bool);
		//virtual void setDrawnVertices();

signals:
		//void valueChanged();
		void refresh();
		void selectedVertex(int);
		void clearSelected_dbar();

		void changedDrawnVertices(bool, int);
public slots:
		void unsetSelected_dbar();

private:
	std::vector<Eigen::MatrixXd> bar;
	void setupColors_dbar();
	int NoColors_dbar;
	vsColor **brushColor_dbar;
	vsColor **penColor_dbar;

	bool isVisible_dbar;
	QString PathName_dbar;
	void paintline_dbar(QPainter *painter);
	void paintedges_dbar(QPainter *painter);
	void paintEvent(QPaintEvent *event);
	void paint(QPainter *painter, QPaintEvent *event);
	
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);
	//void h(QMouseEvent * event);
	virtual void keyReleaseEvent(QKeyEvent *event);

	//void setupColors();

	void calculateVertexSize_dbar();

	void copySelected_dbar();

	void calculateEdgeMeanSD_dbar(bool onlyDrawn);
	void setAbstractDrawingVertices_dbar(bool onlyDrawn);


	QBrush background_dbar;
	QBrush circleBrush_dbar;
	QPen circlePen_dbar;
	QFont textFont_dbar;
	QPen textPen_dbar;
	QBrush selectedBrush_dbar;
	QPen selectedPen_dbar;

	QTimer *timer_dbar;

	QPoint lastPos_dbar;
	QPoint lastClickedPos_dbar;
	int HitVertex_dbar;
	bool mousePressed_dbar;
	QPoint moveD_dbar;
	float scaleFactor_dbar;
	QPoint drawingBorder_dbar;
	bool drawSelectionBox_dbar;
	QRect selectionBox_dbar;
	QRect pointBox_dbar;

	enum layout{Random, Geometric, FR, FromFile};
	layout Layout_dbar;
	bool PositionsSet_dbar;

	

	

	

	int VertexOpacity_dbar;
	int EdgeOpacity_dbar;
	int EdgeWidth_dbar;
	int dVertexSize_dbar;
	int VertexSize_dbar;
	int ReferenceVertexSize_dbar;
	int minVertexSize_dbar;
	int maxVertexSize_dbar;

	QSize DrawingBox_dbar;

	float CuttingThreshold_dbar;
	//float EdgeMean;
	//float EdgeSD;
	int NoDrawingVertices_dbar;
	QAction *isShownAction_dbar;
	std::vector<std::vector<bar_point* >> bar_points;
	std::vector<bar_point* > bar_drawing_positions;
	double width_of_last_bar;
	double height_of_last_bar;
	double ratio_width_dbar;
	double ratio_height_dbar;
	MdiChild *parent_dbar;
	QAction *dynamic_numbers_displayed;
	QAction *dynamic_dates_displayed;
	std::vector<std::string> names_of_files;
	bool ctrl_clicked;
	QColor colorInit_vertice;
	QColor colorTarget_vertice;
	double line_distances;
	int number_of_dots;
	std::vector<double> points_location_x;
	std::vector<int> selected_vertices;
	std::vector<bool> is_vector_i_selected;
	double bar_position_y;
	bool selected_point_hitted;
	int which_point_hitted;
	double x1;
	double x2;
	double moving_point_location_x;
	bool has_moved;
	double point_h;
	double point_width;
	int number_of_points;
	double fontsize;
	double h_point;
	std::vector<double> points_location_x_after_scale_moved;
	int height_upgrade_for_red_points;
	int height_upgrade;
private:
	//QPointF** V;
	//QPointF** DP;

};

#endif // WIDGET_H
