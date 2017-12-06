#include <QDialog>

#include "ui_parfuhe.h"

class MainWindow;

class ParfuheScoreDlg : public QDialog
{
	Q_OBJECT

public:
	ParfuheScoreDlg(QWidget *parent = 0);
	~ParfuheScoreDlg();

	public slots:
		void okButtonClicked();

public:
	MainWindow* mainWindow;
	Ui::ParfuheScoreDlg ui;
};

// OVERBITELOWERPAIRPOINTSCOREDLG_H
