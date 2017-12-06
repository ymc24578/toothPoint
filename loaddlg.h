#pragma once
#ifndef LOADDLG_H
#define LOADDLG_H

#include <QDialog>

#include "ui_loaddlg.h"

class MainWindow;

class LoadDlg : public QDialog
{
	Q_OBJECT

public:
	LoadDlg(QWidget *parent = 0);
	~LoadDlg();

	public slots:
		void loadButtonClicked();

private:
	Ui::loadDlg ui;

	MainWindow* mainWindow;
};

#endif // SAVEDLG_H
