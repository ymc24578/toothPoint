#include "overbitelowerpairpointscoredlg.h"

#include "mainwindow.h"

OverbiteLowerPairPointScoreDlg::OverbiteLowerPairPointScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

OverbiteLowerPairPointScoreDlg::~OverbiteLowerPairPointScoreDlg()
{
}

void OverbiteLowerPairPointScoreDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));

	double score = (this->ui.lineEditScore->text()).toDouble();
	this->mainWindow->setScore(score);

	accept();
}
