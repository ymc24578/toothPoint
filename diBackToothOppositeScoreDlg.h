#pragma once
#ifndef diBackToothOppositeScoreDlg_H
#define diBackToothOppositeScoreDlg_H

#include <QDialog>

#include "ui_diBackToothOppositeScoreDlg.h"

class MainWindow;

class diBackToothOppositeScoreDlg : public QDialog
{
	Q_OBJECT

public:
	diBackToothOppositeScoreDlg(QWidget *parent = 0);
	~diBackToothOppositeScoreDlg();
	MainWindow* mainWindow;
	public slots:
		void okButtonClicked();

private:
	Ui::diBackToothOppositeScoreDlg ui;

	
};

#endif // DIOTHERSCOREDLG_H
