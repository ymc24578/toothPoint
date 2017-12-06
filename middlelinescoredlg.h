#ifndef MIDDLELINESCOREDLG_H
#define MIDDLELINESCOREDLG_H

#include <QDialog>

#include "ui_middlelinescoredlg.h"

class MainWindow;


class MiddleLineScoreDlg : public QDialog
{
	Q_OBJECT

public:
	MiddleLineScoreDlg(QWidget *parent = 0);
	~MiddleLineScoreDlg();

public slots:
	void okButtonClicked();

public:
	MainWindow* mainWindow;
	Ui::MiddleLineScoreDlg ui;
};

#endif // MIDDLELINESCOREDLG_H
