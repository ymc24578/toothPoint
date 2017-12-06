#include "diBackToothLockScoreDlg.h"

#include "mainwindow.h"

diBackToothLockScoreDlg::diBackToothLockScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

diBackToothLockScoreDlg::~diBackToothLockScoreDlg()
{
}

void diBackToothLockScoreDlg::okButtonClicked()
{
	double score = (this->ui.lineEditScore->text()).toDouble();
	mainWindow->setScore(score * 2);

	accept();
}
