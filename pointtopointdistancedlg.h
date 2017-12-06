#ifndef POINTTOPOINTDISTANCEDLG_H
#define POINTTOPOINTDISTANCEDLG_H

#include <QDialog>
#include "ui_pointtopointdistancedlg.h"

class PointToPointDistanceDlg : public QDialog
{
	Q_OBJECT

public:
	PointToPointDistanceDlg(QWidget *parent = 0);
	~PointToPointDistanceDlg();

	void setDistance(double distance);

public slots:
	void okButtonClicked();

private:
	Ui::PointToPointDistanceDlg ui;
};

#endif // POINTTOPOINTDISTANCEDLG_H
