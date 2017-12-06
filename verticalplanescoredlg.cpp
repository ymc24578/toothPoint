#include "verticalplanescoredlg.h"

#include <QMessageBox>

#include "mainwindow.h"

VerticalPlaneScoreDlg::VerticalPlaneScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

VerticalPlaneScoreDlg::~VerticalPlaneScoreDlg()
{
}

void VerticalPlaneScoreDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));

	double score = (this->ui.lineEditVerticalPlaneScore->text()).toDouble();
	this->mainWindow->setScore(score);

	accept();
}
