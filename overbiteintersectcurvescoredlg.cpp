#include "overbiteintersectcurvescoredlg.h"

#include "mainwindow.h"

OverbiteIntersectCurveScoreDlg::OverbiteIntersectCurveScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
}

OverbiteIntersectCurveScoreDlg::~OverbiteIntersectCurveScoreDlg()
{

}

void OverbiteIntersectCurveScoreDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));

	double score = (this->ui.lineEditScore->text()).toDouble();
	this->mainWindow->setScore(score);

	accept();
}
