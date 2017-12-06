#include "dicloserelationdlg.h"

#include "mainwindow.h"

DiCloseRelationDlg::DiCloseRelationDlg(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	mainWindow = (MainWindow*)(parent);

	classNum = 1;

	connect(this->ui.pushButtonOk, SIGNAL(clicked()), this,SLOT(okButtonClicked()));

	connect(this->ui.radioButtonClass1, SIGNAL(clicked(bool)), this, SLOT(radioChange()));
	connect(this->ui.radioButtonClass2, SIGNAL(clicked(bool)), this, SLOT(radioChange()));
	connect(this->ui.radioButtonClass3, SIGNAL(clicked(bool)), this, SLOT(radioChange()));
	connect(this->ui.radioButtonClass4, SIGNAL(clicked(bool)), this, SLOT(radioChange()));
	connect(this->ui.radioButtonClass5, SIGNAL(clicked(bool)), this, SLOT(radioChange()));
	setAttribute (Qt::WA_DeleteOnClose);
}

DiCloseRelationDlg::~DiCloseRelationDlg()
{
}

void DiCloseRelationDlg::okButtonClicked()
{
	if(classNum == 1 || classNum == 2 || classNum == 3)
	{
		double score = 2 * (classNum - 1);
		mainWindow->setScore(score);
	}
	else if(classNum == 4 || classNum == 5)
	{
		cout<<classNum<<endl;
		mainWindow->setDiCloseRelationType(classNum - 3);
		mainWindow->initCloseRelationMeasureTreeWidget();
		//mainWindow->initCloseRelationMeasureTreeWidget2();
	}


	accept();
}

void DiCloseRelationDlg::radioChange()
{
	if(sender() == this->ui.radioButtonClass1)
	{
		classNum = 1;
		this->ui.lineEditCloseRelationScore->setEnabled(true);
	}
	else if(sender() == this->ui.radioButtonClass2)
	{
		classNum = 2;
		this->ui.lineEditCloseRelationScore->setEnabled(true);
	}
	else if(sender() == this->ui.radioButtonClass3)
	{
		classNum = 3;
		this->ui.lineEditCloseRelationScore->setEnabled(true);
	}
	else if(sender() == this->ui.radioButtonClass4)
	{
		classNum = 4;
		this->ui.lineEditCloseRelationScore->setEnabled(false);
	}
	else if(sender() == this->ui.radioButtonClass5)
	{
		classNum = 5;
		this->ui.lineEditCloseRelationScore->setEnabled(false);
	}
}
