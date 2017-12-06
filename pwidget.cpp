#include "pwidget.h"
#include<QMessageBox>
#include <iostream>
#include <QRgb>
using namespace std;

PWidget::PWidget(QWidget *parent ):QWidget(parent)
{

	setCursor(Qt::CrossCursor);		//设置鼠标维十字

	points=new vector<double>;
        pt2=new vector<double>;
		showSPT = new vector<double>;
		sAPt = new vector<double>;
		showAPt2Vec = new vector<double>;
		chongheTpt = new vector<double>;
		chongheSpt = new vector<double>;
		points4Compare = new vector<double>;
		for (int i=0;i<3;i++)
		{
			points4CompareV[i] = new vector<double>;
		}
		currentCompPtType = -1;
		ptTst = new vector<double>;
		ruler = new vector<double>;
		mask1Pt = new vector<double>;
		mask2Pt = new vector<double>;
		tmask = new vector<double>;
		moveLinePt = new vector<double>;
		sourceTracingPt = new vector<double>;
		tracingPt = new vector<double>;
		isFull = false;
		for (int i=0;i<10;i++)
		{
			dLine[i] = new vector<double>;
		}

		for (int i=0;i<20;i++)
		{
			showPtVector[i] = new vector<double>;
			showPtType[i] = 0;
		}

		lineWidth = 6;
		pointSize = 15;
		//lineLenth = 200;
		lineIndex = -1;

		lineCount = 0;
		drawLine = false;
		movableLine = false;
		sourceTracingCoinFlag = false;
		tracingCoinFlag = false;
		mask1Or2 = 0;
		ptName = new vector<QString>;
		selecrRuler = false;
		moveImageOrMask = false;
	setMinimumSize(400, 400);
	setMaximumSize(10000, 10000);
	scale=scaleMask=1;
	mid[0]=mid[1]=startPt[0]=startPt[1]=driftPt[0]=driftPt[1]=0;
        pDraw=false;
		sPt=false;
		ptSize=10;
		flag=false;
		fileS = false;
		sComPt = false;
		drawMask = false;
		transImg = false;
		mask1[0][0].x = 0; mask1[0][0].y = 0; 
		mask1[0][1].x = 180; mask1[0][1].y = 0;
		mask1[1][0].x = 0; mask1[1][0].y = 37; 
		mask1[1][1].x = 180; mask1[1][1].y = 32;
		mask1[2][0].x = 0; mask1[2][0].y = 74; 
		mask1[2][1].x = 180; mask1[2][1].y = 69;
		mask1[3][0].x = 0; mask1[3][0].y = 112; 
		mask1[3][1].x = 180; mask1[3][1].y = 107;
		mask1[4][0].x = 0; mask1[4][0].y = 150; 
		mask1[4][1].x = 180; mask1[4][1].y = 151;

		mask2[0][0].x = 0; mask2[0][0].y = 0; 
		mask2[0][1].x = 180; mask2[0][1].y = 0;
		mask2[1][0].x = 0; mask2[1][0].y = 0; 
		mask2[1][1].x = 0; mask2[1][1].y = 0;
		mask2[2][0].x = 0; mask2[2][0].y = 0; 
		mask2[2][1].x = 0; mask2[2][1].y = 0;
		mask2[3][0].x = 0; mask2[3][0].y = 0; 
		mask2[3][1].x = 0; mask2[3][1].y = 0;
		mask2[4][0].x = 0; mask2[4][0].y = 150; 
		mask2[4][1].x = 180; mask2[4][1].y = 151;

		markRuler = 0;
		cLen = true;
		transMatrix.setMatrix(1,0,0,1,0,0);
}

PWidget::~PWidget()
{
	delete points;
        delete pt2;
		delete sAPt;
		delete showSPT;
		delete showAPt2Vec;
		delete chongheSpt;
		delete chongheTpt;
		for (int i=0;i<20;i++)
		{
			delete showPtVector[i];
		}
}

void PWidget::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons()& Qt::LeftButton)
	{
		double cx=e->pos().x()-startPt[0];
		double cy=e->pos().y()-startPt[1];
		if (cx<=sourceImage.width()*scale&&cx>=0&&cy>=0&&cy<=sourceImage.height()*scale)
		{
			double tx = cx/(double)sourceImage.width()/scale;
			double ty = cy/(double)sourceImage.height()/scale;
			if (sourceTracingCoinFlag)
			{
				if (sourceTracingPt->size()==8)
				{
					return;
				}
				double ttx = tx*sourceImage.width();
				double tty = ty*sourceImage.height();
				sourceTracingPt->push_back(ttx);
				sourceTracingPt->push_back(tty);
				if (sourceTracingPt->size()==8)
				{
					emit setSourceTracingPtSignal(sourceTracingPt,selfIndex);
				}
			}
			else if (tracingCoinFlag)
			{
				if (tracingPt->size()==4)
				{
					return;
				}
				double ttx = tx*sourceImage.width();
				double tty = ty*sourceImage.height();
				tracingPt->push_back(ttx);
				tracingPt->push_back(tty);
				//cerr<<"tracing pts="<<tracingPt->size()<<endl;
				if (tracingPt->size()==4)
				{
					emit setTracingCoinPtSignal(tracingPt,selfIndex);
				}
			}

			if (sPt)
			{
				points->push_back(tx);
				points->push_back(ty);
				double ttx = tx*sourceImage.width();
				double tty = ty*sourceImage.height();
				emit PtSelected(ttx,tty);
			}
			else if (sComPt)
			{
				tx = tx*sourceImage.width();
				ty = ty*sourceImage.height();
				/*points4Compare->push_back(tx);
				points4Compare->push_back(ty);*/
				//cerr<<"current "<<currentCompPtType<<endl;
				points4CompareV[currentCompPtType]->push_back(tx);
				points4CompareV[currentCompPtType]->push_back(ty);
				/*for (int i=0;i<3;i++)
				{
					cerr<<"pt size "<<i<<" = "<<points4CompareV[i]->size()/2<<endl;
				}*/
			}
			else if (selecrRuler)
			{
				cLen = selecrRuler;
				if (ruler->size()==0)
				{
					ruler->push_back(tx);
					ruler->push_back(ty);
				}
				else
				{
					ruler->clear();
					ruler->push_back(tx);
					ruler->push_back(ty);
				}
			}
			else if (drawLine)
			{
				if (dLine[lineCount-1]->size()==4)
				{
					dLine[lineCount-1]->clear();
					
				}
				dLine[lineCount-1]->push_back(tx);
				dLine[lineCount-1]->push_back(ty);
			}
			else if (!movableLine)
			{
				tx = tx*sourceImage.width();
				ty = ty*sourceImage.height();
				lineIndex = isOnLine(tx,ty);
				ptIndex = isOnPoint(tx,ty);
				
				if (lineIndex!=-1)
				{
					//cerr<<lineIndex<<endl;
					lastlinePos = e->pos();
				}
				else if (ptIndex!=-1)
				{
					//cerr<<ptIndex<<endl;
					lastPtPos = e->pos();
				}
				/*else
				{
					
					moveLinePt->push_back(tx);
					moveLinePt->push_back(ty);

					if (moveLinePt->size()%4==0)
					{
						int s = moveLinePt->size();
						double x1 = moveLinePt->at(s-4);
						double y1 = moveLinePt->at(s-3);
						double x2 = x1+ty-y1;
						double y2 = y1+x1-tx;
						moveLinePt->push_back(x1);
						moveLinePt->push_back(y1);
						moveLinePt->push_back(x2);
						moveLinePt->push_back(y2);
					}
				}*/
			}
		}	
	}
	else if (e->buttons()& Qt::RightButton)
	{
            if(pDraw&&(!pt2->empty ()))
            {
                pt2->pop_back();
                if(!pt2->empty ())
                    pt2->pop_back();
            }
			else if (sourceTracingCoinFlag && sourceTracingPt->size()!=0)
			{
				sourceTracingPt->pop_back();
				sourceTracingPt->pop_back();
			}
			else if (tracingCoinFlag && tracingPt->size()!=0)
			{
				tracingPt->pop_back();
				tracingPt->pop_back();
			}
            else
            {
                if(!points->empty ())
                    points->pop_back();
                if(!points->empty ())
                    points->pop_back();
				if (!points4Compare->empty())
				{
					points4Compare->pop_back();
				}
				if (!points4Compare->empty())
				{
					points4Compare->pop_back();
				}
				if (!points4CompareV[currentCompPtType]->empty())
				{
					points4CompareV[currentCompPtType]->pop_back();
				}
            }
	}
	else if (e->buttons()& Qt::MidButton)
	{
		lastPose=e->pos();//points->clear();
	}
	update();
}

