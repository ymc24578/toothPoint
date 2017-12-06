#ifndef SAVEDLG_H
#define SAVEDLG_H

#include <QDialog>

#include "ui_savedlg.h"

class MainWindow;

class SaveDlg : public QDialog
{
	Q_OBJECT

public:
	SaveDlg(QWidget *parent = 0);
	~SaveDlg();

public slots:
	void saveButtonClicked();

private:
	Ui::SaveDlg ui;

	MainWindow* mainWindow;
};

#endif // SAVEDLG_H
