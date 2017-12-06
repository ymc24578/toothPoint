#include "middlelinescoredlg.h"

#include "mainwindow.h"

MiddleLineScoreDlg::MiddleLineScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

MiddleLineScoreDlg::~MiddleLineScoreDlg()
{
}

void MiddleLineScoreDlg::okButtonClicked()
{

	double score = (this->ui.lineEditScore->text()).toDouble();
	this->mainWindow->setScore(score);

	accept();
}
