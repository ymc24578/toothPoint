#include <QtGui>
#include "customNewMesuerDialog.h"
#include <QMessageBox>
#include <QFile>
#include<iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <cmath>
using namespace std;

Dialog_NewMesuer::Dialog_NewMesuer(QWidget *parent )
	:QDialog(parent)
{
	tabWidget = new QTabWidget;

	for (int i=0;i<6;i++)
	{
		ptBox[i] = new QComboBox;
		pointCord[i] = new QLabel;
		connect(ptBox[i],SIGNAL(currentIndexChanged ( int  )),this,SLOT(valueChanged()));
	}

	saveNameType1 = new QTextEdit;
	saveNameType1->setMaximumHeight(25);
	saveNameType2 = new QTextEdit;
	saveNameType2->setMaximumHeight(25);

	res1 = new QLabel("Length = ");
	res2 = new QLabel("Angle = ");

	QWidget *tab1 = new QWidget;
	QGridLayout *layout1 = new QGridLayout;

	QLabel *tab1Pt1Lb = new QLabel(tr("点1"));
	QLabel *tab1Pt2Lb = new QLabel(tr("点2"));

	QPushButton *saveBt = new QPushButton(tr("保存"));
	connect(saveBt,SIGNAL(clicked()),this,SLOT(saveBtTriggered()));


	layout1->addWidget(tab1Pt1Lb,0,0);
	layout1->addWidget(tab1Pt2Lb,0,1);
	layout1->addWidget(ptBox[0],1,0);
	layout1->addWidget(ptBox[1],1,1);
	layout1->addWidget(pointCord[0],2,0);
	layout1->addWidget(pointCord[1],2,1);
	layout1->addWidget(saveNameType1,3,0);
	layout1->addWidget(saveBt,3,1);
	layout1->addWidget(res1);

	tab1->setLayout(layout1);
	tabWidget->addTab(tab1,tr("两点距离"));

	QWidget *tab2 = new QWidget;
	QGridLayout *layout2 = new QGridLayout;

	QLabel *discribeLabel = new QLabel(tr("所测角度为直线pt1-pt2 与 直线pt3-pt4 之间的夹角"));
	QLabel *tab2pt1lb = new QLabel(tr("PT1"));
	QLabel *tab2pt2lb = new QLabel(tr("PT2"));
	QLabel *tab2pt3lb = new QLabel(tr("PT3"));
	QLabel *tab2pt4lb = new QLabel(tr("PT4"));

	QPushButton *saveBt2 = new QPushButton(tr("保存"));
	connect(saveBt2,SIGNAL(clicked()),this,SLOT(saveBtTriggered()));

	layout2->addWidget(discribeLabel,0,0,1,4);
	layout2->addWidget(tab2pt1lb,1,0,1,1);
	layout2->addWidget(tab2pt2lb,1,1,1,1);
	layout2->addWidget(tab2pt3lb,1,2,1,1);
	layout2->addWidget(tab2pt4lb,1,3,1,1);

	for (int i=0;i<4;i++)
	{
		layout2->addWidget(ptBox[i+2],2,i,1,1);
		layout2->addWidget(pointCord[i+2],3,i,1,1);
	}
	layout2->addWidget(saveNameType2,4,0);
	layout2->addWidget(saveBt2,4,1);
	layout2->addWidget(res2,5,0,1,4);

	tab2->setLayout(layout2);
	tabWidget->addTab(tab2,tr("角度"));

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(tabWidget);
	setLayout(mainLayout);
	setWindowTitle(tr("添加测量项"));
}

void Dialog_NewMesuer::setIniPara(QString si,QString im,QString mn, QString mi)
{
	sampleindex = si;
	imagename = im;
	mname = mn;
	mindex = mi;
}

void Dialog_NewMesuer::iniCombox()
{
	for (int i=0;i<6;i++)
	{
		disconnect(ptBox[i],SIGNAL(currentIndexChanged ( int  )),this,SLOT(valueChanged()));
	}
	QSqlQuery query;
	QString getAllPt = "select distinct pointnameeng from disptvalue ";
	QStringList ptNameList;
	ptNameList.append("");
	query.exec(getAllPt);
	while (query.next())
	{
		QString ptn = query.value(0).toString();
		ptNameList.append(ptn);
	}
	for (int i=0;i<6;i++)
	{
		ptBox[i]->addItems(ptNameList);
	}
	for (int i=0;i<6;i++)
	{
		connect(ptBox[i],SIGNAL(currentIndexChanged ( int  )),this,SLOT(valueChanged()));
	}
}

