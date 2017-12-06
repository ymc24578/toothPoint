#ifndef FITPLANEDLG_H
#define FITPLANEDLG_H

#include <QDialog>

#include "ui_FitPlaneScoreDialog.h"

class MainWindow;

class FitPlaneDlg : public QDialog
{
	Q_OBJECT

public:
	FitPlaneDlg(QWidget *parent);
	~FitPlaneDlg();

public slots:
	void okButtonClicked();

public:
	Ui::FitPlaneScoreDialog dlg;

	MainWindow* mainWindow;

private:
	
};

#endif // FITPLANEDLG_H
