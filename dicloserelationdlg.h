#ifndef DICLOSERELATIONDLG_H
#define DICLOSERELATIONDLG_H

#include <QDialog>

#include "ui_dicloserelationdlg.h"

class MainWindow;

class DiCloseRelationDlg : public QDialog
{
	Q_OBJECT

public:
	DiCloseRelationDlg(QWidget *parent = 0);
	~DiCloseRelationDlg();

public slots:
	void okButtonClicked();

	void radioChange();

private:
	Ui::DiCloseRelationDlg ui;

	MainWindow* mainWindow;

	int classNum;
};

#endif // DICLOSERELATIONDLG_H
