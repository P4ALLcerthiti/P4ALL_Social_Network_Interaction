#ifndef DYNAMIC_MATP_WIDGET_H
#define DYNAMIC_MATP_WIDGET_H

//#include </usr/include/qt4/QtOpenGL/QGLWidget>
#include <QGLWidget>

#include <QBrush>
#include <QFont>
#include <QPen>

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



class matp_point{
	public:
		matp_point(double xvalue,double yvalue,double size,bool selected)
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
class dynamic_matp_widget : public QGLWidget
	//class GLWidget : public QWidget
{
	Q_OBJECT

public:
	//explicit GLWidget(QWidget *parent = NULL, vsGraph *graph = NULL, int drawingfeature = 0);
	dynamic_matp_widget(QWidget *parent,MdiChild* parentmdich,  std::vector<Eigen::MatrixXd> MatP,int dynamic_numberoffeatures,std::vector<std::string> names_of_files=std::vector<std::string>());
	~dynamic_matp_widget();
	int get_HitVertex(){return HitVertex_dmatp;}
	void setPathName(QString name){PathName_dmatp=name;}
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
	
	int getNoDrawingVertices(){return NoDrawingVertices_dmatp;}
	int type_of_labels_displayed;
	int metric_selected;
	bool hasSelectedVertices_dmatp;
	bool show_selected;
	int dynamic_numberoffeatures;
	std::vector<bool> is_vector_i_selected_global;
	public slots:
		void animate();
		void ShowMatpChoices(const QPoint&);
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
		void clearSelected_dmatp();

		void changedDrawnVertices(bool, int);
public slots:
		void unsetSelected_dmatp();

private:
	std::vector<Eigen::MatrixXd> MatP;
	void setupColors_dmatp();
	int NoColors_dmatp;
	vsColor **brushColor_dmatp;
	vsColor **penColor_dmatp;

	bool isVisible_dmatp;
	QString PathName_dmatp;
	void paintVertices_dmatp(QPainter *painter);
	void paintedges_dmatp(QPainter *painter);
	void paintEvent(QPaintEvent *event);
	void paint(QPainter *painter, QPaintEvent *event);
	
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent * event);

	//void setupColors();

	void calculateVertexSize_dmatp();

	void copySelected_dmatp();

	void calculateEdgeMeanSD_dmatp(bool onlyDrawn);
	void setAbstractDrawingVertices_dmatp(bool onlyDrawn);


	QBrush background_dmatp;
	QBrush circleBrush_dmatp;
	QPen circlePen_dmatp;
	QFont textFont_dmatp;
	QPen textPen_dmatp;
	QBrush selectedBrush_dmatp;
	QPen selectedPen_dmatp;

	QTimer *timer_dmatp;

	QPoint lastPos_dmatp;
	QPoint lastClickedPos_dmatp;
	int HitVertex_dmatp;
	bool mousePressed_dmatp;
	QPoint moveD_dmatp;
	float scaleFactor_dmatp;
	QPoint drawingBorder_dmatp;
	bool drawSelectionBox_dmatp;
	QRect selectionBox_dmatp;

	enum layout{Random, Geometric, FR, FromFile};
	layout Layout_dmatp;
	bool PositionsSet_dmatp;

	

	

	

	int VertexOpacity_dmatp;
	int EdgeOpacity_dmatp;
	int EdgeWidth_dmatp;
	int dVertexSize_dmatp;
	int VertexSize_dmatp;
	int ReferenceVertexSize_dmatp;
	int minVertexSize_dmatp;
	int maxVertexSize_dmatp;

	QSize DrawingBox_dmatp;

	float CuttingThreshold_dmatp;
	//float EdgeMean;
	//float EdgeSD;
	int NoDrawingVertices_dmatp;
	QAction *isShownAction_dmatp;
	std::vector<std::vector<matp_point* >> matp_points;
	std::vector<matp_point* > matp_drawing_positions;
	double width_of_last_matp;
	double height_of_last_matp;
	double ratio_width_dmatp;
	double ratio_height_dmatp;
	MdiChild *parent_dmatp;
	QAction *dynamic_numbers_displayed;
	QAction *dynamic_dates_displayed;
	std::vector<std::string> names_of_files;
	bool ctrl_clicked;
	QColor colorInit_vertice;
	QColor colorTarget_vertice;
private:
	//QPointF** V;
	//QPointF** DP;

};

#endif // WIDGET_H
