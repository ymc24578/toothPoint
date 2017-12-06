#ifndef OVERBITELOWERPAIRPOINTSCOREDLG_H
#define OVERBITELOWERPAIRPOINTSCOREDLG_H

#include <QDialog>

#include "ui_overbitelowerpairpointscoredlg.h"

class MainWindow;

class OverbiteLowerPairPointScoreDlg : public QDialog
{
	Q_OBJECT

public:
	OverbiteLowerPairPointScoreDlg(QWidget *parent = 0);
	~OverbiteLowerPairPointScoreDlg();

public slots:
	void okButtonClicked();

public:
	MainWindow* mainWindow;
	Ui::OverbiteLowerPairPointScoreDlg ui;
};

#endif // OVERBITELOWERPAIRPOINTSCOREDLG_H
