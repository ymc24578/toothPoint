#ifndef FACESIDESCOREDLG_H
#define FACESIDESCOREDLG_H

#include <QDialog>

#include "ui_facesidescoredlg.h"

class MainWindow;

class FaceSideScoreDlg : public QDialog
{
	Q_OBJECT

public:
	FaceSideScoreDlg(QWidget *parent = 0);
	~FaceSideScoreDlg();

public slots:
	void okButtonClicked();

public:
	MainWindow* mainWindow;

private:
	Ui::FaceSideScoreDlg ui;
};

#endif // FACESIDESCOREDLG_H
