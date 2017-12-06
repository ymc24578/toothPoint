#pragma once
#ifndef diBackToothLockScoreDlg_H
#define diBackToothLockScoreDlg_H

#include <QDialog>

#include "ui_diBackToothLockScoreDlg.h"

class MainWindow;

class diBackToothLockScoreDlg : public QDialog
{
	Q_OBJECT

public:
	diBackToothLockScoreDlg(QWidget *parent = 0);
	~diBackToothLockScoreDlg();
		MainWindow* mainWindow;
	public slots:
		void okButtonClicked();

private:
	Ui::diBackToothLockScoreDlg ui;


};

#endif // DIOTHERSCOREDLG_H
