#include "savedlg.h"

#include "mainwindow.h"

SaveDlg::SaveDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	mainWindow = (MainWindow*)(parent);

	connect(this->ui.pushButtonSave, SIGNAL(clicked()), this,SLOT(saveButtonClicked()));
}

SaveDlg::~SaveDlg()
{
}

void SaveDlg::saveButtonClicked()
{
	QString no = this->ui.noLineEdit->text();
	this->mainWindow->saveData(no);

	hide();
}