void Dialog_NewMesuer::valueChanged()
{
	if (tabWidget->currentIndex()==1)
	{
		calAngle();
	}
	else
	{
		calLength();
	}
}

void Dialog_NewMesuer::calAngle()
{
	QSqlQuery query;

	double x[4]={0};
	double y[4]={0};

	for (int i=0;i<4;i++)
	{
		QString pName = ptBox[i+2]->currentText();
		QString getXY = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and mindex='"+mindex+"' and mname='"+mname+"' and pointnameeng='"+pName+"'";
		query.exec(getXY);
		if (query.size()==0)
		{
			QString temp = "Angle = ";
			res2->setText(temp);
			return;
		}
		query.next();
		x[i] = query.value(0).toDouble();
		y[i] = query.value(1).toDouble();
		QString Cord = "( " + QString::number(x[i]) + " , " + QString::number(y[i]) + " )";
		pointCord[i+2]->setText(Cord);
	}
	
	double v1x = x[1]-x[0];
	double v1y = y[1]-y[0];
	double v2x = x[3]-x[2];
	double v2y = y[3]-y[2];

	double v1_Mul_v2 = v1x*v2x + v1y*v2y;
	double lenthV1 = sqrt(v1x*v1x + v1y*v1y);
	double lenthV2 = sqrt(v2x*v2x + v2y*v2y);
	double angle = v1_Mul_v2 / (lenthV1 * lenthV2);
	angle = acos(angle);
	double PI = 3.1415926535;
	angle = angle*180/PI;

	QString temp = "Angle = ";
	temp += QString::number(angle);
	res2->setText(temp);
}

void Dialog_NewMesuer::calLength()
{
	QSqlQuery query;
	QString p1Name = ptBox[0]->currentText();
	QString p2Name = ptBox[1]->currentText();
	QString getXY = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and mindex='"+mindex+"' and mname='"+mname+"' and pointnameeng='"+p1Name+"'";
	query.exec(getXY);
	if (query.size()==0)
	{
		QString tempRes1 = "Length = ";
		res1->setText(tempRes1);
		return;
	}
	query.next();
	double x1 = query.value(0).toDouble();
	double y1 = query.value(1).toDouble();
	QString Cord = "( " + QString::number(x1) + " , " + QString::number(y1) + " )";
	pointCord[0]->setText(Cord);

	getXY = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and mindex='"+mindex+"' and mname='"+mname+"' and pointnameeng='"+p2Name+"'";
	query.exec(getXY);
	if (query.size()==0)
	{
		QString tempRes1 = "Length = ";
		res1->setText(tempRes1);
		return;
	}
	query.next();
	double x2 = query.value(0).toDouble();
	double y2 = query.value(1).toDouble();
	Cord = "( " + QString::number(x2) + " , " + QString::number(y2) + " )";
	pointCord[1]->setText(Cord);

	double res = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
	
	res = res*25.4/144;

	QString tempRes1 = "Length = ";
	tempRes1 += QString::number(res) + tr(" mm");
	res1->setText(tempRes1);
}

void Dialog_NewMesuer::saveBtTriggered()
{
	QString saveName;
	QString line="";
	if (tabWidget->currentIndex()==0)
	{
		saveName = saveNameType1->text();
		QString pt1 = ptBox[0]->currentText();
		QString pt2 = ptBox[1]->currentText();
		QString mesType = "11";	//两点距离
		line = mesType + "\t" + saveName  + "\t" + pt1 + "\t" + pt2 +"\r\n";
	}
	else
	{
		saveName = saveNameType2->text();
		QString mesType = "12";	//两点距离
		line = mesType + "\t" + saveName;
		for (int i=2;i<6;i++)
		{
			QString temp = ptBox[i]->currentText();
			line += "\t" + temp;
		}
		line += "\r\n";
	}
	QFile file("./CustomedProj/newCusProg.txt");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream out(&file);
	out<<line;
	file.close();
	 QMessageBox::information(QApplication::activeWindow(),"Information",tr("添加成功"));
}