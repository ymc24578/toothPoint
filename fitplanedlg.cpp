#include "fitplanedlg.h"

#include <QMessageBox>

#include "mainwindow.h"

FitPlaneDlg::FitPlaneDlg(QWidget *parent) : QDialog(parent)
{
	this->dlg.setupUi(this);

	mainWindow = (MainWindow*)(parent);

	this->dlg.frontToothDistanceLineEdit->setText(QString::number(mainWindow->getFrontToothDistance()));

	connect(this->dlg.okButton, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
}

FitPlaneDlg::~FitPlaneDlg()
{
}

void FitPlaneDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));
	double score = (this->dlg.scoreLineEdit->text()).toDouble();
	mainWindow->setScore(score);
}
