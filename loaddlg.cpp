#include "loaddlg.h"

#include "mainwindow.h"

LoadDlg::LoadDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	mainWindow = (MainWindow*)(parent);

	connect(this->ui.pushButtonSave, SIGNAL(clicked()), this,SLOT(loadButtonClicked()));
}

LoadDlg::~LoadDlg()
{
}

void LoadDlg::loadButtonClicked()
{
	QString no = this->ui.noLineEdit->text();
	this->mainWindow->loadData(no);

	hide();
}
