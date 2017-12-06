#include "imageShowWidget.h"
#include<QMessageBox>
#include <iostream>
using namespace std;

ImageShowWidget::ImageShowWidget(QWidget *parent):QWidget(parent)
{
	
	//transMatrix.setMatrix(1,0,0,1,0,0);
	
	transForm.setMatrix(1,0,0,0,1,0,0,0,1);
	scale = 1;
	selectRotateCenterEnable = false;
	type=0;
	bindFlag = true;
	v1 = new vector<double>;
	v2 = new vector<double>;
	startPt[0]=startPt[1]=0;
	setWindowTitle(tr("÷ÿµ˛Õº∆¨œ‘ æ"));

	for (int i=0;i<4;i++)
	{
		line[i] = 0;
	}
	for (int i=0;i<6;i++)
	{
		cordPt[i] = 0;
	}

	resize(800, 600);
	
}

void ImageShowWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.save();

	painter.translate(startPt[0],startPt[1]);
	painter.scale(scale,scale);
	painter.setTransform(transForm,true);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setRenderHint(QPainter::Antialiasing,true);

	painter.drawImage(0,0,sourceImage);


	int pointSize = 40;

	/*painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
	painter.drawEllipse(pt3,20,20);
	painter.setPen(QPen(Qt::red,Qt::SolidPattern));
	painter.drawEllipse(pt4,20,20);*/

	if (rcX!=0 && rcY!=0)
	{
		painter.setPen(QPen(Qt::red,Qt::SolidPattern));
		painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
		painter.drawEllipse(rcX-pointSize/2,rcY-pointSize/2,pointSize,pointSize);
	}

	if (pt1.x()!=0 && pt2.x()!=0)
	{
		painter.setPen(QPen(Qt::red,Qt::SolidPattern));
		painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
		painter.drawEllipse(pt1,pointSize,pointSize);

		painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
		painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
		painter.drawEllipse(pt2,pointSize,pointSize);
	}

	if (aPt1.x()!=0 && aPt2.x()!=0 && aPt3.x()!=0 && aPt4.x()!=0)
	{
		/*double a = aPt2.y() - aPt1.y();
		double b = -aPt2.x() + aPt1.x();
		double c = b*aPt1.y() + a*aPt1.x();
		double d = aPt4.y() - aPt3.y();
		double e = -aPt4.x() + aPt3.x();
		double f = e*aPt3.y() + d*aPt3.x();

		double tx = (b*f - c*e)/(b*d-a*e);
		double ty = (c*d - a*f)/(b*d-a*e);

		QPointF aPt0;
		aPt0.setX(tx);
		aPt0.setY(ty);*/

		painter.setPen(QPen(Qt::blue,2,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));
		painter.drawLine(aPt1,aPt2);
		double v1x = aPt2.x() - aPt1.x();
		double v1y = aPt2.y() - aPt1.y();
		double l = sqrt(v1x*v1x+v1y*v1y);
		double angle = acos(v1x/l);
		angle = 180*angle/3.1415926;
		if (v1y<0)
		{
			angle = -angle;
		}
		painter.save();
		painter.translate(aPt1.x(),aPt1.y());
		painter.rotate(angle);
		double aleny = 4;
		double alenx = aleny*sqrt(3.0);
		painter.drawLine(l-alenx,aleny,l,0);
		painter.drawLine(l-alenx,-aleny,l,0);
		painter.restore();
/*
		painter.setPen(QPen(Qt::red,2,Qt::DotLine,Qt::RoundCap,Qt::MiterJoin));
		painter.drawLine(aPt0,aPt1);*/

		painter.setPen(QPen(Qt::red,2,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));
		painter.drawLine(aPt3,aPt4);
		v1x = aPt4.x() - aPt3.x();
		v1y = aPt4.y() - aPt3.y();
		l = sqrt(v1x*v1x+v1y*v1y);
		angle = acos(v1x/l);
		angle = 180*angle/3.1415926;
		if (v1y<0)
		{
			angle = -angle;
		}
		painter.save();
		painter.translate(aPt3.x(),aPt3.y());
		painter.rotate(angle);
		aleny = 4;
		alenx = aleny*sqrt(3.0);
		painter.drawLine(l-alenx,aleny,l,0);
		painter.drawLine(l-alenx,-aleny,l,0);
		painter.restore();

		painter.setPen(QPen(Qt::red,2,Qt::DotLine,Qt::RoundCap,Qt::MiterJoin));
		QPointF aPt5(aPt1.x()+aPt4.x()-aPt3.x(), aPt1.y()+aPt4.y()-aPt3.y());
		painter.drawLine(aPt1,aPt5);
		painter.save();
		v1x = aPt5.x() - aPt1.x();
		v1y = aPt5.y() - aPt1.y();
		l = sqrt(v1x*v1x+v1y*v1y);
		angle = acos(v1x/l);
		angle = 180*angle/3.1415926;
		if (v1y<0)
		{
			angle = -angle;
		}
		painter.translate(aPt1.x(),aPt1.y());
		painter.rotate(angle);
		aleny = 4;
		alenx = aleny*sqrt(3.0);
		painter.setPen(QPen(Qt::red,2,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));
		painter.drawLine(l-alenx,aleny,l,0);
		painter.drawLine(l-alenx,-aleny,l,0);
		painter.restore();
	}

	for (int i=0;i<v1->size()/2;i++)
	{
		painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
		QPointF pt;
		pt.setX(v1->at(2*i));
		pt.setY(v1->at(2*i+1));
		//painter.drawEllipse(pt,ptSize,ptSize);

		QPointF ppt1,ppt2,ppt3,ppt4;
		ppt1.setX(pt.x()-pointSize);
		ppt1.setY(pt.y());

		ppt2.setX(pt.x()+pointSize);
		ppt2.setY(pt.y());

		ppt3.setX(pt.x());
		ppt3.setY(pt.y()-pointSize);

		ppt4.setX(pt.x());
		ppt4.setY(pt.y()+pointSize);

		painter.drawLine(ppt1,ppt2);
		painter.drawLine(ppt3,ppt4);
	}

	for (int i=0;i<v2->size()/2;i++)
	{
		painter.setPen(QPen(Qt::green,Qt::SolidPattern));
		QPointF pt;
		pt.setX(v2->at(2*i));
		pt.setY(v2->at(2*i+1));
		//painter.drawEllipse(pt,ptSize,ptSize);

		QPointF ppt1,ppt2,ppt3,ppt4;
		ppt1.setX(pt.x()-pointSize);
		ppt1.setY(pt.y());

		ppt2.setX(pt.x()+pointSize);
		ppt2.setY(pt.y());

		ppt3.setX(pt.x());
		ppt3.setY(pt.y()-pointSize);

		ppt4.setX(pt.x());
		ppt4.setY(pt.y()+pointSize);

		painter.drawLine(ppt1,ppt2);
		painter.drawLine(ppt3,ppt4);
	}

	if (cordPt[0]!=0&&cordPt[1]!=0&&cordPt[2]!=0&&cordPt[3]!=0&&cordPt[4]!=0&&cordPt[5]!=0)
	{
		QPointF ppt1,ppt2,ppt3;
		ppt1.setX(cordPt[0]);
		ppt1.setY(cordPt[1]);
		ppt2.setX(cordPt[0] + cordPt[4] - cordPt[2]);
		ppt2.setY(cordPt[1] + cordPt[5] - cordPt[3]);
		painter.setPen(QPen(Qt::red,Qt::SolidPattern));
		painter.drawLine(ppt1,ppt2);
		/*double yy = cordPt[3]-100;
		double yx = cordPt[2] + 100*(cordPt[5] - cordPt[3])/(cordPt[4] - cordPt[2]);
		*/
		//cerr<<cordPt[0] + cordPt[5] - cordPt[3]<<" "<<cordPt[1] - cordPt[4] + cordPt[2]<<endl;
		ppt3.setX(cordPt[0] + cordPt[5] - cordPt[3]);
		ppt3.setY(cordPt[1] - cordPt[4] + cordPt[2]);
		painter.drawLine(ppt1,ppt3);
	}

	for (int i=0;i<4;i++)
	{
		QPointF pt1,pt2;
		pt1.setX(line[0]);
		pt1.setY(line[1]);
		pt2.setX(line[2]);
		pt2.setY(line[3]);
		if (type==1)
		{
			painter.setPen(QPen(Qt::red,Qt::SolidPattern));
		}
		else
			painter.setPen(QPen(Qt::blue,Qt::SolidPattern));

		painter.drawLine(pt1,pt2);
	}
	//painter.drawImage(0,0,sourceImage);	
	painter.restore();
}

