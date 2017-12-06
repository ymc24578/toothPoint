#ifndef DIOTHERSCOREDLG_H
#define DIOTHERSCOREDLG_H

#include <QDialog>

#include "ui_DiOtherScoreDlg.h"

class MainWindow;

class DiOtherScoreDlg : public QDialog
{
	Q_OBJECT

public:
	DiOtherScoreDlg(QWidget *parent = 0);
	~DiOtherScoreDlg();

public slots:
	void okButtonClicked();

private:
	Ui::DiOtherScoreDlg ui;

	MainWindow* mainWindow;
};

#endif // DIOTHERSCOREDLG_H
