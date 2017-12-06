#include "parfuhe.h"

#include "mainwindow.h"

ParfuheScoreDlg::ParfuheScoreDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
	connect(this->ui.pushButtonCancel, SIGNAL(clicked()), this,SLOT(reject()));
	setAttribute (Qt::WA_DeleteOnClose);
}

ParfuheScoreDlg::~ParfuheScoreDlg()
{
}

void ParfuheScoreDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));

	double score = (this->ui.lineEditScore->text()).toDouble();
	this->mainWindow->setScore(score);

	accept();
}
