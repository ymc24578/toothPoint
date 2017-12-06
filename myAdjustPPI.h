#ifndef MYADJUSTPPI_H
#define MYADJUSTPPI_H

#include <QDialog>
#include <QDir>
#include <QtGui/QtGui>
#include "myDatabase.h"
#include <iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QTextStream>
using namespace std;

class myAdjustPPIDialogC : public QDialog
{
	Q_OBJECT

public:
	myAdjustPPIDialogC(QWidget *parent = 0);
	~myAdjustPPIDialogC();
	void createWidgets();
	void iniSampleIndex();
	bool isInited();
	void exportSingmeMnameRulerPoint(QString mname, QString sampleindex);
	void exportAverageRulerPoint(QString sampleindex);

public slots:
	void exportRulerPointBt_triggered();
	void uniPPIBt_triggered();
private:
	QComboBox *sampleIndexCBox;
	myDataBaseC mydb;
	bool inited;
};

#endif