void ImageShowWidget::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons()& Qt::MidButton)
	{
		lastPose=e->pos();
	}
	else if (e->buttons() & Qt::LeftButton)
	{
		if (selectRotateCenterEnable)
		{
			cerr<<"select center pt";
			double tx = e->pos().x() - startPt[0];
			double ty = e->pos().y() - startPt[1];
			rcX = tx/scale;
			rcY = ty/scale;
			QTransform inverTrans = transForm.inverted();
			double rrcx = rcX * inverTrans.m11() + rcY*inverTrans.m21() + inverTrans.dx();
			double rrcy = rcX * inverTrans.m12() + rcY*inverTrans.m22() + inverTrans.dy();
			rcX = rrcx;
			rcY = rrcy;
			//cerr<<"type"<<type<<" "<<tx<<" "<<ty<<endl;
		}
	}
	update();
}

void ImageShowWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (e->buttons()& Qt::MidButton)
	{
		startPt[0]+=e->pos().x()-lastPose.x();
		startPt[1]+=e->pos().y()-lastPose.y();
		lastPose=e->pos();
		emit sendStartPt(startPt[0],startPt[1]);
	}
	update();
}

void ImageShowWidget::moveEvent(QMoveEvent *event)
{
	QRect geo = this->geometry();
	if (bindFlag)
	{
		emit sendCurrentGeo(geo);
	}
}

