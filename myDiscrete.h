#ifndef MYDISCRETE_H
#define MYDISCRETE_H

#include <QDialog>
#include <QDir>
#include <QtGui/QtGui>
#include "myDatabase.h"
#include <iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include "customNewMesuerDialog.h"
#include "exportMesTree.h"
#include "exportTreeDialog.h"
using namespace std;

class MyDiscreteC : public QDialog
{
	Q_OBJECT

public:
	MyDiscreteC(QWidget *parent = 0);
	~MyDiscreteC();
	void createWidgets();
	void iniDialogs();
	void initData();
	void iniSampleIndexCBox();
	

	QString currentImage;
	QString currentMName;
	QString currentMId;
	QString currentMindex;
	QString ptCordLableStr;

public slots:
	void mesSelected(QListWidgetItem *it);
	void imageSelected(QListWidgetItem *it);
	void iniFileNameWidget();

	void exportDiscreteBt_triggered();
	void exportMesResBt_triggered();
	void custMesBt_triggered();

	//输出测量项目
	// sl:selecte mes id; ml: mesure method list; sf: savefilename
	void getexportMesSlot(QStringList sl, QStringList ml, QString sf);

	// 输出离散项目
	// sList:selecte mes id; ptList: point list; saveFileName: savefilename
	void judgePoints(QStringList sList,QStringList ptList,QString saveFileName);

signals:
	void setMesureInfoSignal(QString,QString,QString,QString);
	void doubleclickSignal(QListWidgetItem *);
	void setExportMesInfo(QStringList,QStringList,QString);

private:
	QComboBox *sampleIndexCBox;
	QListWidget *imageNameListWidget;	//图片list
	QListWidget *mesListWidget;		// 测量者list
	QLabel *ptCord;

	myDataBaseC mydb;

	Dialog_NewMesuer *newMesuerDialog;
	Dialog_ExportMesTree *exportMesDialog;
	Dialog_ExportTree *exportTreeDialog;
};

#endif