void PWidget::wheelEvent(QWheelEvent *e)
{
	if (!moveImageOrMask)
	{
		scale*=(1+e->delta()*0.001f) ;
	}
	else
		scaleMask*=(1+e->delta()*0.001f) ;
	update();
}

void PWidget::moveEvent(QMoveEvent *e)
{
	//QRect rect = this->geometry();
	//cout<<"move   : "<<rect.x()<<" "<<rect.y()<<" "<<rect.width()<<" "<<rect.height()<<endl;
}

void PWidget::resizeEvent(QResizeEvent *e)
{
	//QRect rect = this->geometry();
	//cout<<"resize : "<<rect.x()<<" "<<rect.y()<<" "<<rect.width()<<" "<<rect.height()<<endl;
}


void PWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (e->buttons()& Qt::LeftButton)
	{

		double cx=e->pos().x()-startPt[0];
		double cy=e->pos().y()-startPt[1];
		if (cx<=sourceImage.width()*scale&&cx>=0&&cy>=0&&cy<=sourceImage.height()*scale &&!sPt &&!sComPt)
		{
			double tx = cx/(double)sourceImage.width()/scale;
			double ty = cy/(double)sourceImage.height()/scale;
			// <<dLine[lineCount-1]->size()<<endl;
			if (selecrRuler)
			{
				if (ruler->size()==2)
				{
					ruler->push_back(tx);
					ruler->push_back(ty);
				}
				else if (ruler->size()==4)
				{
					ruler->pop_back();
					ruler->pop_back();
					ruler->push_back(tx);
					ruler->push_back(ty);
				}
			}
			
			else if (drawLine)
			{
				//cout<<tx<<" "<<ty<<endl;
				if (dLine[lineCount-1]->size()==2)
				{
					dLine[lineCount-1]->push_back(tx);
					dLine[lineCount-1]->push_back(ty);
				}
				else if (dLine[lineCount-1]->size()==4)
				{
					dLine[lineCount-1]->pop_back();
					dLine[lineCount-1]->pop_back();
					dLine[lineCount-1]->push_back(tx);
					dLine[lineCount-1]->push_back(ty);
				}
				
			}
			else if (!movableLine)
			{
				if (lineIndex!=-1)
				{
					double x = (e->pos().x() - lastlinePos.x())/scale;
					double y = (e->pos().y() - lastlinePos.y())/scale;
					moveLinePt->at(lineIndex*4+0) += x;
					moveLinePt->at(lineIndex*4+1) += y;
					moveLinePt->at(lineIndex*4+2) += x;
					moveLinePt->at(lineIndex*4+3) += y;
					/*if (lineIndex%2==1)
					{
						moveLinePt->at((lineIndex-1)*4+0) += x;
						moveLinePt->at((lineIndex-1)*4+1) += y;
						moveLinePt->at((lineIndex-1)*4+2) += x;
						moveLinePt->at((lineIndex-1)*4+3) += y;
					}
					else
					{
						moveLinePt->at((lineIndex+1)*4+0) += x;
						moveLinePt->at((lineIndex+1)*4+1) += y;
						moveLinePt->at((lineIndex+1)*4+2) += x;
						moveLinePt->at((lineIndex+1)*4+3) += y;
					}*/
					lastlinePos=e->pos();
				}
				else if (ptIndex != -1)
				{
					cerr<<"ptindex"<<endl;
					double x = (e->pos().x() - lastPtPos.x())/scale;
					double y = (e->pos().y() - lastPtPos.y())/scale;
					
					double x1,y1;
					/*if (ptIndex%2==1)
					{*/
						if (ptIndex%2==1)
						{
							x1 = moveLinePt->at((ptIndex-1)*2+0);
							y1 = moveLinePt->at((ptIndex-1)*2+1);
						}
						else
						{
							x1 = moveLinePt->at((ptIndex+1)*2+0);
							y1 = moveLinePt->at((ptIndex+1)*2+1);
						}

						double x2 = moveLinePt->at(ptIndex*2+0);
						double y2 = moveLinePt->at(ptIndex*2+1);
					
						double tx = moveLinePt->at(ptIndex*2+0) + x;
						double ty = moveLinePt->at(ptIndex*2+1) + y;

						double vx1 = tx-x1;
						double vy1 = ty-y1;

						double vx2 = x2-x1;
						double vy2 = y2-y1;

						double L = (vx1*vx2 + vy1*vy2)/sqrt( vx2*vx2 + vy2*vy2 );
						double angle = vx2/sqrt(vx2*vx2 + vy2*vy2);
						/*if (y2<y1)
						{
							angle = -angle;
						}*/
						int p1,p2;
						p1=p2=1;
						if (x2<x1)
						{
							p1=-1;
						}
						if (y2<y1)
						{
							p2=-1;
						}

						tx = abs(L*angle);
						ty = L*sqrt(1-angle*angle);

						moveLinePt->at(ptIndex*2+0) = x1 + p1*tx;
						moveLinePt->at(ptIndex*2+1) = y1 + p2*ty;
					//}
					//else
					//{
					//	/*double x = (e->pos().x() - lastPtPos.x())/scale;
					//	double y = (e->pos().y() - lastPtPos.y())/scale;
					//	for (int j=0;j<4;j++)
					//	{
					//		moveLinePt->at((ptIndex+j)*2+0) += x;
					//		moveLinePt->at((ptIndex+j)*2+1) += y;
					//	}*/
					//}
					lastPtPos=e->pos();
				}
			}
		}
		
	}
	else if (e->buttons()& Qt::MidButton)
	{
		if (!moveImageOrMask)
		{
			startPt[0]+=e->pos().x()-lastPose.x();
			startPt[1]+=e->pos().y()-lastPose.y();
		}
		else
		{
			driftPt[0]+=e->pos().x()-lastPose.x();
			driftPt[1]+=e->pos().y()-lastPose.y();
		}
		lastPose=e->pos();
	}
	else 	if (e->buttons()& Qt::RightButton)
	{
                if(pDraw&&(!pt2->empty ()))
                    pt2->clear ();
				else if (sourceTracingCoinFlag)
				{
					sourceTracingPt->clear();
				}
				else if (tracingCoinFlag)
				{
					tracingPt->clear();
				}
                else
				{
					points->clear();
					showSPT->clear();
					sAPt->clear();
					showAPt2Vec->clear();
					chongheTpt->clear();
					chongheSpt->clear();
					points4Compare->clear();
					points4CompareV[currentCompPtType]->clear();
					ptTst->clear();
					moveLinePt->clear();
					for (int j=0;j<lineCount;j++)
					{
						dLine[j]->clear();
					}
					for (int j=0;j<20;j++)
					{
						showPtVector[j]->clear();
						showPtType[j]=0;
					}
					lineCount=0;
				}

	}
	update();
}

void PWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (e->buttons()& Qt::RightButton)
	{
		isFull = !isFull;
		emit setFullScreen(selfIndex,isFull);
	}
}

 void PWidget::paintEvent(QPaintEvent *event)
 { 
	

		 QPainter painter(this);
		 painter.save();

		 painter.setCompositionMode(QPainter::CompositionMode_Source);
		 painter.translate(startPt[0],startPt[1]);
		 painter.scale(scale,scale);
	
		 painter.drawImage(0,0,sourceImage);	

		if (!sComPt)
		{
			for (int i=0;i<points->size()/2;i++)
			{
				painter.setPen(QPen(Qt::red,Qt::SolidPattern));
				QPointF pt;
				pt.setX(points->at(2*i)*sourceImage.width());
				pt.setY(points->at(2*i+1)*sourceImage.height());
				//painter.drawEllipse(pt,ptSize,ptSize);
				QPointF pt1,pt2,pt3,pt4;
				pt1.setX(pt.x()-ptSize);
				pt1.setY(pt.y());

				pt2.setX(pt.x()+ptSize);
				pt2.setY(pt.y());

				pt3.setX(pt.x());
				pt3.setY(pt.y()-ptSize);

				pt4.setX(pt.x());
				pt4.setY(pt.y()+ptSize);

				painter.drawLine(pt1,pt2);
				painter.drawLine(pt3,pt4);

				QString num = QString::number(i+1);
				QFont f( "times", 30, QFont::Bold );
				painter.setFont( f );
				//painter.drawText(pt.x()+10,pt.y()+10,num);
			}
		}
		else
		{
			for (int jj=0;jj<3;jj++)
			{
				for (int i=0;i<points4CompareV[jj]->size()/2;i++)
				{
					/*if (selfIndex == 0)
					{
						painter.setPen(QPen(Qt::red,Qt::SolidPattern));
					}
					else 
						painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
					*/
					switch (jj)
					{
					case 0: painter.setPen(QPen(Qt::red,Qt::SolidPattern));
						break;
					case 1:
						painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
						break;
					case 2:
						painter.setPen(QPen(Qt::green,Qt::SolidPattern));
						break;
					}
					QPointF pt;
					pt.setX(points4CompareV[jj]->at(2*i));//*sourceImage.width());
					pt.setY(points4CompareV[jj]->at(2*i+1));//*sourceImage.height());
					//painter.drawEllipse(pt,ptSize,ptSize);

					QPointF pt1,pt2,pt3,pt4;
					pt1.setX(pt.x()-ptSize);
					pt1.setY(pt.y());

					pt2.setX(pt.x()+ptSize);
					pt2.setY(pt.y());

					pt3.setX(pt.x());
					pt3.setY(pt.y()-ptSize);

					pt4.setX(pt.x());
					pt4.setY(pt.y()+ptSize);

					painter.drawLine(pt1,pt2);
					painter.drawLine(pt3,pt4);

					QString num = QString::number(i+1);
					QFont f( "times", 30, QFont::Bold );
					painter.setFont( f );
					//painter.drawText(pt.x()+10,pt.y()+10,num);
				}
			}
		}

		if (tracingCoinFlag && tracingPt->size()!=0)
		{
			for (int i=0;i<tracingPt->size()/2;i++)
			{
				painter.setPen(QPen(Qt::red,Qt::SolidPattern));
				QPointF pt;
				pt.setX(tracingPt->at(2*i));
				pt.setY(tracingPt->at(2*i+1));
				//painter.drawEllipse(pt,ptSize,ptSize);
				QPointF pt1,pt2,pt3,pt4;
				pt1.setX(pt.x()-ptSize);
				pt1.setY(pt.y());

				pt2.setX(pt.x()+ptSize);
				pt2.setY(pt.y());

				pt3.setX(pt.x());
				pt3.setY(pt.y()-ptSize);

				pt4.setX(pt.x());
				pt4.setY(pt.y()+ptSize);

				painter.drawLine(pt1,pt2);
				painter.drawLine(pt3,pt4);

				QString num = QString::number(i+1);
				QFont f( "times", 30, QFont::Bold );
				painter.setFont( f );
				//painter.drawText(pt.x()+10,pt.y()+10,num);
			}
		}

		if (sourceTracingCoinFlag && sourceTracingPt->size()!=0)
		{
			for (int i=0;i<sourceTracingPt->size()/2;i++)
			{
				painter.setPen(QPen(Qt::red,Qt::SolidPattern));
				QPointF pt;
				pt.setX(sourceTracingPt->at(2*i));
				pt.setY(sourceTracingPt->at(2*i+1));
				//painter.drawEllipse(pt,ptSize,ptSize);
				QPointF pt1,pt2,pt3,pt4;
				pt1.setX(pt.x()-ptSize);
				pt1.setY(pt.y());

				pt2.setX(pt.x()+ptSize);
				pt2.setY(pt.y());

				pt3.setX(pt.x());
				pt3.setY(pt.y()-ptSize);

				pt4.setX(pt.x());
				pt4.setY(pt.y()+ptSize);

				painter.drawLine(pt1,pt2);
				painter.drawLine(pt3,pt4);

				QString num = QString::number(i+1);
				QFont f( "times", 30, QFont::Bold );
				painter.setFont( f );
				//painter.drawText(pt.x()+10,pt.y()+10,num);
			}
		}
	 
	 if (showSPT->size()==2)
	 {
		painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
		QPointF pt;
		pt.setX(showSPT->at(0)*sourceImage.width());
		pt.setY(showSPT->at(1)*sourceImage.height());
		//painter.drawEllipse(pt,ptSize,ptSize);
		painter.setPen(QPen(Qt::green,Qt::SolidPattern));
		QPointF pt1,pt2,pt3,pt4;
		pt1.setX(pt.x()-ptSize);
		pt1.setY(pt.y());

		pt2.setX(pt.x()+ptSize);
		pt2.setY(pt.y());

		pt3.setX(pt.x());
		pt3.setY(pt.y()-ptSize);

		pt4.setX(pt.x());
		pt4.setY(pt.y()+ptSize);

		painter.drawLine(pt1,pt2);
		painter.drawLine(pt3,pt4);
	 }
	 if (showAPt2Vec->size()!=0)
	 {
		 for (int i=0;i<showAPt2Vec->size()/2;i++)
		 {
			 painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
			 QPointF pt;
			 pt.setX(showAPt2Vec->at(i*2)*sourceImage.width());
			 pt.setY(showAPt2Vec->at(i*2+1)*sourceImage.height());
			 //cout<<pt.x()<<" "<<pt.y()<<endl;
			 //painter.drawEllipse(pt,ptSize,ptSize);
			 painter.setPen(QPen(Qt::red,Qt::SolidPattern));
			 QPointF pt1,pt2,pt3,pt4;
			 pt1.setX(pt.x()-ptSize);
			 pt1.setY(pt.y());

			 pt2.setX(pt.x()+ptSize);
			 pt2.setY(pt.y());

			 pt3.setX(pt.x());
			 pt3.setY(pt.y()-ptSize);

			 pt4.setX(pt.x());
			 pt4.setY(pt.y()+ptSize);

			 painter.drawLine(pt1,pt2);
			 painter.drawLine(pt3,pt4);
		 }
	 }
	 if (chongheSpt->size()!=0)
	 {
		 for (int i=0;i<chongheSpt->size()/2;i++)
		 {
			 painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
			 QPointF pt;
			 pt.setX(chongheSpt->at(i*2));//*sourceImage.width());
			 pt.setY(chongheSpt->at(i*2+1));//*sourceImage.height());
			 //cout<<pt.x()<<" "<<pt.y()<<endl;
			 //painter.drawEllipse(pt,ptSize,ptSize);
			 painter.setPen(QPen(Qt::red,Qt::SolidPattern));
			 QPointF pt1,pt2,pt3,pt4;
			 pt1.setX(pt.x()-ptSize);
			 pt1.setY(pt.y());

			 pt2.setX(pt.x()+ptSize);
			 pt2.setY(pt.y());

			 pt3.setX(pt.x());
			 pt3.setY(pt.y()-ptSize);

			 pt4.setX(pt.x());
			 pt4.setY(pt.y()+ptSize);

			 painter.drawLine(pt1,pt2);
			 painter.drawLine(pt3,pt4);
		 }
	 }
	 if (chongheTpt->size()!=0)
	 {
		 for (int i=0;i<chongheTpt->size()/2;i++)
		 {
			 painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
			 QPointF pt;
			 pt.setX(chongheTpt->at(i*2));//*sourceImage.width());
			 pt.setY(chongheTpt->at(i*2+1));//*sourceImage.height());
			 //cout<<pt.x()<<" "<<pt.y()<<endl;
			 //painter.drawEllipse(pt,ptSize,ptSize);
			 painter.setPen(QPen(Qt::green,Qt::SolidPattern));
			 QPointF pt1,pt2,pt3,pt4;
			 pt1.setX(pt.x()-ptSize);
			 pt1.setY(pt.y());

			 pt2.setX(pt.x()+ptSize);
			 pt2.setY(pt.y());

			 pt3.setX(pt.x());
			 pt3.setY(pt.y()-ptSize);

			 pt4.setX(pt.x());
			 pt4.setY(pt.y()+ptSize);

			 painter.drawLine(pt1,pt2);
			 painter.drawLine(pt3,pt4);
		 }
	 }
	 if (sAPt->size()!=0)
	 {
		 for (int i=0;i<sAPt->size()/2;i++)
		 {
			 painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
			 QPointF pt;
			 pt.setX(sAPt->at(2*i)*sourceImage.width());
			 pt.setY(sAPt->at(2*i+1)*sourceImage.height());
			 painter.drawEllipse(pt,ptSize,ptSize);
			/* QPointF pt1,pt2,pt3,pt4;
			 pt1.setX(pt.x()-ptSize);
			 pt1.setY(pt.y());

			 pt2.setX(pt.x()+ptSize);
			 pt2.setY(pt.y());

			 pt3.setX(pt.x());
			 pt3.setY(pt.y()-ptSize);

			 pt4.setX(pt.x());
			 pt4.setY(pt.y()+ptSize);

			 painter.drawLine(pt1,pt2);
			 painter.drawLine(pt3,pt4);*/
		 }
	 }
	 if (ptTst->size()!=0)
	 {
		 for (int i=0;i<ptTst->size()/2;i++)
		 {
			 painter.setPen(QPen(Qt::green,Qt::SolidPattern));
			 QPointF pt;
			 pt.setX(ptTst->at(2*i));//*sourceImage.width());
			 pt.setY(ptTst->at(2*i+1));//*sourceImage.height());
			// painter.drawEllipse(pt,ptSize,ptSize);
			 QPointF pt1,pt2,pt3,pt4;
			 pt1.setX(pt.x()-ptSize);
			 pt1.setY(pt.y());

			 pt2.setX(pt.x()+ptSize);
			 pt2.setY(pt.y());

			 pt3.setX(pt.x());
			 pt3.setY(pt.y()-ptSize);

			 pt4.setX(pt.x());
			 pt4.setY(pt.y()+ptSize);

			 painter.drawLine(pt1,pt2);
			 painter.drawLine(pt3,pt4);
		 }
	 }

	 if (moveLinePt->size()!=0)
	 {
		 QRadialGradient rGradient;
		 painter.setRenderHint(QPainter::Antialiasing,true);

		 for (int i=0;i<moveLinePt->size()/4;i++)
		 {
			 QPointF pt1,pt2;
			 pt1.setX(moveLinePt->at(4*i));//*sourceImage.width());
			 pt1.setY(moveLinePt->at(4*i+1));//*sourceImage.height());
			 pt2.setX(moveLinePt->at(4*i+2));//*sourceImage.width());
			 pt2.setY(moveLinePt->at(4*i+3));//*sourceImage.height());

			 double lineLenth = sqrt( (pt1.x() - pt2.x())*(pt1.x() - pt2.x()) + (pt1.y() - pt2.y())*(pt1.y() - pt2.y()) );
			 double cx = (pt1.x() + pt2.x())/2;
			 double cy = (pt1.y() + pt2.y())/2;

			 double angle = (pt2.x() - pt1.x())/lineLenth;
			 angle = acos(angle);
			 double PI = 3.14159265358;
			 angle = angle*180/PI;
			 if (pt2.y()<pt1.y())
			 {
				 angle = -angle;
			 }

			 painter.save();
			 painter.translate(pt1.x(),pt1.y());
			 painter.rotate(angle);
			 painter.translate(-pt1.x(),-pt1.y());
			 rGradient.setCenter(cx,cy);
			 rGradient.setCenterRadius(lineLenth/2);
			 rGradient.setFocalPoint(cx,cy);
			 rGradient.setColorAt(0,Qt::white);
			 rGradient.setColorAt(1,Qt::blue);
			 painter.setBrush(rGradient);
			 painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
			 painter.drawRect(pt1.x(),pt1.y()-lineWidth/2,lineLenth,lineWidth);
			 painter.restore();
		 }
	 }
	 if (moveLinePt->size()!=0)
	 {
		 QRadialGradient rGradient;
		 for (int i=0;i<moveLinePt->size()/2;i++)
		 {
			 double x = moveLinePt->at(i*2);//*sourceImage.width();
			 double y = moveLinePt->at(i*2+1);//*sourceImage.height();
			 rGradient.setCenter(x,y);
			 rGradient.setCenterRadius(pointSize/2);
			 rGradient.setFocalPoint(x-pointSize/4,y-pointSize/4);
			 rGradient.setColorAt(0,Qt::white);
			 rGradient.setColorAt(1,Qt::yellow);
			 painter.setBrush(rGradient);
			 painter.setPen(QPen(Qt::yellow,Qt::SolidPattern));
			 painter.drawEllipse(x-pointSize/2,y-pointSize/2,pointSize,pointSize);
		 }
	 }

	 if (ruler->size()==4)
	 {
		 //painter.setPen(QPen(Qt::red,Qt::SolidPattern));
		 int rulerWidth = sourceImage.width()/200;
		 painter.setPen(QPen(Qt::red,rulerWidth));
		 QPointF pt,pt2;
		 pt.setX(ruler->at(0)*sourceImage.width());
		 pt.setY(ruler->at(1)*sourceImage.height());
		 pt2.setX(ruler->at(2)*sourceImage.width());
		 pt2.setY(ruler->at(3)*sourceImage.height());
		 if (!cLen)
		 {
			painter.drawLine(pt,pt2);
		 }
		 

		 //double len = (ruler->at(0) - ruler->at(2))*(ruler->at(0) - ruler->at(2)) + (ruler->at(1) - ruler->at(3))*(ruler->at(1) - ruler->at(3));
		 if (cLen)
		 { 
			 double len = (pt.x() - pt2.x())*(pt.x() - pt2.x()) + (pt.y() - pt2.y())*(pt.y() - pt2.y());
			 len = sqrt(len);
			 //cout<<"ruler length = "<<len<<endl;
			 emit rulerPtSe(len);
		 }
		
	 }
	 for (int i=0;i<lineCount;i++)
	 {
		 if (dLine[i]->size()==4)
		 {
			 int lineWidth = sourceImage.width()/400;
			 painter.setPen(QPen(Qt::red,lineWidth));
			 QPointF pt,pt2;
			 //cout<<dLine[i]->at(0)<<" "<<dLine[i]->at(1)<<endl;
			// cout<<dLine[i]->at(2)<<" "<<dLine[i]->at(3)<<endl;
			 pt.setX(dLine[i]->at(0)*sourceImage.width());
			 pt.setY(dLine[i]->at(1)*sourceImage.height());
			 pt2.setX(dLine[i]->at(2)*sourceImage.width());
			 pt2.setY(dLine[i]->at(3)*sourceImage.height());
			 painter.drawLine(pt,pt2);

		 }
	 }

		painter.setRenderHint(QPainter::Antialiasing,true);
		for (int i=0;i<20;i++)
		{
			switch (i)
			{
			case 1:{	//Draw Eclipse
				for (int j=0;j<showPtVector[i]->size()/5;j++)
				{
					double tx = showPtVector[i]->at(j*5+0);
					double ty = showPtVector[i]->at(j*5+1);
					double a = showPtVector[i]->at(j*5+2);
					double b = showPtVector[i]->at(j*5+3);
					double th = showPtVector[i]->at(j*5+4);
					painter.save();
					painter.translate(tx,ty);
					painter.rotate(th);
					painter.translate(-tx,-ty);
					

					QPointF pt;
					pt.setX(tx);
					pt.setY(ty);
					painter.setPen(QPen(Qt::blue,Qt::SolidPattern));
					painter.drawEllipse(pt,a/2,b/2);
					painter.restore();
				}
		
				break;
				   }
			case 2:{
				//cout<<"type = "<<i<<" "<<showPtVector[i]->size()<<endl;
				for (int j=0;j<showPtVector[i]->size()/2;j++)
				{
					int tx = showPtVector[i]->at(j*2);
					int ty = showPtVector[i]->at(j*2+1);

					painter.setPen(QPen(Qt::red,Qt::SolidPattern));
					QPointF pt;
					pt.setX(tx);
					pt.setY(ty);
					//painter.drawEllipse(pt,ptSize,ptSize);
					QPointF pt1,pt2,pt3,pt4;
					pt1.setX(pt.x()-ptSize);
					pt1.setY(pt.y());

					pt2.setX(pt.x()+ptSize);
					pt2.setY(pt.y());

					pt3.setX(pt.x());
					pt3.setY(pt.y()-ptSize);

					pt4.setX(pt.x());
					pt4.setY(pt.y()+ptSize);

					painter.drawLine(pt1,pt2);
					painter.drawLine(pt3,pt4);
				}
				break;
				   }
			case 3:{
				if (showPtVector[i]->size()>0)
				{
					for (int j=0;j<showPtVector[i]->size()/2;j++)
					{
						int tx = showPtVector[i]->at(j*2);
						int ty = showPtVector[i]->at(j*2+1);

						painter.setPen(QPen(Qt::green,Qt::SolidPattern));
						QPointF pt;
						pt.setX(tx);
						pt.setY(ty);
						//painter.drawEllipse(pt,ptSize,ptSize);
						QPointF pt1,pt2,pt3,pt4;
						pt1.setX(pt.x()-ptSize);
						pt1.setY(pt.y());

						pt2.setX(pt.x()+ptSize);
						pt2.setY(pt.y());

						pt3.setX(pt.x());
						pt3.setY(pt.y()-ptSize);

						pt4.setX(pt.x());
						pt4.setY(pt.y()+ptSize);

						painter.drawLine(pt1,pt2);
						painter.drawLine(pt3,pt4);
					}
				}
				break;
				   }
			}
		}
	 painter.restore();
 }


 void PWidget::setImage(QString *imgName,int tIndex)
 {
	 if (selfIndex == tIndex)
	 {
		 sourceImage.load(*imgName);
	
		 ptSize = sourceImage.height()/200;
		 fileS = true;
		 image = sourceImage;
		 scale=scaleMask=1;
		 mid[0]=mid[1]=startPt[0]=startPt[1]=driftPt[0]=driftPt[1]=0;
		 //double ratio=1024/sourceImage.width();
		 //sourceImage.scaled()
		 //        if(sourceImage.height ()!=0)
		 //            QMessageBox::information (this,tr("MyApplication"),tr("load height!=0"),QMessageBox::Ok);

		 pDraw=false;
		 pt2->clear ();

		 points->clear();
		 showSPT->clear();
		 pt2->clear();
		 sAPt->clear();
		 showAPt2Vec->clear();
		 chongheTpt->clear();
		 chongheSpt->clear();
		 points4Compare->clear();
		 ptTst->clear();
		 moveLinePt->clear();
		 sourceTracingPt->clear();
		 tracingPt->clear();

		 update();
	 }
 }

 void PWidget::setImage2(QString *imgName,int tIndex,double scale)
 {
	 // QMessageBox::information(QApplication::activeWindow(),"test",*imgName);
	 if (selfIndex == tIndex)
	 {
		 sourceImage.load(*imgName);
		 //cout<<sourceImage.height()<<" "<<sourceImage.width()<<endl;
		 if (scale!=0)
		 { 
			 sourceImage = sourceImage.scaledToWidth(sourceImage.width()*scale,Qt::FastTransformation);
		 }
		
		 cout<<sourceImage.height()<<" "<<sourceImage.width()<<endl;
		 ptSize = sourceImage.height()/200;
		 fileS = true;
		 image = sourceImage;
		 scale=scaleMask=1;
		 mid[0]=mid[1]=startPt[0]=startPt[1]=driftPt[0]=driftPt[1]=0;

		 pDraw=false;
		 pt2->clear ();
		 update();
	 }
 }

 void PWidget::setImage3(QImage img)
 {
	 sourceImage = img;
	 image = img;
	 points->clear();
	 showSPT->clear();
	 pt2->clear();
	 sAPt->clear();
	 showAPt2Vec->clear();
	 chongheSpt->clear();
	 chongheTpt->clear();
	 points4Compare->clear();
	 ptTst->clear();
	 mask1Pt->clear();
	 mask2Pt->clear();
	 tmask->clear();
	 ruler->clear();
	 moveLinePt->clear();
	 for (int j=0;j<20;j++)
	 {
		 showPtVector[j]->clear();
		 showPtType[j]=0;
	 }
	 scale=scaleMask=1;
	 ptSize = sourceImage.height()/200;
	 update();
 }

 void PWidget::savePts(QString *fileName)
 {
	 if (points->size()==0)
	 {
		 return;
	 }
	 char name[500];
	 strcpy(name, (const char*) fileName->toLatin1().data());
	 ofstream out(name,ios::out);
	for (int i=0;i<points->size()/2;i++)
	{
		out<<points->at(2*i)<<" "<<points->at(2*i+1)<<endl;
	}
        out.close();
 }

 void PWidget::drawPt ()
 {
     pDraw=true;
     update();
 }

 void PWidget::clearAll()
 {
	 points->clear();
 }

 void PWidget::clearLast()
 {
	 points->pop_back();
	 points->pop_back();
 }

 void PWidget::selectPtEn(bool res)
 {
	 sPt=res;
 }

 void PWidget::showSpt(double x,double y)
 {
	 showSPT->pop_back();
	 showSPT->pop_back();
	 showSPT->push_back(x);
	 showSPT->push_back(y);
	 update();
 }

 void PWidget::showSampleRuler(vector<double> *sRuler, bool comLen)
 {
	 ruler = sRuler;
	 cLen = comLen;
 }

 void PWidget::showAllPt(vector<double> *pt, vector<QString> *ptn)
 {
	 sAPt = pt;
	 ptName = ptn;
	 update();
 }

 void PWidget::showAllPt2(vector<double> *pt)
 {
	 //cout<<"pt size = "<<pt->size()<<endl;
	 showAPt2Vec = pt;
	 update();
 }

 void PWidget::changePtSize(int s)
 {
	 //cout<<s<<endl;
	 ptSize = s/10;
	 update();
 }

 void PWidget::addbright()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);
	 for (int i=0;i<sourceImage.width();i++)
	 {
		 for (int j=0;j<sourceImage.height();j++)
		 {
			 QRgb val = sourceImage.pixel(i,j);
			 double gain=1.03;
			 int tcolor = qGray(val)*gain;
			 if (tcolor>255)
			 {
				 tcolor=255;
			 }
			 QRgb grayPixel = qRgb(tcolor,tcolor,tcolor);
			 iGray.setPixel(i,j,grayPixel);
		 }
	 }
	 sourceImage = iGray;
	update();
 }

 void PWidget::decBright()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);
	 for (int i=0;i<sourceImage.width();i++)
	 {
		 for (int j=0;j<sourceImage.height();j++)
		 {
			 QRgb val = sourceImage.pixel(i,j);
			 double gain=1.03;
			 int tcolor = qGray(val)/gain;
			 if (tcolor>255)
			 {
				 tcolor=255;
			 }
			 QRgb grayPixel = qRgb(tcolor,tcolor,tcolor);
			 iGray.setPixel(i,j,grayPixel);
		 }
	 }
	 sourceImage = iGray;
	 update();

 }

 void PWidget::addImageCon()
 {
	
 }
 void PWidget::decImageCon()
 {

 }

 void PWidget::imageinverse()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);

	 for (int i=1;i<sourceImage.width()-1;i++)
	 {
		 for (int j=1;j<sourceImage.height()-1;j++)
		 {
			 int t_gray=0;
			 QRgb val = sourceImage.pixel(i,j);
			 t_gray = 255-qGray(val);
			 QRgb grayPixel = qRgb(t_gray,t_gray,t_gray);
			 iGray.setPixel(i,j,grayPixel);
		 }
	 }
	 sourceImage = iGray;
	 update();
 }

 void PWidget::imagesobelEdge()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);

	 for (int i=1;i<sourceImage.width()-1;i++)
	 {
		 for (int j=1;j<sourceImage.height()-1;j++)
		 {
			 int t_gray=0;
			 QRgb val = sourceImage.pixel(i,j);
			 int t1 = qGray(sourceImage.pixel(i+1,j-1)) +2*qGray(sourceImage.pixel(i+1,j)) +qGray(sourceImage.pixel(i+1,j+1));
			 int t2 = qGray(sourceImage.pixel(i-1,j-1)) +2*qGray(sourceImage.pixel(i-1,j)) +qGray(sourceImage.pixel(i-1,j+1));
			 int t3 = qGray(sourceImage.pixel(i-1,j-1)) +2*qGray(sourceImage.pixel(i,j-1)) +qGray(sourceImage.pixel(i+1,j-1));
			 int t4 = qGray(sourceImage.pixel(i-1,j+1)) +2*qGray(sourceImage.pixel(i,j+1)) +qGray(sourceImage.pixel(i+1,j+1));
			 t_gray = abs(t1-t2)+abs(t3-t4);
			 if (t_gray<0)
			 {
				 t_gray=0;
			 }
			 else if (t_gray>255)
			 {
				 t_gray=255;
			 }
			 QRgb grayPixel = qRgb(t_gray,t_gray,t_gray);
			 iGray.setPixel(i,j,grayPixel);
		 }
	 }
	 sourceImage = iGray;
	 update();
 }

 void PWidget::imagesmooth()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);

	 for (int i=1;i<sourceImage.width()-1;i++)
	 {
		 for (int j=1;j<sourceImage.height()-1;j++)
		 {
			 int t_gray=0;
			 QRgb val = sourceImage.pixel(i,j);
			 t_gray += qGray(val);
			 t_gray += qGray(sourceImage.pixel(i-1,j-1));
			 t_gray += qGray(sourceImage.pixel(i-1,j));
			 t_gray += qGray(sourceImage.pixel(i-1,j+1));
			 t_gray += qGray(sourceImage.pixel(i,j-1));
			 t_gray += qGray(sourceImage.pixel(i,j+1));
			 t_gray += qGray(sourceImage.pixel(i+1,j-1));
			 t_gray += qGray(sourceImage.pixel(i+1,j));
			 t_gray += qGray(sourceImage.pixel(i+1,j+1));
			 t_gray = t_gray/9;
			 QRgb grayPixel = qRgb(t_gray,t_gray,t_gray);
			 iGray.setPixel(i,j,grayPixel);
		 }
	 }
	 sourceImage = iGray;
	 update();
 }

 void PWidget::imageSharpen()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);
	 for (int i=1;i<sourceImage.width()-1;i++)
	 {
		 for (int j=1;j<sourceImage.height()-1;j++)
		 {
			 int t_gray=0;
			 QRgb val = sourceImage.pixel(i,j);
			 t_gray += 9*qGray(val);
			 t_gray -= qGray(sourceImage.pixel(i-1,j-1));
			 t_gray -= qGray(sourceImage.pixel(i-1,j));
			 t_gray -= qGray(sourceImage.pixel(i-1,j+1));
			 t_gray -= qGray(sourceImage.pixel(i,j-1));
			 t_gray -= qGray(sourceImage.pixel(i,j+1));
			 t_gray -= qGray(sourceImage.pixel(i+1,j-1));
			 t_gray -= qGray(sourceImage.pixel(i+1,j));
			 t_gray -= qGray(sourceImage.pixel(i+1,j+1));
			 //t_gray = t_gray/9;
			 if (t_gray<0)
			 {
				 t_gray=abs(t_gray);
			 }
			 if (t_gray>255)
			 {
				 t_gray=255;
			 }
			 QRgb grayPixel = qRgb(t_gray,t_gray,t_gray);
			 iGray.setPixel(i,j,grayPixel);
		 }
	 }
	 sourceImage = iGray;
	 update();
 }

 void PWidget::imageHistEqualise()
 {
	 QImage iGray(sourceImage.width(),sourceImage.height(),QImage::Format_RGB32);
	 double hist[256]={0};
	 for (int i=0;i<sourceImage.width();i++)
	 {
		 for (int j=0;j<sourceImage.height();j++)
		 {
			 QRgb val = sourceImage.pixel(i,j);
			 hist[qGray(val)]++;
		 }
	 }
	 int pixNum = sourceImage.height()*sourceImage.width();
	for (int i=0;i<=255;i++)
	{
		hist[i] = hist[i]/pixNum;
	}
	double eHist[256] = {0};
	for (int i=0;i<=255;i++)
	{
		for (int j=0;j<=i;j++)
		{
			eHist[i] += hist[j];
		}
	}
	for (int i=0;i<sourceImage.width();i++)
	{
		for (int j=0;j<sourceImage.height();j++)
		{
			QRgb val = sourceImage.pixel(i,j);
			int t_gray = eHist[qGray(val)]*255;
			QRgb grayPixel = qRgb(t_gray,t_gray,t_gray);
			iGray.setPixel(i,j,grayPixel);
		}
	}
	sourceImage = iGray;
	 update();
 }

 void PWidget::restoreImage()
 {
	 sourceImage = image;
	 update();
 }

 myHSV PWidget::RGB2HSV(QRgb tc)
 {
	 myHSV res;
	 int r = qRed(tc);
	 int g = qGreen(tc);
	 int b = qBlue(tc);
	 double tem = ((r*2 - g - b)/2) / sqrt( (double)(r-g)*(r-g) + (double)(r-g)*(g-b) );
	 double q = acos( tem);
	 double Pi=3.141592653;
	 q=q*180/Pi;
	 res.tH = q;
	 if(b>g)
		 res.tH = 360 - q;
	 res.tI = (r+g+b)/3;
	 int tmin=r;
	 if (r<=g && r<=b)
	 {
		 tmin = r;
	 }
	 if (g<=r && g<=b)
	 {
		 tmin = g;
	 }
	 if (b<=g && b<=r)
	 {
		 tmin = b;
	 }
	 res.tS = 1- 3*tmin/(r+g+b);
	 return res;
 }

 void PWidget::setHVRuler(double hr,double vr,int tindex)
 {
	 if (tindex==selfIndex)
	 {
		 double hScale = hr;
		 double vScale = vr;
		 sourceImage = sourceImage.scaled(sourceImage.width()*hScale,sourceImage.height()*vScale,Qt::IgnoreAspectRatio,Qt::FastTransformation);
		 update();
	 }
 }

 void PWidget::setRuler(double r,int tindex)
 {
	 if (tindex == selfIndex)
	 {
		 markRuler = r;

		 if ((double)sourceImage.width()/sourceImage.height() < 0.8)
		 {
			 markRuler = markRuler/0.9;
		 }

		// cout<<selfIndex<<"  ruler = "<<markRuler<<endl;
		 double ppi = 25.4/markRuler;
		// cout<<" ppi = "<<ppi<<endl;
		 double dpi = sourceImage.logicalDpiX();
		// cout<<" dpi =  "<<dpi<<endl;
		 double myppi=144;		//将全部片子统一到144ppi
		 double tScale = myppi/ppi;
		 markRuler = 25.4/myppi;
		// cout<<selfIndex<<"  ruler = "<<markRuler<<endl;
		// cout<<sourceImage.height()<<" "<<sourceImage.width()<<endl;
		 sourceImage = sourceImage.scaledToWidth(sourceImage.width()*tScale,Qt::FastTransformation);
		 /*double setDpi = 144/2.54*100;
		 sourceImage.setDotsPerMeterX(setDpi);
		 sourceImage.setDotsPerMeterY(setDpi);
		 sourceImage.save("./test.jpg");
		 cout<<"new dpi = "<<sourceImage.logicalDpiX()<<endl;*/
		  cout<<sourceImage.height()<<" "<<sourceImage.width()<<endl;

		  ptSize = sourceImage.height()/200;
		  emit scaleSet(tScale);
	 }
	
 }

 void PWidget::setChonghePt(vector<double> *csPt,vector<double> *ctPt)
 {
	chongheSpt = csPt;
	chongheTpt = ctPt;
 }
 
 // type = 3 : 显示单个点
 // type = 2 : 显示多个点，包括所选择点的所有记录及当前table中所有点
 // type = 1 : 显示椭圆。pt包含5项，依次为当前点均值，椭圆3个参数
 void PWidget::getShowPointSlot(vector<double> *pt, int type)
 {
	 /*cout<<pt->size()<<endl;*/
	 if (showPtType[type]>0)
	 {
		 showPtVector[type]->clear();
	 }
	 /*showPtVector[type] = pt;*/
	 for (int i=0;i<pt->size();i++)
	 {
		 double tpt = pt->at(i);
		 showPtVector[type]->push_back(tpt);
	 }
	 showPtType[type]++;
	 //cout<<"type = "<<type<<" "<<showPtVector[type]->size()<<endl;
	 update();
 }

 void PWidget::setTransMatrix(QMatrix TM)
 {
	 transMatrix = TM;
 }

 int PWidget::isOnLine(double x, double y)
 {
	 if (moveLinePt->size()==0)
	 {
		 return -1;
	 }
	 int resIndex = -1;
	 for (int i=0;i<moveLinePt->size()/4;i++)
	 {
		 double x1 = moveLinePt->at(4*i);//*sourceImage.width();
		 double y1 = moveLinePt->at(4*i+1);//*sourceImage.height();
		 double x2 = moveLinePt->at(4*i+2);//*sourceImage.width();
		 double y2 = moveLinePt->at(4*i+3);//*sourceImage.height();
		 
		 double tx1 = (5*x1+x2)/6;
		 double tx2 = (x1+5*x2)/6;
		 if (tx1>tx2)
		 {
			 double t=tx2;
			 tx2 = tx1;
			 tx1 = t;
		 }
		 if (tx2-tx1<6)
		 {
			 tx1 = -1;
			 tx2 = 100000;
		 }
		 double ty1 = (5*y1+y2)/6;
		 double ty2 = (y1+5*y2)/6;
		 if (ty1>ty2)
		 {
			 double t=ty2;
			 ty2 = ty1;
			 ty1 = t;
		 }
		 if (ty2-ty1<6)
		 {
			 ty1 = -1;
			 ty2 = 100000;
		 }
		 
		 double vx1 = x2-x1;
		 double vy1 = y2-y1;

		 double vx2 = x-x1;
		 double vy2 = y-y1;

		 double len = abs( vx1*vy2 - vx2*vy1 )/sqrt(vx1*vx1 + vy1*vy1);
		 if ( x>=tx1 && x<=tx2 && y<=ty2 && y>=ty1 && len<=lineWidth*10 )
		 {
			 resIndex = i;
			 //cerr<<"index = "<<i<<" on Line"<<endl;
			 break;
		 }
	 }
	 return resIndex;
 }

 int PWidget::isOnPoint(double x, double y)
 {
	 if (moveLinePt->size()==0)
	 {
		 return -1;
	 }
	 int resIndex = -1;
	 for (int i=0;i<moveLinePt->size()/2;i++)
	 {
		 double x1 = moveLinePt->at(2*i);
		 double y1 = moveLinePt->at(2*i+1);
		 
		 double len = (x-x1)*(x-x1) + (y-y1)*(y-y1);
		 len = sqrt(len);
		 if (len<pointSize*4)
		 {
			 resIndex = i;
			 break;
		 }
	 }
	 return resIndex;
 }

 void PWidget::getRulerPt(double x1,double y1,double x2,double y2)
 {
	 moveLinePt->clear();
	 moveLinePt->push_back(x1);
	 moveLinePt->push_back(y1);
	 moveLinePt->push_back(x2);
	 moveLinePt->push_back(y2);
	 moveLinePt->push_back(x1);
	 moveLinePt->push_back(y1);

	 double ax = x1+y1-y2;
	 double ay = y1+x2-x1;
	 moveLinePt->push_back(ax);
	 moveLinePt->push_back(ay);
	 update();
 }

 void PWidget::setBoolenStatus(int status, bool flag)
 {

	 switch (status)
	 {
	 case 0:
		 sPt = flag;
	 	break;
	 }
 }

 void PWidget::clearAllPt()
 {
	 points->clear();
	 showSPT->clear();
	 pt2->clear();
	 sAPt->clear();
	 showAPt2Vec->clear();
	 chongheSpt->clear();
	 chongheTpt->clear();
	 points4Compare->clear();
	 ptTst->clear();
	 mask1Pt->clear();
	 mask2Pt->clear();
	 tmask->clear();
	 ruler->clear();
	 moveLinePt->clear();
	 for (int j=0;j<20;j++)
	 {
		 showPtVector[j]->clear();
		 showPtType[j]=0;
	 }
	 update();
 }