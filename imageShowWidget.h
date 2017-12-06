#ifndef IMAGESHOWWIDGET_H
#define IMAGESHOWWIDGET_H

#include <QtGui>
#include <QList>
#include <QPoint>
#include <QPixmap>
#include <QMouseEvent>
#include <fstream>
#include <vector>
#include <QPainter>
#include <QWidget>
#include<QLabel>
#include <math.h>
#include <QMouseEvent>
#include <vector>
using namespace std;

class ImageShowWidget:public QWidget
{
	Q_OBJECT
public:
	ImageShowWidget(QWidget *parent = 0);
	void paintEvent(QPaintEvent *event);
	void moveEvent(QMoveEvent *event);
	void resizeEvent(QResizeEvent *event);
	void wheelEvent(QWheelEvent *event);
	void closeEvent(QCloseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);

	QTransform transForm;

	QImage sourceImage;

	bool selectRotateCenterEnable;
	double rcX,rcY;

	int type;
	double scale;
	bool bindFlag;

	double startPt[2];
	QPoint lastPose;

	double cordPt[6];
	double line[4];
	QPointF pt1;
	QPointF pt2;

	QPointF pt3,pt4;

	QPointF aPt1,aPt2,aPt3,aPt4;

	vector<double> *v1,*v2;

private:

	public slots:
		void setImage(QString imgName);
		void getTargetGeo(QRect geo);
		void setScale(double s);
		void getClose();
		void bindUpdate();
		void setMTransMatrix(QTransform TM);
		void setV(vector<double> *v,int index);
		void setStartPt(double px,double py);
		void setCordPt(double ox,double oy, double xx1, double xy1, double xx2, double xy2);
		void getLinePt(double x1,double y1,double x2, double y2);
		void getXYTranslate(double dx, double dy, int t);
		void getRotate(double angle, int t);

signals:
		void sendCurrentGeo(QRect geo);
		void sendCurrentScale(double scale);
		void sendClose();
		void sendStartPt(double px,double py);
};

#endif