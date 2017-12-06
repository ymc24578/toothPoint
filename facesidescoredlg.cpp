#include "facesidescoredlg.h"

#include "mainwindow.h"

FaceSideScoreDlg::FaceSideScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

FaceSideScoreDlg::~FaceSideScoreDlg()
{
}

void FaceSideScoreDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));

	double score = (this->ui.lineEditFaceSideScore->text()).toDouble();
	this->mainWindow->setScore(score);

	accept();
}