void ImageShowWidget::resizeEvent(QResizeEvent *event)
{
	QRect geo = this->geometry();
	if (bindFlag)
	{
		emit sendCurrentGeo(geo);
	}
}

void ImageShowWidget::wheelEvent(QWheelEvent *event)
{
	scale*=(1+event->delta()*0.001f) ;
	emit sendCurrentScale(scale);
	update();
}

void ImageShowWidget::closeEvent(QCloseEvent *e)
{
	emit sendClose();
}

void ImageShowWidget::setImage(QString imgName)
{
	sourceImage.load(imgName);
	update();
}

void ImageShowWidget::getTargetGeo(QRect geo)
{
	this->setGeometry(geo);
}

void ImageShowWidget::setScale(double s)
{
	scale = s;
	update();
}

void ImageShowWidget::bindUpdate()
{
	QRect geo = this->geometry();
	if (bindFlag)
	{
		emit sendCurrentGeo(geo);
	}
}

void ImageShowWidget::getClose()
{
	this->close();
}

void ImageShowWidget::setMTransMatrix(QTransform TM)
{
	transForm = TM;
	/*cout<<"-----------------"<<endl;
	cout<<transMatrix.m11()<<" "<<transMatrix.m12()<<endl;
	cout<<transMatrix.m21()<<" "<<transMatrix.m22()<<endl<<endl;
	double t = transMatrix.m11()*transMatrix.m12() + transMatrix.m21()*transMatrix.m22();
	cout<<"t = "<<t<<endl;
	cout<<"-----------------"<<endl;*/
}

void ImageShowWidget::setV(vector<double> *v,int index)
{
	if (index==1)
	{
		v1 = v;
	}
	else
		v2 = v;
	update();
}

void ImageShowWidget::setStartPt(double px,double py)
{
	startPt[0] = px;
	startPt[1] = py;
	update();
}

void ImageShowWidget::setCordPt(double ox,double oy, double xx1, double xy1, double xx2, double xy2)
{
	cordPt[0] = ox;
	cordPt[1] = oy;		// cord system center

	cordPt[2] = xx1;
	cordPt[3] = xy1;	// cord system x start point

	cordPt[4] = xx2;
	cordPt[5] = xy2;	// cord system x end point
	update();
}

void ImageShowWidget::getLinePt(double x1,double y1,double x2, double y2)
{
	cerr<<"got"<<endl;
}

void ImageShowWidget::getXYTranslate(double dx, double dy,int t)
{
	if (t==type)
	{
		transForm.translate(dx,dy);
		update();
	}
	
}

void ImageShowWidget::getRotate(double angle, int t)
{
	if (t==type)
	{
		transForm.translate(rcX,rcY);
		transForm.rotate(angle);
		transForm.translate(-rcX,-rcY);
		update();
	}
}