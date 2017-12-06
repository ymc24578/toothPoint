#ifndef VERTICALPLANESCOREDLG_H
#define VERTICALPLANESCOREDLG_H

#include <QDialog>

#include "ui_verticalplanescoredlg.h"

class MainWindow;


class VerticalPlaneScoreDlg : public QDialog
{
	Q_OBJECT

public:
	VerticalPlaneScoreDlg(QWidget *parent = 0);
	~VerticalPlaneScoreDlg();

public slots:
	void okButtonClicked();

public:
	MainWindow* mainWindow;

private:
	Ui::VerticalPlaneScoreDlg ui;
};

#endif // VERTICALPLANESCOREDLG_H
