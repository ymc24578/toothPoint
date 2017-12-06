#include "pointtopointdistancedlg.h"

PointToPointDistanceDlg::PointToPointDistanceDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));
}

PointToPointDistanceDlg::~PointToPointDistanceDlg()
{
}

void PointToPointDistanceDlg::okButtonClicked()
{
	//QMessageBox::information(NULL, tr("Action"), QString(tr("ok button clicked...")));

	accept();
}

void PointToPointDistanceDlg::setDistance(double distance)
{
	this->ui.label_distance->setText(QString::number(distance));
}
