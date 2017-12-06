#ifndef MYMARKPOINT_H
#define MYMARKPOINT_H

#include <QDialog>
#include <QDir>
#include <QtGui/QtGui>
#include "myDatabase.h"
#include <iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
using namespace std;

class MyMarkPointC : public QDialog
{
	Q_OBJECT

public:
	MyMarkPointC(QWidget *parent = 0);
	~MyMarkPointC();
	void createWidgets();
	void placeWidgets();
	void initData();

	QCheckBox *getSpt();
	QTableWidget *getPointTable();
	bool getRulerShowChecked();
	QString getSampleIndexBoxText();
	bool mesListWidgetSelected();

	QString currentImage;
	QString currentMName;
	QString currentMId;
	QString currentMindex;
	QString ptCordLableStr;

public slots:
	//���������/�޸�/ɾ������
	void addnewMesTriggered();
	void editCurMesTriggered();
	void deleteCurMesTriggered();
	void addnewMesALLTriggered();
	void addMesPtTriggered();

	void mesSelected(QListWidgetItem *it);
	void imageSelected(QListWidgetItem *it);
	void ptSetSelected(QListWidgetItem *it);
	void getImage(QString *imageName);
	void deleteCurrentSingleImage();

	void iniPtSetWidget();

	void deleteCurrentRow();
	void showCPtActionTriggered();
	void showCurrentAllPtActionTriggered();
	void showAllPtInTableActionTriggered();
	void showEclipseActionTriggered();
	void showAllEllipseBtTriggered();

	void loadImagesTriggered();
	void deleteImagesTriggered();
	void iniSampleIndexCBox();
	void iniFileNameWidget();

	void getScale(double);
	void rulerShowStateChanged(int sta);

signals:
	void doubleclickSignal(QListWidgetItem *);
	void getImageNames(QString *imageName);
	void listAllPt(QListWidgetItem *);
	void imageS(QImage);
	void setMesureInfoSignal(QString,QString,QString,QString);
	void clearAllPtSignal();
	void showMarkPt(double, double);
	void showAllPtSignal(vector<double> *ptV);
	void showRuler(vector<double> *,bool);
	void rulerSelected(double ,int );
	void getShowPointSignal(vector<double> *, int);
	void setHVRulerSignal(double,double,int);
	void getNewMarkRulerCordSignal(double ,double ,double , double );
	void fileSelected(QString *, int,double);

private:
	QComboBox *sampleIndexCBox;
	QListWidget *imageNameListWidget;	//ͼƬlist
	QListWidget *cusPtListWidget;	// �����㼯��list
	QListWidget *mesListWidget;		// ������list
	QLineEdit *nameT,*idT,*indexT,*timeT;	// ������������id��������Σ�����ʱ��
	QTableWidget *pointTable;	// table
	QLabel *ptCord;
	QLineEdit *addPtStartT;

	QCheckBox *sPt,*rulerShow,*drawRuler;

	QStringList ptIndexList,ptNameEngList,ptNameChsList;

	myDataBaseC mydb;
};

#endif