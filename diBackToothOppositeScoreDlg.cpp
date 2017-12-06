#include "diBackToothOppositeScoreDlg.h"

#include "mainwindow.h"

diBackToothOppositeScoreDlg::diBackToothOppositeScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

diBackToothOppositeScoreDlg::~diBackToothOppositeScoreDlg()
{
}

void diBackToothOppositeScoreDlg::okButtonClicked()
{
	double score = (this->ui.lineEditScore->text()).toDouble();
	mainWindow->setScore(score);

	accept();
}
