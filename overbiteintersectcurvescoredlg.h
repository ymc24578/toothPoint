#ifndef OVERBITEINTERSECTCURVESCOREDLG_H
#define OVERBITEINTERSECTCURVESCOREDLG_H

#include <QDialog>

#include "ui_overbiteintersectcurvescoredlg.h"

class MainWindow;

class OverbiteIntersectCurveScoreDlg : public QDialog
{
	Q_OBJECT

public:
	OverbiteIntersectCurveScoreDlg(QWidget *parent = 0);
	~OverbiteIntersectCurveScoreDlg();

public slots:
	void okButtonClicked();

public:
	MainWindow* mainWindow;

private:
	Ui::OverbiteIntersectCurveScoreDlg ui;
};

#endif // OVERBITEINTERSECTCURVESCOREDLG_H
