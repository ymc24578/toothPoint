#include "DiOtherScoreDlg.h"

#include "mainwindow.h"

DiOtherScoreDlg::DiOtherScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	mainWindow = (MainWindow*)(parent);

	connect(this->ui.pushButtonOK, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	setAttribute (Qt::WA_DeleteOnClose);
}

DiOtherScoreDlg::~DiOtherScoreDlg()
{
}

void DiOtherScoreDlg::okButtonClicked()
{
	double score = (this->ui.lineEditScore->text()).toDouble();
	mainWindow->setScore(score);

	accept();
}
