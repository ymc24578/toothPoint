#ifndef PWIDGET_H
#define PWIDGET_H

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
using namespace std;


//class QDragEnterEvent;
//class QDropEvent;
//class QMouseEvent;

struct myHSV
{
	double tH;
	double tS;
	double tI;
};

struct myPt2D
{
	double x;
	double y;
};


class PWidget:public QWidget
{
	Q_OBJECT

	public:
		PWidget(QWidget *parent = 0);
		~PWidget();
		void mousePressEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *);
		void wheelEvent(QWheelEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void moveEvent(QMoveEvent *);
		void resizeEvent(QResizeEvent *);
		//void mouseReleaseEvent(QMouseEvent *);
		double scale,scaleMask;
		void paintEvent(QPaintEvent *event);
		myHSV RGB2HSV(QRgb tc);
		//QPixmap sourceImage;
		QImage sourceImage,image;
		QImage trasImage;
		bool flag;
		vector<double> *points;
		vector<double> *showSPT;
                vector<double> *pt2;
				vector<double> *sAPt,*showAPt2Vec;
				vector<double> *points4Compare;
				vector<double> * points4CompareV[3];
				int currentCompPtType;
				vector<double> *ptTst;
				vector<double> *mask1Pt, *mask2Pt,*tmask;
				vector<QString> *ptName;
				vector<double> *ruler;
				vector<double> *dLine[10],*moveLinePt;
				vector<double> *chongheTpt,*chongheSpt;

				vector<double> *sourceTracingPt,*tracingPt;

				vector<double> *showPtVector[20];
				int showPtType[20];

				int lineCount;

				double lineWidth,pointSize;
				int lineIndex,ptIndex;

		myPt2D mask1[5][2];
		myPt2D mask2[5][2];
		double startPt[2];
		double driftPt[2];
		QPoint lastPose, lastlinePos,lastPtPos;
		double mid[2];
                double px;
                double py;
                bool pDraw;
				bool sPt;
				int ptSize;
				int selfIndex;
				bool fileS;
				bool sComPt;
				bool transImg;
				bool drawMask;
				bool selecrRuler;
				bool drawLine;
				bool movableLine;
				bool moveImageOrMask;
				bool cLen;
				bool sourceTracingCoinFlag;	//重叠原图与Tracing图标志
				bool tracingCoinFlag;	//重叠治疗前后tracing图标志
				int mask1Or2;

				bool isFull;
				double markRuler;

				QMatrix transMatrix;

public slots:
                void setImage(QString *imgName,int tIndex);
				void setImage2(QString *imgName,int tIndex,double scale);
				void setImage3(QImage img);
		void savePts(QString *);
                void drawPt();
				void clearAll();
				void clearLast();
				void selectPtEn(bool res);
				void showSpt(double x,double y);
				void showSampleRuler(vector<double> *sRuler, bool comLen);
				void showAllPt(vector<double> *pt, vector<QString> *ptn);
				void showAllPt2(vector<double> *pt);
				void changePtSize(int s);
				void addbright();
				void decBright();
				void addImageCon();
				void decImageCon();
				void imageHistEqualise();
				void imageSharpen();
				void imagesmooth();
				void imagesobelEdge();
				void imageinverse();
				void restoreImage();
				void setRuler(double r,int tindex);
				void setHVRuler(double hr,double vr,int tindex);
				void setChonghePt(vector<double> *csPt,vector<double> *ctPt);
				void getShowPointSlot(vector<double> *pt, int type);
				void setTransMatrix(QMatrix TM);

				int isOnLine(double x, double y);
				int isOnPoint(double x, double y);

				void getRulerPt(double x1,double y1,double x2,double y2);

				void setBoolenStatus(int status, bool flag);
				void clearAllPt();

signals:
				void PtSelected(double,double);
				void rulerPtSe(double);
				void scaleSet(double);
				void setFullScreen(int ,bool );
				void setSourceTracingPtSignal(vector<double> *,int selfIndex);
				void setTracingCoinPtSignal(vector<double> *,int selfIndex);

};



#endif
