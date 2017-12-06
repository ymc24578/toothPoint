#include "mainwindow.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorStyle.h>
#include <vtkTDxInteractorStyleCamera.h>
#include <vtkTDxInteractorStyleSettings.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkOBJReader.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkCellArray.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkCutter.h>
#include <vtkLineSource.h>
#include <vtkOBBTree.h>
#include <vtkMath.h>
#include <vtkAppendPolyData.h>
#include <vtkBoundingBox.h>
#include <vtkKdTree.h>
#include <vtkKdTreePointLocator.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QSqlError>
#include <QSqlQuery>

#include <map>
#include <vector>
#include <fstream>
#include "pickInteractorStyle.h"
#include "vtkRansacPlane.h"
#include <string>
#include <math.h>
#include <vtkIdList.h>
#include <vtkTriangle.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>

#include <list>

#include "fitplanedlg.h"
#include "verticalplanescoredlg.h"
#include "facesidescoredlg.h"
#include "middlelinescoredlg.h"
#include "OverbiteIntersectCurveScoreDlg.h"
#include "overbitelowerpairpointscoredlg.h"
#include "pointtopointdistancedlg.h"
#include "saveDlg.h"
#include "loadDlg.h"
#include "diotherscoredlg.h"
#include "dicloserelationdlg.h"
#include "parfuhe.h"
#include "diBackToothLockScoreDlg.h"
#include "diBackToothOppositeScoreDlg.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	ui.setupUi(this);

	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("127.0.0.1");
	db.setDatabaseName("test");
	db.setUserName("root");
	db.setPassword("");
	if(!db.open())
	{
		QMessageBox::critical(NULL, tr("Database Error"), db.lastError().text());
	}

	QDir savePath("./data");
	if(!savePath.exists())
	{
		savePath.mkpath("./data");
	}

	//*2D
	/*
	iniCoinImages();
	createActions();
	setupMenus();
	loadMesureProg();
	//设置 glwidget pwidget
	createGridGroupBox();
	groupBox2D->setVisible(false);
	groupBox3D->setVisible(false);
	//modelorder[0]=-1;//初始时没有选中
	isFull=false;
	isconnected=true;
	MinWidgetNum=4;
	selectedNum=0;
	pickerCount = 0;
	Mysetlayout();
	// 初始化对话框
	iniDialogs();
	//resize(1024,768);
	resize(1024,600);
	//resize(600,400);
	setWindowTitle(tr("测量软件"));
	//showFullScreen();
	//showMaximized();

	outPoint();
	*/
	//&2D
	pickerCount = 0;
	connect(this->ui.saveButton, SIGNAL(clicked()), this, SLOT(saveModelInfo()));

	this->ui.dataTreeWidget->setHeaderLabel(tr("Database"));
	initDataWidget();

	// 选中点的颜色
	pickPointColor[0] = 1;
	pickPointColor[1] = 0;
	pickPointColor[2] = 0;

	// 下牙列选中点在合平面上投影点的颜色
	projectToFittingPlanePointColot[0] = 0;
	projectToFittingPlanePointColot[1] = 1;
	projectToFittingPlanePointColot[2] = 0;

	// 上牙列选中点在合平面上投影点的颜色
	topProjectToFittingPlanePointColot[0] = 1;
	topProjectToFittingPlanePointColot[1] = 0;
	topProjectToFittingPlanePointColot[2] = 0;

	planeColor[0] = 0.5647;
	planeColor[1] = 0.6235;
	planeColor[2] = 0.5647;

	frameColor[0] = 0.5882;
	frameColor[1] = 0.7569;
	frameColor[2] = 0.5843;

	intersectLlineColor[0] = 0;
	intersectLlineColor[1] = 1;
	intersectLlineColor[2] = 1;

	intersectCurveColor[0] = 0;
	intersectCurveColor[1] = 0;
	intersectCurveColor[2] = 1;

	topIntersectCurveColor[0] = 1;
	topIntersectCurveColor[1] = 0;
	topIntersectCurveColor[2] = 0;

	QDir dir;
    path = dir.currentPath();
	//QMessageBox::information(NULL, tr("Info"), path);

	//
	// 初始化右键菜单
	//

	this->ui.qvtkObjectViewerWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	contexMenu = NULL;
	connect(this->ui.qvtkObjectViewerWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	//
	// 初始化Matlab引擎
	//

	if (!(ep = engOpen(NULL))) 
	{
		QMessageBox::information(NULL, tr("Error"), QString("Can't start MATLAB engine."));
		return;
	}
	engSetVisible(ep, false);

	//
	// 初始化操作栏
	//

	selectOptionTreeIndex = 0;
	this->ui.optionTreeWidget->setHeaderLabel(tr("Options Column"));
	this->ui.optionTreeWidget->clear();
	connect(this->ui.optionTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, 
		SLOT(clickOptionTreeItem(QTreeWidgetItem*, int)));

	//
	// 初始化主操作区
	//

	vtkRenderWindow* renwin = vtkRenderWindow::New();
	renwin->StereoCapableWindowOn();

	this->ui.qvtkObjectViewerWidget->SetUseTDx(true);
	this->ui.qvtkObjectViewerWidget->SetRenderWindow(renwin);
	renwin->Delete();

	const double angleSensitivity = 0.02;
	const double translationSensitivity = 0.001;

	QVTKInteractor *iren = this->ui.qvtkObjectViewerWidget->GetInteractor();
	vtkInteractorStyle *s = static_cast<vtkInteractorStyle *>(iren->GetInteractorStyle());
	vtkTDxInteractorStyleCamera *t= static_cast<vtkTDxInteractorStyleCamera *>(s->GetTDxStyle());

	t->GetSettings()->SetAngleSensitivity(angleSensitivity);
	t->GetSettings()->SetTranslationXSensitivity(translationSensitivity);
	t->GetSettings()->SetTranslationYSensitivity(translationSensitivity);
	t->GetSettings()->SetTranslationZSensitivity(translationSensitivity);

	ren = vtkRenderer::New();
	this->ui.qvtkObjectViewerWidget->GetRenderWindow()->AddRenderer(ren);

	//
	// 初始化菜单
	//
	connect(this->ui.LoadButton, SIGNAL(clicked()), this, SLOT(loadModelInfo()));
	connect(this->ui.actionLoad_Model, SIGNAL(triggered()), this, SLOT(loadModel()));

	connect(this->ui.Hide, SIGNAL(clicked()), this, SLOT(hideSelect()));
	connect(this->ui.Show, SIGNAL(clicked()), this, SLOT(showSelect()));

	connect(this->ui.Pick_point, SIGNAL(clicked()), this, SLOT(pickPoint()));
	////////////////////////////////////////////////////////////////////////////////////////////
	connect(this->ui.PickPoint, SIGNAL(triggered()), this, SLOT(iPickPoint()));
	connect(this->ui.action_4, SIGNAL(triggered()), this, SLOT(iFittingPlane()));
	connect(this->ui.action_5, SIGNAL(triggered()), this, SLOT(iProjectPoint()));
	connect(this->ui.action_6, SIGNAL(triggered()), this, SLOT(iCutCurve()));
	connect(this->ui.action_9, SIGNAL(triggered()), this, SLOT(iVector()));
	connect(this->ui.action_10, SIGNAL(triggered()), this, SLOT(iPlaneVector()));
	connect(this->ui.action_13, SIGNAL(triggered()), this, SLOT(iPointToPointDistance()));
	connect(this->ui.action_14, SIGNAL(triggered()), this, SLOT(iPointToVectorDistance()));
	connect(this->ui.action_15, SIGNAL(triggered()), this, SLOT(iPointToPlaneDistance()));
	connect(this->ui.action_16, SIGNAL(triggered()), this, SLOT(iPlaneToPlaneDistance()));
	connect(this->ui.action_17, SIGNAL(triggered()), this, SLOT(iVectorAngle()));
	connect(this->ui.action_18, SIGNAL(triggered()), this, SLOT(iPointsAngle()));
	connect(this->ui.action_19, SIGNAL(triggered()), this, SLOT(itoothAngle1()));
	connect(this->ui.action_20, SIGNAL(triggered()), this, SLOT(itoothAngle2()));

	connect(this->ui.Delete_point, SIGNAL(clicked()), this, SLOT(deletePoint()));
	connect(this->ui.Project, SIGNAL(clicked()), this, SLOT(projectPoint()));
	connect(this->ui.Distance, SIGNAL(clicked()), this, SLOT(distancePoint()));

	connect(this->ui.Fittingplane, SIGNAL(clicked()), this, SLOT(calFittingPlane()));
	connect(this->ui.Fittingcurve, SIGNAL(clicked()), this, SLOT(calFittingCurve()));

	connect(this->ui.Score, SIGNAL(clicked()), this, SLOT(score()));

	connect(this->ui.DentalSeg, SIGNAL(clicked()), this, SLOT(segment()));
	connect(this->ui.ResultSave, SIGNAL(clicked()), this, SLOT(saveRes()));
	connect(this->ui.BeginSeg, SIGNAL(clicked()), this, SLOT(StartMeasure()));

	connect(this->ui.Add, SIGNAL(clicked()), this, SLOT(additem()));
	connect(this->ui.Ppointdistance, SIGNAL(triggered()), this, SLOT(measureDistancePointToPoint()));
	


	//
	// 测量
	//

	// 点-点
	measureDistancePointToPointIndex = 0;
	for(int i = 0; i < 2; ++i)
	{
		measureDistancePointToPointPointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	measureDistancePointToPointLineActor = vtkSmartPointer<vtkActor>::New();
	ismeasureDistancePointToPointShow = 0;
	
	
}

MainWindow::~MainWindow()
{
	if(ren)
		ren->Delete();
	if(pickStyle)
		pickStyle->Delete();
	if(contexMenu)
		delete contexMenu;

	engClose(ep);

	db.close();
}

void MainWindow::saveInput(OperateItem it)
{
	OperateList.push_back(it);
}

int MainWindow::returnselectOptionTreeIndex()
{
	return selectOptionTreeIndex;
}

void MainWindow::initDataWidget()
{
	this->ui.dataTreeWidget->clear();
	QString strToothinfo = QString("create table if not exists modelName(id INTEGER PRIMARY KEY, name nvarchar(50))");

	QString strSelectTooth = QString("SELECT id, name FROM toothInfo");
	QSqlQuery queryTooth;
	queryTooth.exec(strToothinfo);
	queryTooth.exec(strSelectTooth);
	while(queryTooth.next())
	{
		int toothId = queryTooth.value(0).toInt();
		QString toothNo = queryTooth.value(1).toString();
		QTreeWidgetItem *toothItem = new QTreeWidgetItem(this->ui.dataTreeWidget, QStringList(toothNo));

		QString queryDiCoverString = QString("SELECT id, toothId, score, distance FROM diCover WHERE toothId = ").append(QString::number(toothId));
		QSqlQuery queryDiCover;
		queryDiCover.exec(queryDiCoverString);
		while(queryDiCover.next())
		{
			double distance = queryDiCover.value(3).toDouble();
			double score = queryDiCover.value(2).toDouble();

			QString itemString = tr("Cover");
			QTreeWidgetItem *diCoverItem = new QTreeWidgetItem(toothItem, QStringList(itemString));
			toothItem->addChild(diCoverItem);

			itemString = tr("Distance");
			QTreeWidgetItem *diCoverDistanceItem = new QTreeWidgetItem(diCoverItem, QStringList(itemString));
			diCoverItem->addChild(diCoverDistanceItem);
			itemString = tr("Score");
			QTreeWidgetItem *diCoverScoreItem = new QTreeWidgetItem(diCoverItem, QStringList(itemString));
			diCoverItem->addChild(diCoverScoreItem);

			QTreeWidgetItem *diCoverDistanceInfoItem = new QTreeWidgetItem(diCoverDistanceItem, QStringList(QString::number(distance)));
			diCoverDistanceItem->addChild(diCoverDistanceInfoItem);
			QTreeWidgetItem *diCoverScoreInfoItem = new QTreeWidgetItem(diCoverScoreItem, QStringList(QString::number(score)));
			diCoverScoreItem->addChild(diCoverScoreInfoItem);
		}
	}
}

void MainWindow::saveModelInfo()
{
	//QMessageBox::information(NULL, tr("Info"), tr("Save button clicked...")); 
	SaveDlg dlg(this);
	dlg.exec();
}

void MainWindow::loadModelInfo()
{
	//QMessageBox::information(NULL, tr("Info"), tr("Save button clicked...")); 
	LoadDlg dlg(this);
	dlg.exec();
}

void MainWindow::saveData(QString no)
{
	//QMessageBox::information(NULL, tr("Info"), no);
// 	QSqlQuery query;
// 	query.prepare("INSERT INTO toothInfo(no) VALUES(:no)");
// 	query.bindValue(":no", no);
// 	query.exec();
// 
// 	QString queryString = QString("SELECT id, no FROM toothInfo WHERE no = '").append(no).append("'");
// 	QSqlQuery queryTooth;
// 	queryTooth.exec(queryString);
// 	int toothId;
// 	while(queryTooth.next())
// 	{
// 		toothId = queryTooth.value(0).toInt();
// 	}
// 	//QMessageBox::information(NULL, tr("Info"), QString::number(toothId));
// 
// 	if(toothId > 0)
// 	{
// 		QSqlQuery queryInsertDiCover;
// 		queryInsertDiCover.prepare("INSERT INTO diCover(toothId, score, distance) VALUES(:toothId, :score, :distance)");
// 		queryInsertDiCover.bindValue(":toothId", toothId);
// 		queryInsertDiCover.bindValue(":score", diCoverScore);
// 		queryInsertDiCover.bindValue(":distance", diCoverDistance);
// 		queryInsertDiCover.exec();
// 	}
// 
// 	initDataWidget();

// 	QFile outFile(no);
// 	if (!outFile.open(QIODevice::WriteOnly))
// 	{
// 		QMessageBox::information(QApplication::activeWindow(),"information","Open Failed!");
// 	}
// 	QTextStream out(&outFile);
// 	for (int i=0;i<outList.size();i++)
// 	{
// 		out<<outList.at(i);
// 	}
// 	outFile.close();
// 
// 	QMessageBox::information(QApplication::activeWindow() ,"information",tr("输出成功!\r\n结果保存在："+saveFileName));
// 	QString strToothinfo = QString("create table if not exists ? (id INTEGER PRIMARY KEY, num int, mode int, x double, y double, z double)");
// 	QSqlQuery sql_query;
// 	sql_query.prepare(strToothinfo);
// 	sql_query.addBindValue(no);
// 	sql_query.exec();
// 	QString select_max_sql = "select max(id) from ?";
// 	int max_id = 0;
// 	sql_query.prepare(select_max_sql);
// 	sql_query.addBindValue(no);
// 	if(!sql_query.exec())
// 	{
// 		qDebug()<<sql_query.lastError();
// 	}
// 	else
// 	{
// 		while(sql_query.next())
// 		{
// 			max_id = sql_query.value(0).toInt();
// 			qDebug()<<QString("max id:%1").arg(max_id);
// 		}
// 	}
// 	for (int it = 0;it < OperateList.size();it++)
// 	{
// 		int Num = OperateList[it].OpeNum;
// 		int Mode = OperateList[it].OpeMode;
// 		double x = OperateList[it].x;
// 		double y = OperateList[it].y;
// 		double z = OperateList[it].z;
// 		QString insert_sql = "insert into ? values (?, ?, ?, ?, ?, ?)";
// 		sql_query.prepare(insert_sql);
// 		sql_query.addBindValue(no);
// 		sql_query.addBindValue(++max_id);
// 		sql_query.addBindValue(Num);
// 		sql_query.addBindValue(Mode);
// 		sql_query.addBindValue(x);
// 		sql_query.addBindValue(y);
// 		sql_query.addBindValue(z);
// 		if(!sql_query.exec())
// 		{
// 			qDebug()<<sql_query.lastError();
// 		}
// 		else
// 		{
// 			qDebug()<<"inserted!";
// 		}
// 
// 	}

	QFile outFile(no);
	if (!outFile.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(QApplication::activeWindow(),"information","Open Failed!");
	}
	QTextStream out(&outFile);

	
	for (int it = 0;it < OperateList.size();it++)
	{
		int Num = OperateList[it].OpeNum;
		//int Mode = OperateList[it].OpePickMode;
		int type = OperateList[it].Opetype;
		QTreeWidgetItem *tree = OperateList[it].treeItem;
// 		double x = OperateList[it].x;
// 		double y = OperateList[it].y;
// 		double z = OperateList[it].z;
		out<<Num<<' '<<type<<endl;
		cout<<"load "<<Num<<' '<<type<<endl;
		if(type == 0)//select
		{
			int Mode = OperateList[it].OpePickMode;
			double x = OperateList[it].x;
			double y = OperateList[it].y;
			double z = OperateList[it].z;
			out<<Mode<<' '<<x<<' '<<y<<' '<<z<<endl;
			//out<<Mode<<endl;
			cout<<"load "<<Mode<<' '<<x<<' '<<y<<' '<<z;
		}

	}
	outFile.close();
	OperateList.clear();

}

void MainWindow::loadInput()
{
	for (int it = 0;it < InputOperateList.size();it++)
	{
		OperateItem tempOI = InputOperateList[it];
		int mode = tempOI.OpePickMode;
		selectOptionTreeIndex = tempOI.OpeNum;
		pickPoint();
		double currentSelectPosition[3] = {};
		currentSelectPosition[0] = tempOI.x;
		currentSelectPosition[1] = tempOI.y;
		currentSelectPosition[2] = tempOI.z;
		if(mode == 1)
		{
			setPickPointPosition(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);
		}
		else if(mode == 2)
		{
			setPickPointForMeasureDistancePointToPoint(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);				
		}
		else if (mode == 3)
		{
			setSegPickPointPoisition(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);		
		}
	}
	
}

void MainWindow::loadData(QString no)//load data into InputOperateList
{
// 	QString queryString = QString("select * from ?");
// 	QSqlQuery query;
// 	query.prepare(queryString);
// 	query.addBindValue(no);
// 	query.exec();
// 	InputOperateList.clear();
// 	if(query.next())
// 	{
// 		OperateItem tempOpe;
// 		int id = query.value(0).toInt();
// 		tempOpe.OpeNum = query.value(1).toInt();
// 		tempOpe.OpePickMode = query.value(2).toInt();
// 		tempOpe.x = query.value(3).toDouble();
// 		tempOpe.y = query.value(4).toDouble();
// 		tempOpe.z = query.value(5).toDouble();
// 		InputOperateList.push_back(tempOpe);
// 	}
	ifstream inF;
	inF.open("pointlist.txt");
	double coor[10000][3] = {};
	int ind = 0;
	int ttt;
	QString FN = "p-";
	FN = FN + no;
	QFile inFile(no);
	QFile inFile2(FN);
	if(inFile.open(QIODevice::ReadOnly))
	{
		cout<<"load success"<<endl;
	}
	inFile2.open(QIODevice::ReadOnly);
	QTextStream in(&inFile); 
	QTextStream in2(&inFile2); 
	while (!in.atEnd())
	{
		int Num;
		in >> Num;
		int type;
		in >> type;
		void *t;
// 		QTreeWidgetItem *tree;
// 		in >> t;
// 		tree = (QTreeWidgetItem *)t
		cout<<"in:"<<Num<<' '<<type<<endl;
		selectOptionTreeIndex = Num;
		//QTreeWidgetItem *currentOptionTreeWidgetItem;
		QTreeWidgetItemIterator it(this->ui.optionTreeWidget);
		int count = 0;
		while (*it) {
			//do something like
			//->text(0);
			QVariant data = (*it)->data(0, Qt::UserRole);
			if (selectOptionTreeIndex ==  getSelectOptionTreeIndex(data))
			{
				currentOptionTreeWidgetItem = *it;
				count ++;
				QString st = (*it)->text(0);
				QByteArray ss(st.toAscii());
				cout<<ss.data()<<endl;
			}
			++it;
		}
		cout<<"OpeIndex HitNum:"<<count<<endl;
		//selectOptionTreeIndex = getSelectOptionTreeIndex(data);
		//currentOptionTreeWidgetItem = tree;
		if (type == 0)
		{
			int mode;
			double x,y,z;
			in >> mode;
			int pointIndex;
			in2 >> pointIndex;
			cout<<'E'<<endl;
			x = pointlist[pointIndex][0];
			y = pointlist[pointIndex][1];
			z = pointlist[pointIndex][2];
			coor[ind][0] = x;
			coor[ind][1] = y;
			coor[ind][2] = z;
			ind ++;
			cout<<"in:"<<mode<<' '<<x<<' '<<y<<' '<<z<<endl;
			OperateItem It;
			It.Opetype = 0;
			It.OpePickMode = mode;
			It.OpeNum = selectOptionTreeIndex;
			It.x = x;
			It.y = y;
			It.z = z;
			saveInput(It);
			double currentSelectPosition[3] = {};
			currentSelectPosition[0] = x;
			currentSelectPosition[1] = y;
			currentSelectPosition[2] = z;
			if(mode == 1)
			{
				setPickPointPosition(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);
			}
			else if(mode == 2)
			{
				setPickPointForMeasureDistancePointToPoint(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);				
			}
			else if (mode == 3)
			{
				setSegPickPointPoisition(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);		
			}

		}
		else
		{
			switch (type)
			{
			case 1:
				pickPoint();
				break;
			case 2:
				deletePoint();
				break;
			case 3:
				projectPoint();
				break;
			case 4:
				distancePoint();
				break;
			case 5:
				calFittingPlane();
				break;
			case 6:
				calFittingCurve();
				break;
			case 7:
				additem();
				break;
			case 8:
				measureDistancePointToPoint();
				break;
			}
		}
	}
	inF.close();

	OperateList.clear();
// 	inFile.close();
// 	ofstream ofile;
// 	ofile.open("pointlist.txt");
// 	for (int i = 0;i < ind;i ++)
// 	{
// 		ofile << coor[i][0]<<' '<<coor[i][1]<<' '<<coor[i][2]<<endl;
// 	}
// 	ofile.close();
	//loadInput();
}

void MainWindow::saveRes()
{
	cout<<"in save"<<endl;
	ofstream out;
	out.open("res.csv");
	/*QTreeWidgetItemIterator it(ui.optionTreeWidget);*/
// 	while (*it) {
// 		//do something like
// 		QString str = (*it)->text(0);
// 		QByteArray ba(str.toAscii());//toAscii()返回一个QByteArray类型  
// 		out << ba.data()<<endl;//data()返回char *  
// 		++it;
// 	}
	
	int n = this->ui.optionTreeWidget->topLevelItemCount();
	for (int i = 0;i < n;i ++)
	{
		QTreeWidgetItem *st = this->ui.optionTreeWidget->topLevelItem(i);
		QString str = st->text(0);
		//QByteArray ba(str.toAscii());//toAscii()返回一个QByteArray类型  
		//out << ba.data()<<endl;//data()返回char *  
		int m = st->childCount();
		for (int j = 0;j < m;j ++)
		{
			QTreeWidgetItem *st2 = st->child(j);
			QString str2 = st2->text(0);
			//QByteArray ba(str2.toAscii());//toAscii()返回一个QByteArray类型  
			//out << ba.data()<<endl;//data()返回char *  
			int q = st2->childCount();
			for (int k = 0;k < q;k ++)
			{
				QTreeWidgetItem *st3 = st2->child(k);
				QString str3 = st3->text(0);
				if (str3 == QString("距离")||str3 == QString("得分"))
				{
					if (st3->childCount() != 0)
					{
						//QByteArray ba(str3.toAscii());//toAscii()返回一个QByteArray类型  
						//out << ba.data()<<endl;//data()返回char *  
						QTreeWidgetItem *st4 = st3->child(0);
						QString str4 = st4->text(0);
						str4 = str + str2 + str3 + "," + str4;
						QByteArray ba2(str4.toAscii());//toAscii()返回一个QByteArray类型  
						out << ba2.data()<<endl;//data()返回char *  
						continue;
					}
					
				}
				int p = st3->childCount();
				for (int l = 0;l < p;l ++)
				{
					QTreeWidgetItem *st4 = st3->child(l);
					QString str4 = st4->text(0);
					if (str4 == QString("距离")||str4 == QString("得分"))
					{
						if (st4->childCount() != 0)
						{
							//QByteArray ba(str3.toAscii());//toAscii()返回一个QByteArray类型  
							//out << ba.data()<<endl;//data()返回char *  
							QTreeWidgetItem *st5 = st4->child(0);
							QString str5 = st5->text(0);
							str5 = str + str2 + str3 + str4 + "," + str5;
							QByteArray ba2(str5.toAscii());//toAscii()返回一个QByteArray类型  
							out << ba2.data()<<endl;//data()返回char *  
							continue;
						}

					}
					int r = st4->childCount();
					for (int c = 0;c < r;c ++)
					{
						QTreeWidgetItem *st5 = st4->child(c);
						QString str5 = st5->text(0);
						if (str5 == QString("距离")||str5 == QString("得分"))
						{
							if (st5->childCount() != 0)
							{
								//QByteArray ba(str3.toAscii());//toAscii()返回一个QByteArray类型  
								//out << ba.data()<<endl;//data()返回char *  
								QTreeWidgetItem *st6 = st5->child(0);
								QString str6 = st6->text(0);
								str6 = str + str2 + str3 + str4 + str5 + "," + str6;
								QByteArray ba2(str6.toAscii());//toAscii()返回一个QByteArray类型  
								out << ba2.data()<<endl;//data()返回char *  
								continue;
							}

						}

					}
				}

			}
		}
	}
	out.close();
}

void MainWindow::ScanTree()
{
	QTreeWidgetItemIterator it(ui.optionTreeWidget);
	while (*it) {
		//do something like
		(*it)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
		(*it)->setCheckState(0, Qt::Unchecked);
		++it;
	}
}
void MainWindow::showTop()
{
	if(isTopToothShow == 0)
		switchTopToothShow();
}
void MainWindow::showLow()
{
	if(isLowToothShow == 0)
		switchLowToothShow();
}
void MainWindow::hideTop()
{
	if(isTopToothShow == 1)
		switchTopToothShow();
}
void MainWindow::hideLow()
{
	if(isLowToothShow == 1)
		switchLowToothShow();
}



void MainWindow::showContextMenu(const QPoint& pos)
{
	if(contexMenu)
    {
        delete contexMenu;
        contexMenu = NULL;
	}
	contexMenu = new QMenu(this->ui.qvtkObjectViewerWidget);

	QMenu *showMenu = contexMenu->addMenu(tr("Show"));
	QMenu *hideMenu = contexMenu->addMenu(tr("Hide"));
	QAction *pickPointAction = contexMenu->addAction(tr("pick point"));
	connect(pickPointAction, SIGNAL(triggered(bool)), this, SLOT(pickPoint()));

	QAction *showTopAction = showMenu->addAction(tr("Top"));
	QAction *showLowAction = showMenu->addAction(tr("Low"));
	QAction *hideTopAction = hideMenu->addAction(tr("Top"));
	QAction *hideLowAction = hideMenu->addAction(tr("Low"));
	connect(showTopAction, SIGNAL(triggered(bool)), this, SLOT(showTop()));
	connect(showLowAction, SIGNAL(triggered(bool)), this, SLOT(showLow()));
	connect(hideTopAction, SIGNAL(triggered(bool)), this, SLOT(hideTop()));
	connect(hideLowAction, SIGNAL(triggered(bool)), this, SLOT(hideLow()));
// 	QMenu *pointOptionMenu = contexMenu->addMenu(tr("Point option"));
// 	QAction *pickPointAction = pointOptionMenu->addAction(tr("pick point"));
// 	QAction *projectAction = pointOptionMenu->addAction(tr("project"));
// 	QAction *distanceAction = pointOptionMenu->addAction(tr("distance"));
// 	QAction *deletePointAction = pointOptionMenu->addAction(tr("delete point"));
// 	connect(pickPointAction, SIGNAL(triggered(bool)), this, SLOT(pickPoint()));
// 	connect(projectAction, SIGNAL(triggered(bool)), this, SLOT(projectPoint()));
// 	connect(distanceAction, SIGNAL(triggered(bool)), this, SLOT(distancePoint()));
// 	connect(deletePointAction, SIGNAL(triggered(bool)), this, SLOT(deletePoint()));
// 
// 	QMenu *fittingMenu = contexMenu->addMenu(tr("Fitting"));
// 	QAction *fittingPlaneAction = fittingMenu->addAction(tr("Fitting plane"));
// 	QAction *fittingCurveAction = fittingMenu->addAction(tr("Fitting curve"));
// 	connect(fittingPlaneAction, SIGNAL(triggered(bool)), this, SLOT(calFittingPlane()));
// 	connect(fittingCurveAction, SIGNAL(triggered(bool)), this, SLOT(calFittingCurve()));
// 
// 	contexMenu->addSeparator();
// 
// 	QAction *pauseRotateAction = contexMenu->addAction(tr("Pause Rotate"));
// 	connect(pauseRotateAction, SIGNAL(triggered(bool)), this, SLOT(pause()));
// 
// 	QAction *continueRotateAction = contexMenu->addAction(tr("Continue Rotate"));
// 	connect(continueRotateAction, SIGNAL(triggered(bool)), this, SLOT(continueRotate()));

	QPoint position = QCursor::pos();
	position.setX(position.x() - 3);
	position.setY(position.y() - 3);
	contexMenu->exec(position);
}

void MainWindow::pause()
{
	QMessageBox::information(NULL, tr("Info"), tr("Pause...")); 
}

void MainWindow::continueRotate()
{
	QMessageBox::information(NULL, tr("Info"), tr("Continue rotate...")); 
}

void MainWindow::loadModel()
{
	this->ui.optionTreeWidget->clear();

	OperateList.clear();
	diOverbiteFlag = 0;


	//
	// 初始化牙列操作栏
	//

	QString itemText = tr("Tooth");
	QTreeWidgetItem *modelItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	modelItem->setData(0, Qt::UserRole, 111);

	
	// 下牙列
	QFileDialog *lowFileDialog = new QFileDialog(this); 
	lowFileDialog->setWindowTitle(tr("Open Lower Tooth Model")); 
	lowFileDialog->setDirectory("."); 
	lowFileDialog->setFilter(tr("Model Files(*.obj *.stl)"));

	while(true)
	{
		if(lowFileDialog->exec() == QDialog::Accepted) 
		{
			lowerModelName = lowFileDialog->selectedFiles()[0]; 
			//string n = lowerModelName;
			//cout<<n<<endl;
			vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
			if(lowerModelName.endsWith("stl", Qt::CaseInsensitive))
			{
				vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
				reader->SetFileName(qPrintable(lowerModelName));
				reader->Update();

				triangleFilter->SetInputConnection(reader->GetOutputPort());
				triangleFilter->Update();
				lowerToothData = triangleFilter->GetOutput();
			}
			else if(lowerModelName.endsWith("obj", Qt::CaseInsensitive))
			{
				vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();			
				reader->SetFileName(qPrintable(lowerModelName));
				reader->Update();
				
				triangleFilter->SetInputConnection(reader->GetOutputPort());
				triangleFilter->Update();
				lowerToothData = triangleFilter->GetOutput();
			}			

			vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(triangleFilter->GetOutputPort());

			lowerToothActor = vtkSmartPointer<vtkActor>::New();
			lowerToothActor->SetMapper(mapper);

			break;
		}
		else 
		{ 
			QMessageBox::information(NULL, tr("Info"), tr("You didn't select any files.\nPlease select one...")); 
		}
	}

	itemText = tr("Low Tooth");
	QTreeWidgetItem *lowModelItem = new QTreeWidgetItem(modelItem, QStringList(itemText));
	lowModelItem->setData(0, Qt::UserRole, 2);
	modelItem->addChild(lowModelItem);

	// 上牙列
	QFileDialog *topFileDialog = new QFileDialog(this); 
	topFileDialog->setWindowTitle(tr("Open Top Tooth Model")); 
	topFileDialog->setDirectory("."); 
	topFileDialog->setFilter(tr("Model Files(*.obj *.stl)"));

	while(true)
	{
		if(topFileDialog->exec() == QDialog::Accepted) 
		{
			topModelName = topFileDialog->selectedFiles()[0]; 

			vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
			if(topModelName.endsWith("stl", Qt::CaseInsensitive))
			{
				vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();			
				reader->SetFileName(qPrintable(topModelName));
				reader->Update();

				
				triangleFilter->SetInputConnection(reader->GetOutputPort());
				triangleFilter->Update();
				topToothData = triangleFilter->GetOutput();
			}
			else if(topModelName.endsWith("stl", Qt::CaseInsensitive))
			{
				vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();			
				reader->SetFileName(qPrintable(topModelName));
				reader->Update();

				triangleFilter->SetInputConnection(reader->GetOutputPort());
				triangleFilter->Update();
				topToothData = triangleFilter->GetOutput();
			}

			vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(triangleFilter->GetOutputPort());

			topToothActor = vtkSmartPointer<vtkActor>::New();
			topToothActor->SetMapper(mapper);

			break;
		}
		else 
		{ 
			QMessageBox::information(NULL, tr("Info"), tr("You didn't select any files.\nPlease select one...")); 
		}
	}

	itemText = tr("Top Tooth");
	QTreeWidgetItem *topModelItem = new QTreeWidgetItem(modelItem, QStringList(itemText));
	topModelItem->setData(0, Qt::UserRole, 1);
	modelItem->addChild(topModelItem);
	modelItem->setExpanded(true);

	ifstream fin;
	
	QString file_full, file_name, file_path;  
	QFileInfo fi;  

	file_full = lowerModelName;
	fi = QFileInfo(file_full);  
	file_name = fi.fileName();  
	file_path = fi.absolutePath(); 

	QString TT = file_name;
	int len = TT.size();
	QByteArray ba = TT.toLatin1();
	char* n = ba.data();
	string nto = ".txt";
	string nt = n;
	// 		char nt[100];
	// 		for (int j = 0;j < len;j ++)
	// 		{
	// 			nt[j] = n[len - 13 + j];
	// 		}
	/*strcat(nt,nto);*/
	nt = nt + nto;
	cout<<nt<<endl;

	fin.open(nt);
	for (int i = 0;i < 194;i ++)
	{
		for (int j = 0;j < 3;j ++)
		{
			fin >> pointlist[i][j];
		}
	}
	fin.close();
	//vtkSmartPointer<vtkPolyData> input1 = vtkSmartPointer<vtkPolyData>::New();
	//vtkSmartPointer<vtkPolyData> input2 = vtkSmartPointer<vtkPolyData>::New();
	//input1->DeepCopy(topToothData);
	//input2->DeepCopy(lowerToothData);

	//vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	//appendFilter->AddInput(topToothData);
	//appendFilter->AddInput(lowerToothData);
	//appendFilter->Update();

	//toothData = appendFilter->GetOutput();

	init();
	if(ren)
	{
		this->ren->RemoveAllViewProps();
		this->ui.qvtkObjectViewerWidget->GetRenderWindow()->RemoveRenderer(ren);
		ren->Delete();
	}
	ren = vtkRenderer::New();

	ren->AddViewProp(lowerToothActor);
	ren->AddViewProp(topToothActor);
	isLowToothShow = isTopToothShow = 1;

	this->ui.qvtkObjectViewerWidget->GetRenderWindow()->AddRenderer(ren);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = this->ui.qvtkObjectViewerWidget->GetRenderWindow()->GetInteractor();
	renderWindowInteractor->SetRenderWindow(this->ren->GetRenderWindow());
	renderWindowInteractor->Initialize();

	this->pickStyle = PickInteractorStyle::New();
	this->pickStyle->setMainWindow(this);
	this->pickStyle->mode = 0;
	pickStyle->SetDefaultRenderer(this->ren);
	pickStyle->Data = topToothData;
	renderWindowInteractor->SetInteractorStyle(pickStyle);

	double bounds[6], minBound[3], maxBound[3];
	lowerToothActor->GetBounds(bounds);
	lowerToothActor->GetBounds(modelBound);
	minBound[0] = bounds[0];
	minBound[1] = bounds[2];
	minBound[2] = bounds[4];
	maxBound[0] = bounds[1];
	maxBound[1] = bounds[3];
	maxBound[2] = bounds[5];
	double lowerPointSize = sqrt(vtkMath::Distance2BetweenPoints(minBound, maxBound)) / 200.0;

	topToothActor->GetBounds(bounds);
	for(int i = 0; i < 6; i += 2)
	{
		if(modelBound[i] > bounds[i])
			modelBound[i] = bounds[i];
		if(modelBound[i + 1] < bounds[i + 1])
			modelBound[i] = bounds[i];
	}
	minBound[0] = bounds[0];
	minBound[1] = bounds[2];
	minBound[2] = bounds[4];
	maxBound[0] = bounds[1];
	maxBound[1] = bounds[3];
	maxBound[2] = bounds[5];
	double topPointSize = sqrt(vtkMath::Distance2BetweenPoints(minBound, maxBound)) / 200.0;

	pointSize = (lowerPointSize + topPointSize) / 2;

	//
	// 初始化操作栏
	//
	itemText = tr("选点");
	QTreeWidgetItem *pointItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	pointItem->setData(0, Qt::UserRole, 1111);
	//diMeasureItem->addChild(planeItem);
	pointItem->setExpanded(true);

	itemText = tr("拟合平面");
	QTreeWidgetItem * iplaneItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iplaneItem->setData(0, Qt::UserRole, 1112);
	//diMeasureItem->addChild(planeItem);
	iplaneItem->setExpanded(true);

	itemText = tr("投影点");
	QTreeWidgetItem * iprojectItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iprojectItem->setData(0, Qt::UserRole, 1113);
	//diMeasureItem->addChild(planeItem);
	iprojectItem->setExpanded(true);

	itemText = tr("切割曲线");
	QTreeWidgetItem * icutcurveItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	icutcurveItem->setData(0, Qt::UserRole, 1114);
	//diMeasureItem->addChild(planeItem);
	icutcurveItem->setExpanded(true);

	itemText = tr("向量");
	QTreeWidgetItem * ivectorItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	ivectorItem->setData(0, Qt::UserRole, 1115);
	//diMeasureItem->addChild(planeItem);
	ivectorItem->setExpanded(true);

	itemText = tr("点-点距离");
	QTreeWidgetItem * iPointPointItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iPointPointItem->setData(0, Qt::UserRole, 1116);
	//diMeasureItem->addChild(planeItem);
	iPointPointItem->setExpanded(true);

	itemText = tr("点-向量距离");
	QTreeWidgetItem * iPointVectorItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iPointVectorItem->setData(0, Qt::UserRole, 1117);
	//diMeasureItem->addChild(planeItem);
	iPointVectorItem->setExpanded(true);

	itemText = tr("点-面距离");
	QTreeWidgetItem * iPointPlaneItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iPointPlaneItem->setData(0, Qt::UserRole, 1118);
	//diMeasureItem->addChild(planeItem);
	iPointPlaneItem->setExpanded(true);

	itemText = tr("面-面距离");
	QTreeWidgetItem * iPlanePlaneItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iPlanePlaneItem->setData(0, Qt::UserRole, 1119);
	//diMeasureItem->addChild(planeItem);
	iPlanePlaneItem->setExpanded(true);

	itemText = tr("向量夹角");
	QTreeWidgetItem * iVectorVectorItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iVectorVectorItem->setData(0, Qt::UserRole, 1120);
	//diMeasureItem->addChild(planeItem);
	iVectorVectorItem->setExpanded(true);

	itemText = tr("三点夹角");
	QTreeWidgetItem * iPointAngleItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iPointAngleItem->setData(0, Qt::UserRole, 1121);
	//diMeasureItem->addChild(planeItem);
	iPointAngleItem->setExpanded(true);


	// 平面
	itemText = tr("平面");
	QTreeWidgetItem *planeItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	planeItem->setData(0, Qt::UserRole, 113);
	//diMeasureItem->addChild(planeItem);
	planeItem->setExpanded(true);

	// 合平面
	itemText = tr("合平面");
	QTreeWidgetItem *fitPlaneItem = new QTreeWidgetItem(planeItem, QStringList(itemText));
	fitPlaneItem->setData(0, Qt::UserRole, 4);
	planeItem->addChild(fitPlaneItem);
	// 正中矢状面
	itemText = tr("正中矢状面");
	QTreeWidgetItem *middleVerticalPlaneItem = new QTreeWidgetItem(planeItem, QStringList(itemText));
	middleVerticalPlaneItem->setData(0, Qt::UserRole, 23);
	planeItem->addChild(middleVerticalPlaneItem);
	// 冠状面
	itemText = tr("冠状面");
	QTreeWidgetItem *articlePlaneItem = new QTreeWidgetItem(planeItem, QStringList(itemText));
	articlePlaneItem->setData(0, Qt::UserRole, 85);
	planeItem->addChild(articlePlaneItem);

	// 牙弓拟合曲线
	itemText = tr("牙弓拟合曲线");
	QTreeWidgetItem *coverFitCoverItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	coverFitCoverItem->setData(0, Qt::UserRole, 66);
	//diMeasureItem->addChild(coverFitCoverItem);

	itemText = tr("牙轴倾角");
	QTreeWidgetItem * iToothAngle1Item = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iToothAngle1Item->setData(0, Qt::UserRole, 1122);
	//diMeasureItem->addChild(planeItem);
	iToothAngle1Item->setExpanded(true);

	itemText = tr("牙转矩角");
	QTreeWidgetItem * iToothAngle2Item = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	iToothAngle2Item->setData(0, Qt::UserRole, 1123);
	//diMeasureItem->addChild(planeItem);
	iToothAngle2Item->setExpanded(true);

	// DI参数测量
	itemText = tr("DI参数测量");
	QTreeWidgetItem *diMeasureItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	diMeasureItem->setData(0, Qt::UserRole, 112);
	diMeasureItem->setExpanded(true);

	// ABO-OGS指数测量
	itemText = tr("ABO-OGS指数测量");
	QTreeWidgetItem *aboMeasureItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	aboMeasureItem->setData(0, Qt::UserRole, 1500);
	aboMeasureItem->setExpanded(false);

	// ABO-OGS排列 操作数 * 100 + i

	itemText = tr("ABO-OGS排列");
	QTreeWidgetItem *aboArrangeItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboArrangeItem->setData(0, Qt::UserRole, 1501);
	aboMeasureItem->addChild(aboArrangeItem);
	aboArrangeItem->setExpanded(false);

	itemText = tr("点");
	QTreeWidgetItem *aboPointItem = new QTreeWidgetItem(aboArrangeItem, QStringList(itemText));
	aboPointItem->setData(0, Qt::UserRole, 1502);
	aboArrangeItem->addChild(aboPointItem);
	aboPointItem->setExpanded(true);

	for(int i = 0;i < 26; ++i)
	{
		itemText = QString(tr("点对")).append(QString::number(i + 1));
		QTreeWidgetItem *diAboPointPairItem = new QTreeWidgetItem(aboPointItem, QStringList(itemText));
		diAboPointPairItem->setData(0, Qt::UserRole, 1510 * 100 + i);
		aboPointItem->addChild(diAboPointPairItem);

		itemText = tr("点A");
		QTreeWidgetItem *diAboPointPairPointAItem = new QTreeWidgetItem(diAboPointPairItem, QStringList(itemText));
		diAboPointPairPointAItem->setData(0, Qt::UserRole, 1511 * 100 + i);
		diAboPointPairItem->addChild(diAboPointPairPointAItem);

		itemText = tr("点B");
		QTreeWidgetItem *diAboPointPairPointBItem = new QTreeWidgetItem(diAboPointPairItem, QStringList(itemText));
		diAboPointPairPointBItem->setData(0, Qt::UserRole, 1512 * 100 + i);
		diAboPointPairItem->addChild(diAboPointPairPointBItem);

		itemText = tr("牙弓放射状相交平面 O");
		QTreeWidgetItem *diAboPlaneItem = new QTreeWidgetItem(diAboPointPairItem, QStringList(itemText));
		diAboPlaneItem->setData(0, Qt::UserRole, 1513 * 100 + i);
		diAboPointPairItem->addChild(diAboPlaneItem);

		itemText = tr("初始点");
		QTreeWidgetItem *diAboPlanePointItem = new QTreeWidgetItem(diAboPlaneItem, QStringList(itemText));
		diAboPlanePointItem->setData(0, Qt::UserRole, 1514 * 100 + i);
		diAboPlaneItem->addChild(diAboPlaneItem);

		itemText = tr("法向量");
		QTreeWidgetItem *diAboPlaneNormalItem = new QTreeWidgetItem(diAboPlaneItem, QStringList(itemText));
		diAboPlaneNormalItem->setData(0, Qt::UserRole, 1515 * 100 + i);
		diAboPlaneItem->addChild(diAboPlaneNormalItem);

		itemText = tr("投影点");
		QTreeWidgetItem *diAboPlaneProjectPointItem = new QTreeWidgetItem(diAboPlaneItem, QStringList(itemText));
		diAboPlaneProjectPointItem->setData(0, Qt::UserRole, 1516 * 100 + i);
		diAboPlaneItem->addChild(diAboPlaneProjectPointItem);

		itemText = tr("点 A");
		QTreeWidgetItem *diAboPlaneProjectPointAItem = new QTreeWidgetItem(diAboPlaneProjectPointItem, QStringList(itemText));
		diAboPlaneProjectPointAItem->setData(0, Qt::UserRole, 1517 * 100 + i);
		diAboPlaneProjectPointItem->addChild(diAboPlaneProjectPointAItem);

		itemText = tr("点 B");
		QTreeWidgetItem *diAboPlaneProjectPointBItem = new QTreeWidgetItem(diAboPlaneProjectPointItem, QStringList(itemText));
		diAboPlaneProjectPointBItem->setData(0, Qt::UserRole, 1518 * 100 + i);
		diAboPlaneProjectPointItem->addChild(diAboPlaneProjectPointBItem);

		itemText = tr("投影点");
		QTreeWidgetItem *diAboPlaneProjectItem = new QTreeWidgetItem(diAboPointPairItem, QStringList(itemText));
		diAboPlaneProjectItem->setData(0, Qt::UserRole, 1519 * 100 + i);
		diAboPointPairItem->addChild(diAboPlaneProjectItem);

		itemText = tr("点 A");
		QTreeWidgetItem *diAboProjectPointAItem = new QTreeWidgetItem(diAboPlaneProjectItem, QStringList(itemText));
		diAboProjectPointAItem->setData(0, Qt::UserRole, 1520 * 100 + i);
		diAboPlaneProjectItem->addChild(diAboProjectPointAItem);

		itemText = tr("点 B");
		QTreeWidgetItem *diAboProjectPointBItem = new QTreeWidgetItem(diAboPlaneProjectItem, QStringList(itemText));
		diAboProjectPointBItem->setData(0, Qt::UserRole, 1521 * 100 + i);
		diAboPlaneProjectItem->addChild(diAboProjectPointBItem);

		itemText = tr("距离");
		QTreeWidgetItem *diAboProjectPointDistanceItem = new QTreeWidgetItem(diAboPlaneProjectItem, QStringList(itemText));
		diAboProjectPointDistanceItem->setData(0, Qt::UserRole, 1522 * 100 + i);
		diAboPlaneProjectItem->addChild(diAboProjectPointDistanceItem);

		itemText = tr("得分");
		QTreeWidgetItem *diAboPointPairScoreItem = new QTreeWidgetItem(diAboPointPairItem, QStringList(itemText));
		diAboPointPairScoreItem->setData(0, Qt::UserRole, 1523 * 100 + i);
		diAboPointPairItem->addChild(diAboPointPairScoreItem);
	}

	itemText = tr("得分");
	QTreeWidgetItem *aboArrangeScoreItem = new QTreeWidgetItem(aboArrangeItem, QStringList(itemText));
	aboArrangeScoreItem->setData(0, Qt::UserRole, 1503);
	aboArrangeItem->addChild(aboArrangeScoreItem);
	aboArrangeScoreItem->setExpanded(false);

	// ABO-OGS边缘嵴高度
	itemText = tr("ABO-OGS边缘嵴高度");
	QTreeWidgetItem *aboEdgeVertebralHeighItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboEdgeVertebralHeighItem->setData(0, Qt::UserRole, 2000);
	aboMeasureItem->addChild(aboEdgeVertebralHeighItem);
	aboEdgeVertebralHeighItem->setExpanded(false);

	itemText = tr("点");
	QTreeWidgetItem *aboEdgeVertebralHeighPointItem = new QTreeWidgetItem(aboEdgeVertebralHeighItem, QStringList(itemText));
	aboEdgeVertebralHeighPointItem->setData(0, Qt::UserRole, 2001);
	aboEdgeVertebralHeighItem->addChild(aboEdgeVertebralHeighPointItem);
	aboEdgeVertebralHeighPointItem->setExpanded(true);

	for(int i = 0;i < 12; ++i)
	{
		itemText = QString(tr("点对")).append(QString::number(i + 1));
		QTreeWidgetItem *aboEdgeVertebralHeighPointPairItem = new QTreeWidgetItem(aboEdgeVertebralHeighPointItem, QStringList(itemText));
		aboEdgeVertebralHeighPointPairItem->setData(0, Qt::UserRole, 2010 + i * 25);
		aboEdgeVertebralHeighPointItem->addChild(aboEdgeVertebralHeighPointPairItem);

		itemText = tr("点 A");
		QTreeWidgetItem *diAboPointPairPointAItem = new QTreeWidgetItem(aboEdgeVertebralHeighPointPairItem, QStringList(itemText));
		diAboPointPairPointAItem->setData(0, Qt::UserRole, 2011 + i * 25);
		aboEdgeVertebralHeighPointPairItem->addChild(diAboPointPairPointAItem);

		itemText = tr("点 B");
		QTreeWidgetItem *diAboPointPairPointBItem = new QTreeWidgetItem(aboEdgeVertebralHeighPointPairItem, QStringList(itemText));
		diAboPointPairPointBItem->setData(0, Qt::UserRole, 2012 + i * 25);
		aboEdgeVertebralHeighPointPairItem->addChild(diAboPointPairPointBItem);

		itemText = tr("投影");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectItem = new QTreeWidgetItem(aboEdgeVertebralHeighPointPairItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectItem->setData(0, Qt::UserRole, 2013 + i * 25);
		aboEdgeVertebralHeighPointPairItem->addChild(aboEdgeVertebralHeighProjectItem);

		itemText = tr("正中矢状面");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem->setData(0, Qt::UserRole, 2014 + i * 25);
		aboEdgeVertebralHeighProjectItem->addChild(aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem);

		itemText = tr("点 A");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectMiddleVerticalPlaneAItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectMiddleVerticalPlaneAItem->setData(0, Qt::UserRole, 2015 + i * 25);
		aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem->addChild(aboEdgeVertebralHeighProjectMiddleVerticalPlaneAItem);

		itemText = tr("点 B");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectMiddleVerticalPlaneBItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectMiddleVerticalPlaneBItem->setData(0, Qt::UserRole, 2016 + i * 25);
		aboEdgeVertebralHeighProjectMiddleVerticalPlaneItem->addChild(aboEdgeVertebralHeighProjectMiddleVerticalPlaneBItem);

		itemText = tr("冠状面");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectArticlePlaneItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectArticlePlaneItem->setData(0, Qt::UserRole, 2017 + i * 25);
		aboEdgeVertebralHeighProjectItem->addChild(aboEdgeVertebralHeighProjectArticlePlaneItem);

		itemText = tr("点 A");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectArticlePlaneAItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectArticlePlaneItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectArticlePlaneAItem->setData(0, Qt::UserRole, 2018 + i * 25);
		aboEdgeVertebralHeighProjectArticlePlaneItem->addChild(aboEdgeVertebralHeighProjectArticlePlaneAItem);

		itemText = tr("点 B");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectArticlePlaneBItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectArticlePlaneItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectArticlePlaneBItem->setData(0, Qt::UserRole, 2019 + i * 25);
		aboEdgeVertebralHeighProjectArticlePlaneItem->addChild(aboEdgeVertebralHeighProjectArticlePlaneBItem);

		itemText = tr("距离");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectDistanceItem = new QTreeWidgetItem(aboEdgeVertebralHeighProjectArticlePlaneItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectDistanceItem->setData(0, Qt::UserRole, 2020 + i * 25);
		aboEdgeVertebralHeighProjectArticlePlaneItem->addChild(aboEdgeVertebralHeighProjectDistanceItem);

		itemText = tr("得分");
		QTreeWidgetItem *aboEdgeVertebralHeighProjectScoreItem = new QTreeWidgetItem(aboEdgeVertebralHeighPointPairItem, QStringList(itemText));
		aboEdgeVertebralHeighProjectScoreItem->setData(0, Qt::UserRole, 2021 + i * 25);
		aboEdgeVertebralHeighPointPairItem->addChild(aboEdgeVertebralHeighProjectScoreItem); 
	}

	itemText = tr("得分");
	QTreeWidgetItem *aboEdgeVertebralHeighScoreItem = new QTreeWidgetItem(aboEdgeVertebralHeighItem, QStringList(itemText));
	aboEdgeVertebralHeighScoreItem->setData(0, Qt::UserRole, 2002);
	aboEdgeVertebralHeighItem->addChild(aboEdgeVertebralHeighScoreItem);
	aboEdgeVertebralHeighScoreItem->setExpanded(false);

	// ABO-OGS颊舌向倾斜度
	itemText = tr("ABO-OGS颊舌向倾斜度");
	QTreeWidgetItem *aboChapMeasureItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboChapMeasureItem->setData(0, Qt::UserRole, 2500);
	aboMeasureItem->addChild(aboChapMeasureItem);
	aboChapMeasureItem->setExpanded(false);

	itemText = tr("接触拟合面");
	QTreeWidgetItem *aboChapMeasureFitPlaneItem = new QTreeWidgetItem(aboChapMeasureItem, QStringList(itemText));
	aboChapMeasureFitPlaneItem->setData(0, Qt::UserRole, 2501);
	aboChapMeasureItem->addChild(aboChapMeasureFitPlaneItem);
	aboChapMeasureFitPlaneItem->setExpanded(false);

	itemText = tr("法向量");
	QTreeWidgetItem *aboChapMeasureFitPlaneNormalItem = new QTreeWidgetItem(aboChapMeasureFitPlaneItem, QStringList(itemText));
	aboChapMeasureFitPlaneNormalItem->setData(0, Qt::UserRole, 2502);
	aboChapMeasureFitPlaneItem->addChild(aboChapMeasureFitPlaneNormalItem);

	for(int i = 0; i < 14; ++i)
	{
		itemText = QString(tr("测量")).append(QString::number(i + 1));
		QTreeWidgetItem *aboChapMeasurePointPareItem = new QTreeWidgetItem(aboChapMeasureItem, QStringList(itemText));
		aboChapMeasurePointPareItem->setData(0, Qt::UserRole, 2510 + i * 25);
		aboChapMeasureItem->addChild(aboChapMeasurePointPareItem);

		itemText = tr("点");
		QTreeWidgetItem *aboChapMeasurePointItem = new QTreeWidgetItem(aboChapMeasurePointPareItem, QStringList(itemText));
		aboChapMeasurePointItem->setData(0, Qt::UserRole, 2511 + i * 25);
		aboChapMeasurePointPareItem->addChild(aboChapMeasurePointItem);

		itemText = tr("投影");
		QTreeWidgetItem *aboChapMeasureProjectPointItem = new QTreeWidgetItem(aboChapMeasurePointPareItem, QStringList(itemText));
		aboChapMeasureProjectPointItem->setData(0, Qt::UserRole, 2512 + i * 25);
		aboChapMeasurePointPareItem->addChild(aboChapMeasureProjectPointItem);

		itemText = tr("距离");
		QTreeWidgetItem *aboChapMeasurePointDistanceItem = new QTreeWidgetItem(aboChapMeasurePointPareItem, QStringList(itemText));
		aboChapMeasurePointDistanceItem->setData(0, Qt::UserRole, 2513 + i * 25);
		aboChapMeasurePointPareItem->addChild(aboChapMeasurePointDistanceItem);

		itemText = tr("得分");
		QTreeWidgetItem *aboChapMeasurePointScoreItem = new QTreeWidgetItem(aboChapMeasurePointPareItem, QStringList(itemText));
		aboChapMeasurePointScoreItem->setData(0, Qt::UserRole, 2514 + i * 25);
		aboChapMeasurePointPareItem->addChild(aboChapMeasurePointScoreItem);
	}

	itemText = tr("得分");
	QTreeWidgetItem *aboChapScoreItem = new QTreeWidgetItem(aboChapMeasureItem, QStringList(itemText));
	aboChapScoreItem->setData(0, Qt::UserRole, 2503);
	aboChapMeasureItem->addChild(aboChapScoreItem);
	aboChapScoreItem->setExpanded(false);


	// ABO-OGS牙合关系
	itemText = tr("ABO-OGS牙合关系");
	QTreeWidgetItem *aboToothCloseRelationItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboToothCloseRelationItem->setData(0, Qt::UserRole, 3000);
	aboMeasureItem->addChild(aboToothCloseRelationItem);
	aboEdgeVertebralHeighItem->setExpanded(false);

	for(int i = 0; i < 10; ++i)
	{
		itemText = QString(tr("测量")).append(QString::number(i + 1));
		QTreeWidgetItem *aboToothCloseRelationMeasureItem = new QTreeWidgetItem(aboToothCloseRelationItem, QStringList(itemText));
		aboToothCloseRelationMeasureItem->setData(0, Qt::UserRole, 3010 + i * 25);
		aboToothCloseRelationItem->addChild(aboToothCloseRelationMeasureItem);

		itemText = tr("上牙列");
		QTreeWidgetItem *aboToothCloseRelationMeasureTopToothItem = new QTreeWidgetItem(aboToothCloseRelationMeasureItem, QStringList(itemText));
		aboToothCloseRelationMeasureTopToothItem->setData(0, Qt::UserRole, 3011 + i * 25);
		aboToothCloseRelationMeasureItem->addChild(aboToothCloseRelationMeasureTopToothItem);

		itemText = tr("点");
		QTreeWidgetItem *aboToothCloseRelationMeasureTopToothPointItem = new QTreeWidgetItem(aboToothCloseRelationMeasureTopToothItem, QStringList(itemText));
		aboToothCloseRelationMeasureTopToothPointItem->setData(0, Qt::UserRole, 3012 + i * 25);
		aboToothCloseRelationMeasureTopToothItem->addChild(aboToothCloseRelationMeasureTopToothItem);

		itemText = tr("牙弓放射状相交平面 O");
		QTreeWidgetItem *aboToothCloseRelationMeasureTopToothPlaneOItem = new QTreeWidgetItem(aboToothCloseRelationMeasureTopToothItem, QStringList(itemText));
		aboToothCloseRelationMeasureTopToothPlaneOItem->setData(0, Qt::UserRole, 3013 + i * 25);
		aboToothCloseRelationMeasureTopToothItem->addChild(aboToothCloseRelationMeasureTopToothPlaneOItem);

		itemText = tr("Point");
		QTreeWidgetItem *aboToothCloseRelationMeasureTopToothPlaneOPointItem = new QTreeWidgetItem(aboToothCloseRelationMeasureTopToothPlaneOItem, QStringList(itemText));
		aboToothCloseRelationMeasureTopToothPlaneOPointItem->setData(0, Qt::UserRole, 3014 + i * 25);
		aboToothCloseRelationMeasureTopToothPlaneOItem->addChild(aboToothCloseRelationMeasureTopToothPlaneOPointItem);

		itemText = tr("Normal");
		QTreeWidgetItem *aboToothCloseRelationMeasureTopToothPlaneONormalItem = new QTreeWidgetItem(aboToothCloseRelationMeasureTopToothPlaneOItem, QStringList(itemText));
		aboToothCloseRelationMeasureTopToothPlaneONormalItem->setData(0, Qt::UserRole, 3015 + i * 25);
		aboToothCloseRelationMeasureTopToothPlaneOItem->addChild(aboToothCloseRelationMeasureTopToothPlaneONormalItem);

		itemText = tr("Low Tooth");
		QTreeWidgetItem *aboToothCloseRelationMeasureLowToothItem = new QTreeWidgetItem(aboToothCloseRelationMeasureItem, QStringList(itemText));
		aboToothCloseRelationMeasureLowToothItem->setData(0, Qt::UserRole, 3016 + i * 25);
		aboToothCloseRelationMeasureItem->addChild(aboToothCloseRelationMeasureLowToothItem);

		itemText = tr("Point");
		QTreeWidgetItem *aboToothCloseRelationMeasureLowToothPointItem = new QTreeWidgetItem(aboToothCloseRelationMeasureLowToothItem, QStringList(itemText));
		aboToothCloseRelationMeasureLowToothPointItem->setData(0, Qt::UserRole, 3017 + i * 25);
		aboToothCloseRelationMeasureLowToothItem->addChild(aboToothCloseRelationMeasureLowToothPointItem);

		itemText = tr("Distance");
		QTreeWidgetItem *aboToothCloseRelationMeasureDistanceItem = new QTreeWidgetItem(aboToothCloseRelationMeasureItem, QStringList(itemText));
		aboToothCloseRelationMeasureDistanceItem->setData(0, Qt::UserRole, 3018 + i * 25);
		aboToothCloseRelationMeasureItem->addChild(aboToothCloseRelationMeasureDistanceItem);

		itemText = tr("Score");
		QTreeWidgetItem *aboToothCloseRelationMeasureScoreItem = new QTreeWidgetItem(aboToothCloseRelationMeasureItem, QStringList(itemText));
		aboToothCloseRelationMeasureScoreItem->setData(0, Qt::UserRole, 3019 + i * 25);
		aboToothCloseRelationMeasureItem->addChild(aboToothCloseRelationMeasureScoreItem);
	}

	itemText = tr("Score");
	QTreeWidgetItem *aboToothCloseRelationScoreItem = new QTreeWidgetItem(aboToothCloseRelationItem, QStringList(itemText));
	aboToothCloseRelationScoreItem->setData(0, Qt::UserRole, 3001);
	aboToothCloseRelationItem->addChild(aboToothCloseRelationScoreItem);
	aboToothCloseRelationItem->setExpanded(false);

	// ABO-OGS牙合接触

	itemText = tr("ABO-OGS牙合接触");
	QTreeWidgetItem *aboToothCloseTouchItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboToothCloseTouchItem->setData(0, Qt::UserRole, 3500);
	aboMeasureItem->addChild(aboToothCloseTouchItem);
	aboToothCloseTouchItem->setExpanded(false);

	for(int i = 0; i < 20; ++i)
	{
		itemText = QString(tr("Measure")).append(QString::number(i + 1));
		QTreeWidgetItem *aboToothCloseTouchMeasureItem = new QTreeWidgetItem(aboToothCloseTouchItem, QStringList(itemText));
		aboToothCloseTouchMeasureItem->setData(0, Qt::UserRole, 3510 + i * 25);
		aboToothCloseTouchItem->addChild(aboToothCloseTouchMeasureItem);

		itemText = tr("Point A");
		QTreeWidgetItem *aboToothCloseTouchMeasurePointAItem = new QTreeWidgetItem(aboToothCloseTouchMeasureItem, QStringList(itemText));
		aboToothCloseTouchMeasurePointAItem->setData(0, Qt::UserRole, 3511 + i * 25);
		aboToothCloseTouchMeasureItem->addChild(aboToothCloseTouchMeasurePointAItem);

		itemText = tr("Intersect Plane O");
		QTreeWidgetItem *aboToothCloseTouchMeasurePlaneOItem = new QTreeWidgetItem(aboToothCloseTouchMeasureItem, QStringList(itemText));
		aboToothCloseTouchMeasurePlaneOItem->setData(0, Qt::UserRole, 3512 + i * 25);
		aboToothCloseTouchMeasureItem->addChild(aboToothCloseTouchMeasurePlaneOItem);

		itemText = tr("Point");
		QTreeWidgetItem *aboToothCloseTouchMeasurePlaneOPointItem = new QTreeWidgetItem(aboToothCloseTouchMeasurePlaneOItem, QStringList(itemText));
		aboToothCloseTouchMeasurePlaneOPointItem->setData(0, Qt::UserRole, 3513 + i * 25);
		aboToothCloseTouchMeasurePlaneOItem->addChild(aboToothCloseTouchMeasurePlaneOPointItem);

		itemText = tr("Normal");
		QTreeWidgetItem *aboToothCloseTouchMeasurePlaneONormalItem = new QTreeWidgetItem(aboToothCloseTouchMeasurePlaneOItem, QStringList(itemText));
		aboToothCloseTouchMeasurePlaneONormalItem->setData(0, Qt::UserRole, 3514 + i * 25);
		aboToothCloseTouchMeasurePlaneOItem->addChild(aboToothCloseTouchMeasurePlaneONormalItem);

		itemText = tr("Contour Curve");
		QTreeWidgetItem *aboToothCloseTouchMeasureCurveItem = new QTreeWidgetItem(aboToothCloseTouchMeasureItem, QStringList(itemText));
		aboToothCloseTouchMeasureCurveItem->setData(0, Qt::UserRole, 3515 + i * 25);
		aboToothCloseTouchMeasureItem->addChild(aboToothCloseTouchMeasureCurveItem);

		itemText = tr("Point B");
		QTreeWidgetItem *aboToothCloseTouchMeasurePointBItem = new QTreeWidgetItem(aboToothCloseTouchMeasureItem, QStringList(itemText));
		aboToothCloseTouchMeasurePointBItem->setData(0, Qt::UserRole, 3516 + i * 25);
		aboToothCloseTouchMeasureItem->addChild(aboToothCloseTouchMeasurePointBItem);

		itemText = tr("Distance");
		QTreeWidgetItem *aboToothCloseTouchMeasureDistanceItem = new QTreeWidgetItem(aboToothCloseTouchMeasureItem, QStringList(itemText));
		aboToothCloseTouchMeasureDistanceItem->setData(0, Qt::UserRole, 3517 + i * 25);
		aboToothCloseTouchMeasureItem->addChild(aboToothCloseTouchMeasureDistanceItem);

		itemText = tr("Score");
		QTreeWidgetItem *aboToothCloseTouchMeasureScoreItem = new QTreeWidgetItem(aboToothCloseTouchMeasureItem, QStringList(itemText));
		aboToothCloseTouchMeasureScoreItem->setData(0, Qt::UserRole, 3518 + i * 25);
		aboToothCloseTouchMeasureItem->addChild(aboToothCloseTouchMeasureScoreItem);
	}

	itemText = tr("Score");
	QTreeWidgetItem *aboToothCloseTouchScoreItem = new QTreeWidgetItem(aboToothCloseTouchItem, QStringList(itemText));
	aboToothCloseTouchScoreItem->setData(0, Qt::UserRole, 3501);
	aboToothCloseTouchItem->addChild(aboToothCloseTouchScoreItem);
	aboToothCloseTouchItem->setExpanded(false);

	// ABO-OGS覆盖
	itemText = tr("ABO-OGS覆盖");
	QTreeWidgetItem *aboCoverItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboCoverItem->setData(0, Qt::UserRole, 4000);
	aboMeasureItem->addChild(aboCoverItem);
	aboCoverItem->setExpanded(false);

	for(int i = 0; i < 18; ++i)
	{
		itemText = QString(tr("Measure")).append(QString::number(i + 1));
		QTreeWidgetItem *aboCoverMeasureItem = new QTreeWidgetItem(aboCoverItem, QStringList(itemText));
		aboCoverMeasureItem->setData(0, Qt::UserRole, 4010 + i * 25);
		aboCoverItem->addChild(aboCoverMeasureItem);

		itemText = tr("Point A");
		QTreeWidgetItem *aboCoverMeasurePointAItem = new QTreeWidgetItem(aboCoverMeasureItem, QStringList(itemText));
		aboCoverMeasurePointAItem->setData(0, Qt::UserRole, 4011 + i * 25);
		aboCoverMeasureItem->addChild(aboCoverMeasurePointAItem);

		itemText = tr("Intersect Plane O");
		QTreeWidgetItem *aboCoverMeasurePlaneOItem = new QTreeWidgetItem(aboCoverMeasureItem, QStringList(itemText));
		aboCoverMeasurePlaneOItem->setData(0, Qt::UserRole, 4012 + i * 25);
		aboCoverMeasureItem->addChild(aboCoverMeasurePlaneOItem);

		itemText = tr("Point");
		QTreeWidgetItem *aboCoverMeasurePlaneOPointItem = new QTreeWidgetItem(aboCoverMeasurePlaneOItem, QStringList(itemText));
		aboCoverMeasurePlaneOPointItem->setData(0, Qt::UserRole, 4013 + i * 25);
		aboCoverMeasurePlaneOItem->addChild(aboCoverMeasurePlaneOPointItem);

		itemText = tr("Normal");
		QTreeWidgetItem *aboCoverMeasurePlaneONormalItem = new QTreeWidgetItem(aboCoverMeasurePlaneOItem, QStringList(itemText));
		aboCoverMeasurePlaneONormalItem->setData(0, Qt::UserRole, 4014 + i * 25);
		aboCoverMeasurePlaneOItem->addChild(aboCoverMeasurePlaneONormalItem);

		itemText = tr("Contour Curve");
		QTreeWidgetItem *aboCoverMeasureIntersectCurveItem = new QTreeWidgetItem(aboCoverMeasureItem, QStringList(itemText));
		aboCoverMeasureIntersectCurveItem->setData(0, Qt::UserRole, 4015 + i * 25);
		aboCoverMeasureItem->addChild(aboCoverMeasureIntersectCurveItem);

		itemText = tr("Top Tooth");
		QTreeWidgetItem *aboCoverMeasureIntersectCurveTopToothItem = new QTreeWidgetItem(aboCoverMeasureIntersectCurveItem, QStringList(itemText));
		aboCoverMeasureIntersectCurveTopToothItem->setData(0, Qt::UserRole, 4016 + i * 25);
		aboCoverMeasureIntersectCurveItem->addChild(aboCoverMeasureIntersectCurveTopToothItem);

		itemText = tr("Point B");
		QTreeWidgetItem *aboCoverPointBItem = new QTreeWidgetItem(aboCoverMeasureIntersectCurveTopToothItem, QStringList(itemText));
		aboCoverPointBItem->setData(0, Qt::UserRole, 4018 + i * 25);
		aboCoverMeasureIntersectCurveTopToothItem->addChild(aboCoverPointBItem);

		itemText = tr("Low Tooth");
		QTreeWidgetItem *aboCoverMeasureIntersectCurveLowToothItem = new QTreeWidgetItem(aboCoverMeasureIntersectCurveItem, QStringList(itemText));
		aboCoverMeasureIntersectCurveLowToothItem->setData(0, Qt::UserRole, 4017 + i * 25);
		aboCoverMeasureIntersectCurveItem->addChild(aboCoverMeasureIntersectCurveLowToothItem);

		itemText = tr("Distance");
		QTreeWidgetItem *aboCoverMeasureDistanceItem = new QTreeWidgetItem(aboCoverMeasureItem, QStringList(itemText));
		aboCoverMeasureDistanceItem->setData(0, Qt::UserRole, 4019 + i * 25);
		aboCoverMeasureItem->addChild(aboCoverMeasureIntersectCurveItem);

		itemText = tr("Score");
		QTreeWidgetItem *aboCoverMeasureScoreItem = new QTreeWidgetItem(aboCoverMeasureItem, QStringList(itemText));
		aboCoverMeasureScoreItem->setData(0, Qt::UserRole, 4020 + i * 25);
		aboCoverMeasureItem->addChild(aboCoverMeasureScoreItem);
	}

	itemText = tr("Score");
	QTreeWidgetItem *aboCoverScoreItem = new QTreeWidgetItem(aboCoverItem, QStringList(itemText));
	aboCoverScoreItem->setData(0, Qt::UserRole, 4001);
	aboCoverItem->addChild(aboCoverScoreItem);
	aboCoverItem->setExpanded(false);

	// ABO-OGS邻牙接触关系
	itemText = tr("ABO-OGS邻牙接触关系");
	QTreeWidgetItem *aboTouchRelationItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboTouchRelationItem->setData(0, Qt::UserRole, 4500);
	aboMeasureItem->addChild(aboTouchRelationItem);
	aboTouchRelationItem->setExpanded(false);

	for(int i = 0; i < 26; ++i)
	{
		itemText = QString(tr("Measure")).append(QString::number(i + 1));
		QTreeWidgetItem *aboTouchRelationMeasureItem = new QTreeWidgetItem(aboTouchRelationItem, QStringList(itemText));
		aboTouchRelationMeasureItem->setData(0, Qt::UserRole, 4510 + i * 25);
		aboTouchRelationItem->addChild(aboTouchRelationMeasureItem);

		itemText = tr("Point");
		QTreeWidgetItem *aboTouchRelationMeasurePointItem = new QTreeWidgetItem(aboTouchRelationMeasureItem, QStringList(itemText));
		aboTouchRelationMeasurePointItem->setData(0, Qt::UserRole, 4511 + i * 25);
		aboTouchRelationMeasureItem->addChild(aboTouchRelationMeasurePointItem);

		itemText = tr("Project");
		QTreeWidgetItem *aboTouchRelationMeasureProjectItem = new QTreeWidgetItem(aboTouchRelationMeasureItem, QStringList(itemText));
		aboTouchRelationMeasureProjectItem->setData(0, Qt::UserRole, 4512 + i * 25);
		aboTouchRelationMeasureItem->addChild(aboTouchRelationMeasureProjectItem);

		itemText = tr("Distance");
		QTreeWidgetItem *aboTouchRelationMeasureDistanceItem = new QTreeWidgetItem(aboTouchRelationMeasureItem, QStringList(itemText));
		aboTouchRelationMeasureDistanceItem->setData(0, Qt::UserRole, 4513 + i * 25);
		aboTouchRelationMeasureItem->addChild(aboTouchRelationMeasureDistanceItem);

		itemText = tr("Score");
		QTreeWidgetItem *aboTouchRelationMeasureScoreItem = new QTreeWidgetItem(aboTouchRelationMeasureItem, QStringList(itemText));
		aboTouchRelationMeasureScoreItem->setData(0, Qt::UserRole, 4514 + i * 25);
		aboTouchRelationMeasureItem->addChild(aboTouchRelationMeasureDistanceItem);
	}

	itemText = tr("Score");
	QTreeWidgetItem *aboTouchRelationScoreItem = new QTreeWidgetItem(aboTouchRelationItem, QStringList(itemText));
	aboTouchRelationScoreItem->setData(0, Qt::UserRole, 4501);
	aboTouchRelationItem->addChild(aboTouchRelationScoreItem);
	aboTouchRelationItem->setExpanded(false);

	// ABO-OGS得分
	itemText = tr("ABO-OGS得分");
	QTreeWidgetItem *aboScoreItem = new QTreeWidgetItem(aboMeasureItem, QStringList(itemText));
	aboScoreItem->setData(0, Qt::UserRole, 5000);
	aboMeasureItem->addChild(aboScoreItem);
	aboScoreItem->setExpanded(false); 
	// DI覆盖
	/*
	itemText = tr("Cover");
	QTreeWidgetItem *diCoverItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diCoverItem->setData(0, Qt::UserRole, 114);
	diMeasureItem->addChild(diCoverItem);

	itemText = tr("Point");
	QTreeWidgetItem *diPointItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diPointItem->setData(0, Qt::UserRole, 118);
	diCoverItem->addChild(diPointItem);
	itemText = tr("Plane");
	QTreeWidgetItem *diCoverPlaneItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diCoverPlaneItem->setData(0, Qt::UserRole, 115);
	diCoverItem->addChild(diCoverPlaneItem);
	itemText = tr("Intersect Line");
	QTreeWidgetItem *diCoverIntersectLineItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diCoverIntersectLineItem->setData(0, Qt::UserRole, 116);
	diCoverItem->addChild(diCoverIntersectLineItem);
	itemText = tr("Score");
	QTreeWidgetItem *dScoreItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	dScoreItem->setData(0, Qt::UserRole, 117);
	diCoverItem->addChild(dScoreItem);

	itemText = tr("Intersect Plane O");
	QTreeWidgetItem *diCoverPlaneOItem = new QTreeWidgetItem(diCoverPlaneItem, QStringList(itemText));
	diCoverPlaneOItem->setData(0, Qt::UserRole, 119);
	diCoverPlaneItem->addChild(diCoverPlaneOItem);
	itemText = tr("Intersect Plane P");
	QTreeWidgetItem *diCoverPlanePItem = new QTreeWidgetItem(diCoverPlaneItem, QStringList(itemText));
	diCoverPlanePItem->setData(0, Qt::UserRole, 120);
	diCoverPlaneItem->addChild(diCoverPlanePItem);

	itemText = tr("Point");
	QTreeWidgetItem *diCoverPlaneOPointItem = new QTreeWidgetItem(diCoverPlaneOItem, QStringList(itemText));
	diCoverPlaneOPointItem->setData(0, Qt::UserRole, 123);
	diCoverPlaneOItem->addChild(diCoverPlaneOPointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *diCoverPlaneONormalItem = new QTreeWidgetItem(diCoverPlaneOItem, QStringList(itemText));
	diCoverPlaneONormalItem->setData(0, Qt::UserRole, 124);
	diCoverPlaneOItem->addChild(diCoverPlaneONormalItem);

	itemText = tr("Point");
	QTreeWidgetItem *diCoverPlanePPointItem = new QTreeWidgetItem(diCoverPlanePItem, QStringList(itemText));
	diCoverPlanePPointItem->setData(0, Qt::UserRole, 125);
	diCoverPlanePItem->addChild(diCoverPlanePPointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *diCoverPlanePNormalItem = new QTreeWidgetItem(diCoverPlanePItem, QStringList(itemText));
	diCoverPlanePNormalItem->setData(0, Qt::UserRole, 126);
	diCoverPlanePItem->addChild(diCoverPlanePNormalItem);

	itemText = tr("Intersect Point A");
	QTreeWidgetItem *diIntersectPointAItem = new QTreeWidgetItem(diCoverIntersectLineItem, QStringList(itemText));
	diIntersectPointAItem->setData(0, Qt::UserRole, 121);
	diCoverIntersectLineItem->addChild(diIntersectPointAItem);
	itemText = tr("Intersect Point B");
	QTreeWidgetItem *diIntersectPointBItem = new QTreeWidgetItem(diCoverIntersectLineItem, QStringList(itemText));
	diIntersectPointBItem->setData(0, Qt::UserRole, 122);
	diCoverIntersectLineItem->addChild(diIntersectPointBItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diIntersectPointDistanceItem = new QTreeWidgetItem(diCoverIntersectLineItem, QStringList(itemText));
	diIntersectPointDistanceItem->setData(0, Qt::UserRole, 127);
	diCoverIntersectLineItem->addChild(diIntersectPointDistanceItem);
	*/
	itemText = tr("Cover");
	QTreeWidgetItem *diCoverItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diCoverItem->setData(0, Qt::UserRole, 118);
	diMeasureItem->addChild(diCoverItem);

	itemText = tr("Point");
	QTreeWidgetItem *diCoverPointItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diCoverPointItem->setData(0, Qt::UserRole, 119);
	diCoverItem->addChild(diCoverPointItem);
	itemText = tr("Intersect Plane O");
	QTreeWidgetItem *diCoverPlaneOItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diCoverPlaneOItem->setData(0, Qt::UserRole, 120);
	diCoverItem->addChild(diCoverPlaneOItem);
	itemText = tr("Contour Curve");
	QTreeWidgetItem *diCoverContourItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diCoverContourItem->setData(0, Qt::UserRole, 121);
	diCoverItem->addChild(diCoverContourItem);
	itemText = tr("Score");
	QTreeWidgetItem *diCoverScoreItem = new QTreeWidgetItem(diCoverItem, QStringList(itemText));
	diCoverScoreItem->setData(0, Qt::UserRole, 122);
	diCoverItem->addChild(diCoverScoreItem);

	itemText = tr("Point");
	QTreeWidgetItem *diCoverPlanePointItem = new QTreeWidgetItem(diCoverPlaneOItem, QStringList(itemText));
	diCoverPlanePointItem->setData(0, Qt::UserRole, 123);
	diCoverPlaneOItem->addChild(diCoverPlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *diCoverPlaneNormalItem = new QTreeWidgetItem(diCoverPlaneOItem, QStringList(itemText));
	diCoverPlaneNormalItem->setData(0, Qt::UserRole, 124);
	diCoverPlaneOItem->addChild(diCoverPlaneNormalItem);

	itemText = tr("Point");
	QTreeWidgetItem *diCoverCurvePointItem = new QTreeWidgetItem(diCoverContourItem, QStringList(itemText));
	diCoverCurvePointItem->setData(0, Qt::UserRole, 125);
	diCoverContourItem->addChild(diCoverCurvePointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diCoverCurveDistanceItem = new QTreeWidgetItem(diCoverContourItem, QStringList(itemText));
	diCoverCurveDistanceItem->setData(0, Qt::UserRole, 126);
	diCoverContourItem->addChild(diCoverCurveDistanceItem);

	// DI覆合
	itemText = tr("Overbite");
	QTreeWidgetItem *diOverbiteItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diOverbiteItem->setData(0, Qt::UserRole, 128);
	diMeasureItem->addChild(diOverbiteItem);

	itemText = tr("Point");
	QTreeWidgetItem *diOverbitePointItem = new QTreeWidgetItem(diOverbiteItem, QStringList(itemText));
	diOverbitePointItem->setData(0, Qt::UserRole, 129);
	diOverbiteItem->addChild(diOverbitePointItem);
	itemText = tr("Intersect Plane O");
	QTreeWidgetItem *diOverbitePlaneOItem = new QTreeWidgetItem(diOverbiteItem, QStringList(itemText));
	diOverbitePlaneOItem->setData(0, Qt::UserRole, 130);
	diOverbiteItem->addChild(diOverbitePlaneOItem);
	itemText = tr("Contour Curve");
	QTreeWidgetItem *diContourItem = new QTreeWidgetItem(diOverbiteItem, QStringList(itemText));
	diContourItem->setData(0, Qt::UserRole, 131);
	diOverbiteItem->addChild(diContourItem);
	itemText = tr("Score");
	QTreeWidgetItem *diOverbiteScoreItem = new QTreeWidgetItem(diOverbiteItem, QStringList(itemText));
	diOverbiteScoreItem->setData(0, Qt::UserRole, 132);
	diOverbiteItem->addChild(diOverbiteScoreItem);

	itemText = tr("Point");
	QTreeWidgetItem *diOverbitePlanePointItem = new QTreeWidgetItem(diOverbitePlaneOItem, QStringList(itemText));
	diOverbitePlanePointItem->setData(0, Qt::UserRole, 133);
	diOverbitePlaneOItem->addChild(diOverbitePlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *diOverbitePlaneNormalItem = new QTreeWidgetItem(diOverbitePlaneOItem, QStringList(itemText));
	diOverbitePlaneNormalItem->setData(0, Qt::UserRole, 134);
	diOverbitePlaneOItem->addChild(diOverbitePlaneNormalItem);

	itemText = tr("Point");
	QTreeWidgetItem *diOverbiteCurvePointItem = new QTreeWidgetItem(diContourItem, QStringList(itemText));
	diOverbiteCurvePointItem->setData(0, Qt::UserRole, 135);
	diContourItem->addChild(diOverbiteCurvePointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diOverbiteCurveDistanceItem = new QTreeWidgetItem(diContourItem, QStringList(itemText));
	diOverbiteCurveDistanceItem->setData(0, Qt::UserRole, 136);
	diContourItem->addChild(diOverbiteCurveDistanceItem);

	// par指数测量
	
	itemText = tr("PAR指数测量");
	QTreeWidgetItem *parMeasureItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	parMeasureItem->setData(0, Qt::UserRole, 2000);
	parMeasureItem->setExpanded(true);
	
	itemText = tr("Front Tooth");
	QTreeWidgetItem *frontToothItem = new QTreeWidgetItem(parMeasureItem, QStringList(itemText));
	frontToothItem->setData(0, Qt::UserRole, 3);
	itemText = tr("Face Side");
	QTreeWidgetItem *faceSideItem = new QTreeWidgetItem(parMeasureItem, QStringList(itemText));
	faceSideItem->setData(0, Qt::UserRole, 18);
	itemText = tr("Cover");
	QTreeWidgetItem *coverItem = new QTreeWidgetItem(parMeasureItem, QStringList(itemText));
	coverItem->setData(0, Qt::UserRole, 65);
	itemText = tr("Inverse Fit Cover");
	QTreeWidgetItem *inverseFitCoverItem = new QTreeWidgetItem(coverItem, QStringList(itemText));
	inverseFitCoverItem->setData(0, Qt::UserRole, 49);
	itemText = tr("Overbite");
	QTreeWidgetItem *overbiteItem = new QTreeWidgetItem(parMeasureItem, QStringList(itemText));
	overbiteItem->setData(0, Qt::UserRole, 43);
	itemText = tr("覆合");
	QTreeWidgetItem *contourItem = new QTreeWidgetItem(overbiteItem, QStringList(itemText));
	contourItem->setData(0, Qt::UserRole, 44);
	overbiteItem->addChild(contourItem);
	itemText = tr("Open Close");
	QTreeWidgetItem *openCloseItem = new QTreeWidgetItem(overbiteItem, QStringList(itemText));
	openCloseItem->setData(0, Qt::UserRole, 61);
	itemText = tr("Middle Line");
	QTreeWidgetItem *middleLineItem = new QTreeWidgetItem(parMeasureItem, QStringList(itemText));
	middleLineItem->setData(0, Qt::UserRole, 34);
	

	/*
	itemText = tr("Front Tooth");
	QTreeWidgetItem *frontToothItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	frontToothItem->setData(0, Qt::UserRole, 3);
	itemText = tr("Face Side");
	QTreeWidgetItem *faceSideItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	faceSideItem->setData(0, Qt::UserRole, 18);
	itemText = tr("Cover");
	QTreeWidgetItem *coverItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	coverItem->setData(0, Qt::UserRole, 65);
	itemText = tr("Inverse Fit Cover");
	QTreeWidgetItem *inverseFitCoverItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	inverseFitCoverItem->setData(0, Qt::UserRole, 49);
	itemText = tr("Overbite");
	QTreeWidgetItem *overbiteItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	overbiteItem->setData(0, Qt::UserRole, 43);
	itemText = tr("Open Close");
	QTreeWidgetItem *openCloseItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	openCloseItem->setData(0, Qt::UserRole, 61);
	itemText = tr("Middle Line");
	QTreeWidgetItem *middleLineItem = new QTreeWidgetItem(this->ui.optionTreeWidget, QStringList(itemText));
	middleLineItem->setData(0, Qt::UserRole, 34);
	*/
	// DI前牙开合
	itemText = tr("Front Tooth Open Close");
	QTreeWidgetItem *diFrontToothOpenCloseItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diFrontToothOpenCloseItem->setData(0, Qt::UserRole, 400);
	diMeasureItem->addChild(diFrontToothOpenCloseItem);

	itemText = tr("Point");
	QTreeWidgetItem *diFrontToothOpenClosePointItem = new QTreeWidgetItem(diFrontToothOpenCloseItem, QStringList(itemText));
	diFrontToothOpenClosePointItem->setData(0, Qt::UserRole, 401);
	diFrontToothOpenCloseItem->addChild(diFrontToothOpenClosePointItem);
	itemText = tr("Score");
	QTreeWidgetItem *diFrontToothOpenCloseScoreItem = new QTreeWidgetItem(diFrontToothOpenCloseItem, QStringList(itemText));
	diFrontToothOpenCloseScoreItem->setData(0, Qt::UserRole, 402);
	diFrontToothOpenCloseItem->addChild(diFrontToothOpenCloseScoreItem);

	// DI侧方开合
	itemText = tr("Side Open Close");
	QTreeWidgetItem *diSideOpenCloseItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diSideOpenCloseItem->setData(0, Qt::UserRole, 137);
	diMeasureItem->addChild(diSideOpenCloseItem);
	diSideOpenCloseToothNum = 8;
	for(int i = 0 ; i < 8; ++i)
	{
		itemText = QString(tr("Grind Tooth")).append(QString::number(i + 1));
		QTreeWidgetItem *diSideOpenCloseGrindToothItem = new QTreeWidgetItem(diSideOpenCloseItem, QStringList(itemText));
		diSideOpenCloseGrindToothItem->setData(0, Qt::UserRole, 140 * 100 + i);
		diSideOpenCloseItem->addChild(diSideOpenCloseGrindToothItem);

		itemText = tr("Point");
		QTreeWidgetItem *diSideOpenClosePointItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenClosePointItem->setData(0, Qt::UserRole, 141 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenClosePointItem);
		itemText = tr("Intersect Plane O");
		QTreeWidgetItem *diSideOpenClosePlaneOItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenClosePlaneOItem->setData(0, Qt::UserRole, 142 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenClosePlaneOItem);
		itemText = tr("Contour Curve");
		QTreeWidgetItem *diSideOpenCloseContourCurveItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenCloseContourCurveItem->setData(0, Qt::UserRole, 143 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenCloseContourCurveItem);
		itemText = tr("Score");
		QTreeWidgetItem *diSideOpenCloseGrindToothScoreItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenCloseGrindToothScoreItem->setData(0, Qt::UserRole, 144 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenCloseGrindToothScoreItem);

		itemText = tr("Point");
		QTreeWidgetItem *diSideOpenClosePlaneOPointItem = new QTreeWidgetItem(diSideOpenClosePlaneOItem, QStringList(itemText));
		diSideOpenClosePlaneOPointItem->setData(0, Qt::UserRole, 145 * 100 + i);
		diSideOpenClosePlaneOItem->addChild(diSideOpenClosePlaneOPointItem);
		itemText = tr("Normal");
		QTreeWidgetItem *diSideOpenClosePlaneONormalItem = new QTreeWidgetItem(diSideOpenClosePlaneOItem, QStringList(itemText));
		diSideOpenClosePlaneONormalItem->setData(0, Qt::UserRole, 146 * 100 + i);
		diSideOpenClosePlaneOItem->addChild(diSideOpenClosePlaneONormalItem);

		itemText = tr("Point");
		QTreeWidgetItem *diSideOpenCloseCurvePointItem = new QTreeWidgetItem(diSideOpenCloseContourCurveItem, QStringList(itemText));
		diSideOpenCloseCurvePointItem->setData(0, Qt::UserRole, 147 * 100 + i);
		diSideOpenCloseContourCurveItem->addChild(diSideOpenCloseCurvePointItem);
		itemText = tr("Distance");
		QTreeWidgetItem *diSideOpenCloseCurveDistanceItem = new QTreeWidgetItem(diSideOpenCloseContourCurveItem, QStringList(itemText));
		diSideOpenCloseCurveDistanceItem->setData(0, Qt::UserRole, 148 * 100 + i);
		diSideOpenCloseContourCurveItem->addChild(diSideOpenCloseCurveDistanceItem);
	}
	itemText = tr("Score");
	QTreeWidgetItem *diSideOpenCloseScoreItem = new QTreeWidgetItem(diSideOpenCloseItem, QStringList(itemText));
	diSideOpenCloseScoreItem->setData(0, Qt::UserRole, 200);
	diSideOpenCloseItem->addChild(diSideOpenCloseScoreItem);

	// DI拥挤度

	itemText = tr("Crowd");
	QTreeWidgetItem *diCrowdItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diCrowdItem->setData(0, Qt::UserRole, 1020);
	diMeasureItem->addChild(diCrowdItem);

	itemText = tr("Grind Tooth");
	QTreeWidgetItem *diGrindToothItem = new QTreeWidgetItem(diCrowdItem, QStringList(itemText));
	diGrindToothItem->setData(0, Qt::UserRole, 1021);
	diCrowdItem->addChild(diGrindToothItem);
	itemText = tr("Tooth Bow");
	QTreeWidgetItem *diToothBowItem = new QTreeWidgetItem(diCrowdItem, QStringList(itemText));
	diToothBowItem->setData(0, Qt::UserRole, 1022);
	diCrowdItem->addChild(diToothBowItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diScrowdDistanceItem = new QTreeWidgetItem(diCrowdItem, QStringList(itemText));
	diScrowdDistanceItem->setData(0, Qt::UserRole, 1024);
	diCrowdItem->addChild(diScrowdDistanceItem);
	itemText = tr("Score");
	QTreeWidgetItem *diScrowdScoreItem = new QTreeWidgetItem(diCrowdItem, QStringList(itemText));
	diScrowdScoreItem->setData(0, Qt::UserRole, 1023);
	diCrowdItem->addChild(diScrowdScoreItem);

	for(int i = 0; i < 10; ++i)
	{
		itemText = QString(tr("Measure")).append(QString::number(i + 1));
		QTreeWidgetItem *diGrindToothMeasureItem = new QTreeWidgetItem(diGrindToothItem, QStringList(itemText));
		diGrindToothMeasureItem->setData(0, Qt::UserRole, 1030 * 100 + i);
		diGrindToothItem->addChild(diGrindToothMeasureItem);

		itemText = tr("Point");
		QTreeWidgetItem *diGrindToothMeasurePointItem = new QTreeWidgetItem(diGrindToothMeasureItem, QStringList(itemText));
		diGrindToothMeasurePointItem->setData(0, Qt::UserRole, 1031 * 100 + i);
		diGrindToothMeasureItem->addChild(diGrindToothMeasurePointItem);
		itemText = tr("Distance");
		QTreeWidgetItem *diGrindToothMeasureDistanceItem = new QTreeWidgetItem(diGrindToothMeasureItem, QStringList(itemText));
		diGrindToothMeasureDistanceItem->setData(0, Qt::UserRole, 1032 * 100 + i);
		diGrindToothMeasureItem->addChild(diGrindToothMeasureDistanceItem);
	}
	itemText = tr("Distance");
	QTreeWidgetItem *diGrindToothMeasureSumDistanceItem = new QTreeWidgetItem(diGrindToothItem, QStringList(itemText));
	diGrindToothMeasureSumDistanceItem->setData(0, Qt::UserRole, 1028);
	diGrindToothItem->addChild(diGrindToothMeasureSumDistanceItem);

	for(int i = 0; i < 4; ++i)
	{
		itemText = QString(tr("Measure")).append(QString::number(i + 1));
		QTreeWidgetItem *diToothBowMeasureItem = new QTreeWidgetItem(diToothBowItem, QStringList(itemText));
		diToothBowMeasureItem->setData(0, Qt::UserRole, 1270 + i * 20);
		diToothBowItem->addChild(diToothBowMeasureItem);

		itemText = tr("Point");
		QTreeWidgetItem *diToothBowMeasurePointItem = new QTreeWidgetItem(diToothBowMeasureItem, QStringList(itemText));
		diToothBowMeasurePointItem->setData(0, Qt::UserRole, 1271 + i * 20);
		diToothBowMeasureItem->addChild(diToothBowMeasurePointItem);
		itemText = tr("Distance");
		QTreeWidgetItem *diToothBowMeasureDistanceItem = new QTreeWidgetItem(diToothBowMeasureItem, QStringList(itemText));
		diToothBowMeasureDistanceItem->setData(0, Qt::UserRole, 1272 + i * 20);
		diToothBowMeasureItem->addChild(diToothBowMeasureDistanceItem);
	}
	itemText = tr("Distance");
	QTreeWidgetItem *diToothBowMeasureSumDistanceItem = new QTreeWidgetItem(diToothBowItem, QStringList(itemText));
	diToothBowMeasureSumDistanceItem->setData(0, Qt::UserRole, 1029);
	diToothBowItem->addChild(diToothBowMeasureSumDistanceItem);

	// DI合关系

	itemText = tr("Close Relation");
	QTreeWidgetItem *diCloseRelationItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diCloseRelationItem->setData(0, Qt::UserRole, 1370);
	diMeasureItem->addChild(diCloseRelationItem);

	itemText = tr("左侧得分");
	QTreeWidgetItem *diCloseRelationScoreItem = new QTreeWidgetItem(diCloseRelationItem, QStringList(itemText));
	diCloseRelationScoreItem->setData(0, Qt::UserRole, 1371);
	diCloseRelationItem->addChild(diCloseRelationScoreItem);

	itemText = tr("右侧得分");
	QTreeWidgetItem *diCloseRelationScoreItem2 = new QTreeWidgetItem(diCloseRelationItem, QStringList(itemText));
	diCloseRelationScoreItem2->setData(0, Qt::UserRole, 1381);
	diCloseRelationItem->addChild(diCloseRelationScoreItem2);

	// DI后牙反合
	itemText = tr("Back Tooth Opposite Close");
	QTreeWidgetItem *diBackToothOppositeCloseItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diBackToothOppositeCloseItem->setData(0, Qt::UserRole, 900);
	diMeasureItem->addChild(diBackToothOppositeCloseItem);

	itemText = tr("Point");
	QTreeWidgetItem *diBackToothOppositeClosePointItem = new QTreeWidgetItem(diBackToothOppositeCloseItem, QStringList(itemText));
	diBackToothOppositeClosePointItem->setData(0, Qt::UserRole, 901);
	diBackToothOppositeCloseItem->addChild(diBackToothOppositeClosePointItem);
	itemText = tr("Project");
	QTreeWidgetItem *diBackToothOppositeCloseProjectPointItem = new QTreeWidgetItem(diBackToothOppositeCloseItem, QStringList(itemText));
	diBackToothOppositeCloseProjectPointItem->setData(0, Qt::UserRole, 902);
	diBackToothOppositeCloseItem->addChild(diBackToothOppositeCloseProjectPointItem);
	itemText = tr("Score");
	QTreeWidgetItem *diBackToothOppositeCloseScoreItem = new QTreeWidgetItem(diBackToothOppositeCloseItem, QStringList(itemText));
	diBackToothOppositeCloseScoreItem->setData(0, Qt::UserRole, 903);
	diBackToothOppositeCloseItem->addChild(diBackToothOppositeCloseScoreItem);

	// DI后牙正锁合
	itemText = tr("Back Tooth Lock Close");
	QTreeWidgetItem *diBackToothLockCloseItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diBackToothLockCloseItem->setData(0, Qt::UserRole, 904);
	diMeasureItem->addChild(diBackToothLockCloseItem);

	itemText = tr("Point");
	QTreeWidgetItem *diBackToothLockClosePointItem = new QTreeWidgetItem(diBackToothLockCloseItem, QStringList(itemText));
	diBackToothLockClosePointItem->setData(0, Qt::UserRole, 905);
	diBackToothLockCloseItem->addChild(diBackToothLockClosePointItem);
	itemText = tr("Project");
	QTreeWidgetItem *diBackToothLockCloseProjectPointItem = new QTreeWidgetItem(diBackToothLockCloseItem, QStringList(itemText));
	diBackToothLockCloseProjectPointItem->setData(0, Qt::UserRole, 906);
	diBackToothLockCloseItem->addChild(diBackToothLockCloseProjectPointItem);
	itemText = tr("Score");
	QTreeWidgetItem *diBackToothLockCloseScoreItem = new QTreeWidgetItem(diBackToothLockCloseItem, QStringList(itemText));
	diBackToothLockCloseScoreItem->setData(0, Qt::UserRole, 907);
	diBackToothLockCloseItem->addChild(diBackToothLockCloseScoreItem);

	// DI其他
	itemText = tr("Other");
	QTreeWidgetItem *diOtherItem = new QTreeWidgetItem(diMeasureItem, QStringList(itemText));
	diOtherItem->setData(0, Qt::UserRole, 1000);
	diMeasureItem->addChild(diOtherItem);

	itemText = tr("Middle Unfit");
	QTreeWidgetItem *diMiddleUnfitItem = new QTreeWidgetItem(diOtherItem, QStringList(itemText));
	diMiddleUnfitItem->setData(0, Qt::UserRole, 1001);
	diOtherItem->addChild(diMiddleUnfitItem);
	itemText = tr("Middle Cut Tooth Gap");
	QTreeWidgetItem *diMiddleCutToothGapItem = new QTreeWidgetItem(diOtherItem, QStringList(itemText));
	diMiddleCutToothGapItem->setData(0, Qt::UserRole, 1005);
	diOtherItem->addChild(diMiddleCutToothGapItem);
	itemText = tr("Score");
	QTreeWidgetItem *diOtherScoreItem = new QTreeWidgetItem(diOtherItem, QStringList(itemText));
	diOtherScoreItem->setData(0, Qt::UserRole, 1009);
	diOtherItem->addChild(diOtherScoreItem);

	itemText = tr("Point");
	QTreeWidgetItem *diMiddleUnfitPointItem = new QTreeWidgetItem(diMiddleUnfitItem, QStringList(itemText));
	diMiddleUnfitPointItem->setData(0, Qt::UserRole, 1002);
	diMiddleUnfitItem->addChild(diMiddleUnfitPointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diMiddleUnfitDistanceItem = new QTreeWidgetItem(diMiddleUnfitItem, QStringList(itemText));
	diMiddleUnfitDistanceItem->setData(0, Qt::UserRole, 1003);
	diMiddleUnfitItem->addChild(diMiddleUnfitDistanceItem);
	itemText = tr("Score");
	QTreeWidgetItem *diMiddleUnfitScoreItem = new QTreeWidgetItem(diMiddleUnfitItem, QStringList(itemText));
	diMiddleUnfitScoreItem->setData(0, Qt::UserRole, 1004);
	diMiddleUnfitItem->addChild(diMiddleUnfitScoreItem);

	itemText = tr("Point");
	QTreeWidgetItem *diMiddleCutToothPointItem = new QTreeWidgetItem(diMiddleCutToothGapItem, QStringList(itemText));
	diMiddleCutToothPointItem->setData(0, Qt::UserRole, 1006);
	diMiddleCutToothGapItem->addChild(diMiddleCutToothPointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diMiddleCutToothDistanceItem = new QTreeWidgetItem(diMiddleCutToothGapItem, QStringList(itemText));
	diMiddleCutToothDistanceItem->setData(0, Qt::UserRole, 1007);
	diMiddleCutToothGapItem->addChild(diMiddleCutToothDistanceItem);
	itemText = tr("Score");
	QTreeWidgetItem *diMiddleCutToothScoreItem = new QTreeWidgetItem(diMiddleCutToothGapItem, QStringList(itemText));
	diMiddleCutToothScoreItem->setData(0, Qt::UserRole, 1008);
	diMiddleCutToothGapItem->addChild(diMiddleCutToothScoreItem);

	//
	// 前牙区不齐
	// 
	
	itemText = tr("Contact Point");
	QTreeWidgetItem *contactPointItem = new QTreeWidgetItem(frontToothItem, QStringList(itemText));
	contactPointItem->setData(0, Qt::UserRole, 8);
	frontToothItem->addChild(contactPointItem);
	itemText = tr("Score");
	QTreeWidgetItem *fitPlaneScoreItem = new QTreeWidgetItem(frontToothItem, QStringList(itemText));
	fitPlaneScoreItem->setData(0, Qt::UserRole, 15);
	frontToothItem->addChild(fitPlaneScoreItem);

	itemText = tr("Fit Point");
	QTreeWidgetItem *fitPlanePointItem = new QTreeWidgetItem(fitPlaneItem, QStringList(itemText));
	fitPlanePointItem->setData(0, Qt::UserRole, 5);
	fitPlaneItem->addChild(fitPlanePointItem);
	itemText = tr("Fit Plane Normal");
	QTreeWidgetItem *fitPlaneNormalItem = new QTreeWidgetItem(fitPlaneItem, QStringList(itemText));
	fitPlaneNormalItem->setData(0, Qt::UserRole, 6);
	fitPlaneItem->addChild(fitPlaneNormalItem);
	itemText = tr("Fit Plane Center");
	QTreeWidgetItem *fitPlaneCenterItem = new QTreeWidgetItem(fitPlaneItem, QStringList(itemText));
	fitPlaneCenterItem->setData(0, Qt::UserRole, 7);
	fitPlaneItem->addChild(fitPlaneCenterItem);
	
	for (int i = 0;i < 10;i ++)
	{
		itemText = QString("缝隙").append(QString::number(i + 1));
		QTreeWidgetItem *gap1Item = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
		gap1Item->setData(0, Qt::UserRole, 9000 + i);
		contactPointItem->addChild(gap1Item);
	}
// 	itemText = tr("Gap1");
// 	QTreeWidgetItem *gap1Item = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
// 	gap1Item->setData(0, Qt::UserRole, 9);
// 	contactPointItem->addChild(gap1Item);
// 	itemText = tr("Gap2");
// 	QTreeWidgetItem *gap2Item = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
// 	gap2Item->setData(0, Qt::UserRole, 10);
// 	contactPointItem->addChild(gap2Item);
// 	itemText = tr("Gap3");
// 	QTreeWidgetItem *gap3Item = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
// 	gap3Item->setData(0, Qt::UserRole, 11);
// 	contactPointItem->addChild(gap3Item);
// 	itemText = tr("Gap4");
// 	QTreeWidgetItem *gap4Item = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
// 	gap4Item->setData(0, Qt::UserRole, 12);
// 	contactPointItem->addChild(gap4Item);
// 	itemText = tr("Gap5");
// 	QTreeWidgetItem *gap5Item = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
// 	gap5Item->setData(0, Qt::UserRole, 13);
// 	contactPointItem->addChild(gap5Item);
	itemText = tr("Project Result");
	QTreeWidgetItem *gapProjectPointItem = new QTreeWidgetItem(contactPointItem, QStringList(itemText));
	gapProjectPointItem->setData(0, Qt::UserRole, 14);
	contactPointItem->addChild(gapProjectPointItem);

	itemText = tr("Front Tooth Distance");
	QTreeWidgetItem *frontToothDistanceItem = new QTreeWidgetItem(fitPlaneScoreItem, QStringList(itemText));
	frontToothDistanceItem->setData(0, Qt::UserRole, 16);
	fitPlaneScoreItem->addChild(frontToothDistanceItem);
	itemText = tr("Score Number");
	QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(fitPlaneScoreItem, QStringList(itemText));
	scoreInfoItem->setData(0, Qt::UserRole, 17);
	fitPlaneScoreItem->addChild(scoreInfoItem);

	//
	// 颊侧区咬合
	//

	itemText = tr("Sagittal Score");
	QTreeWidgetItem *VerticalScoreItem = new QTreeWidgetItem(faceSideItem, QStringList(itemText));
	VerticalScoreItem->setData(0, Qt::UserRole, 19);
	faceSideItem->addChild(VerticalScoreItem);
	itemText = tr("垂直向");
	QTreeWidgetItem *faceSideTopToothItem = new QTreeWidgetItem(faceSideItem, QStringList(itemText));
	faceSideTopToothItem->setData(0, Qt::UserRole, 20);
	faceSideItem->addChild(faceSideTopToothItem);
	itemText = tr("水平向");
	QTreeWidgetItem *faceSideLowToothItem = new QTreeWidgetItem(faceSideItem, QStringList(itemText));
	faceSideLowToothItem->setData(0, Qt::UserRole, 21);
	faceSideItem->addChild(faceSideLowToothItem);
	itemText = tr("Horizonal Score");
	QTreeWidgetItem *horizonalScoreItem = new QTreeWidgetItem(faceSideLowToothItem, QStringList(itemText));
	horizonalScoreItem->setData(0, Qt::UserRole, 22);
	faceSideLowToothItem->addChild(horizonalScoreItem);

// 	itemText = tr("上牙列");
// 	QTreeWidgetItem *topToothVerticalPointItem = new QTreeWidgetItem(faceSideTopToothItem, QStringList(itemText));
// 	topToothVerticalPointItem->setData(0, Qt::UserRole, 28);
	//faceSideTopToothItem->addChild(topToothVerticalPointItem);
	itemText = tr("上牙列");
	QTreeWidgetItem *topToothHorizonalPointItem = new QTreeWidgetItem(faceSideLowToothItem, QStringList(itemText));
	topToothHorizonalPointItem->setData(0, Qt::UserRole, 29);
	faceSideLowToothItem->addChild(topToothHorizonalPointItem);

// 	itemText = tr("Project Result");
// 	QTreeWidgetItem *topToothVerticalPointProjectPointItem = new QTreeWidgetItem(topToothVerticalPointItem, QStringList(itemText));
// 	topToothVerticalPointProjectPointItem->setData(0, Qt::UserRole, 30);
// 	topToothVerticalPointItem->addChild(topToothVerticalPointProjectPointItem);

	itemText = tr("Project Result");
	QTreeWidgetItem *topToothHorizonalPointProjectPointItem = new QTreeWidgetItem(topToothHorizonalPointItem, QStringList(itemText));
	topToothHorizonalPointProjectPointItem->setData(0, Qt::UserRole, 31);
	topToothHorizonalPointItem->addChild(topToothHorizonalPointProjectPointItem);

// 	itemText = tr("下牙列");
// 	QTreeWidgetItem *lowToothVerticalPointItem = new QTreeWidgetItem(faceSideTopToothItem, QStringList(itemText));
// 	lowToothVerticalPointItem->setData(0, Qt::UserRole, 24);
	//faceSideTopToothItem->addChild(lowToothVerticalPointItem);
// 	itemText = tr("下牙列");
// 	QTreeWidgetItem *lowToothHorizonalPointItem = new QTreeWidgetItem(faceSideLowToothItem, QStringList(itemText));
// 	lowToothHorizonalPointItem->setData(0, Qt::UserRole, 25);
// 	faceSideLowToothItem->addChild(lowToothHorizonalPointItem);

	itemText = tr("Middle Vertical Plane Init Point");
	QTreeWidgetItem *middleVerticalPlaneInitPointItem = new QTreeWidgetItem(middleVerticalPlaneItem, QStringList(itemText));
	middleVerticalPlaneInitPointItem->setData(0, Qt::UserRole, 26);
	middleVerticalPlaneItem->addChild(middleVerticalPlaneInitPointItem);
	itemText = tr("Middle Vertical Normal");
	QTreeWidgetItem *middleVerticalPlaneNormalItem = new QTreeWidgetItem(middleVerticalPlaneItem, QStringList(itemText));
	middleVerticalPlaneNormalItem->setData(0, Qt::UserRole, 27);
	middleVerticalPlaneItem->addChild(middleVerticalPlaneNormalItem);

// 	itemText = tr("Project Result");
// 	QTreeWidgetItem *lowToothVerticalPointProjectPointItem = new QTreeWidgetItem(lowToothVerticalPointItem, QStringList(itemText));
// 	lowToothVerticalPointProjectPointItem->setData(0, Qt::UserRole, 32);
// 	lowToothVerticalPointItem->addChild(lowToothVerticalPointProjectPointItem);
// 
// 	itemText = tr("Project Result");
// 	QTreeWidgetItem *lowToothHorizonalProjectPointItem = new QTreeWidgetItem(lowToothHorizonalPointItem, QStringList(itemText));
// 	lowToothHorizonalProjectPointItem->setData(0, Qt::UserRole, 33);
// 	lowToothHorizonalPointItem->addChild(lowToothHorizonalProjectPointItem);


		//
	// 颊侧区咬合 测量部分
	//
	
	itemText = tr("补充测量");
	QTreeWidgetItem *JCoverItem = new QTreeWidgetItem(faceSideTopToothItem, QStringList(itemText));
	JCoverItem->setData(0, Qt::UserRole, 6800);
	faceSideTopToothItem->addChild(JCoverItem);

	itemText = tr("Point");
	QTreeWidgetItem *JCoverPointItem = new QTreeWidgetItem(JCoverItem, QStringList(itemText));
	JCoverPointItem->setData(0, Qt::UserRole, 6900);
	JCoverItem->addChild(JCoverPointItem);
	itemText = tr("Intersect Plane O");
	QTreeWidgetItem *JCoverPlaneOItem = new QTreeWidgetItem(JCoverItem, QStringList(itemText));
	JCoverPlaneOItem->setData(0, Qt::UserRole, 7000);
	JCoverItem->addChild(JCoverPlaneOItem);
	itemText = tr("Contour Curve");
	QTreeWidgetItem *JCoverContourItem = new QTreeWidgetItem(JCoverItem, QStringList(itemText));
	JCoverContourItem->setData(0, Qt::UserRole, 7100);
	JCoverItem->addChild(JCoverContourItem);
	itemText = tr("Score");
	QTreeWidgetItem *JCoverScoreItem = new QTreeWidgetItem(JCoverItem, QStringList(itemText));
	JCoverScoreItem->setData(0, Qt::UserRole, 7200);
	JCoverItem->addChild(JCoverScoreItem);

	itemText = tr("Point");
	QTreeWidgetItem *JCoverPlanePointItem = new QTreeWidgetItem(JCoverPlaneOItem, QStringList(itemText));
	JCoverPlanePointItem->setData(0, Qt::UserRole, 7300);
	JCoverPlaneOItem->addChild(JCoverPlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *JCoverPlaneNormalItem = new QTreeWidgetItem(JCoverPlaneOItem, QStringList(itemText));
	JCoverPlaneNormalItem->setData(0, Qt::UserRole, 7400);
	JCoverPlaneOItem->addChild(JCoverPlaneNormalItem);

	itemText = tr("Point");
	QTreeWidgetItem *JCoverCurvePointItem = new QTreeWidgetItem(JCoverContourItem, QStringList(itemText));
	JCoverCurvePointItem->setData(0, Qt::UserRole, 7500);
	JCoverContourItem->addChild(JCoverCurvePointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *JCoverCurveDistanceItem = new QTreeWidgetItem(JCoverContourItem, QStringList(itemText));
	JCoverCurveDistanceItem->setData(0, Qt::UserRole, 7600);
	JCoverContourItem->addChild(JCoverCurveDistanceItem);

	//
	// 覆盖
	//
	
	itemText = tr("Point");
	QTreeWidgetItem *coverFitCoverPointItem = new QTreeWidgetItem(coverFitCoverItem, QStringList(itemText));
	coverFitCoverPointItem->setData(0, Qt::UserRole, 67);
	coverFitCoverItem->addChild(coverFitCoverPointItem);

	itemText = tr("Cover");
	QTreeWidgetItem *CoverItem = new QTreeWidgetItem(coverItem, QStringList(itemText));
	CoverItem->setData(0, Qt::UserRole, 68);
	coverItem->addChild(CoverItem);

	itemText = tr("Point");
	QTreeWidgetItem *CoverPointItem = new QTreeWidgetItem(CoverItem, QStringList(itemText));
	CoverPointItem->setData(0, Qt::UserRole, 69);
	CoverItem->addChild(CoverPointItem);
	itemText = tr("Intersect Plane O");
	QTreeWidgetItem *CoverPlaneOItem = new QTreeWidgetItem(CoverItem, QStringList(itemText));
	CoverPlaneOItem->setData(0, Qt::UserRole, 70);
	CoverItem->addChild(CoverPlaneOItem);
	itemText = tr("Contour Curve");
	QTreeWidgetItem *CoverContourItem = new QTreeWidgetItem(CoverItem, QStringList(itemText));
	CoverContourItem->setData(0, Qt::UserRole, 71);
	CoverItem->addChild(CoverContourItem);
	itemText = tr("Score");
	QTreeWidgetItem *CoverScoreItem = new QTreeWidgetItem(CoverItem, QStringList(itemText));
	CoverScoreItem->setData(0, Qt::UserRole, 72);
	CoverItem->addChild(CoverScoreItem);

	itemText = tr("Point");
	QTreeWidgetItem *CoverPlanePointItem = new QTreeWidgetItem(CoverPlaneOItem, QStringList(itemText));
	CoverPlanePointItem->setData(0, Qt::UserRole, 73);
	CoverPlaneOItem->addChild(CoverPlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *CoverPlaneNormalItem = new QTreeWidgetItem(CoverPlaneOItem, QStringList(itemText));
	CoverPlaneNormalItem->setData(0, Qt::UserRole, 74);
	CoverPlaneOItem->addChild(CoverPlaneNormalItem);

	itemText = tr("Point");
	QTreeWidgetItem *CoverCurvePointItem = new QTreeWidgetItem(CoverContourItem, QStringList(itemText));
	CoverCurvePointItem->setData(0, Qt::UserRole, 75);
	CoverContourItem->addChild(CoverCurvePointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *CoverCurveDistanceItem = new QTreeWidgetItem(CoverContourItem, QStringList(itemText));
	CoverCurveDistanceItem->setData(0, Qt::UserRole, 76);
	CoverContourItem->addChild(CoverCurveDistanceItem);


	//
	// 反合覆盖
	//
// 	itemText = tr("Top Tooth");
// 	QTreeWidgetItem *topToothForInverseFitCoverItem = new QTreeWidgetItem(inverseFitCoverItem, QStringList(itemText));
// 	topToothForInverseFitCoverItem->setData(0, Qt::UserRole, 50);
// 	//inverseFitCoverItem->addChild(topToothForInverseFitCoverItem);
// 	itemText = tr("Low Tooth");
// 	QTreeWidgetItem *lowToothForInverseFitCoverItem = new QTreeWidgetItem(inverseFitCoverItem, QStringList(itemText));
// 	lowToothForInverseFitCoverItem->setData(0, Qt::UserRole, 60);
// 	//inverseFitCoverItem->addChild(lowToothForInverseFitCoverItem);
	itemText = tr("Score");
	QTreeWidgetItem *scoreForInverseFitCoverItem = new QTreeWidgetItem(inverseFitCoverItem, QStringList(itemText));
	scoreForInverseFitCoverItem->setData(0, Qt::UserRole, 55);
	inverseFitCoverItem->addChild(scoreForInverseFitCoverItem);

// 	itemText = tr("Point Pair1");
// 	QTreeWidgetItem *pointPair1TopToothForInverseFitCoverItem = new QTreeWidgetItem(topToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair1TopToothForInverseFitCoverItem->setData(0, Qt::UserRole, 51);
// 	topToothForInverseFitCoverItem->addChild(pointPair1TopToothForInverseFitCoverItem);
// 	itemText = tr("Point Pair2");
// 	QTreeWidgetItem *pointPair2TopToothForInverseFitCoverItem = new QTreeWidgetItem(topToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair2TopToothForInverseFitCoverItem->setData(0, Qt::UserRole, 52);
// 	topToothForInverseFitCoverItem->addChild(pointPair2TopToothForInverseFitCoverItem);
// 	itemText = tr("Point Pair3");
// 	QTreeWidgetItem *pointPair3TopToothForInverseFitCoverItem = new QTreeWidgetItem(topToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair3TopToothForInverseFitCoverItem->setData(0, Qt::UserRole, 53);
// 	topToothForInverseFitCoverItem->addChild(pointPair3TopToothForInverseFitCoverItem);
// 	itemText = tr("Point Pair4");
// 	QTreeWidgetItem *pointPair4TopToothForInverseFitCoverItem = new QTreeWidgetItem(topToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair4TopToothForInverseFitCoverItem->setData(0, Qt::UserRole, 54);
// 	topToothForInverseFitCoverItem->addChild(pointPair4TopToothForInverseFitCoverItem);

// 	itemText = tr("Point Pair1");
// 	QTreeWidgetItem *pointPair1LowToothForInverseFitCoverItem = new QTreeWidgetItem(lowToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair1LowToothForInverseFitCoverItem->setData(0, Qt::UserRole, 56);
// 	lowToothForInverseFitCoverItem->addChild(pointPair1LowToothForInverseFitCoverItem);
// 	itemText = tr("Point Pair2");
// 	QTreeWidgetItem *pointPair2LowToothForInverseFitCoverItem = new QTreeWidgetItem(lowToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair2LowToothForInverseFitCoverItem->setData(0, Qt::UserRole, 57);
// 	lowToothForInverseFitCoverItem->addChild(pointPair2LowToothForInverseFitCoverItem);
// 	itemText = tr("Point Pair3");
// 	QTreeWidgetItem *pointPair3LowToothForInverseFitCoverItem = new QTreeWidgetItem(lowToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair3LowToothForInverseFitCoverItem->setData(0, Qt::UserRole, 58);
// 	lowToothForInverseFitCoverItem->addChild(pointPair3LowToothForInverseFitCoverItem);
// 	itemText = tr("Point Pair4");
// 	QTreeWidgetItem *pointPair4LowToothForInverseFitCoverItem = new QTreeWidgetItem(lowToothForInverseFitCoverItem, QStringList(itemText));
// 	pointPair4LowToothForInverseFitCoverItem->setData(0, Qt::UserRole, 59);
// 	lowToothForInverseFitCoverItem->addChild(pointPair4LowToothForInverseFitCoverItem);

	//
	// 覆合
	//


	itemText = tr("Score");
	QTreeWidgetItem *overbiteScore = new QTreeWidgetItem(contourItem, QStringList(itemText));
	overbiteScore->setData(0, Qt::UserRole, 48);
	contourItem->addChild(overbiteScore);

	itemText = tr("Intersect plane");
	QTreeWidgetItem *intersectPlaneItem = new QTreeWidgetItem(contourItem, QStringList(itemText));
	intersectPlaneItem->setData(0, Qt::UserRole, 45);
	contourItem->addChild(intersectPlaneItem);
	
	itemText = tr("Init Point");
	QTreeWidgetItem *intersectPlaneInitPointItem = new QTreeWidgetItem(intersectPlaneItem, QStringList(itemText));
	intersectPlaneInitPointItem->setData(0, Qt::UserRole, 46);
	intersectPlaneItem->addChild(intersectPlaneInitPointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *intersectPlaneNormalItem = new QTreeWidgetItem(intersectPlaneItem, QStringList(itemText));
	intersectPlaneNormalItem->setData(0, Qt::UserRole, 47);
	intersectPlaneItem->addChild(intersectPlaneNormalItem);

	//
	// 开合
	//

// 	itemText = tr("Front Tooth Open Close");
// 	QTreeWidgetItem *frontToothOpenCloseItem = new QTreeWidgetItem(openCloseItem, QStringList(itemText));
// 	frontToothOpenCloseItem->setData(0, Qt::UserRole, 84);
// 	openCloseItem->addChild(frontToothOpenCloseItem);

// 	itemText = tr("Back Tooth Open Close");
// 	QTreeWidgetItem *backToothOpenCloseItem = new QTreeWidgetItem(openCloseItem, QStringList(itemText));
// 	backToothOpenCloseItem->setData(0, Qt::UserRole, 62);
// 	openCloseItem->addChild(backToothOpenCloseItem);

	//itemText = tr("Article Plane");
	//QTreeWidgetItem *articlePlaneItem = new QTreeWidgetItem(frontToothOpenCloseItem, QStringList(itemText));
	//articlePlaneItem->setData(0, Qt::UserRole, 85);
	//frontToothOpenCloseItem->addChild(articlePlaneItem);
// 	itemText = tr("Point");
// 	QTreeWidgetItem *articlePlanePointToProjectItem = new QTreeWidgetItem(openCloseItem, QStringList(itemText));
// 	articlePlanePointToProjectItem->setData(0, Qt::UserRole, 88);
// 	openCloseItem->addChild(articlePlanePointToProjectItem);
	itemText = tr("Intersect Plane");
	QTreeWidgetItem *frontToothOpenCloseIntersectPlaneItem = new QTreeWidgetItem(openCloseItem, QStringList(itemText));
	frontToothOpenCloseIntersectPlaneItem->setData(0, Qt::UserRole, 90);
	openCloseItem->addChild(frontToothOpenCloseIntersectPlaneItem);
	itemText = tr("拟合轮廓曲线");
	QTreeWidgetItem *frontToothOpenCloseFittingCurveItem = new QTreeWidgetItem(openCloseItem, QStringList(itemText));
	frontToothOpenCloseFittingCurveItem->setData(0, Qt::UserRole, 89);
	openCloseItem->addChild(frontToothOpenCloseFittingCurveItem);

	itemText = tr("Init Point");
	QTreeWidgetItem *articlePlanePointItem = new QTreeWidgetItem(articlePlaneItem, QStringList(itemText));
	articlePlanePointItem->setData(0, Qt::UserRole, 86);
	articlePlaneItem->addChild(articlePlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *articlePlaneNormalItem = new QTreeWidgetItem(articlePlaneItem, QStringList(itemText));
	articlePlaneNormalItem->setData(0, Qt::UserRole, 87);
	articlePlaneItem->addChild(articlePlaneNormalItem);
	

// 	itemText = tr("Top Tooth");
// 	QTreeWidgetItem *articlePlanePointTopToothItem = new QTreeWidgetItem(articlePlanePointToProjectItem, QStringList(itemText));
// 	articlePlanePointTopToothItem->setData(0, Qt::UserRole, 101);
// 	articlePlanePointToProjectItem->addChild(articlePlanePointTopToothItem);
// 	itemText = tr("Low Tooth");
// 	QTreeWidgetItem *articlePlanePointLowToothItem = new QTreeWidgetItem(articlePlanePointToProjectItem, QStringList(itemText));
// 	articlePlanePointLowToothItem->setData(0, Qt::UserRole, 102);
// 	articlePlanePointToProjectItem->addChild(articlePlanePointLowToothItem);

// 	itemText = tr("Point Pair1");
// 	QTreeWidgetItem *articlePlanePointPair1TopToothItem = new QTreeWidgetItem(articlePlanePointTopToothItem, QStringList(itemText));
// 	articlePlanePointPair1TopToothItem->setData(0, Qt::UserRole, 103);
// 	articlePlanePointTopToothItem->addChild(articlePlanePointPair1TopToothItem);
// 	itemText = tr("Point Pair2");
// 	QTreeWidgetItem *articlePlanePointPair2TopToothItem = new QTreeWidgetItem(articlePlanePointTopToothItem, QStringList(itemText));
// 	articlePlanePointPair2TopToothItem->setData(0, Qt::UserRole, 104);
// 	articlePlanePointTopToothItem->addChild(articlePlanePointPair2TopToothItem);
// 	itemText = tr("Point Pair3");
// 	QTreeWidgetItem *articlePlanePointPair3TopToothItem = new QTreeWidgetItem(articlePlanePointTopToothItem, QStringList(itemText));
// 	articlePlanePointPair3TopToothItem->setData(0, Qt::UserRole, 105);
// 	articlePlanePointTopToothItem->addChild(articlePlanePointPair3TopToothItem);
// 	itemText = tr("Point Pair4");
// 	QTreeWidgetItem *articlePlanePointPair4TopToothItem = new QTreeWidgetItem(articlePlanePointTopToothItem, QStringList(itemText));
// 	articlePlanePointPair4TopToothItem->setData(0, Qt::UserRole, 106);
// 	articlePlanePointTopToothItem->addChild(articlePlanePointPair4TopToothItem);
// 
// 	itemText = tr("Point Pair1");
// 	QTreeWidgetItem *articlePlanePointPair1LowToothItem = new QTreeWidgetItem(articlePlanePointLowToothItem, QStringList(itemText));
// 	articlePlanePointPair1LowToothItem->setData(0, Qt::UserRole, 107);
// 	articlePlanePointLowToothItem->addChild(articlePlanePointPair1LowToothItem);
// 	itemText = tr("Point Pair2");
// 	QTreeWidgetItem *articlePlanePointPair2LowToothItem = new QTreeWidgetItem(articlePlanePointLowToothItem, QStringList(itemText));
// 	articlePlanePointPair2LowToothItem->setData(0, Qt::UserRole, 108);
// 	articlePlanePointLowToothItem->addChild(articlePlanePointPair2LowToothItem);
// 	itemText = tr("Point Pair3");
// 	QTreeWidgetItem *articlePlanePointPair3LowToothItem = new QTreeWidgetItem(articlePlanePointLowToothItem, QStringList(itemText));
// 	articlePlanePointPair3LowToothItem->setData(0, Qt::UserRole, 109);
// 	articlePlanePointLowToothItem->addChild(articlePlanePointPair3LowToothItem);
// 	itemText = tr("Point Pair4");
// 	QTreeWidgetItem *articlePlanePointPair4LowToothItem = new QTreeWidgetItem(articlePlanePointLowToothItem, QStringList(itemText));
// 	articlePlanePointPair4LowToothItem->setData(0, Qt::UserRole, 110);
// 	articlePlanePointLowToothItem->addChild(articlePlanePointPair4LowToothItem);

// 	itemText = tr("Point");
// 	QTreeWidgetItem *frontToothFittingCurvePointItem = new QTreeWidgetItem(frontToothOpenCloseFittingCurveItem, QStringList(itemText));
// 	frontToothFittingCurvePointItem->setData(0, Qt::UserRole, 91);
// 	frontToothOpenCloseFittingCurveItem->addChild(frontToothFittingCurvePointItem);

	itemText = tr("Point");
	QTreeWidgetItem *frontToothOpenCloseIntersectPlanePointItem = new QTreeWidgetItem(frontToothOpenCloseIntersectPlaneItem, QStringList(itemText));
	frontToothOpenCloseIntersectPlanePointItem->setData(0, Qt::UserRole, 92);
	frontToothOpenCloseIntersectPlaneItem->addChild(frontToothOpenCloseIntersectPlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *frontToothOpenCloseIntersectPlaneNormalItem = new QTreeWidgetItem(frontToothOpenCloseIntersectPlaneItem, QStringList(itemText));
	frontToothOpenCloseIntersectPlaneNormalItem->setData(0, Qt::UserRole, 93);
	frontToothOpenCloseIntersectPlaneItem->addChild(frontToothOpenCloseIntersectPlaneNormalItem);
	itemText = tr("Contour Curve");
	QTreeWidgetItem *frontToothOpenCloseIntersectContourCurveItem = new QTreeWidgetItem(frontToothOpenCloseFittingCurveItem, QStringList(itemText));//////////////////////
	frontToothOpenCloseIntersectContourCurveItem->setData(0, Qt::UserRole, 94);
	frontToothOpenCloseFittingCurveItem->addChild(frontToothOpenCloseIntersectContourCurveItem);

	itemText = tr("Point");
	QTreeWidgetItem *frontToothOpenCloseIntersectContourCurvePointItem = new QTreeWidgetItem(frontToothOpenCloseIntersectContourCurveItem, QStringList(itemText));
	frontToothOpenCloseIntersectContourCurvePointItem->setData(0, Qt::UserRole, 95);
	frontToothOpenCloseIntersectContourCurveItem->addChild(frontToothOpenCloseIntersectContourCurvePointItem);
	itemText = tr("Distance");
	QTreeWidgetItem *frontToothOpenCloseIntersectContourCurveDistanceItem = new QTreeWidgetItem(frontToothOpenCloseIntersectContourCurveItem, QStringList(itemText));
	frontToothOpenCloseIntersectContourCurveDistanceItem->setData(0, Qt::UserRole, 96);
	frontToothOpenCloseIntersectContourCurveItem->addChild(frontToothOpenCloseIntersectContourCurveDistanceItem);
// 
// 	itemText = tr("Fitting Curve");
// 	QTreeWidgetItem *backToothOpenCloseFittingCurveItem = new QTreeWidgetItem(backToothOpenCloseItem, QStringList(itemText));
// 	backToothOpenCloseFittingCurveItem->setData(0, Qt::UserRole, 63);
// 	backToothOpenCloseItem->addChild(backToothOpenCloseFittingCurveItem);
// 	itemText = tr("Back Tooth Open Close Intersect Plane");
// 	QTreeWidgetItem *backToothOpenCloseIntersectPlaneItem = new QTreeWidgetItem(backToothOpenCloseItem, QStringList(itemText));
// 	backToothOpenCloseIntersectPlaneItem->setData(0, Qt::UserRole, 78);
// 	backToothOpenCloseItem->addChild(backToothOpenCloseIntersectPlaneItem);
// 
// 	itemText = tr("Point");
// 	QTreeWidgetItem *backToothOpenCloseFittingCurvePointItem = new QTreeWidgetItem(backToothOpenCloseFittingCurveItem, QStringList(itemText));
// 	backToothOpenCloseFittingCurvePointItem->setData(0, Qt::UserRole, 64);
// 	backToothOpenCloseFittingCurveItem->addChild(backToothOpenCloseFittingCurvePointItem);
// 
// 	itemText = tr("Point");
// 	QTreeWidgetItem *backToothOpenCloseIntersectPlanePointItem = new QTreeWidgetItem(backToothOpenCloseIntersectPlaneItem, QStringList(itemText));
// 	backToothOpenCloseIntersectPlanePointItem->setData(0, Qt::UserRole, 79);
// 	backToothOpenCloseIntersectPlaneItem->addChild(backToothOpenCloseIntersectPlanePointItem);
// 	itemText = tr("Normal");
// 	QTreeWidgetItem *backToothOpenCloseIntersectPlaneNormalItem = new QTreeWidgetItem(backToothOpenCloseIntersectPlaneItem, QStringList(itemText));
// 	backToothOpenCloseIntersectPlaneNormalItem->setData(0, Qt::UserRole, 80);
// 	backToothOpenCloseIntersectPlaneItem->addChild(backToothOpenCloseIntersectPlaneNormalItem);
// 	itemText = tr("Contour Curve");
// 	QTreeWidgetItem *backToothOpenCloseIntersectPlaneContourCurveItem = new QTreeWidgetItem(backToothOpenCloseIntersectPlaneItem, QStringList(itemText));
// 	backToothOpenCloseIntersectPlaneContourCurveItem->setData(0, Qt::UserRole, 81);
// 	backToothOpenCloseIntersectPlaneItem->addChild(backToothOpenCloseIntersectPlaneContourCurveItem);
// 
// 	itemText = tr("Point");
// 	QTreeWidgetItem *pointOfBackToothContourCurveItem = new QTreeWidgetItem(backToothOpenCloseIntersectPlaneContourCurveItem, QStringList(itemText));
// 	pointOfBackToothContourCurveItem->setData(0, Qt::UserRole, 82);
// 	backToothOpenCloseIntersectPlaneContourCurveItem->addChild(pointOfBackToothContourCurveItem);
// 	itemText = tr("Distance");
// 	QTreeWidgetItem *distanceOfBackToothContourCurveItem = new QTreeWidgetItem(backToothOpenCloseIntersectPlaneContourCurveItem, QStringList(itemText));
// 	distanceOfBackToothContourCurveItem->setData(0, Qt::UserRole, 83);
// 	backToothOpenCloseIntersectPlaneContourCurveItem->addChild(distanceOfBackToothContourCurveItem);

	//
	// 中线
	//

	itemText = tr("vertical Parallel Plane1");
	QTreeWidgetItem *verticalParallelPlaneItem = new QTreeWidgetItem(middleLineItem, QStringList(itemText));
	verticalParallelPlaneItem->setData(0, Qt::UserRole, 35);
	middleLineItem->addChild(verticalParallelPlaneItem);
	itemText = tr("vertical Parallel Plane2");
	QTreeWidgetItem *verticalParallelPlane2Item = new QTreeWidgetItem(middleLineItem, QStringList(itemText));
	verticalParallelPlane2Item->setData(0, Qt::UserRole, 97);
	middleLineItem->addChild(verticalParallelPlane2Item);
	itemText = tr("Distance");
	QTreeWidgetItem *distanceBetweenParallelVerticalPlaneItem = new QTreeWidgetItem(middleLineItem, QStringList(itemText));
	distanceBetweenParallelVerticalPlaneItem->setData(0, Qt::UserRole, 98);
	middleLineItem->addChild(distanceBetweenParallelVerticalPlaneItem);
	//itemText = tr("Point");
	//QTreeWidgetItem *pointProjectToFitPlaneItem = new QTreeWidgetItem(middleLineItem, QStringList(itemText));
	//pointProjectToFitPlaneItem->setData(0, Qt::UserRole, 39);
	//middleLineItem->addChild(pointProjectToFitPlaneItem);
	itemText = tr("Score");
	QTreeWidgetItem *verticalParallelPlaneScoreItem = new QTreeWidgetItem(middleLineItem, QStringList(itemText));
	verticalParallelPlaneScoreItem->setData(0, Qt::UserRole, 42);
	middleLineItem->addChild(verticalParallelPlaneScoreItem);

	itemText = tr("Plane Init Point");
	QTreeWidgetItem *verticalParallelPlaneInitPointItem = new QTreeWidgetItem(verticalParallelPlaneItem, QStringList(itemText));
	verticalParallelPlaneInitPointItem->setData(0, Qt::UserRole, 36);
	verticalParallelPlaneItem->addChild(verticalParallelPlaneInitPointItem);
	itemText = tr("Plane Normal");
	QTreeWidgetItem *verticalParallelPlaneNormalItem = new QTreeWidgetItem(verticalParallelPlaneItem, QStringList(itemText));
	verticalParallelPlaneNormalItem->setData(0, Qt::UserRole, 37);
	verticalParallelPlaneItem->addChild(verticalParallelPlaneNormalItem);
	//itemText = tr("vertical plane and parallel Plane Distance");
	//QTreeWidgetItem *verticalParallelPlaneDistanceItem = new QTreeWidgetItem(verticalParallelPlaneItem, QStringList(itemText));
	//verticalParallelPlaneDistanceItem->setData(0, Qt::UserRole, 38);
	//verticalParallelPlaneItem->addChild(verticalParallelPlaneDistanceItem);

	itemText = tr("Plane Init Point");
	QTreeWidgetItem *verticalParallelPlaneInitPoint2Item = new QTreeWidgetItem(verticalParallelPlane2Item, QStringList(itemText));
	verticalParallelPlaneInitPoint2Item->setData(0, Qt::UserRole, 99);
	verticalParallelPlane2Item->addChild(verticalParallelPlaneInitPoint2Item);
	itemText = tr("Plane Normal");
	QTreeWidgetItem *verticalParallelPlaneNormal2Item = new QTreeWidgetItem(verticalParallelPlane2Item, QStringList(itemText));
	verticalParallelPlaneNormal2Item->setData(0, Qt::UserRole, 100);
	verticalParallelPlane2Item->addChild(verticalParallelPlaneNormal2Item);

	//itemText = tr("Project Result");
	//QTreeWidgetItem *middleLineProjectResultItem = new QTreeWidgetItem(pointProjectToFitPlaneItem, QStringList(itemText));
	//middleLineProjectResultItem->setData(0, Qt::UserRole, 40);
	//middleLineProjectResultItem->addChild(verticalParallelPlaneDistanceItem);

	//itemText = tr("Distance");
	//QTreeWidgetItem *middleLineProjectResultDistanceItem = new QTreeWidgetItem(middleLineProjectResultItem, QStringList(itemText));
	//middleLineProjectResultDistanceItem->setData(0, Qt::UserRole, 41);
	//middleLineProjectResultDistanceItem->addChild(verticalParallelPlaneDistanceItem);
	

// 	itemText = tr("(28.675 2.88735 -23.1923)");
// 	QTreeWidgetItem *p1 = new QTreeWidgetItem(pointItem, QStringList(itemText));
// 	pointItem->addChild(p1);
// 
// 	itemText = tr("(-28.3588 4.9621 -21.4882)");
// 	QTreeWidgetItem *p2 = new QTreeWidgetItem(pointItem, QStringList(itemText));
// 	pointItem->addChild(p2);
// 
// 
// 
// 	itemText = tr("平面1");
// 	QTreeWidgetItem * plane1 = new QTreeWidgetItem(iplaneItem, QStringList(itemText));
// 	iplaneItem->addChild(plane1);
// 
// 	itemText = tr("(0.25614 3.42414 13.866)");
// 	QTreeWidgetItem * proj1 = new QTreeWidgetItem(iprojectItem, QStringList(itemText));
// 	iprojectItem->addChild(proj1);
// 
// 	itemText = tr("曲线1");
// 	QTreeWidgetItem * curve1 = new QTreeWidgetItem(icutcurveItem, QStringList(itemText));
// 	icutcurveItem->addChild(curve1);
// 
// 
// 	itemText = tr("向量1");
// 	QTreeWidgetItem * vector1 = new QTreeWidgetItem(ivectorItem, QStringList(itemText));
// 	ivectorItem->addChild(vector1);
// 
// 	itemText = tr("15.3245");
// 	QTreeWidgetItem * d11 = new QTreeWidgetItem(iPointPointItem, QStringList(itemText));
// 	iPointPointItem->addChild(d11);
// 
// 	itemText = tr("6.751");
// 	QTreeWidgetItem * d21 = new QTreeWidgetItem(iPointVectorItem, QStringList(itemText));
// 	iPointVectorItem->addChild(d21);
// 
// 	itemText = tr("2.337");
// 	QTreeWidgetItem * d31 = new QTreeWidgetItem(iPointPlaneItem, QStringList(itemText));
// 	iPointPlaneItem->addChild(d31);
// 
// 
// 	itemText = tr("3.159");
// 	QTreeWidgetItem * d41 = new QTreeWidgetItem(iPlanePlaneItem, QStringList(itemText));
// 	iPlanePlaneItem->addChild(d41);
// 
// 	itemText = tr("0.2214");
// 	QTreeWidgetItem * d51 = new QTreeWidgetItem(iVectorVectorItem, QStringList(itemText));
// 	iVectorVectorItem->addChild(d51);
// 
// 	itemText = tr("0.3861");
// 	QTreeWidgetItem * d61 = new QTreeWidgetItem(iPointAngleItem, QStringList(itemText));
// 	iPointAngleItem->addChild(d61);

	ScanTree();
	loadData("进度");
}

void MainWindow::showSelect()
{
	//if(selectOptionTreeIndex == 0 || selectOptionTreeIndex == 3 || selectOptionTreeIndex == 6 || selectOptionTreeIndex == 7)
	//{
	//	QMessageBox::information(NULL, tr("Info"), QString(tr("Please select show things...")));
	//	return; 
	//}
	//else 
	/*QMessageBox::information(NULL, tr("Action"), QString::number(selectOptionTreeIndex));*/
	if(selectOptionTreeIndex == 1)
	{
		if(isTopToothShow == 0)
		{
			isTopToothShow = 1;
			this->ren->AddActor(topToothActor);
		}
	}
	else if(selectOptionTreeIndex == 2)
	{
		if(isLowToothShow == 0)
		{
			isLowToothShow = 1;
			this->ren->AddActor(lowerToothActor);
		}
	}

	else if((selectOptionTreeIndex - 1510) >= 0 && (selectOptionTreeIndex - 1510) < 500 && (selectOptionTreeIndex - 1510) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1510) / 25;

		for(int i = 0; i < aboArrangePointPickNum[pointPareIndex]; ++i)
		{
			if(isAboArrangePointShow[pointPareIndex][i] == 0)
				switchAboArrangePointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 1513) >= 0 && (selectOptionTreeIndex - 1513) < 500 && (selectOptionTreeIndex - 1513) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1513) / 25;

		if(isAboArrangePlaneOShow[pointPareIndex] == 0)
			switchAboArrangePlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 1519) >= 0 && (selectOptionTreeIndex - 1519) < 500 && (selectOptionTreeIndex - 1519) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1519) / 25;

		for(int i = 0; i < aboArrangePointPickNum[pointPareIndex]; ++i)
		{
			if(isAboArrangeProjectPointShow[pointPareIndex][i] == 0)
				switchAboArrangeProjectPointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 2010) >= 0 && (selectOptionTreeIndex - 2010) < 500 && (selectOptionTreeIndex - 2010) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2010) / 25;

		if(isAboEdgeVertebralHeighPointShow[pointPareIndex][0] == 0 || isAboEdgeVertebralHeighPointShow[pointPareIndex][1] == 0)
			switchAboEdgeVertebralHeighPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 2013) >= 0 && (selectOptionTreeIndex - 2013) < 500 && (selectOptionTreeIndex - 2013) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2013) / 25;

		if(isAboEdgeVertebralHeighProjectPointShow[pointPareIndex][0] == 0 || isAboEdgeVertebralHeighProjectPointShow[pointPareIndex][1] == 0)
			switchAboEdgeVertebralHeighProjectPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 2512) >= 0 && (selectOptionTreeIndex - 2512) < 500 && (selectOptionTreeIndex - 2512) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2512) / 25;

		if(isChapProjectPointShow[pointPareIndex][0] == 0 && isChapProjectPointShow[pointPareIndex][1] == 0)
			switchChapProjectPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 2511) >= 0 && (selectOptionTreeIndex - 2511) < 500 && (selectOptionTreeIndex - 2511) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2511) / 25;
		for(int i = 0; i < aboChapPointPickNum[pointPareIndex]; ++i)
		{
			if(isChapPointShow[pointPareIndex][i] == 0)
				switchChapPointShow(pointPareIndex);
		}
	}
	else if(selectOptionTreeIndex == 2501)
	{
		//QMessageBox::information(NULL, tr("Info"), QString::number(isChapFitPlaneShow));
		if(isChapFitPlaneShow == 0)
			switchChapFitPlaneShow();
	}
	else if((selectOptionTreeIndex - 3012) >= 0 && (selectOptionTreeIndex - 3012) < 500 && (selectOptionTreeIndex - 3012) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3012) / 25;
		if(isAboToothCloseRelationPointShow[pointPareIndex][0] == 0)
			switchAboToothColoseRelationLowPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3013) >= 0 && (selectOptionTreeIndex - 3013) < 500 && (selectOptionTreeIndex - 3013) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3013) / 25;
		if(isAboToothCloseRelationPlaneOShow[pointPareIndex] == 0)
			switchAboToothCloseRelationPlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3017) >= 0 && (selectOptionTreeIndex - 3017) < 500 && (selectOptionTreeIndex - 3017) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3017) / 25;
		if(isAboToothCloseRelationPointShow[pointPareIndex][1] == 0)
			switchAboToothCloseRelationTopPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3511) >= 0 && (selectOptionTreeIndex - 3511) < 500 && (selectOptionTreeIndex - 3511) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3511) / 25;
		if(isAboToothCloseTouchPointShow[pointPareIndex][0] == 0)
			switchAboToothCloseTouchPointAShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3516) >= 0 && (selectOptionTreeIndex - 3516) < 500 && (selectOptionTreeIndex - 3516) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3516) / 25;
		if(isAboToothCloseTouchPointShow[pointPareIndex][1] == 0)
			switchAboToothCloseTouchPointBShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3512) >= 0 && (selectOptionTreeIndex - 3512) < 500 && (selectOptionTreeIndex - 3512) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3512) / 25;
		if(isAboToothCloseTouchPlaneOShow[pointPareIndex] == 0)
			switchAboToothCloseTouchPlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3515) >= 0 && (selectOptionTreeIndex - 3515) < 500 && (selectOptionTreeIndex - 3515) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3515) / 25;
		if(isAboToothCloseTouchIntersectCurveShow[pointPareIndex] == 0)
			switchAboToothCloseTouchIntersectCurveShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4011) >= 0 && (selectOptionTreeIndex - 4011) < 500 && (selectOptionTreeIndex - 4011) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4011) / 25;
		if(isAboCoverPointShow[pointPareIndex][1] == 0)
			switchAboCoverPointAShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4018) >= 0 && (selectOptionTreeIndex - 4018) < 500 && (selectOptionTreeIndex - 4018) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4018) / 25;
		if(isAboCoverPointShow[pointPareIndex][0] == 0)
			switchAboCoverPointBShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4012) >= 0 && (selectOptionTreeIndex - 4012) < 500 && (selectOptionTreeIndex - 4012) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4012) / 25;
		if(isAboCoverPlaneOShow[pointPareIndex] == 0)
			switchAboCoverPlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4015) >= 0 && (selectOptionTreeIndex - 4015) < 500 && (selectOptionTreeIndex - 4015) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4015) / 25;
		if(isCoverIntersectCurveShow[pointPareIndex][0] == 0 && isCoverIntersectCurveShow[pointPareIndex][1] == 0)
			switchisCoverIntersectCurveShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4511) >= 0 && (selectOptionTreeIndex - 4511) < 500 && (selectOptionTreeIndex - 4511) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4511) / 25;
		for(int i = 0; i < aboTouchRelationPointNum[pointPareIndex]; ++i)
		{
			if(isAboTouchRelationPointShow[pointPareIndex][i] == 0)
				switchboTouchRelationPointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 4512) >= 0 && (selectOptionTreeIndex - 4512) < 500 && (selectOptionTreeIndex - 4512) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4512) / 25;
		for(int i = 0; i < aboTouchRelationPointNum[pointPareIndex]; ++i)
		{
			if(isAboTouchRelationProjectPointShow[pointPareIndex][i] == 0)
				switchAboTouchRelationProjectPointShow(pointPareIndex);
		}
	}
	else if(selectOptionTreeIndex == 4)
	{
		if(isFitPlaneShow == 0)
		{
			isFitPlaneShow = 1;
			this->ren->AddActor(fitPlaneActor);
			this->ren->AddActor(fitPlaneFrameActor);
		}
	}
	else if(selectOptionTreeIndex == 1373)
	{
		if(isDiCloseRelationPointAShow == 0)
			switchDiCloseRelationPointAShow();
	}
	else if(selectOptionTreeIndex == 1374)
	{
		if(isDiCloseRelationPointBShow == 0)
			switchDiCloseRelationPointBShow();
	}
	else if(selectOptionTreeIndex == 1375)
	{
		if(isDiCloseRelationPlaneOShow == 0)
			switchDiCloseRelationPlaneOShow();
	}
	else if(selectOptionTreeIndex == 5)
	{
		if(isFitPlanePointShow == 0)
			switchFitPlanePointShow();
	}
	/*
	else if(selectOptionTreeIndex == 118)
	{
		if(isDiPointA0Show == 0)
			switchDiPointA0Show();
	}
	else if(selectOptionTreeIndex == 119)
	{
		if(isDiPlaneOShow == 0)
			switchDiPlaneOShow();
	}
	else if(selectOptionTreeIndex == 120)
	{
		if(isDiPlanePShow == 0)
			switchDiPlanePShow();
	}
	else if(selectOptionTreeIndex == 116)
	{
		if(isDiCoverIntersectLineShow == 0)
			switchDiCoverIntersectLineShow();
	}
	*/
	else if(selectOptionTreeIndex == 69)
	{
		if(isDiCoverPointShow == 0)
			switchDiCoverPointShow();
	}
	else if(selectOptionTreeIndex == 70)
	{
		if(isDiCoverPlaneOShow == 0)
			switchDiCoverPlaneOShow();
	}
	else if(selectOptionTreeIndex == 71)
	{
		if(isDiCoverIntersectCurveShow == 0)
			switchDiCoverIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 75)
	{
		if(isDiCoverIntersectCurvePointShow == 0)
			switchDiCoverIntersectCurvePointShow();
	}
	else if(selectOptionTreeIndex == 119)
	{
		if(isDiCoverPointShow == 0)
			switchDiCoverPointShow();
	}
	else if(selectOptionTreeIndex == 120)
	{
		if(isDiCoverPlaneOShow == 0)
			switchDiCoverPlaneOShow();
	}
	else if(selectOptionTreeIndex == 121)
	{
		if(isDiCoverIntersectCurveShow == 0)
			switchDiCoverIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 125)
	{
		if(isDiCoverIntersectCurvePointShow == 0)
			switchDiCoverIntersectCurvePointShow();
	}

	else if(selectOptionTreeIndex == 129)
	{
		if(isDiOverbitePointShow == 0)
			switchDiOverbitePointShow();
	}
	else if(selectOptionTreeIndex == 130)
	{
		if(isDiOverbitePlaneOShow == 0)
			switchDiOverbitePlaneOShow();
	}
	else if(selectOptionTreeIndex == 131)
	{
		if(isDiOverbiteIntersectCurveShow == 0)
			switchDiOverbiteIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 135)
	{
		if(isDiOverbiteIntersectCurvePointShow == 0)
			switchDiOverbiteIntersectCurvePointShow();
	}
	else if(selectOptionTreeIndex / 100 == 141)
	{
		int grindToothIndex = selectOptionTreeIndex - 14100;
		if(isDiSideOpenClosePointShow[grindToothIndex] == 0)
			switchDiSideOpenClosePointShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex / 100 == 142)
	{
		int grindToothIndex = selectOptionTreeIndex - 14200;
		if(isDiSideOpenClosePlaneOShow[grindToothIndex] == 0)
			switchDiSideOpenClosePlaneOShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex / 100 == 143)
	{
		int grindToothIndex = selectOptionTreeIndex - 14300;
		if(isDiSideOpenCloseIntersectCurveShow[grindToothIndex] == 0)
			switchDiSideOpenCloseIntersectCurveShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex / 100 == 147)
	{
		int grindToothIndex = selectOptionTreeIndex - 14700;
		if(isDiSideOpenCloseIntersectCurvePointShow[grindToothIndex] == 0)
			switchDiSideOpenCloseIntersectCurvePointShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex == 401)
	{
		for(int i = 0; i < 20; ++i)
		{
			if(isDiFrontOpenClosePointShow[i] == 0)
				switchDiFrontOpenClosePointShow(i);
		}
	}
	else if(selectOptionTreeIndex > 420 && selectOptionTreeIndex < 820)
	{
		if((( selectOptionTreeIndex - 421) % 20) == 0)
		{
			int measureIndex = ( selectOptionTreeIndex - 421) / 20;
			if(isDiFrontOpenClosePointShow[measureIndex] == 0)
				switchDiFrontOpenClosePointShow(measureIndex);
		}
	}
	else if(selectOptionTreeIndex == 901)
	{
		if(isDiBackToothOppositeClosePointShow == 0)
			switchDiBackToothOppositeClosePointShow();
	}
	else if(selectOptionTreeIndex == 902)
	{
		if(isDiBackToothOppositeCloseProjectPointShow == 0)
			switchDiBackToothOppositeCloseProjectPointShow();
	}
	else if(selectOptionTreeIndex == 905)
	{
		if(isDiBackToothLockClosePointShow == 0)
			switchDiBackToothLockClosePointShow();
	}
	else if(selectOptionTreeIndex == 906)
	{
		if(isDiBackToothLockCloseProjectPointShow == 0)
			switchDiBackToothLockCloseProjectPointShow();
	}
	else if(selectOptionTreeIndex == 1002)
	{
		if(diMiddleUnfitPointShow == 0)
			switchDiMiddleUnfitPointShow();
	}
	else if(selectOptionTreeIndex == 1006)
	{
		if(isDiMiddleCutToothGapShow == 0)
			switchDiMiddleCutToothGapShow();
	}
	else if(selectOptionTreeIndex == 8)
	{
		if(isGapPointShow == 0)
			switchGapPointShow();
	}	
	else if(selectOptionTreeIndex == 23)
	{
		//QMessageBox::information(NULL, tr("Action"), QString(tr("Show vertical plane...")).append(QString::number(isVerticalPlaneShow)));
		if(isVerticalPlaneShow == 0)
		{
			switchVerticalPlaneShow();
		}
	}
	else if(selectOptionTreeIndex == 26)
	{
		if(isPointInVerticalPlaneShow == 0)
			switchPointInVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 28)
	{
		if(isTopToothPointForVerticalPlaneShow == 0)
			switchTopToothPointForVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 30)
	{
		if(isTopToothProjectPointForVerticalPlane == 0)
			switchTopToothProjectPointForVerticalPlane();
	}
	else if(selectOptionTreeIndex == 29)
	{
		if(isTopToothPointForFitPlaneShow == 0)
			switchTopToothPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 31)
	{
		if(isTopToothProjectPointForFitPlaneShow == 0)
			switchTopToothProjectPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 24)
	{
		if(isLowToothPointForVerticalPlaneShow == 0)
			switchLowToothPointForVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 32)
	{
		if(isLowToothProjectPointForVerticalPlaneShow == 0)
			switchLowToothProjectPointForVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 25)
	{
		if(isLowToothPointForFitPlaneShow == 0)
			switchLowToothPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 33)
	{
		if(isLowToothProjectPointForFitPlaneShow == 0)
			switchLowToothProjectPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 35)
	{
		// 显示(中线->矢状平行面1)
		if(isVerticalParallelPlaneShow == 0)
			switchVerticalParallelPlaneShow();
	}
	else if(selectOptionTreeIndex == 97)
	{
		// 显示(中线->矢状平行面2)
		if(isVerticalParallelPlane2Show == 0)
			switchVerticalParallelPlane2Show();
	}
	else if(selectOptionTreeIndex == 36)
	{
		// 显示(中线->矢状平行面1->初始点)
		if(isVerticalParallelPlaneInitPointShow == 0)
			switchVerticalParallelPlaneInitPointShow();
	}
	else if(selectOptionTreeIndex == 99)
	{
		// 显示(中线->矢状平行面2->初始点)
		if(isVerticalParallelPlaneInitPoint2Show == 0)
			switchVerticalParallelPlaneInitPoint2Show();
	}
	else if(selectOptionTreeIndex == 39)
	{
		if(isTwoPointToProjectFitPlaneShow == 0)
			switchTwoPointToProjectFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 44)
	{
		if(isOverbiteIntersectCurveShow == 0)
			switchOverbiteIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 45)
	{
		if(isOverbiteParrelPlaneShow == 0)
		{
			switchOverbiteParrelPlaneShow();
		}
	}
	else if(selectOptionTreeIndex == 46)
	{
		if(isPointInOverbiteParallelPlaneShow == 0)
			switchPointInOverbiteParallelPlaneShow();
	}
	else if(selectOptionTreeIndex == 63)
	{
		if(isBackToothOpenCloseFittingCurveShow == 0)
			switchBackToothOpenCloseFittingCurveShow();
	}
	else if(selectOptionTreeIndex == 64)
	{
		if(isBackToothOpenCloseFittingPointShow == 0)
			switchBackToothOpenCloseFittingPointShow();
	}
	else if(selectOptionTreeIndex == 66)
	{
		if(isFittingCurveShow == 0)
			switchFittingCurveShow();
	}
	else if(selectOptionTreeIndex == 67)
	{
		if(isSplinePointShow == 0)
			switchSplinePointShow();
	}
	else if(selectOptionTreeIndex == 78)
	{
		if(isBackToothPlaneOShow == 0)
			switchBackToothPlaneOShow();
	}
	else if(selectOptionTreeIndex == 79)
	{
		if(isBackToothPointA0Show == 0)
			switchBackToothPointA0Show();
	}
	else if(selectOptionTreeIndex == 81)
	{
		if(isBackToothContourCurveShaow == 0)
			switchBackToothContourCurveShaow();
	}
	else if(selectOptionTreeIndex == 82)
	{
		if(isBackToothContiourCurvePointShow == 0)
			switchBackToothContiourCurvePointShow();
	}
	else if(selectOptionTreeIndex == 86)
	{
		if(isPointForArticlePlaneShow == 0)
			switchPointForArticlePlaneShow();
	}
	else if(selectOptionTreeIndex == 85)
	{
		if(isArticlePlaneShow == 0)
			switchArticlePlaneShow();
	}
	else if(selectOptionTreeIndex == 91)
	{
		if(isFrontToothOpenCloseFittingPointShow == 0)
			switchFrontToothOpenCloseFittingPointShow();
	}
	else if(selectOptionTreeIndex == 89)
	{
		if(isFrontToothOpenCloseFittingCurveShow == 0)
			switchFrontToothOpenCloseFittingCurveShow();
	}
	else if(selectOptionTreeIndex == 90)
	{
		if(isFrontToothPlaneOShow == 0)
			switchFrontToothPlaneOShow();
	}
	else if(selectOptionTreeIndex == 94)
	{
		if(isFrontToothContourCurveShaow == 0)
			switchFrontToothContourCurveShaow();
	}
	else if(selectOptionTreeIndex == 95)
	{
		if(isFrontToothContiourCurvePointShow == 0)
			switchFrontToothContiourCurvePointShow();
	}
	else if(selectOptionTreeIndex == 49)
	{
		if(isInverseFitCoverPointShow == 0 && isInverseFitCoverProjectShow == 0)
			switchInverseFitCoverShow();
	}
	else if(selectOptionTreeIndex == 14)
	{
		if(isGapProjectPointShow == 0)
			switchGapProjectPointShow();
	}
	else if(selectOptionTreeIndex == 88)
	{
		if(isProjectArticlePlane == 0)
			switchProjectArticlePlane();
	}
	else if(selectOptionTreeIndex == 92)
	{
		if(idFrontToothToProjectPointPositionA0Show == 0)
			switchFrontToothToProjectPointPositionA0Show();
	}
	else if((selectOptionTreeIndex - 1510) >= 0 && (selectOptionTreeIndex - 1510) < 500 && (selectOptionTreeIndex - 1510) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1510) / 25;

		for(int i = 0; i < aboArrangePointPickNum[pointPareIndex]; ++i)
		{
			if(isAboArrangePointShow[pointPareIndex][i] == 0)
				switchAboArrangePointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 1513) >= 0 && (selectOptionTreeIndex - 1513) < 500 && (selectOptionTreeIndex - 1513) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1513) / 25;

		if(isAboArrangePlaneOShow[pointPareIndex] == 0)
			switchAboArrangePlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 1516) >= 0 && (selectOptionTreeIndex - 1516) < 500 && (selectOptionTreeIndex - 1516) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1516) / 25;

		for(int i = 0; i < aboArrangePointPickNum[pointPareIndex]; ++i)
		{
			if(isAboArrangeProjectPointShow[pointPareIndex][i] == 0)
				switchAboArrangeProjectPointShow(pointPareIndex);
		}
	}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select show things...")));
	}


	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::pickPoint()
{
	//QMessageBox::information(NULL, tr("Action"), QString::number(selectOptionTreeIndex));

	if(selectOptionTreeIndex == 4)
	{
		// 合平面选点

		setOperatePreCondition(4, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if (selectOptionTreeIndex == 1111)
	{
		setOperatePreCondition(selectOptionTreeIndex, 3);
		this->pickStyle->mode = 3;
	}
	else if((selectOptionTreeIndex - 2510) >= 0 && (selectOptionTreeIndex - 2510) < 500 && (selectOptionTreeIndex - 2510) % 25 == 0)
	{
		//
		// ABO-OGS颊舌向倾斜度
		//		选点
		//

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if((selectOptionTreeIndex - 4510) >= 0 && (selectOptionTreeIndex - 4510) < 650 && (selectOptionTreeIndex - 4510) % 25 == 0)
	{
		//
		// ABO-OGS邻牙接触关系
		//		选点
		//

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if((selectOptionTreeIndex - 4018) >= 0 && (selectOptionTreeIndex - 4018) < 500 && (selectOptionTreeIndex - 4018) % 25 == 0)
	{
		//
		// ABO-OGS覆盖
		//		上牙选点
		//

		setOperatePreCondition(selectOptionTreeIndex, 2);
		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if((selectOptionTreeIndex - 4011) >= 0 && (selectOptionTreeIndex - 4011) < 500 && (selectOptionTreeIndex - 4011) % 25 == 0)
	{
		//
		// ABO-OGS覆盖
		//		下牙选点
		//

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if((selectOptionTreeIndex - 3510) >= 0 && (selectOptionTreeIndex - 3510) < 500 && (selectOptionTreeIndex - 3510) % 25 == 0)
	{
		//
		// ABO-OGS牙合接触
		//		选点
		//

		//QMessageBox::information(NULL, tr("Info"), tr("ABO Tooth Close Relation Pick Point..."));

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if((selectOptionTreeIndex - 3010) >= 0 && (selectOptionTreeIndex - 3010) < 500 && (selectOptionTreeIndex - 3010) % 25 == 0)
	{
		//
		// ABO-OGS牙合关系
		//		选点
		//

		//QMessageBox::information(NULL, tr("Info"), tr("ABO Tooth Close Relation Pick Point..."));

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if((selectOptionTreeIndex - 2010) >= 0 && (selectOptionTreeIndex - 2010) < 500 && (selectOptionTreeIndex - 2010) % 25 == 0)
	{
		//
		// ABO-OGS边缘嵴高度
		//		选点
		//

		//QMessageBox::information(NULL, tr("Info"), tr("ABO Edge Vertebral Heigh Pick Point..."));

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	/*else if((selectOptionTreeIndex - 1510) >= 0 && (selectOptionTreeIndex - 1510) < 500 && (selectOptionTreeIndex - 1510) % 25 == 0)*/
	else if(selectOptionTreeIndex / 100 == 1510)
	{
		//
		// ABO-OGS排列
		//		选点
		//

		//QMessageBox::information(NULL, tr("Info"), tr("ABO Arrange Pick Point..."));

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 6900)
	{
		//QMessageBox::information(NULL, tr("Info"), tr("DI Cover Pick Point..."));

		// 颊侧区咬合补充测量选点
		/*
		setOperatePreCondition(118, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
		*/
		setOperatePreCondition(6900, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 69)
	{
		//QMessageBox::information(NULL, tr("Info"), tr("DI Cover Pick Point..."));

		// 覆盖选点
		/*
		setOperatePreCondition(118, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
		*/
		setOperatePreCondition(69, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 119)
	{
		//QMessageBox::information(NULL, tr("Info"), tr("DI Cover Pick Point..."));

		// DI覆盖选点
		/*
		setOperatePreCondition(118, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
		*/
		setOperatePreCondition(119, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 129)
	{
		// DI覆合选点

		setOperatePreCondition(129, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 1373)
	{
		// 选点(DI合关系->测量->点A)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 1374)
	{
		// 选点(DI合关系->测量->点B)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}else if(selectOptionTreeIndex == 1383)
	{
		// 选点(DI合关系->测量->点A)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 1384)
	{
		// 选点(DI合关系->测量->点B)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 7500)
	{
		// 选点(覆盖->轮廓曲线->点)

		setOperatePreCondition(7500, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 75)
	{
		// 选点(覆盖->轮廓曲线->点)

		setOperatePreCondition(75, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 125)
	{
		// 选点(DI覆合->轮廓曲线->点)

		setOperatePreCondition(125, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 135)
	{
		// 选点(DI覆合->轮廓曲线->点)

		setOperatePreCondition(135, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else //if(selectOptionTreeIndex == 141 || selectOptionTreeIndex == 161 || selectOptionTreeIndex == 181)
		if(selectOptionTreeIndex / 100 == 141)
	{
		// 选点(DI侧方开合->磨牙1/2/3->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else //if(selectOptionTreeIndex == 147 || selectOptionTreeIndex == 167 || selectOptionTreeIndex == 187)
		if(selectOptionTreeIndex / 100 == 147)
	{
		// 选点(DI侧方开合->磨牙1/2/3->轮廓曲线->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 401)
	{
		// 选点(DI前牙开合->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex < 820 && selectOptionTreeIndex > 420)
	{
		if(((selectOptionTreeIndex - 427) % 20) == 0)
		{
			// 选点(DI前牙开合->测量1/.../20->轮廓曲线->点)

			setOperatePreCondition(selectOptionTreeIndex, 2);

			this->pickStyle->Data = lowerToothData;
			this->pickStyle->mode = 1;
		}
		else if(((selectOptionTreeIndex - 421) % 20) == 0)
		{
			// 选点(DI前牙开合->测量1/.../20->点)

			setOperatePreCondition(selectOptionTreeIndex, 2);

			this->pickStyle->Data = lowerToothData;
			this->pickStyle->mode = 1;
		}
	}
	else if(selectOptionTreeIndex / 100 < 1270 && selectOptionTreeIndex / 100 > 1030)
	{
		if((selectOptionTreeIndex - 1031 * 100) < 100)
		{
			// 选点(DI拥挤度->磨牙->测量1/../12->点)

			setOperatePreCondition(selectOptionTreeIndex, 2);

			this->pickStyle->Data = lowerToothData;
			this->pickStyle->mode = 1;
		}
	}
	else if(selectOptionTreeIndex > 1270 && selectOptionTreeIndex < 1350)
	{
		if(((selectOptionTreeIndex - 1271) % 20) == 0)
		{
			// 选点(DI拥挤度->牙弓->测量1/../4->点)

			setOperatePreCondition(selectOptionTreeIndex, 2);

			this->pickStyle->Data = lowerToothData;
			this->pickStyle->mode = 1;
		}
	}
	else if(selectOptionTreeIndex == 901)
	{
		// 选点(DI后牙反合->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 905)
	{
		// 选点(DI后牙正锁合->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 1002)
	{
		// 选点(DI其他->中线不调->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 1006)
	{
		// 选点(DI其他->中切牙牙间隙->点)

		setOperatePreCondition(selectOptionTreeIndex, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex / 1000 == 9 && selectOptionTreeIndex % 1000 < 10)
	{
		// 缝隙选点(前牙区不齐->接触点->缝隙1/2/3/4/5)

		for(int i = 0; i < 10; ++i)
		{
			setOperatePreCondition(9000 + i, 2);
		}

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 23)
	{
		// 选点(颊侧区咬合->下牙列->正中矢状面), 初始点

		setOperatePreCondition(23, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 28)
	{
		// 选点(颊侧区咬合->上牙列->点(矢状向))

		setOperatePreCondition(28, 2);

		this->pickStyle->Data = topToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 29)
	{
		turnoffAllResult();
		if(isTopToothShow == 0)
		{
			switchTopToothShow();
		}
		if(isLowToothShow == 1)
		{
			switchLowToothShow();
		}

		if(isFitPlanePointShow == 1)
		{
			switchFitPlanePointShow();
		}
		if(isFitPlaneShow == 1)
		{
			switchFitPlaneShow();
		}
		if(isGapPointShow == 1)
		{
			switchGapPointShow();
		}
		if(isGapProjectPointShow == 1)
		{
			switchGapProjectPointShow();
		}

		if(isVerticalPlaneShow == 1)
		{
			switchVerticalPlaneShow();
		}
		if(isPointInVerticalPlaneShow == 1)
		{
			switchPointInVerticalPlaneShow();
		}
		if(isTopToothPointForVerticalPlaneShow == 1)
		{
			switchTopToothPointForVerticalPlaneShow();
		}
		if(isTopToothProjectPointForVerticalPlane == 1)
		{
			switchTopToothProjectPointForVerticalPlane();
		}

		this->pickStyle->Data = topToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 24)
	{
		// 下牙列矢状向选点

		setOperatePreCondition(24, 2);

		if(!currentOptionTreeWidgetItem->isExpanded())
			currentOptionTreeWidgetItem->setExpanded(true);		
		
		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 25)
	{
		// 下牙列水平选点

		setOperatePreCondition(25, 2);

		if(!currentOptionTreeWidgetItem->isExpanded())
			currentOptionTreeWidgetItem->setExpanded(true);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 35)
	{
		// 中线矢状平行面1初始点

		setOperatePreCondition(35, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 97)
	{
		// 中线矢状平行面1初始点

		setOperatePreCondition(97, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 39)
	{
		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		if(isVerticalParallelPlaneInitPointShow == 1)
			switchVerticalParallelPlaneInitPointShow();

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 45)
	{
		turnoffAllResult();
		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		//if(isFitPlaneShow == 0)
			//switchFitPlaneShow();


		if(!currentOptionTreeWidgetItem->isExpanded())
			currentOptionTreeWidgetItem->setExpanded(true);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 51 || selectOptionTreeIndex == 52 || selectOptionTreeIndex == 53 || selectOptionTreeIndex == 54 ||
		selectOptionTreeIndex == 56 || selectOptionTreeIndex == 57 || selectOptionTreeIndex == 58 || selectOptionTreeIndex == 59)
	{
		QTreeWidgetItem *parent = currentOptionTreeWidgetItem->parent();
		
		if(getSelectOptionTreeIndex(parent->data(0, Qt::UserRole)) == 50)
			topLowIndex = 0;
		else if(getSelectOptionTreeIndex(parent->data(0, Qt::UserRole)) == 60)
			topLowIndex = 1;
		if (selectOptionTreeIndex == 51)
		{
			turnoffAllResult();
		}
		//turnoffAllResult();
		//turnoffFitPlaneResult();
		if(topLowIndex == 0)
		{
			if(isTopToothShow == 0)
				switchTopToothShow();
		}
		else if(topLowIndex == 1)
		{
			if(isLowToothShow == 0)
				switchLowToothShow();
		}

		pairIndex = currentOptionTreeWidgetItem->text(0).right(1).toInt() - 1;
		//QMessageBox::information(NULL, tr("Info"), QString::number(topLowIndex).append(", ").append(QString::number(pairIndex)));

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 64)
	{
		turnoffAllResult();
		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 6600)
	{
		// 选点(覆盖->拟合曲线->点)

		setOperatePreCondition(6600, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 66)
	{
		// 选点(覆盖->拟合曲线->点)

		setOperatePreCondition(66, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	/*
	else if(selectOptionTreeIndex == 72)
	{
		// 选点(覆盖->平面->牙弓放射状相交平面->点) 

		setOperatePreCondition(72, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	
	else if(selectOptionTreeIndex == 76)
	{
		setOperatePreCondition(76, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	*/
	else if(selectOptionTreeIndex == 79)
	{
		setOperatePreCondition(79, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 82)
	{
		setOperatePreCondition(82, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 85)
	{
		// 选点(冠状面)
		setOperatePreCondition(85, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 92)
	{
		// 选点(开合->前牙开合->牙弓放射状相交平面->点)
		setOperatePreCondition(92, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 91)
	{
		// 选点(开合->前牙开合->拟合曲线->点)
		setOperatePreCondition(91, 2);

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 95)
	{
		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 103 || selectOptionTreeIndex == 104 ||
		selectOptionTreeIndex == 105 || selectOptionTreeIndex == 106)
	{
		// 选点(开合->前牙开合->冠状面->点->上牙列->点对1/2/3/4)

		for(int i = 0; i < 4; ++i)
		{
			setOperatePreCondition(103 + i, 2);
		}

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	else if(selectOptionTreeIndex == 107 || selectOptionTreeIndex == 108 ||
		selectOptionTreeIndex == 109 || selectOptionTreeIndex == 110)
	{
		// 选点(开合->前牙开合->冠状面->点->下牙列->点对1/2/3/4)

		for(int i = 0; i < 4; ++i)
		{
			setOperatePreCondition(107 + i, 2);
		}

		this->pickStyle->Data = lowerToothData;
		this->pickStyle->mode = 1;
	}
	//else if(selectOptionTreeIndex == 88)
	//{
	//	// 投影(开合->前牙开合->冠状面->点)

	//	setOperatePreCondition(88, 6);

	//	this->pickStyle->mode = 0;
	//}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
		return;
	}

	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 1;
	It.OpeNum = returnselectOptionTreeIndex();
// 	It.x = currentSelectPosition[0];
// 	It.y = currentSelectPosition[1];
// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
}

void MainWindow::distancePoint()
{
	if(98 == selectOptionTreeIndex)
	{
		// 距离(中线->距离)

		if(verticalParallelPlaneStat[0] && verticalParallelPlaneStat[1])
		{
			double projectPoint[3];
			vtkPlane::ProjectPoint(verticalParallelPlaneInitPointPosition,
				verticalParallelPlaneInitPoint2Position, verticalPlaneNormal,
				projectPoint);

			distanceBetweenTwoVerticalPlane = sqrt(vtkMath::Distance2BetweenPoints(projectPoint, verticalParallelPlaneInitPointPosition));
			this->removeChildItem(currentOptionTreeWidgetItem);
			QString text = QString::number(distanceBetweenTwoVerticalPlane);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
			currentOptionTreeWidgetItem->addChild(distanceInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}
		}
	}else if(selectOptionTreeIndex == 1024)
	{
		if (diGrindToothScrowdSumDistance != 0 && diToothBowScrowdSumDistance != 0)
		{
			double dist = diGrindToothScrowdSumDistance - diToothBowScrowdSumDistance;
			this->removeChildItem(currentOptionTreeWidgetItem);
			QString text = QString::number(dist);
			QTreeWidgetItem *CrowddistanceInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
			currentOptionTreeWidgetItem->addChild(CrowddistanceInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}
		}
	}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
		return;
	}
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 4;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
}

void MainWindow::StartMeasure()
{
	int OpeIndex = selectOptionTreeIndex;
	cout<<"StartMeasure"<<endl;
	cout<<OpeIndex<<endl;
	turnoffAllResult();
	if (OpeIndex == 118)//覆盖
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("请手动选点")));
		currentOptionTreeWidgetItem = getChildItem(currentOptionTreeWidgetItem, 119);
		selectOptionTreeIndex = 119;
		pickPoint();
	}else if (OpeIndex == 128)//覆合
	{
		QMessageBox message(QMessageBox::Warning,"判断","是否开合?",QMessageBox::Yes|QMessageBox::No,NULL);  
		if (message.exec()==QMessageBox::Yes)  
		{  
			diOverbiteScore = 0;
			diOverbiteFlag = 1;
			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem, 132);
			removeChildItem(scoreItem);
			QString itemText = QString::number(diOverbiteScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}
			ScoreSet[0][1] = diOverbiteScore;
			return;
		}else
		{
			diOverbiteFlag = 0;
			currentOptionTreeWidgetItem = getChildItem(currentOptionTreeWidgetItem, 129);
			selectOptionTreeIndex = 129;
			pickPoint();
		}
	}else if (OpeIndex == 400)//前牙开合
	{
		if (diOverbiteFlag == 0)
		{
			QMessageBox::information(NULL, tr("Info"), QString(tr("已进行DI覆合测量")));
			return;
		}
		bool ok;
		QString name = QInputDialog::getText(NULL,tr("前牙开合"),tr("13-23是否有未完全萌出或牙弓之外的牙，如存在请输入该牙序号，以空格分割:"),  
			QLineEdit::Normal,  
			"",  
			&ok);  
		QString tName[6] = {"13","12","11","21","22","23"}; 
		for (int i = 0;i < 6;i++)
		{
			diFrontOpenCloseFlag[i] = 0;
		}
		QStringList sec;
		if(ok) {  
			sec = name.split(" ");
		}

		QList<QString>::Iterator it = sec.begin(),itend = sec.end();
		for (;it != itend; it++){
			for (int i = 0;i < 6;i ++)
			{
				if (*it == tName[i])
				{
					diFrontOpenCloseFlag[i] = 1;
				}
			}
		}
		//进行自动测量
		loadData("前牙开合");
		int sumScore = 0;
		for (int i = 0;i < 6;i ++)
		{
			if (diFrontOpenCloseFlag[i] == 0)
			{
				sumScore += diFrontOpenCloseMeasureScore[i];
			}
		}
		QString itemText = QString::number(sumScore);
		QTreeWidgetItem *scoreItem = getOptionTreeByData(402);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
		scoreItem->addChild(scoreInfoItem);
		ScoreSet[0][2] = sumScore;
	}else if (OpeIndex == 137)//侧方开合
	{
		bool ok;
		QString name = QInputDialog::getText(NULL,tr("侧方开合"),tr("17-14、24-27是否有未完全萌出或牙弓之外的牙，如存在请输入该牙序号，以空格分割:"),  
			QLineEdit::Normal,  
			"",  
			&ok);  
		QString tName[8] = {"17","16","15","14","24","25","26","27"}; 
		for (int i = 0;i < 8;i++)
		{
			diSideOpenClosFlag[i] = 0;
		}
		QStringList sec;
		if(ok) {  
			sec = name.split(" ");
		}

		QList<QString>::Iterator it = sec.begin(),itend = sec.end();
		for (;it != itend; it++){
			for (int i = 0;i < 8;i ++)
			{
				if (*it == tName[i])
				{
					diSideOpenClosFlag[i] = 1;
				}
			}
		}
		//进行自动测量
		loadData("侧方开合");

		int sumScore = 0;
		sumScore = diSideOpenCloseScore;
		QString itemText = QString::number(sumScore);
		QTreeWidgetItem *scoreItem = getOptionTreeByData(200);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
		scoreItem->addChild(scoreInfoItem);
		ScoreSet[0][3] = sumScore;
	}else if (OpeIndex == 1020)//拥挤度
	{
		loadData("拥挤度");
		selectOptionTreeIndex = 1023;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		score();
		ScoreSet[0][4] = diScrowdScore;

	}else if (OpeIndex == 1371)//合关系
	{
		diCloseRelationScore = 0;
		DiCloseRelationType = 0;
		currentOptionTreeWidgetItem = getOptionTreeByData(1371);
		selectOptionTreeIndex = 1371;
		score();
		if (DiCloseRelationType == 1)//左
		{
			cout<<"合关系11"<<endl;
			loadData("合关系11");//d
		}
		else if (DiCloseRelationType == 2)
		{
			cout<<"合关系12"<<endl;
			loadData("合关系12");//e
		}
	}else if (OpeIndex == 1381)
	{
		DiCloseRelationType = 0;
		currentOptionTreeWidgetItem = getOptionTreeByData(1381);
		selectOptionTreeIndex = 1381;
		score();
		if (DiCloseRelationType == 1)//右
		{
			loadData("合关系21");//d
		}
		else if (DiCloseRelationType == 2)
		{
			loadData("合关系22");//e
		}
		ScoreSet[0][5] = diCloseRelationScore;
	}
	else if (OpeIndex == 900)
	{
		loadData("后牙反合");
		selectOptionTreeIndex = 903;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		score();
		//ScoreSet[0][6] = diBackToothOppositeCloseScore;
	}
	else if (OpeIndex == 904)
	{
		loadData("后牙锁合");
		selectOptionTreeIndex = 907;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		score();
		//ScoreSet[0][7] = diBakcToothLockCloseScore;
	}
	else if (OpeIndex == 1501)//ABO排列
	{
		for (int i = 0;i < 28;i ++)
		{
			aboArrangeScoreT[i] = 0;
		}
		loadData("abo排列");
		ScoreSet[1][0] = aboArrangeScore;
	}else if (OpeIndex == 2000)//ABO边缘嵴高度
	{
		for (int i = 0;i < 28;i ++)
		{
			aboEdgeVertebralScoreT[i] = 0;
		}
		loadData("abo边缘嵴");
		ScoreSet[1][1] = aboEdgeVertebralHeighScore;
	}else if (OpeIndex == 2500)//颊舌向倾斜度
	{
// 		for (int i = 0;i < 28;i ++)
// 		{
// 			aboChapScoreT[i] = 0;
// 		}
		loadData("abo颊舌向");
		ScoreSet[1][2] = aboChapScore;
	}else if (OpeIndex == 3000)//牙合关系
	{
		QMessageBox customMsgBox;  
		customMsgBox.setWindowTitle(tr("请选择"));  
		QPushButton *Button1 = customMsgBox.addButton(tr("安氏Ⅰ类"),QMessageBox::ActionRole);  
		QPushButton *Button2 = customMsgBox.addButton(tr("安氏Ⅱ类"),QMessageBox::ActionRole);  
		QPushButton *Button3 = customMsgBox.addButton(tr("安氏Ⅲ类"),QMessageBox::ActionRole);  
		customMsgBox.setText(tr("选择牙型种类"));  
		customMsgBox.exec();  

		if(customMsgBox.clickedButton() == Button1)  
		{
			loadData("牙合关系1");
		} 
		if(customMsgBox.clickedButton() == Button2)  
		{
			loadData("牙合关系2");
		}
		if(customMsgBox.clickedButton() == Button3)
		{
			loadData("牙合关系3");
		}
		ScoreSet[1][3] = aboToothCloseRelationScore;
	}else if (OpeIndex == 3500)//牙合接触
	{
		loadData("牙合接触");
		ScoreSet[1][4] = aboToothCloseTouchScore;
	}else if (OpeIndex == 4000)//ABO覆盖
	{
		loadData("abo覆盖");
		ScoreSet[1][5] = aboCoverScore;
	}else if (OpeIndex == 4500)//邻牙接触关系
	{
		for (int i = 0;i < 28;i ++)
		{
			aboTouchRelationScoreT[i] = 0;
		}
		loadData("abo邻牙接触");
		ScoreSet[1][6] = aboTouchRelationScore;
	}else if (OpeIndex == 3)//前牙区不齐
	{
		QMessageBox customMsgBox;  
		customMsgBox.setWindowTitle(tr("请选择"));  
		QPushButton *Button1 = customMsgBox.addButton(tr("有"),QMessageBox::ActionRole);  
		QPushButton *Button2 = customMsgBox.addButton(tr("无"),QMessageBox::ActionRole);  
		customMsgBox.setText(tr("有无阻生牙"));  
		customMsgBox.exec();  

		if(customMsgBox.clickedButton() == Button1)  
		{
			selectOptionTreeIndex = 15;
			currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
			setScore(5);
		} 
		if(customMsgBox.clickedButton() == Button2)  
		{
			loadData("前牙区不齐");
			selectOptionTreeIndex = 15;
			currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
			score();
		}
		ScoreSet[2][0] = frontToothScore;
	}else if (OpeIndex == 19)//矢状向
	{
		double scoreT = 0;
		QMessageBox customMsgBox;  
		customMsgBox.setWindowTitle(tr("请选择"));  
		QPushButton *Button1 = customMsgBox.addButton(tr("很好的安氏Ⅰ，Ⅱ，Ⅲ类嵌合"),QMessageBox::ActionRole);  
		QPushButton *Button2 = customMsgBox.addButton(tr("少于半个牙尖"),QMessageBox::ActionRole);  
		QPushButton *Button3 = customMsgBox.addButton(tr("多于半个牙尖"),QMessageBox::ActionRole);  
		customMsgBox.setText(tr("选择牙型种类"));  
		customMsgBox.exec();  

		if(customMsgBox.clickedButton() == Button1)  
		{
			scoreT = 0;
		} 
		if(customMsgBox.clickedButton() == Button2)  
		{
			scoreT = 1;
		}
		if(customMsgBox.clickedButton() == Button3)
		{
			scoreT = 2;
		}
		selectOptionTreeIndex = 19;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setScore(scoreT);
		ScoreSet[2][1] = faceSideSagittalScore;
	}else if (OpeIndex == 20)//垂直向
	{
		bool ok;
		QString name = QInputDialog::getText(NULL,tr("颊侧区咬合垂直向"),tr("27-24、17-14是否有可疑牙位，如存在请输入该牙序号，以空格分割:"),  
			QLineEdit::Normal,  
			"",  
			&ok);  
		if (name.isEmpty())
		{
			setScore(0);
			return;
		}
		QString tName[8] = {"27","26","25","24","17","16","15","14"}; 
		for (int i = 0;i < 8;i++)
		{
			JVerticalFlag[i] = 0;
		}
		QStringList sec;
		if(ok) {  
			sec = name.split(" ");
		}

		QList<QString>::Iterator it = sec.begin(),itend = sec.end();
		for (;it != itend; it++){
			for (int i = 0;i < 8;i ++)
			{
				if (*it == tName[i])
				{
					JVerticalFlag[i] = 1;
				}
			}
		}
		int pName[8] = {91,79 ,73 ,66 ,2 ,9 ,20 ,27};
		for (int i = 0;i < 8 ;i ++)
		{
			if (JVerticalFlag[i] == 1)
			{
				remove("p-垂直向");
				ofstream fout;
				fout.open("p-垂直向");
				fout << pName[i]<<endl;
				fout.close();
				break;
			}
		}
		loadData("垂直向");
		ScoreSet[2][1] += faceSideVerticalScore;
	}else if (OpeIndex == 21)//水平向
	{
		loadData("水平向");
		selectOptionTreeIndex = 22;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		score();
		//ScoreSet[2][1] += faceSideHorizonalScore;
	}else if (OpeIndex == 49)//反合
	{
		selectOptionTreeIndex = 55;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		score();
		//ScoreSet[2][2] = inverseFitCoverScore;
	}else if (OpeIndex == 68)//覆盖
	{
		selectOptionTreeIndex = 69;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		pickPoint();
	}else if (OpeIndex == 44)//覆合
	{
		selectOptionTreeIndex = 45;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		pickPoint();
		selectOptionTreeIndex = 48;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		score();
	}else if (OpeIndex == 61)//开合
	{
		bool ok;
		QString name = QInputDialog::getText(NULL,tr("开合"),tr("12、11、21、22是否有可疑牙位，如存在请输入该牙序号，以空格分割:"),  
			QLineEdit::Normal,  
			"",  
			&ok);  
		if (name.isEmpty())
		{
			setScore(0);
			return;
		}
		QString tName[4] = {"12","11","21","22"}; 
		int tFlag[4] = {};
		QStringList sec;
		if(ok) {  
			sec = name.split(" ");
		}

		QList<QString>::Iterator it = sec.begin(),itend = sec.end();
		for (;it != itend; it++){
			for (int i = 0;i < 4;i ++)
			{
				if (*it == tName[i])
				{
					tFlag[i] = 1;
				}
			}
		}
		int pName[4] = {37 ,43 ,49 ,55};
		for (int i = 0;i < 4 ;i ++)
		{
			if (JVerticalFlag[i] == 1)
			{
				remove("p-开合");
				ofstream fout;
				fout.open("p-开合");
				fout << pName[i]<<endl;
				fout.close();
				break;
			}
		}
		loadData("开合");

	}else if (OpeIndex == 34)//中线
	{
		double pA[3],pB[3];
		for (int i = 0;i < 3;i ++)
		{
			pA[i] = (pointlist[46][i] + pointlist[52][i]) / 2;
			pB[i] = (pointlist[145][i] + pointlist[149][i]) / 2;
		}

		selectOptionTreeIndex = 35;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setPickPointPosition(pB[0],pB[1],pB[2]);

		selectOptionTreeIndex = 97;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setPickPointPosition(pA[0],pA[1],pA[2]);

		selectOptionTreeIndex = 98;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		distancePoint();

		double pC[3],pD[3],disB;
		for (int i = 0;i < 3;i ++)
		{
			pC[i] = pointlist[149][i];
			pD[i] = pointlist[150][i];
		}
		disB = sqrt(vtkMath::Distance2BetweenPoints(pC, pD));
		double scoreT = 0;
		if (distanceBetweenTwoVerticalPlane < disB / 4)
		{
			scoreT = 0;
		}else if (distanceBetweenTwoVerticalPlane <= disB / 2)
		{
			scoreT = 1;
		}else if (distanceBetweenTwoVerticalPlane > disB / 2)
		{
			scoreT = 2;
		}

		selectOptionTreeIndex = 42;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setScore(scoreT);
		ScoreSet[2][4] = scoreT;
	}
}

void MainWindow::updateAboSumScoreInfo(QTreeWidgetItem *item)
{
	aboScore = 0;
	aboScore = abs(aboArrangeScore) + abs(aboEdgeVertebralHeighScore) + abs(aboToothCloseRelationScore) + abs(aboToothCloseTouchScore) + abs(aboCoverScore) + abs(aboTouchRelationScore);//总分为分数绝对值相加

	removeChildItem(item);
	QString itemText = QString::number(aboScore);
	QTreeWidgetItem *infoItem = new QTreeWidgetItem(item, QStringList(itemText));
	item->addChild(infoItem);
	item->setExpanded(true);
}

void MainWindow::updateDiToothBowScrowdSumDistance(QTreeWidgetItem *item)
{
	bool canUpdate = true;
	for(int i = 0; i < 4; ++i)
	{
		if(diToothBowScrowdPointState[i] == false)
		{
			canUpdate = false;
			break;
		}
	}

	if(canUpdate)
	{
		diToothBowScrowdSumDistance = 0;
		for(int i = 0; i < 4; ++i)
		{
			diToothBowScrowdSumDistance += diToothBowScrowdDistance[i];
		}

		removeChildItem(item);

		QString itemText = QString::number(diToothBowScrowdSumDistance);
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(item, QStringList(itemText));
		item->addChild(pointInfoItem);
		if(!item->isExpanded())
		{
			item->setExpanded(true);
		}
		if (diGrindToothScrowdSumDistance != 0)
		{

		}
	}
}

void MainWindow::updateDiGrindToothScrowdSumDistance(QTreeWidgetItem *item)
{
	bool canUpdate = true;
	for(int i = 0; i < 10; ++i)
	{
		if(diGrindToothScrowdPointState[i] == false)
		{
			canUpdate = false;
			break;
		}
	}

	if(canUpdate)
	{
		diGrindToothScrowdSumDistance = 0;
		for(int i = 0; i < 10; ++i)
		{
			diGrindToothScrowdSumDistance += diGrindToothScrowdDistance[i];
		}

		removeChildItem(item);

		QString itemText = QString::number(diGrindToothScrowdSumDistance);
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(item, QStringList(itemText));
		item->addChild(pointInfoItem);
		if(!item->isExpanded())
		{
			item->setExpanded(true);
		}
	}
}

void MainWindow::updatePointInfoItem(QTreeWidgetItem *item, double (*points)[3], bool *pointsState, int pointNum)
{
	removeChildItem(item);

	for(int i = 0; i < pointNum; ++i)
	{
		if(pointsState[i])
		{
			QString text = QString("(").
				append(QString::number(points[i][0])).append(", ").
				append(QString::number(points[i][1])).append(", ").
				append(QString::number(points[i][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(item, QStringList(text));
			item->addChild(pointInfoItem);
		}
	}

	if(!item->isExpanded())
	{
		item->setExpanded(true);
	}
}

void MainWindow::projectPoint()
{
	if(8 == selectOptionTreeIndex)
	{
		// 投影(前牙区不齐->接触点)

		setOperatePreCondition(8, 6);


		QTreeWidgetItem *projectPointItem = getChildItem(currentOptionTreeWidgetItem, 14);
		this->removeChildItem(projectPointItem);
		//QMessageBox::information(NULL, tr("Info"), currentOptionTreeWidgetItem->text(0));

		for(int i = 0; i < 10; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(gapPointStat[i][j])
				{
					vtkPlane::ProjectPoint(gapPointPosition[i][j], fitPlaneCenter, fitPlaneNormal, gapProjecPointPosition[i][j]);

					double color[3] = {0, 1, 0};
					drawPoint(gapProjecPointPosition[i][j], color, gapProjectPointActor[i][j]);
					isGapProjectPointShow = 1;

					QString text = QString("(").
						append(QString::number(gapProjecPointPosition[i][j][0])).append(", ").
						append(QString::number(gapProjecPointPosition[i][j][1])).append(", ").
						append(QString::number(gapProjecPointPosition[i][j][2])).append(")");
					QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(projectPointItem, QStringList(text));
					projectPointItem->addChild(projectPointInfoItem);

					if(!projectPointItem->isExpanded())
					{
						projectPointItem->setExpanded(true);
					}
				}
			}
		}
	}
	else if(selectOptionTreeIndex == 902)
	{
		// 投影(DI后牙反合->投影)

		setOperatePreCondition(902, 6);
		this->pickStyle->mode = 0;
 
		for(int i = 0; i < 16; ++i)
		{
			if(diBackToothOppositeClosePointState[i])
			{
				vtkPlane::ProjectPoint(diBackToothOppositeClosePoint[i], fitPlaneCenter, fitPlaneNormal, diBackToothOppositeCloseProjectPoint[i]);

				drawPoint(diBackToothOppositeCloseProjectPoint[i], projectToFittingPlanePointColot, diBackToothOppositeCloseProjectPointActor[i]);
				isDiBackToothOppositeCloseProjectPointShow = 1;
			}
		}

		updatePointInfoItem(currentOptionTreeWidgetItem, &diBackToothOppositeCloseProjectPoint[0], diBackToothOppositeClosePointState, 16);
	}
	else if(selectOptionTreeIndex == 906)
	{
		// 投影(DI后牙正锁合->投影)

		setOperatePreCondition(906, 6);
		this->pickStyle->mode = 0;
 
		for(int i = 0; i < 10; ++i)
		{
			if(diBackToothLockClosePointState[i])
			{
				vtkPlane::ProjectPoint(diBackToothLockClosePoint[i], fitPlaneCenter, fitPlaneNormal, diBackToothLockCloseProjectPoint[i]);

				drawPoint(diBackToothLockCloseProjectPoint[i], projectToFittingPlanePointColot, diBackToothLockCloseProjectPointActor[i]);
				isDiBackToothLockCloseProjectPointShow = 1;
			}
		}

		updatePointInfoItem(currentOptionTreeWidgetItem, &diBackToothLockCloseProjectPoint[0], diBackToothLockClosePointState, 10);
	}
	else if(selectOptionTreeIndex == 88)
	{
		// 投影(开合->前牙开合->冠状面->点)
		setOperatePreCondition(88, 6);
		this->pickStyle->mode = 0;

		for(int i = 0; i < 4; ++i)
		{
			if(topPointToProjectArticlePlaneStat[i][0] && topPointToProjectArticlePlaneStat[i][1])
			{
				double endPoint[2][3];
				vtkPlane::ProjectPoint(topPointToProjectArticlePlanePosition[i][0],
					pointPositionForArticlePlane, articlePlaneNormal,
					endPoint[0]);
				vtkPlane::ProjectPoint(topPointToProjectArticlePlanePosition[i][1],
					pointPositionForArticlePlane, articlePlaneNormal,
					endPoint[1]);

				double topEndPointColor[2][3] = {
					{1, 1, 1},
					{1, 1, 1}
				};
				double lineColor[3] = {0.6549, 0.9569, 0.6471};
				drawLine(endPoint[0], topEndPointColor[0], topPointProjectArticlePlaneActor[i][0],
					endPoint[1], topEndPointColor[1], topPointProjectArticlePlaneActor[i][1],
					2, lineColor, topLineProjectArticlePlaneActor[i]);

				isProjectArticlePlane = 1;
			}

			if(lowPointToProjectArticlePlaneStat[i][0] && lowPointToProjectArticlePlaneStat[i][1])
			{
				double endPoint[2][3];
				vtkPlane::ProjectPoint(lowPointToProjectArticlePlanePosition[i][0],
					pointPositionForArticlePlane, articlePlaneNormal,
					endPoint[0]);
				vtkPlane::ProjectPoint(lowPointToProjectArticlePlanePosition[i][1],
					pointPositionForArticlePlane, articlePlaneNormal,
					endPoint[1]);

				double lowEndPointColor[2][3] = {
					{1, 0, 0},
					{0, 0, 1}
				};
				double lineColor[3] = {0.6549, 0.9569, 0.6471};
				drawLine(endPoint[0], lowEndPointColor[0], lowPointProjectArticlePlaneActor[i][0],
					endPoint[1], lowEndPointColor[1], lowPointProjectArticlePlaneActor[i][1],
					2, lineColor, lowLineProjectArticlePlaneActor[i]);

				isProjectArticlePlane = 1;
			}
		}
	}
	else if(selectOptionTreeIndex == 31)
	{
		// 投影点(颊侧区咬合->上牙列->点(水平向)->投影结果)

		setOperatePreCondition(31, 6);

		for(int i = 0; i < 16; ++i)
		{
			if(topToothPointForFitPlaneStat[i])
			{
				vtkPlane::ProjectPoint(topToothPointForFitPlanePosition[i], 
					fitPlaneCenter, fitPlaneNormal, 
					topToothProjectPointForFitPlanePosition[i]);
				//topProjectToFittingPlanePointColot??=??color
				double color[3] = {0, 0, 1};
				if (i >= 8)
				{
					drawPoint(topToothProjectPointForFitPlanePosition[i], topProjectToFittingPlanePointColot, topToothProjectPointForFitPlaneActor[i]);
				}else
				{
					drawPoint(topToothProjectPointForFitPlanePosition[i], color, topToothProjectPointForFitPlaneActor[i]);
				}
				cout<<1<<endl;
				isTopToothProjectPointForFitPlaneShow = 1;

				QString text = QString("(").
					append(QString::number(topToothProjectPointForFitPlanePosition[i][0])).append(", ").
					append(QString::number(topToothProjectPointForFitPlanePosition[i][1])).append(", ").
					append(QString::number(topToothProjectPointForFitPlanePosition[i][2])).append(")");
				QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(projectPointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}
				cout<<2<<endl;
			}
		}
	}
	else if(selectOptionTreeIndex == 32)
	{
		// 投影(颊侧区咬合->下牙列->点(矢状向)->投影结果)

		setOperatePreCondition(32, 6);

		this->removeChildItem(currentOptionTreeWidgetItem);
		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForVerticalPlaneStat[i])
			{
				vtkPlane::ProjectPoint(lowToothPointForVerticalPlanePosition[i],
					pointInVerticalPlanePosition[0], verticalPlaneNormal,
					lowToothProjectPointForVerticalPlanePosition[i]);

				double color[3] = {0, 1, 0};
				drawPoint(lowToothProjectPointForVerticalPlanePosition[i], projectToFittingPlanePointColot, lowToothProjectPointForVerticalPlaneActor[i]);
				isLowToothProjectPointForVerticalPlaneShow = 1;

				QString text = QString("(").
					append(QString::number(lowToothProjectPointForVerticalPlanePosition[i][0])).append(", ").
					append(QString::number(lowToothProjectPointForVerticalPlanePosition[i][1])).append(", ").
					append(QString::number(lowToothProjectPointForVerticalPlanePosition[i][2])).append(")");
				QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(projectPointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}
			}
		}
	}
	else if(selectOptionTreeIndex == 33)
	{
		// 投影点(颊侧区咬合->下牙列->点(水平向)->投影结果)

		setOperatePreCondition(33, 6);

		this->removeChildItem(currentOptionTreeWidgetItem);
		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForFitPlaneStat[i])
			{
				vtkPlane::ProjectPoint(lowToothPointForFitPlanePosition[i], 
					fitPlaneCenter, fitPlaneNormal,
					lowToothProjectPointForFitPlanePosition[i]);

				//double color[3] = {0, 1, 0};
				drawPoint(lowToothProjectPointForFitPlanePosition[i], projectToFittingPlanePointColot, lowToothProjectPointForFitPlaneActor[i]);
				isLowToothProjectPointForFitPlaneShow = 1;

				QString text = QString("(").
					append(QString::number(lowToothProjectPointForFitPlanePosition[i][0])).append(", ").
					append(QString::number(lowToothProjectPointForFitPlanePosition[i][1])).append(", ").
					append(QString::number(lowToothProjectPointForFitPlanePosition[i][2])).append(")");
				QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(projectPointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}
			}			
		}
	}
	else if(selectOptionTreeIndex == 40)
	{
		if(!currentOptionTreeWidgetItem->isExpanded())
			currentOptionTreeWidgetItem->setExpanded(true);

		if(isVerticalParallelPlaneInitPointShow == 1)
			switchVerticalParallelPlaneInitPointShow();

		removeChildItemExcept(currentOptionTreeWidgetItem, 41);

		for(int i = 0; i < 2; ++i)
		{
			vtkPlane::ProjectPoint(towPointToProjectFitPlanePositon[i],
				fitPlaneCenter, fitPlaneNormal,
				towPointProjectFitPlanePositon[i]);

			QString text = QString("(").
				append(QString::number(towPointProjectFitPlanePositon[i][0])).append(", ").
				append(QString::number(towPointProjectFitPlanePositon[i][1])).append(", ").
				append(QString::number(towPointProjectFitPlanePositon[i][2])).append(")");
			QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
			currentOptionTreeWidgetItem->addChild(projectPointInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			double color[3] = {0, 1, 0};
			drawPoint(towPointProjectFitPlanePositon[i], color, towPointProjectFitPlaneActor[i]);
			isTwoPointProjectFitPlaneShow = 1;
		}

		distanceBetweenTwoProjectOnFitPlanePoint = sqrt(vtkMath::Distance2BetweenPoints(towPointProjectFitPlanePositon[0],
			towPointProjectFitPlanePositon[1]));
		
		QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem, 41);
		removeChildItem(distanceItem);
		QString text = QString::number(distanceBetweenTwoProjectOnFitPlanePoint);
		QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(text));
		distanceItem->addChild(distanceInfoItem);
		if(!distanceItem->isExpanded())
		{
			distanceItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 50 || selectOptionTreeIndex == 60)
	{
		if(selectOptionTreeIndex == 50)
			topLowProjectIndex = 0;
		else if(selectOptionTreeIndex == 60)
			topLowProjectIndex = 1;

		if(isFitPlaneShow == 0)
			switchFitPlaneShow();

		for(int i = 0; i < 16; ++i)
		{
			vtkPlane::ProjectPoint(inverseFitCoverPointPosition[i],
				fitPlaneCenter, fitPlaneNormal,
				inverseFitConvertProjectPointPosition[i]);
		}

		if(topLowProjectIndex == 0)
		{
			for(int i = 0; i < 4; ++i)
			{
				double pointColor[3] = {0, 1, 0};
				double lineColor[3] = {1, 1, 0};
				drawLine(inverseFitConvertProjectPointPosition[topLowProjectIndex * 8 + i * 2], pointColor, inverseFitCoverProjectPointActor[topLowProjectIndex * 8 + i * 2],
					inverseFitConvertProjectPointPosition[topLowProjectIndex * 8 + i * 2 + 1], pointColor, inverseFitCoverProjectPointActor[topLowProjectIndex * 8 + i * 2 + 1],
					2, lineColor, inverseFitCoverProjectLineActor[topLowProjectIndex * 4 + i]);
			}
		}
		else if(topLowProjectIndex == 1)
		{
			for(int i = 0; i < 4; ++i)
			{
				double pointColor[3] = {0, 0, 1};
				double lineColor[3] = {1, 1, 0};
				drawLine(inverseFitConvertProjectPointPosition[topLowProjectIndex * 8 + i * 2], pointColor, inverseFitCoverProjectPointActor[topLowProjectIndex * 8 + i * 2],
					inverseFitConvertProjectPointPosition[topLowProjectIndex * 8 + i * 2 + 1], pointColor, inverseFitCoverProjectPointActor[topLowProjectIndex * 8 + i * 2 + 1],
					2, lineColor, inverseFitCoverProjectLineActor[topLowProjectIndex * 4 + i]);
			}
		}
	}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
		return;
	}
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 3;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);

}

void MainWindow::getPointPositionFromTreeItem(QTreeWidgetItem *item, double *position)
{
	QString poingInfo = item->text(0);
	poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
	QStringList stringList = poingInfo.split(", ");
	for(int i = 0; i < stringList.length(); ++i)
	{
		position[i] = stringList.at(i).toDouble();
	}
}

void MainWindow::deletePoint()
{
	//QMessageBox::information(NULL, tr("Info"), QString::number(selectOptionTreeIndex));

	if(!currentOptionTreeWidgetItem || !(currentOptionTreeWidgetItem->parent()))
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
		return;
	}

	QTreeWidgetItem *parentItem = currentOptionTreeWidgetItem->parent();
	QVariant data = parentItem->data(0, Qt::UserRole);
	QVariant currentData = currentOptionTreeWidgetItem->data(0, Qt::UserRole);
	if(5 == getSelectOptionTreeIndex(data))
	{
		// 删除合平面拟合点

		this->pickStyle->mode = 0;
		setOperatePreCondition(5, 5);

		//QString poingInfo = currentOptionTreeWidgetItem->text(0);
		//poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		//QStringList stringList = poingInfo.split(", ");
		double position[3];
		getPointPositionFromTreeItem(currentOptionTreeWidgetItem, position);
		//for(int i = 0; i < stringList.length(); ++i)
		//{
		//	position[i] = stringList.at(i).toDouble();
		//}
		//QMessageBox::information(NULL, tr("Info"), QString::number(position[0]).append(", ").append(QString::number(position[1])).append(", ").append(QString::number(position[2])));

		int index = this->getFitPointIndex(position[0], position[1], position[2]);
		if(index > -1)
		{
			fitPlanePointStat[index] = false;
			this->ren->RemoveActor(fitPlanePointActor[index]);
		}

		int fitPlanePointCount = 0;
		for(int i = 0; i < 20; ++i)
		{
			if(fitPlanePointStat[i])
				++fitPlanePointCount;
		}

		if(fitPlanePointCount > 3)
		{
			initFitPlane();
		}
		else
		{
			if(isFitPlaneShow == 1)
				switchFitPlaneShow();
		}

		//
		// 修改操作框信息
		//
		QTreeWidgetItem *fitPlaneNormalItem = getChildItem(parentItem->parent(), 6);
		fitPlaneNormalItem->removeChild(fitPlaneNormalItem->child(0));
		QTreeWidgetItem *fitPlaneCenterItem = getChildItem(parentItem->parent(), 7);
		fitPlaneCenterItem->removeChild(fitPlaneCenterItem->child(0));
		if(fitPlanePointCount > 3)
		{
			QString itemText = QString("(").
				append(QString::number(fitPlaneNormal[0])).append(", ").
				append(QString::number(fitPlaneNormal[1])).append(", ").
				append(QString::number(fitPlaneNormal[2])).append(")");
			QTreeWidgetItem *fitPlaneNormalInfoItem = new QTreeWidgetItem(fitPlaneNormalItem, QStringList(itemText));
			fitPlaneNormalItem->addChild(fitPlaneNormalInfoItem);
			if(!fitPlaneNormalItem->isExpanded())
				fitPlaneNormalItem->setExpanded(true);

			itemText = QString("(").
				append(QString::number(fitPlaneCenter[0])).append(", ").
				append(QString::number(fitPlaneCenter[1])).append(", ").
				append(QString::number(fitPlaneCenter[2])).append(")");
			QTreeWidgetItem *fitPlaneCenterInfoItem = new QTreeWidgetItem(fitPlaneCenterItem, QStringList(itemText));
			fitPlaneCenterItem->addChild(fitPlaneCenterItem);
			if(!fitPlaneCenterItem->isExpanded())
				fitPlaneCenterItem->setExpanded(true);
		}

		parentItem->removeChild(currentOptionTreeWidgetItem);
		currentOptionTreeWidgetItem = NULL;
		selectOptionTreeIndex = 0;
	}
	else if (selectOptionTreeIndex - 1111000 > 0)
	{
		parentItem->removeChild(currentOptionTreeWidgetItem);
		//removeChildItem(currentOptionTreeWidgetItem);
		cout<<selectOptionTreeIndex<<endl;
		//SegPointMap.erase(selectOptionTreeIndex);
		for (vector<pair<int,double*>>::iterator it = SegPointMap.begin();it != SegPointMap.end();it ++)
		{
			if (it->first == selectOptionTreeIndex)
			{
				SegPointMap.erase(it);
				break;
			}
		}
		int currP = selectOptionTreeIndex - 1111000 - 1;
		this->ren->RemoveActor(SegPointActor[currP]);

		this->ui.qvtkObjectViewerWidget->update();
		PickPointState[currP] = 0;
	}
	else if(135 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI覆合->轮廓曲线->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);

		diOverbiteIntersectCurvePointIndex = 0;
	}
	else if(125 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI覆合->轮廓曲线->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);

		diCoverIntersectCurvePointIndex = 0;
	}
	else if(75 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI覆合->轮廓曲线->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);

		CoverIntersectCurvePointIndex = 0;
	}
	else if(901 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI后牙反合->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);
		for(int i = 0; i < 16; ++i)
		{
			diBackToothOppositeClosePointState[i] = false;
		}
	}
	else if(905 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI后牙正锁合->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);
		for(int i = 0; i < 10; ++i)
		{
			diBackToothLockClosePointState[i] = false;
		}
	}
	else if(1002 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI其他->中线不调->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);
		diMiddleUnfitPointIndex = 0;
	}
	else if(1006 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(DI其他->中线不调->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);
		diMiddleCutToothGapIndex = 0;
	}
	else if(147 == getSelectOptionTreeIndex(currentData) / 100)
	{
		// 删除点(DI侧方开合->磨牙1/2/3->轮廓曲线->点)

		int grindToothIndex = getSelectOptionTreeIndex(currentData) - 14700;

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		removeChildItem(currentOptionTreeWidgetItem);

		diSideOpenCloseIntersectCurvePointIndex[grindToothIndex] = 0;
		diSideOpenCloseDistance[grindToothIndex] = 0;
		diSideOpenCloseGrindToothScore[grindToothIndex] = 0;

		if(isDiSideOpenCloseIntersectCurvePointShow[grindToothIndex] == 1)
			switchDiSideOpenCloseIntersectCurvePointShow(grindToothIndex);
	}
	else if(420 < getSelectOptionTreeIndex(currentData) && getSelectOptionTreeIndex(currentData) < 820)
	{
		if((getSelectOptionTreeIndex(currentData) - 427) % 20 == 0)
		{
			// 删除点(DI前牙开合->测量1/../20->轮廓曲线->点)

			int measureIndex = (getSelectOptionTreeIndex(currentData) - 427) / 20;

			this->pickStyle->mode = 0;
			setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

			removeChildItem(currentOptionTreeWidgetItem);

			diFrontOpenCloseIntersectCurvePointIndex[measureIndex] = 0;
			diFrontOpenCloseDistance[measureIndex] = 0;
			diFrontOpenCloseMeasureScore[measureIndex] = 0;

			if(isDiFrontOpenCloseIntersectCurvePointShow[measureIndex] == 1)
				switchDiFrontOpenCloseIntersectCurvePointShow(measureIndex);
		}
	}
	
	else if(67 == getSelectOptionTreeIndex(currentData))
	{
		// 删除点(覆盖->拟合曲线->点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(currentData), 5);

		//QString poingInfo = currentOptionTreeWidgetItem->text(0);
		//poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		//QStringList stringList = poingInfo.split(", ");
		//double position[3];
		//for(int i = 0; i < stringList.length(); ++i)
		//{
		//	position[i] = stringList.at(i).toDouble();
		//}

		//int index = this->getSplinePointIndex(position[0], position[1], position[2]);
		//if(index > -1)
		//{
		//	splinePointState[index] = false;
		//	this->ren->RemoveActor(splinePointActor[index]);
		//	this->ren->RemoveActor(splineProjectPointActor[index]);

		//	switchCurrentSelectPointShow();
		//}

		for(int i = 0; i < 20; ++i)
		{
			splinePointState[i] = false;

			this->ren->RemoveActor(splinePointActor[i]);
			this->ren->RemoveActor(splineProjectPointActor[i]);
			isSplinePointShow = isSplineProjectPointShow = 0;
		}

		//parentItem->removeChild(currentOptionTreeWidgetItem);
		removeChildItem(currentOptionTreeWidgetItem);

		currentOptionTreeWidgetItem = NULL;
		selectOptionTreeIndex = 0;
	}
	
	else if(getSelectOptionTreeIndex(currentData) / 1000 == 9 && getSelectOptionTreeIndex(currentData) % 1000 < 10)
	{
		// 删除缝隙点(前牙区不齐->接触点->缝隙x)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(data), 5);

		QString poingInfo = currentOptionTreeWidgetItem->text(0);
		poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		QStringList stringList = poingInfo.split(", ");
		double position[3];
		for(int i = 0; i < stringList.length(); ++i)
		{
			position[i] = stringList.at(i).toDouble();
		}

		int index[2];
		this->getGapPointIndex(position[0], position[1], position[2], index);
		if(index[0] > -1 &&index[1] > -1)
		{
			gapPointStat[index[0]][index[1]] = false;
			this->ren->RemoveActor(gapPointActor[index[0]][index[1]]);

			if(isCurrentSelectPointShow == 1)
			{
				switchCurrentSelectPointShow();
			}
		}

		parentItem->removeChild(currentOptionTreeWidgetItem);
		currentOptionTreeWidgetItem = NULL;
		selectOptionTreeIndex = 0;
	}
	else if(26 == getSelectOptionTreeIndex(data))
	{
		// 删除点(颊侧区咬合->下牙列->正中矢状面->初始点)

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(data), 5);

		QString poingInfo = currentOptionTreeWidgetItem->text(0);
		poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		QStringList stringList = poingInfo.split(", ");
		double position[3];
		for(int i = 0; i < stringList.length(); ++i)
		{
			position[i] = stringList.at(i).toDouble();
		}

		int index = this->getPointInVerticalPlaneIndex(position[0], position[1], position[2]);
		if(index > -1)
		{
			pointInVerticalPlaneIndex = index - 1;

			pointInVerticalPlaneStat[index] = false;
			this->ren->RemoveActor(pointInVerticalPlaneActor[index]);

			if(isCurrentSelectPointShow == 1)
			{
				switchCurrentSelectPointShow();
			}
		}

		QTreeWidgetItem *normalItem = this->getChildItem(parentItem->parent(), 27);
		parentItem->removeChild(currentOptionTreeWidgetItem);
		this->removeChildItem(normalItem);

		currentOptionTreeWidgetItem = NULL;
		selectOptionTreeIndex = 0;
	}
	else if(24 == getSelectOptionTreeIndex(data))
	{
		// 删除点(颊侧区咬合->下牙列->点(矢状向))

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(data), 5);

		QString poingInfo = currentOptionTreeWidgetItem->text(0);
		poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		QStringList stringList = poingInfo.split(", ");
		double position[3];
		for(int i = 0; i < stringList.length(); ++i)
		{
			position[i] = stringList.at(i).toDouble();
		}

		int index = this->getLowToothPointForVerticalPlaneIndex(position[0], position[1], position[2]);
		if(index > -1)
		{
			if(isCurrentSelectPointShow == 1)
			{
				lowToothPointForVerticalPlaneStat[index] = false;
				this->ren->RemoveActor(lowToothPointForVerticalPlaneActor[index]);
				this->ren->RemoveActor(lowToothProjectPointForVerticalPlaneActor[index]);

				switchCurrentSelectPointShow();
			}
		}

		parentItem->removeChild(currentOptionTreeWidgetItem);

		currentOptionTreeWidgetItem = NULL;
		selectOptionTreeIndex = 0;
	}
	else if(25 == getSelectOptionTreeIndex(data))
	{
		// 删除点(颊侧区咬合->下牙列->点(水平向))

		this->pickStyle->mode = 0;
		setOperatePreCondition(getSelectOptionTreeIndex(data), 5);

		QString poingInfo = currentOptionTreeWidgetItem->text(0);
		poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		QStringList stringList = poingInfo.split(", ");
		double position[3];
		for(int i = 0; i < stringList.length(); ++i)
		{
			position[i] = stringList.at(i).toDouble();
		}

		int index = this->getLowToothPointForFitPlaneIndex(position[0], position[1], position[2]);
		if(index > -1)
		{
			if(isCurrentSelectPointShow == 1)
			{
				lowToothPointForFitPlaneStat[index] = false;
				this->ren->RemoveActor(lowToothPointForFitPlaneActor[index]);
				this->ren->RemoveActor(lowToothProjectPointForFitPlaneActor[index]);

				switchCurrentSelectPointShow();
			}
		}

		parentItem->removeChild(currentOptionTreeWidgetItem);

		currentOptionTreeWidgetItem = NULL;
		selectOptionTreeIndex = 0;
	}	
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("no support option...")));
		return;
	}
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 2;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
}

void MainWindow::iPickPoint()
{
	if (selectOptionTreeIndex == 1111)
	{
		setOperatePreCondition(selectOptionTreeIndex, 3);
		this->pickStyle->mode = 3;
	}
}

void MainWindow::iFittingPlane()
{
	cout<<selectOptionTreeIndex<<endl;
	if (selectOptionTreeIndex == 1112)
	{
		
		vector<QTreeWidgetItem*> Itembox;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pIndex / 1000 != 1111)
				{
					QMessageBox::information(NULL, "Error", "please Only choose points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}else
				{
					Itembox.push_back(*it);
				}
			}
			++it;
		}
		cout<<"点集大小"<<Itembox.size()<<endl;
		if (Itembox.size() < 3)
		{
			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			errFlag = 1;
		}
		if (errFlag == 1)
		{
			return;
		}
		double points[100][3] = {};
		double center[3] = {};
		double normal[3] = {};
		double planepoint[9] = {};

		for (int i = 0;i < Itembox.size();i ++)
		{
			getPointPositionFromTreeItem(Itembox[i],points[i]);
		}
		icalcFitPlane(points,Itembox.size(), center,normal,planepoint);
		
		MyPlane P;
		P.PlaneActor = vtkSmartPointer<vtkActor>::New();
		P.PlaneFrameActor = vtkSmartPointer<vtkActor>::New();
		for (int i = 0 ;i < 3;i ++)
		{
			P.centre[i] = center[i];
			P.normal[i] = normal[i];
			for (int j = 0;j < 3;j ++)
			{
				P.points[i][j] = planepoint[i * 3 + j];
			}
		}

		for (int i = 0;i < 300;i ++)
		{
			if (PlaneState[i] == 0)
			{
				PlaneCount = i;
				PlaneState[i] = 1;
				break;
			}
		}
		QString text;

		text = QString("Plane").append(QString::number(PlaneNum + 1));
		QTreeWidgetItem *planeInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		planeInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(planeInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		planeInfoItem->setData(0, Qt::UserRole, 1112 * 1000 + PlaneNum + 1);
		
		text = QString("Centre:(").
			append(QString::number(center[0])).append(", ").
			append(QString::number(center[1])).append(", ").
			append(QString::number(center[2])).append(")");
		QTreeWidgetItem *planeInfoItem1 = new QTreeWidgetItem(planeInfoItem, QStringList(text));
		planeInfoItem->addChild(planeInfoItem1);
		planeInfoItem->setExpanded(true);

		text = QString("Normal:(").
			append(QString::number(normal[0])).append(", ").
			append(QString::number(normal[1])).append(", ").
			append(QString::number(normal[2])).append(")");
		QTreeWidgetItem *planeInfoItem2 = new QTreeWidgetItem(planeInfoItem, QStringList(text));
		planeInfoItem->addChild(planeInfoItem2);
		planeInfoItem->setExpanded(true);

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {0.5882, 0.7569, 0.5843};
// 		for (int i = 0 ;i < 3;i ++)
// 		{
// 			for (int j = 0;j < 3;j ++)
// 			{
// 				cout<<P.points[i][j]<<' ';
// 			}
// 			cout<<endl;
// 		}

		drawPlane(P.points[0], P.points[1], P.points[2], planeColor, 0.4, P.PlaneActor, frameColor, 2, 2, P.PlaneFrameActor);
		ScanTree();
		PlaneMapSet.push_back(pair<int,MyPlane>(1112 * 1000 + PlaneNum + 1,P));
		PlaneNum = PlaneNum + 1;

	}
}

void MainWindow::icalcFitPlane(double ipoints[100][3],int pointLenth, double *center,double *normal,double *planepoint1)
{
	double planepoint[3][3] = {};
	vtkSmartPointer<vtkRANSACPlane> rANSACPlane = vtkSmartPointer<vtkRANSACPlane>::New();
	rANSACPlane->SetInlierThreshold(0.01);
	rANSACPlane->SetMaxIterations(1000);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();

	vtkBoundingBox boundingBox;
	for(int i = 0; i < pointLenth; ++i)
	{

		vtkIdType pid[1];
		pid[0] = points->InsertNextPoint(ipoints[i][0], 
			ipoints[i][1], ipoints[i][2]);

		vertices->InsertNextCell(1, pid );
		boundingBox.AddPoint(ipoints[i]);

	}

	vtkSmartPointer<vtkPlane> bestPlane = vtkSmartPointer<vtkPlane>::New();
	BestFitPlane(points, bestPlane);
	for(int i = 0; i < 3; ++i)
	{
		center[i] = bestPlane->GetOrigin()[i];
		normal[i] = bestPlane->GetNormal()[i];
	}

	double fitPointBound[6];
	boundingBox.GetBounds(fitPointBound);
	double fitPointBoundLength[3] = {
		fitPointBound[1] -fitPointBound[0],
		fitPointBound[3] -fitPointBound[2],
		fitPointBound[5] -fitPointBound[4]
	};
	int minLengthIndex = 0;
	if(fitPointBoundLength[0] < fitPointBoundLength[1] && fitPointBoundLength[0] < fitPointBoundLength[2])
		minLengthIndex = 0;
	else if(fitPointBoundLength[1] < fitPointBoundLength[0] && fitPointBoundLength[1] < fitPointBoundLength[2])
		minLengthIndex = 1;
	else
		minLengthIndex = 2;
	//QMessageBox::information(NULL, tr("Info"), QString::number(minLengthIndex));

	double origin[3];
	double direction1[3], direction2[3];
	cout<<"minLengthIndex : "<<minLengthIndex<<endl;
	if(minLengthIndex == 0)
	{
		//
		// 合平面几乎平行于yoz面
		//

		double pointToProject[3];

		// 原点
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];
		bestPlane->ProjectPoint(pointToProject, origin);
		planepoint[0][0] = origin[0];
		planepoint[0][1] = origin[1];
		planepoint[0][2] = origin[2];

		// 轴一的方向
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[5];	
		bestPlane->ProjectPoint(pointToProject, direction1);
		direction1[0] = direction1[0] - origin[0];
		direction1[1] = direction1[1] - origin[1];
		direction1[2] = direction1[2] - origin[2];
		vtkMath::Normalize(direction1);

		// 轴二的方向
		vtkMath::Cross(normal, direction1, direction2);
		vtkMath::Normalize(direction2);
		if(direction2[1] < 0)
		{
			direction2[0] = -direction2[0];
			direction2[1] = -direction2[1];
			direction2[2] = -direction2[2];
		}
		double point1[3], point2[3];
		point1[0] = origin[0] + 150 * pointSize * direction1[0];
		point1[1] = origin[1] + 150 * pointSize * direction1[1];
		point1[2] = origin[2] + 150 * pointSize * direction1[2];
		point2[0] = origin[0] + 150 * pointSize * direction2[0];
		point2[1] = origin[1] + 150 * pointSize * direction2[1];
		point2[2] = origin[2] + 150 * pointSize * direction2[2];

		planepoint[1][0] = point1[0];
		planepoint[1][1] = point1[1];
		planepoint[1][2] = point1[2];
		planepoint[2][0] = point2[0];
		planepoint[2][1] = point2[1];
		planepoint[2][2] = point2[2];
	}
	else if(minLengthIndex == 1)
	{
		//
		// 合平面几乎平行于xoz面
		//

		double pointToProject[3];

		// 原点
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];
		bestPlane->ProjectPoint(pointToProject, origin);
		planepoint[0][0] = origin[0];
		planepoint[0][1] = origin[1];
		planepoint[0][2] = origin[2];

		// 轴一的方向
		pointToProject[0] = modelBound[1];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];	
		bestPlane->ProjectPoint(pointToProject, direction1);
		direction1[0] = direction1[0] - origin[0];
		direction1[1] = direction1[1] - origin[1];
		direction1[2] = direction1[2] - origin[2];
		cout<<"t:"<<direction1[0]<<endl;
		vtkMath::Normalize(direction1);
		cout<<direction1[0]<<endl;
		// 轴二的方向
		vtkMath::Cross(normal, direction1, direction2);
		vtkMath::Normalize(direction2);
		if(direction2[2] < 0)
		{
			direction2[0] = -direction2[0];
			direction2[1] = -direction2[1];
			direction2[2] = -direction2[2];
		}

		double point1[3], point2[3];
		point1[0] = origin[0] + 150 * pointSize * direction1[0];
		point1[1] = origin[1] + 150 * pointSize * direction1[1];
		point1[2] = origin[2] + 150 * pointSize * direction1[2];
		point2[0] = origin[0] + 150 * pointSize * direction2[0];
		point2[1] = origin[1] + 150 * pointSize * direction2[1];
		point2[2] = origin[2] + 150 * pointSize * direction2[2];

		planepoint[1][0] = point1[0];
		planepoint[1][1] = point1[1];
		planepoint[1][2] = point1[2];
		planepoint[2][0] = point2[0];
		planepoint[2][1] = point2[1];
		planepoint[2][2] = point2[2];
	}
	else if(minLengthIndex == 2)
	{
		//
		// 合平面平行于xoy面
		//

		double pointToProject[3];

		// 原点
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];
		bestPlane->ProjectPoint(pointToProject, origin);
		planepoint[0][0] = origin[0];
		planepoint[0][1] = origin[1];
		planepoint[0][2] = origin[2];

		// 轴一的方向
		pointToProject[0] = modelBound[1];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];	
		bestPlane->ProjectPoint(pointToProject, direction1);
		direction1[0] = direction1[0] - origin[0];
		direction1[1] = direction1[1] - origin[1];
		direction1[2] = direction1[2] - origin[2];
		vtkMath::Normalize(direction1);

		// 轴二的方向
		vtkMath::Cross(normal, direction1, direction2);
		vtkMath::Normalize(direction2);
		if(direction2[1] < 0)
		{
			direction2[0] = -direction2[0];
			direction2[1] = -direction2[1];
			direction2[2] = -direction2[2];
		}
		double point1[3], point2[3];
		point1[0] = origin[0] + 150 * pointSize * direction1[0];
		point1[1] = origin[1] + 150 * pointSize * direction1[1];
		point1[2] = origin[2] + 150 * pointSize * direction1[2];
		point2[0] = origin[0] + 150 * pointSize * direction2[0];
		point2[1] = origin[1] + 150 * pointSize * direction2[1];
		point2[2] = origin[2] + 150 * pointSize * direction2[2];

		planepoint[1][0] = point1[0];
		planepoint[1][1] = point1[1];
		planepoint[1][2] = point1[2];
		planepoint[2][0] = point2[0];
		planepoint[2][1] = point2[1];
		planepoint[2][2] = point2[2];
	}
	double planeColor[3] = {0.5647, 0.6235, 0.5647};
	double frameColor[3] = {0.5882, 0.7569, 0.5843};
	cout<<planepoint[0][0]<<' '<<planepoint[0][1]<<' '<<planepoint[0][2]<<endl;
	for (int i = 0;i < 3;i ++)
	{
		for (int j = 0;j < 3;j ++)
		{
			cout<<planepoint[i][j]<<',';
			planepoint1[i * 3 + j] = planepoint[i][j];
		}
		cout<<endl;
	}
	//drawPlane(fitPlanePoint[0], fitPlanePoint[1], fitPlanePoint[2], planeColor, 0.4, fitPlaneActor, frameColor, 2, 2, fitPlaneFrameActor);
	//isFitPlaneShow = 1;
}

void MainWindow::iProjectPoint()
{
	if (selectOptionTreeIndex == 1113)
	{

		QTreeWidgetItem *PointItem;
		QTreeWidgetItem *PlaneItem;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag = 0;
		bool planeFlag = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag == 0 && pIndex / 1000 == 1111)
				{
					PointItem = *it;
					pointFlag = 1;
				}else if (planeFlag == 0 && pIndex / 1000 == 1112)
				{
					PlaneItem = *it;
					planeFlag = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}
		
// 		if (Itembox.size() != 3)
// 		{
// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
// 			errFlag = 1;
// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3] = {};

		double center[3] = {};
		double normal[3] = {};

		getPointPositionFromTreeItem(PointItem,points);

		QVariant dt = PlaneItem->data(0, Qt::UserRole);
		int planeIndex = getSelectOptionTreeIndex(dt);
		MyPlane P;

		for (vector<pair<int,MyPlane>>::iterator it = PlaneMapSet.begin();it != PlaneMapSet.end();it ++)
		{
			if (it->first == planeIndex)
			{
				P = it->second;
			}
		}
		for (int i = 0;i < 3;i ++)
		{
			center[i] = P.centre[i];
			normal[i] = P.normal[i];
		}

		double ProjPoint[3];

		vtkPlane::ProjectPoint(points, center, normal, ProjPoint);

		double x = ProjPoint[0];
		double y = ProjPoint[1];
		double z = ProjPoint[2];
		QString text = QString("(").
			append(QString::number(ProjPoint[0])).append(", ").
			append(QString::number(ProjPoint[1])).append(", ").
			append(QString::number(ProjPoint[2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		for (int i = 0;i < 300;i ++)
		{
			if (projPointState[i] == 0)
			{
				projCount = i;
				projPointState[i] = 1;
				break;
			}
		}
		projPointPosition[projCount][0] = x;
		projPointPosition[projCount][1] = y;
		projPointPosition[projCount][2] = z;
		pointInfoItem->setData(0, Qt::UserRole, 1113 * 1000 + projCount + 1);
		projPointMapSet.push_back(pair<int,double*>(1113 * 1000 + projCount + 1 ,projPointPosition[projCount] ));
		double pointColor[3] = {0, 0, 1};
		drawPoint(ProjPoint,pointColor,projPointActor[projCount]);
		ScanTree();

	}
}

void MainWindow::iCutCurve()
{
	if (selectOptionTreeIndex == 1114)
	{

		QTreeWidgetItem *PlaneItem;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool planeFlag = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (planeFlag == 0 && pIndex / 1000 == 1112)
				{
					PlaneItem = *it;
					planeFlag = 1;
					break;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3] = {};

		double center[3] = {};
		double normal[3] = {};

		QVariant dt = PlaneItem->data(0, Qt::UserRole);
		int planeIndex = getSelectOptionTreeIndex(dt);
		MyPlane P;
		for (int i = 0;i < PlaneMapSet.size();i ++)
		{
			if (PlaneMapSet[i].first == planeIndex)
			{
				P = PlaneMapSet[i].second;
			}
		}
		for (int i = 0;i < 3;i ++)
		{
			center[i] = P.centre[i];
			normal[i] = P.normal[i];
		}
		MyCurve mC;
		for (int i = 0;i < 3;i ++)
		{
			mC.centre[i] = P.centre[i];
			mC.normal[i] = P.normal[i];
		}
		mC.lowCurveActor = vtkSmartPointer<vtkActor>::New();
		mC.topCurveActor = vtkSmartPointer<vtkActor>::New();
		drawIntersectCurve(lowerToothData, 
			center, normal, 
			intersectCurveColor, 2, 
			mC.lowCurveActor);
		drawIntersectCurve(topToothData, 
			center, normal, 
			intersectCurveColor, 2, 
			mC.topCurveActor);
		cutCurveCount ++;


		QString text = QString("Curve ").
			append(QString::number(cutCurveCount));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		pointInfoItem->setData(0, Qt::UserRole, 1114 * 1000 + cutCurveCount);
		cutCurveMapSet.push_back(pair<int,MyCurve>(1114 * 1000 + cutCurveCount ,mC ));
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);
		ScanTree();

	}
}
void MainWindow::iVector()
{
	if (selectOptionTreeIndex == 1115)
	{

		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1111)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[2][3] = {};

		getPointPositionFromTreeItem(PointItem1,points[0]);
		getPointPositionFromTreeItem(PointItem2,points[1]);

		MyVector mV;
		for (int i = 0;i < 3;i ++)
		{
			mV.point1[i] = points[0][i];
			mV.point2[i] = points[1][i];
		}
		vtkSmartPointer<vtkActor> point1Actor = vtkSmartPointer<vtkActor>::New();
		vtkSmartPointer<vtkActor> point2Actor = vtkSmartPointer<vtkActor>::New();
		vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
		drawLine(mV.point1, pickPointColor,point1Actor,
			mV.point2, pickPointColor,  point2Actor, 
			2, intersectLlineColor,  lineActor);
		VectorCount ++;


		QString text = QString("Vector ").
			append(QString::number(VectorCount));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		pointInfoItem->setData(0, Qt::UserRole, 1115 * 1000 + VectorCount);
		VecMapSet.push_back(pair<int,MyVector>(1115 * 1000 + VectorCount ,mV ));
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);
		
		ScanTree();

	}
}
void MainWindow::iPlaneVector()
{
	if (selectOptionTreeIndex == 1115)
	{

		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1112)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1112)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose two planes", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}

		QVariant dt1 = PointItem1->data(0, Qt::UserRole);
		int planeIndex1 = getSelectOptionTreeIndex(dt1);
		MyPlane V1;
		for (int i = 0;i < PlaneMapSet.size();i ++)
		{
			if (PlaneMapSet[i].first == planeIndex1)
			{
				V1 = PlaneMapSet[i].second;
			}
		}

		QVariant dt2 = PointItem2->data(0, Qt::UserRole);
		int planeIndex2 = getSelectOptionTreeIndex(dt2);
		MyPlane V2;
		for (int i = 0;i < PlaneMapSet.size();i ++)
		{
			if (PlaneMapSet[i].first == planeIndex2)
			{
				V2 = PlaneMapSet[i].second;
			}
		}

		double points[2][3];
		vtkSmartPointer<vtkPlane> myPlane =  vtkSmartPointer<vtkPlane>::New();
		myPlane->SetOrigin(V1.centre[0],V1.centre[1],V1.centre[2]);
		myPlane->SetNormal(V1.normal[0],V1.normal[1],V1.normal[2]);

		double p1[3],p2[3];//0&1 0&2
		double t; // 交点的线段参量（比例）坐标，p1是0，p2是1
		double x[3]; // 交点坐标
		double pcoords[3];
		int subId;
		for (int i = 0;i < 3;i ++)
		{
			p1[i] = V2.points[0][i];
			p2[i] = V2.points[1][i];
		}
		myPlane->IntersectWithLine(p1, p2, t, x);
		
		for (int i = 0;i < 3;i ++)
		{
			points[0][i] = x[i]; 
		}

		for (int i = 0;i < 3;i ++)
		{
			p1[i] = V2.points[0][i];
			p2[i] = V2.points[2][i];
		}
		myPlane->IntersectWithLine(p1, p2, t, x);

		for (int i = 0;i < 3;i ++)
		{
			points[1][i] = x[i]; 
		}


		MyVector mV;
		for (int i = 0;i < 3;i ++)
		{
			mV.point1[i] = points[0][i];
			mV.point2[i] = points[1][i];
		}

		vtkSmartPointer<vtkActor> point1Actor = vtkSmartPointer<vtkActor>::New();
		vtkSmartPointer<vtkActor> point2Actor = vtkSmartPointer<vtkActor>::New();
		vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
		drawLine(mV.point1, pickPointColor,point1Actor,
			mV.point2, pickPointColor,  point2Actor, 
			2, intersectLlineColor,  lineActor);

		VectorCount ++;


		QString text = QString("Vector ").
			append(QString::number(VectorCount));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		pointInfoItem->setData(0, Qt::UserRole, 1115 * 1000 + VectorCount);
		VecMapSet.push_back(pair<int,MyVector>(1115 * 1000 + VectorCount ,mV ));
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}
void MainWindow::iPointToPointDistance()
{
	if (selectOptionTreeIndex == 1116)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1111)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[2][3] = {};

		getPointPositionFromTreeItem(PointItem1,points[0]);
		getPointPositionFromTreeItem(PointItem2,points[1]);

		double dis = vtkMath::Distance2BetweenPoints(points[0],points[1]);

		QString text = QString("distance: ").
			append(QString::number(dis));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}

}
void MainWindow::iPointToVectorDistance()
{
	if (selectOptionTreeIndex == 1117)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1115)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3] = {};

		getPointPositionFromTreeItem(PointItem1,points);

		QVariant dt = PointItem2->data(0, Qt::UserRole);
		int planeIndex = getSelectOptionTreeIndex(dt);
		MyVector V;
		for (int i = 0;i < VecMapSet.size();i ++)
		{
			if (VecMapSet[i].first == planeIndex)
			{
				V = VecMapSet[i].second;
			}
		}
		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = points[i] - V.point1[i];
			v2[i] = V.point2[i] - V.point1[i];
		}
		vtkMath::Cross(v1,v2,v3);
		double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		

		QString text = QString("distance: ").
			append(QString::number(dis));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}
void MainWindow::iPointToPlaneDistance()
{
	if (selectOptionTreeIndex == 1118)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1112)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3] = {};

		getPointPositionFromTreeItem(PointItem1,points);

		QVariant dt = PointItem2->data(0, Qt::UserRole);
		int planeIndex = getSelectOptionTreeIndex(dt);
		MyPlane V;
		for (int i = 0;i < PlaneMapSet.size();i ++)
		{
			if (PlaneMapSet[i].first == planeIndex)
			{
				V = PlaneMapSet[i].second;
			}
		}
		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = points[i] - V.centre[i];
			v2[i] = V.normal[i];
		}
		double dis = vtkMath::Dot(v1,v2)/vtkMath::Norm(v2);
		//double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		

		QString text = QString("distance: ").
			append(QString::number(dis));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}
void MainWindow::iPlaneToPlaneDistance()
{
	if (selectOptionTreeIndex == 1119)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1112)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1112)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		QVariant dt1 = PointItem1->data(0, Qt::UserRole);
		int planeIndex1 = getSelectOptionTreeIndex(dt1);
		MyPlane V1;
		for (int i = 0;i < PlaneMapSet.size();i ++)
		{
			if (PlaneMapSet[i].first == planeIndex1)
			{
				V1 = PlaneMapSet[i].second;
			}
		}

		QVariant dt2 = PointItem2->data(0, Qt::UserRole);
		int planeIndex2 = getSelectOptionTreeIndex(dt2);
		MyPlane V2;
		for (int i = 0;i < PlaneMapSet.size();i ++)
		{
			if (PlaneMapSet[i].first == planeIndex2)
			{
				V2 = PlaneMapSet[i].second;
			}
		}


		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = V1.centre[i];
			v2[i] = V2.centre[i];
		}
		double dis = vtkMath::Distance2BetweenPoints(v1,v2);
		//double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		

		QString text = QString("distance: ").
			append(QString::number(dis));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}
void MainWindow::iVectorAngle()
{
	if (selectOptionTreeIndex == 1120)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1115)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1115)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}

		QVariant dt1 = PointItem1->data(0, Qt::UserRole);
		int planeIndex1 = getSelectOptionTreeIndex(dt1);
		MyVector V1;
		for (int i = 0;i < VecMapSet.size();i ++)
		{
			if (VecMapSet[i].first == planeIndex1)
			{
				V1 = VecMapSet[i].second;
			}
		}

		QVariant dt2 = PointItem2->data(0, Qt::UserRole);
		int planeIndex2 = getSelectOptionTreeIndex(dt2);
		MyVector V2;
		for (int i = 0;i < VecMapSet.size();i ++)
		{
			if (VecMapSet[i].first == planeIndex2)
			{
				V2 = VecMapSet[i].second;
			}
		}


		

		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = V1.point1[i] - V1.point2[i];
			v2[i] = V2.point1[i] - V2.point2[i];
		}
		double angle = acos(vtkMath::Dot(v1,v2)/(vtkMath::Norm(v1) * vtkMath::Norm(v2)));
		//double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		

		QString text = QString("angle: ").
			append(QString::number(angle));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}
void MainWindow::iPointsAngle()
{
	if (selectOptionTreeIndex == 1121)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItem *PointItem3;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		bool pointFlag3 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1111)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else if (pointFlag3 == 0 && pIndex / 1000 == 1111)
				{
					PointItem3 = *it;
					pointFlag3 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3][3] = {};

		getPointPositionFromTreeItem(PointItem1,points[0]);
		getPointPositionFromTreeItem(PointItem2,points[1]);
		getPointPositionFromTreeItem(PointItem3,points[2]);

		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = points[0][i] - points[1][i];
			v2[i] = points[2][i] - points[1][i];
		}
		double angle = acos(vtkMath::Dot(v1,v2)/(vtkMath::Norm(v1) * vtkMath::Norm(v2)));
		//double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		

		QString text = QString("angle: ").
			append(QString::number(angle));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}

void MainWindow::itoothAngle1()
{
	if (selectOptionTreeIndex == 1122)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItem *PointItem3;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		bool pointFlag3 = 0;
		bool pointFlag4 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1111)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else if (pointFlag3 == 0 && pIndex / 1000 == 1111)
				{
					PointItem3 = *it;
					pointFlag3 = 1;
				}else if (pointFlag4 == 0 && pIndex / 1000 == 1111)
				{
					PointItem3 = *it;
					pointFlag3 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3][3] = {};

		getPointPositionFromTreeItem(PointItem1,points[0]);
		getPointPositionFromTreeItem(PointItem2,points[1]);
		getPointPositionFromTreeItem(PointItem3,points[2]);

		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = points[0][i] - points[1][i];
			v2[i] = points[2][i] - points[1][i];
		}
		double angle = acos(vtkMath::Dot(v1,v2)/(vtkMath::Norm(v1) * vtkMath::Norm(v2)));
		
		angle = angle * 0.7;
		//double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		
		cout<<"倾角"<<angle<<endl;
		QString text = QString("angle: ").
			append(QString::number(angle));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}

void MainWindow::itoothAngle2()
{
	if (selectOptionTreeIndex == 1123)
	{
		QTreeWidgetItem *PointItem1;
		QTreeWidgetItem *PointItem2;
		QTreeWidgetItem *PointItem3;
		QTreeWidgetItemIterator it(ui.optionTreeWidget);
		bool errFlag = 0;
		bool pointFlag1 = 0;
		bool pointFlag2 = 0;
		bool pointFlag3 = 0;
		while (*it) {
			//do something like
			if ((*it)->checkState(0) == Qt::Checked)
			{
				QVariant dt = (*it)->data(0, Qt::UserRole);
				int pIndex = getSelectOptionTreeIndex(dt);
				if (pointFlag1 == 0 && pIndex / 1000 == 1111)
				{
					PointItem1 = *it;
					pointFlag1 = 1;
				}else if (pointFlag2 == 0 && pIndex / 1000 == 1111)
				{
					PointItem2 = *it;
					pointFlag2 = 1;
				}else if (pointFlag3 == 0 && pIndex / 1000 == 1111)
				{
					PointItem3 = *it;
					pointFlag3 = 1;
				}else
				{
					QMessageBox::information(NULL, "Error", "please Only choose point and plane", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					errFlag = 1;
					break;
				}
			}
			++it;
		}

		// 		if (Itembox.size() != 3)
		// 		{
		// 			QMessageBox::information(NULL, "Error", "Not enough points", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		// 			errFlag = 1;
		// 		}
		if (errFlag == 1)
		{
			return;
		}
		double points[3][3] = {};

		getPointPositionFromTreeItem(PointItem1,points[0]);
		getPointPositionFromTreeItem(PointItem2,points[1]);
		getPointPositionFromTreeItem(PointItem3,points[2]);

		double v1[3],v2[3],v3[3];
		for (int i = 0;i < 3;i ++)
		{
			v1[i] = points[0][i] - points[1][i];
			v2[i] = points[2][i] - points[1][i];
		}
		double angle = acos(vtkMath::Dot(v1,v2)/(vtkMath::Norm(v1) * vtkMath::Norm(v2)));
		angle = 3.14-angle;
		//double dis = (vtkMath::Norm(v3))/(vtkMath::Norm(v2));		

		QString text = QString("angle: ").
			append(QString::number(angle));
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);
		//drawPoint(point1,pointColor,SegPointActor[pickerCount]);

		ScanTree();

	}
}

void MainWindow::init()
{
	for(int i = 0; i < 20; ++i)
	{
		fitPlanePointStat[i] = false;
		fitPlanePointActor[i] = vtkSmartPointer<vtkActor>::New();
	}

	fitPlaneActor		= vtkSmartPointer<vtkActor>::New();
	fitPlaneFrameActor	= vtkSmartPointer<vtkActor>::New();

	selectOptionTreeIndex			= 0;
	currentOptionTreeWidgetItem		= NULL;

	isLowToothShow			= isTopToothShow = 0;
	isFitPlaneShow			= 0;
	isFitPlanePointShow		= 0;

	isCurrentSelectPointShow = 0;
	currentSelectPointActor  = vtkSmartPointer<vtkActor>::New();

	for (int i = 0;i < 300;i ++)
	{
		SegPointActor[i] = vtkSmartPointer<vtkActor>::New();
		PickPointState[i] = 0;
		
	}
	SegPointMap.clear();
	//平面
	for (int i = 0;i < 300;i ++)
	{
		PlaneState[i] = 0;
	}
	PlaneNum = 0;
	PlaneMapSet.clear();
	//投影点
	for (int i = 0;i < 300;i ++)
	{
		projPointActor[i] = vtkSmartPointer<vtkActor>::New();
		projPointState[i] = 0;

	}
	projPointMapSet.clear();


	//切割曲线
	cutCurveCount = 0;
	cutCurveMapSet.clear();
	
	//向量
	VectorCount = 0;
	VecMapSet.clear();




	//前牙区不齐
	for(int i = 0; i < 10; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			gapPointStat[i][j]			= false;
			gapPointActor[i][j]			= vtkSmartPointer<vtkActor>::New();
			gapProjectPointActor[i][j]	= vtkSmartPointer<vtkActor>::New();
		}
	}
	isGapPointShow			= 0;
	isGapProjectPointShow	= 0;

	pointInVerticalPlaneIndex = -1;
	for(int i = 0; i < 2; ++i)
	{
		pointInVerticalPlaneStat[i] = false;
		pointInVerticalPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	isPointInVerticalPlaneShow		= isVerticalPlaneShow = 0;
	verticalPlaneActor				= vtkSmartPointer<vtkActor>::New();
	verticalWireFramePlaneActor		= vtkSmartPointer<vtkActor>::New();

	topToothPointForVerticalPlaneIndex = 0;
	lowToothPointForVerticalPlaneIndex = 0;
	//颊侧区咬合
	for(int i = 0; i < 16; ++i)
	{
		topToothPointForVerticalPlaneActor[i]			= vtkSmartPointer<vtkActor>::New();
		topToothProjectPointForVerticalPlaneActor[i]	= vtkSmartPointer<vtkActor>::New();

		topToothPointForFitPlaneStat[i] = false;
		topToothPointForFitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
		topToothProjectPointForFitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();

// 		lowToothPointForVerticalPlaneStat[i] = false;
// 		lowToothPointForVerticalPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
// 		lowToothProjectPointForVerticalPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
// 
// 		lowToothPointForFitPlaneStat[i] = false;
// 		lowToothPointForFitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
// 		lowToothProjectPointForFitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	isTopToothPointForVerticalPlaneShow = isTopToothProjectPointForVerticalPlane = 0;
	isTopToothPointForFitPlaneShow = isTopToothProjectPointForFitPlaneShow = 0;
	isLowToothPointForVerticalPlaneShow = isLowToothProjectPointForVerticalPlaneShow = 0;
	isLowToothPointForFitPlaneShow = isLowToothProjectPointForFitPlaneShow = 0;

	verticalParallelPlaneStat[0] = verticalParallelPlaneStat[1] = false;

	verticalParallelInitPointActor = vtkSmartPointer<vtkActor>::New();
	isVerticalParallelPlaneInitPointShow = 0;
	verticalparallelPlaneActor = vtkSmartPointer<vtkActor>::New();
	verticalParallelWireFramePlaneActor = vtkSmartPointer<vtkActor>::New();
	isVerticalParallelPlaneShow = 0;

	verticalParallelInitPoint2Actor = vtkSmartPointer<vtkActor>::New();
	isVerticalParallelPlaneInitPoint2Show = 0;
	verticalparallelPlane2Actor = vtkSmartPointer<vtkActor>::New();
	verticalParallelWireFramePlane2Actor = vtkSmartPointer<vtkActor>::New();
	isVerticalParallelPlane2Show = 0;


	//
	// 颊侧区咬合 测量部分
	//
	JCoverPointActor = vtkSmartPointer<vtkActor>::New();
	JCoverProjectPointActor = vtkSmartPointer<vtkActor>::New();
	isJCoverPointShow = 0;
	JCoverCenterPointActor = vtkSmartPointer<vtkActor>::New();
	JCoverPlaneOActor = vtkSmartPointer<vtkActor>::New();
	JCoverPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isJCoverPlaneOShow = 0;
	JCoverIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	TopJCoverIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	isParJCoverIntersectCurveShow = 0;
	for(int i = 0; i < 2; ++i)
	{
		JCoverIntersectCurvePointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	JCoverIntersectCurvePointIndex = 0;
	isJCoverIntersectCurvePointShow = 0;

	//
	// 覆盖
	//
	CoverPointActor = vtkSmartPointer<vtkActor>::New();
	CoverProjectPointActor = vtkSmartPointer<vtkActor>::New();
	isCoverPointShow = 0;
	CoverCenterPointActor = vtkSmartPointer<vtkActor>::New();
	CoverPlaneOActor = vtkSmartPointer<vtkActor>::New();
	CoverPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isCoverPlaneOShow = 0;
	CoverIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	TopCoverIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	isParCoverIntersectCurveShow = 0;
	for(int i = 0; i < 2; ++i)
	{
		CoverIntersectCurvePointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	CoverIntersectCurvePointIndex = 0;
	isCoverIntersectCurvePointShow = 0;
	
	indexOfSplinePoint = 0;
	for(int i = 0; i < 20; ++i)
	{
		splinePointState[i] = false;

		splinePointActor[i]				= vtkSmartPointer<vtkActor>::New();
		splineProjectPointActor[i]		= vtkSmartPointer<vtkActor>::New();

		intersectionPointsStat[i]		= false;
		intersectionPointsAction[i]		= vtkSmartPointer<vtkActor>::New();
	}
	isSplinePointShow = isSplineProjectPointShow = 0;

	for(int i = 0; i < 40; ++i)
	{
		fittingCurveActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	
	isPointA0Show = 0;
	isPlaneOShow = 0;
	isFittingCurveShow = 0;

	pointA0Actor					= vtkSmartPointer<vtkActor>::New();
	projectPointA0Actor				= vtkSmartPointer<vtkActor>::New();
	pointAActor						= vtkSmartPointer<vtkActor>::New();
	planeOActor						= vtkSmartPointer<vtkActor>::New();
	planeOWireframeActor			= vtkSmartPointer<vtkActor>::New();
	planePActor						= vtkSmartPointer<vtkActor>::New();
	planePWireframeActor			= vtkSmartPointer<vtkActor>::New();
	coverPOIntersectLineActor		= vtkSmartPointer<vtkActor>::New();
	coverSelectIntersectPointActor	= vtkSmartPointer<vtkActor>::New();
	isPlanePShow					= 0;
	isCoverPOIntersectLineShow		= 0;
	isIntersectionPointsShow		= 0;
	coverSelectIntersectPointIndex	= -1;
	

	//
	// 反合覆盖
	//
	for(int i = 0; i < 16; ++i)
	{
		inverseFitCoverPointStat[i] = false;

		inverseFitCoverPointActor[i] = vtkSmartPointer<vtkActor>::New();
		inverseFitCoverProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	for(int i = 0; i < 8; ++i)
	{
		inverseFitCoverProjectLineActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	isInverseFitCoverPointShow = 0;

	//
	// 覆合
	//
	pointForOverbitParallelPlaneActor = vtkSmartPointer<vtkActor>::New();
	overbiteParallelPlaneActor = vtkSmartPointer<vtkActor>::New();
	overbiteParallelPlaneWireFrameActor = vtkSmartPointer<vtkActor>::New();
	overbiteIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	topOverbiteIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	isPointInOverbiteParallelPlaneShow = isOverbiteParrelPlaneShow = isOverbiteIntersectCurveShow = 0;

	//
	// 开合
	//
	pointForArticleActor = vtkSmartPointer<vtkActor>::New();
	articlePlaneActor = vtkSmartPointer<vtkActor>::New();
	articalPlaneWireframeActor = vtkSmartPointer<vtkActor>::New();
	isPointForArticlePlaneShow = isArticlePlaneShow = 0;

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			topPointToProjectArticlePlaneStat[i][j] = false;
			topPointToProjectArticlePlaneActor[i][j] = vtkSmartPointer<vtkActor>::New();

			lowPointToProjectArticlePlaneStat[i][j] = false;
			lowPointToProjectArticlePlaneActor[i][j] = vtkSmartPointer<vtkActor>::New();

			topPointProjectArticlePlaneActor[i][j] = vtkSmartPointer<vtkActor>::New();
			lowPointProjectArticlePlaneActor[i][j] = vtkSmartPointer<vtkActor>::New();
		}

		topLineProjectArticlePlaneActor[i] = vtkSmartPointer<vtkActor>::New();
		lowLineProjectArticlePlaneActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	isTopPointToProjectArticlePlaneShow = isLowPointToProjectArticlePlaneShow = 0;
	isProjectArticlePlane = 0;

	for(int i = 0; i < 20; ++i)
	{
		frontToothOpenCloseFittingPointStat[i] = false;
		backToothOpenCloseFittingPointStat[i] = false;

		frontToothOpenCloseFittingPointActor[i] = vtkSmartPointer<vtkActor>::New();
		frontToothOpenCloseFittingProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
		backToothOpenCloseFittingPointActor[i] = vtkSmartPointer<vtkActor>::New();
		backToothOpenCloseFittingProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	for(int i = 0; i < 40; ++i)
	{
		frontToothOpenCloseFittingCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		backToothOpenCloseFittingCurveActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	isFrontToothOpenCloseFittingPointShow = isFrontToothOpenCloseFittingCurveShow = 0;
	isBackToothOpenCloseFittingPointShow = isBackToothOpenCloseFittingCurveShow = 0;

	frontToothToProjectPointA0Actor = vtkSmartPointer<vtkActor>::New();
	frontToothProjectPointA0Actor = vtkSmartPointer<vtkActor>::New();
	idFrontToothToProjectPointPositionA0Show = 0;

	frontToothPlaneCenterActor = vtkSmartPointer<vtkActor>::New();
	frontToothPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	frontToothPlaneOActor = vtkSmartPointer<vtkActor>::New();
	backToothPointA0Actor = vtkSmartPointer<vtkActor>::New();
	backToothProjectPointA0Actor = vtkSmartPointer<vtkActor>::New();
	backToothPlaneCenterActor = vtkSmartPointer<vtkActor>::New();
	backToothPlaneOActor = vtkSmartPointer<vtkActor>::New();
	backToothPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isFrontToothPlaneOShow = 0;
	isBackToothPointA0Show = isBackToothPlaneOShow = 0;
	frontToothContourCurveActor = vtkSmartPointer<vtkActor>::New();
	frontToothTopContourCurveActor = vtkSmartPointer<vtkActor>::New();
	isFrontToothContourCurveShaow = 0;
	backToothContourCurveActor = vtkSmartPointer<vtkActor>::New();
	backToothTopContourCurveActor = vtkSmartPointer<vtkActor>::New();
	isBackToothContourCurveShaow = 0;
	
	for(int i = 0; i < 2; ++i)
	{
		frontToothContourCurvePointActor[i] = vtkSmartPointer<vtkActor>::New();
		backToothContourCurvePointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	frontToothContourPointIndex = 0;
	isFrontToothContiourCurvePointShow = 0;
	backToothContourPointIndex = 0;
	isBackToothContiourCurvePointShow = 0;

	indexOfTwoSelectPointOnLowerModel = 0;
	for(int i = 0; i < 2; ++i)
	{
		towPointToProjectFitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
		towPointProjectFitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	isTwoPointToProjectFitPlaneShow = isTwoPointProjectFitPlaneShow = 0;

	//
	// DI覆盖
	//
	/*
	diPointA0Actor = vtkSmartPointer<vtkActor>::New();
	diProjectPointA0Actor = vtkSmartPointer<vtkActor>::New();
	isDiPointA0Show = 0;
	diPointAActor = vtkSmartPointer<vtkActor>::New();
	diPlaneOActor = vtkSmartPointer<vtkActor>::New();
	diPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isDiPlaneOShow = 0;
	diPlanePActor = vtkSmartPointer<vtkActor>::New();
	diPlanePWireframeActor = vtkSmartPointer<vtkActor>::New();
	isDiPlanePShow = 0;
	diCoverIntersectLineActor = vtkSmartPointer<vtkActor>::New();
	isDiCoverIntersectLineShow = 0;
	diCoverIntersectPointActor = vtkSmartPointer<vtkActor>::New();
	*/

	diCoverPointActor = vtkSmartPointer<vtkActor>::New();
	diCoverProjectPointActor = vtkSmartPointer<vtkActor>::New();
	isDiCoverPointShow = 0;
	diCoverCenterPointActor = vtkSmartPointer<vtkActor>::New();
	diCoverPlaneOActor = vtkSmartPointer<vtkActor>::New();
	diCoverPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isDiCoverPlaneOShow = 0;
	diCoverIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	diTopCoverIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	isDiCoverIntersectCurveShow = 0;
	for(int i = 0; i < 2; ++i)
	{
		diCoverIntersectCurvePointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	diCoverIntersectCurvePointIndex = 0;
	isDiCoverIntersectCurvePointShow = 0;
	//
	// DI覆合
	//

	diOverbitePointActor = vtkSmartPointer<vtkActor>::New();
	diOverbiteProjectPointActor = vtkSmartPointer<vtkActor>::New();
	isDiOverbitePointShow = 0;
	diOverbiteCenterPointActor = vtkSmartPointer<vtkActor>::New();
	diOverbitePlaneOActor = vtkSmartPointer<vtkActor>::New();
	diOverbitePlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isDiOverbitePlaneOShow = 0;
	diOverbiteIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	diTopOverbiteIntersectCurveActor = vtkSmartPointer<vtkActor>::New();
	isDiOverbiteIntersectCurveShow = 0;
	for(int i = 0; i < 2; ++i)
	{
		diOverbiteIntersectCurvePointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	diOverbiteIntersectCurvePointIndex = 0;
	isDiOverbiteIntersectCurvePointShow = 0;

	//
	// DI前牙开合
	//

	for(int i = 0; i < 20; ++i)
	{
		diFrontOpenClosePointStat[i] = false;
		diFrontOpenClosePointActor[i] = vtkSmartPointer<vtkActor>::New();
		diFrontOpenCloseProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
		isDiFrontOpenClosePointShow[i] = 0;

		diFrontOpenCloseCenterPointActor[i] = vtkSmartPointer<vtkActor>::New();
		diFrontOpenClosePlaneOActor[i] = vtkSmartPointer<vtkActor>::New();
		diFrontOpenClosePlaneOWireframeActor[i] = vtkSmartPointer<vtkActor>::New();
		isDiFrontOpenClosePlaneOShow[i] = 0;

		diFrontOpenCloseIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		diTopFrontOpenCloseIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		isDiFrontOpenCloseIntersectCurveShow[i] = 0;

		diFrontOpenCloseIntersectCurvePointIndex[i] = 0;
		diFrontOpenCloseIntersectCurvePointActor[i][0] = vtkSmartPointer<vtkActor>::New();
		diFrontOpenCloseIntersectCurvePointActor[i][1] = vtkSmartPointer<vtkActor>::New();
		diFrontOpenCloseDistance[i] = 0;
		diFrontOpenCloseMeasureScore[i] = 0;
		isDiFrontOpenCloseIntersectCurvePointShow[i] = 0;
	}
	for (int i = 0;i < 6;i++)
	{
		diFrontOpenCloseFlag[i] = 0;
	}

	//
	// DI侧方开合
	//

	for(int i = 0; i < 10; ++i)
	{
		diSideOpenClosePointActor[i] = vtkSmartPointer<vtkActor>::New();
		diSideOpenCloseProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
		isDiSideOpenClosePointShow[i] = 0;

		diSideOpenCloseCenterPointActor[i] = vtkSmartPointer<vtkActor>::New();
		diSideOpenClosePlaneOActor[i] = vtkSmartPointer<vtkActor>::New();
		diSideOpenClosePlaneOWireframeActor[i] = vtkSmartPointer<vtkActor>::New();
		isDiSideOpenClosePlaneOShow[i] = 0;

		diSideOpenCloseIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		diTopSideOpenCloseIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		isDiSideOpenCloseIntersectCurveShow[i] = 0;

		for(int j = 0; j < 2; ++j)
		{
			diSideOpenCloseIntersectCurvePointActor[i][j] = vtkSmartPointer<vtkActor>::New();
		}
		diSideOpenCloseGrindToothScore[i] = 0;
		diSideOpenCloseIntersectCurvePointIndex[i] = 0;
		isDiSideOpenCloseIntersectCurvePointShow[i] = 0;
	}
	for (int i = 0;i < 8;i++)
	{
		diSideOpenClosFlag[i] = 0;
	}
	//
	// DI拥挤度
	//

	for(int i = 0; i < 12; ++i)
	{
		diGrindToothScrowdPointIndex[i] = 0;
		diGrindToothScrowdPointState[i] = false;
		diGrindToothScrowdPointActor[i * 2] = vtkSmartPointer<vtkActor>::New();
		diGrindToothScrowdPointActor[i * 2 + 1] = vtkSmartPointer<vtkActor>::New();
		diGrindToothScrowdDistance[i] = 0;
		isDiGrindToothScrowdToothShow[i] = 0;
	}	
	diGrindToothScrowdSumDistance = 0;

	for(int i = 0; i < 4; ++i)
	{
		diToothBowScrowdPointIndex[i] = 0;
		diToothBowScrowdPointState[i] = false;
		diToothBowScrowdPointActor[i * 2] = vtkSmartPointer<vtkActor>::New();
		diToothBowScrowdPointActor[i * 2 + 1] = vtkSmartPointer<vtkActor>::New();
		isDiToothBowScrowdShow[i] = 0;
		diToothBowScrowdDistance[i] = 0;
	}
	diToothBowScrowdSumDistance = 0;

	diScrowdDistance = 0;

	//
	// DI合关系
	//

	diCloseRelationPointAActor = vtkSmartPointer<vtkActor>::New();
	diCloseRelationPointBActor = vtkSmartPointer<vtkActor>::New();
	isDiCloseRelationPointAShow = isDiCloseRelationPointBShow = 0;

	diCloseRelationProjectPointBActor = vtkSmartPointer<vtkActor>::New();
	diCloseRelationPlanePointAActor = vtkSmartPointer<vtkActor>::New();
	diCloseRelationPlaneOActor = vtkSmartPointer<vtkActor>::New();
	diCloseRelationPlaneOWireframeActor = vtkSmartPointer<vtkActor>::New();
	isDiCloseRelationPlaneOShow = 0;

	//
	// DI后牙反合
	//

	for(int i = 0; i < 16; ++i)
	{
		diBackToothOppositeClosePointActor[i] = vtkSmartPointer<vtkActor>::New();
		diBackToothOppositeCloseProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
		diBackToothOppositeClosePointState[i] = false;
	}
	isDiBackToothOppositeClosePointShow = isDiBackToothOppositeCloseProjectPointShow = 0;

	//
	// DI后牙正锁合
	//

	for(int i = 0; i < 10; ++i)
	{
		diBackToothLockClosePointActor[i] = vtkSmartPointer<vtkActor>::New();
		diBackToothLockCloseProjectPointActor[i] = vtkSmartPointer<vtkActor>::New();
		diBackToothLockClosePointState[i] = false;
	}
	isDiBackToothLockClosePointShow = isDiBackToothLockCloseProjectPointShow = 0;

	//
	// DI其他->中线不调
	//

	for(int i = 0; i < 2; ++i)
	{
		diMiddleUnfitPointActor[i] = vtkSmartPointer<vtkActor>::New();
		diMiddleUnfitPlaneActor[i] = vtkSmartPointer<vtkActor>::New();
		diMiddleUnfitPlaneWireframeActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	diMiddleUnfitPointIndex = 0;
	diMiddleUnfitPointShow = 0;

	//
	// DI其他->中切牙牙间隙
	//

	for(int i = 0; i < 2; ++i)
	{
		diMiddleCutToothGapPointActor[i] = vtkSmartPointer<vtkActor>::New();
	}
	diMiddleCutToothGapIndex = 0;
	isDiMiddleCutToothGapShow = 0;


	//
	// ABO-OGS排列
	//

	for(int i = 0; i < 26; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			aboArrangePointProjectToPlaneOAcotr[i][j] = vtkSmartPointer<vtkActor>::New();
			aboArrangePointProjectToFitPlane[i][j] = vtkSmartPointer<vtkActor>::New();

			isAboArrangePointShow[i][j] = 0;
			isAboArrangeProjectPointShow[i][j] = 0;
		}

		aboArrangePointPareScore[i] = 0;
		aboArrangePointPickNum[i] = 0;

		aboArrangePlaneOActor[i] = vtkSmartPointer<vtkActor>::New();
		aboArrangePlaneOWireframeActor[i] = vtkSmartPointer<vtkActor>::New();
		isAboArrangePlaneOShow[i] = 0;
	}

	aboArrangeScore = 0;

	//
	// ABO-OGS边缘嵴高度
	//

	for(int i = 0; i < 20; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboEdgeVertebralHeighPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			aboEdgeVertebralHeighPointProjectToMiddlePlaneAcotr[i][j] = vtkSmartPointer<vtkActor>::New();
			aboEdgeVertebralHeighPointProjectToArticlePlaneActor[i][j] = vtkSmartPointer<vtkActor>::New();

			isAboEdgeVertebralHeighPointShow[i][j] = 0;
			isAboEdgeVertebralHeighProjectPointShow[i][j] = 0;
		}

		aboEdgeVertebralHeighPointPickNum[i] = 0;
		aboEdgeVertebralHeighPointPareDistance[i] = 0;
		aboEdgeVertebralHeighPointPareScore[i] = 0;
	}
	aboEdgeVertebralHeighScore = 0;


	//
	// ABO-OGS颊舌向倾斜度
	//

	aboChapFitPlaneActor = vtkSmartPointer<vtkActor>::New();
	aboChapFitPlaneWireframeActor = vtkSmartPointer<vtkActor>::New();
	isChapFitPlaneShow = 0;

	for(int i = 0; i < 14; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboChapPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			aboChapProjectPointAcotr[i][j] = vtkSmartPointer<vtkActor>::New();

			isChapPointShow[i][j] = 0;
			isChapProjectPointShow[i][j] = 0;
		}

		aboChapPointPickNum[i] = 0;
		aboChapPointPareDistance[i] = 0;
		aboChapPointPareScore[i] = 0;
	}
	aboChapScore = 0;

	//
	// ABO-OGS牙合关系
	//

	for(int i = 0; i < 10; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboToothCloseRelationPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();

			isAboToothCloseRelationPointShow[i][j] = 0;
		}

		aboToothCloseRelationPointPickNum[i] = 0;
		aboToothCloseRelationPointPlaneDistance[i] = 0;
		aboToothCloseRelationMeasureScore[i] = 0;

		aboToothCloseRelationPlaneOActor[i] = vtkSmartPointer<vtkActor>::New();
		aboToothCloseRelationPlaneOWireframeActor[i] = vtkSmartPointer<vtkActor>::New();

		isAboToothCloseRelationPlaneOShow[i] = 0;
	}
	aboToothCloseRelationScore = 0;

	//
	// ABO-OGS牙合接触
	//

	for(int i = 0; i < 20; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboToothCloseTouchPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			isAboToothCloseTouchPointShow[i][j] = 0;
		}

		aboToothCloseTouchMeasureDistance[i] = 0;
		aboToothCloseTouchMeasureScore[i] = 0;

		aboToothCloseTouchPlaneOActor[i] = vtkSmartPointer<vtkActor>::New();
		aboToothCloseTouchPlaneOWireframeActor[i] = vtkSmartPointer<vtkActor>::New();
		isAboToothCloseTouchPlaneOShow[i] = 0;

		aboToothCloseTouchTopIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		aboToothCloseTouchLowIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		isAboToothCloseTouchIntersectCurveShow[i] = 0;
	}
	aboToothCloseTouchScore = 0;

	//
	// ABO-OGS覆盖
	//

	for(int i = 0; i < 18; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboCoverAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			isAboCoverPointShow[i][j] = 0;

			isCoverIntersectCurveShow[i][j] = 0;
		}

		aboTCoverMeasureDistance[i] = 0;
		aboCoverMeasureScore[i] = 0;

		aboCoverPlaneOActor[i] = vtkSmartPointer<vtkActor>::New();
		aboCoverPlaneOWireframeActor[i] = vtkSmartPointer<vtkActor>::New();
		isAboCoverPlaneOShow[i] = 0;

		aboCoverIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();
		aboCoverLowIntersectCurveActor[i] = vtkSmartPointer<vtkActor>::New();		
	}
	aboCoverScore = 0;

	//
	// ABO-OGS邻牙接触关系
	//

	for(int i = 0; i < 26; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			aboTouchRelationPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			aboTouchRelationProjectPointAcotor[i][j] = vtkSmartPointer<vtkActor>::New();
			isAboTouchRelationPointShow[i][j] = 0;
			isAboTouchRelationProjectPointShow[i][j] = 0;
		}

		aboTouchRelationPointNum[i] = 0;

		aboTouchRelationMeasureDistance[i] = 0;
		aboTouchRelationMeasureScore[i] = 0;
	}
	aboTouchRelationScore = 0;

	//
	// ABO-OGS得分
	//

	aboScore = 0;
}

void MainWindow::drawIntersectCurve(vtkPolyData *data, double *planeCenter, double *planeNormal, double *color, double width, vtkSmartPointer<vtkActor> actor)
{
	width = 6;////////////////////////////
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(planeCenter);
	plane->SetNormal(planeNormal);

	vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetCutFunction(plane);
	cutter->SetInput(data);
	cutter->Update();

	vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cutterMapper->SetInputConnection(cutter->GetOutputPort());

	actor->GetProperty()->SetColor(color);
	actor->GetProperty()->SetLineWidth(width);
	actor->SetMapper(cutterMapper);

	this->ren->AddActor(actor);
	this->ui.qvtkObjectViewerWidget->update();	
}

void MainWindow::initPlaneByFittingCurve(double curvePlaneCenter[3], double curvePlaneNormal[3], 
		double (*fittingPoint)[3], bool *fittingPointStat, int fittingPointNum,
		double planePoint[3],
		int planeSize,
		double planeCenter[3], double planeNrmal[3], double planePoints[3][3])
{
	mxArray *fromMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
	memcpy((void *)mxGetPr(fromMxArray), (void *)curvePlaneNormal, sizeof(curvePlaneNormal));
	engPutVariable(ep, "fromVec", fromMxArray);

	double to[3] = {0, 0, 1};
	mxArray *toMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
	memcpy((void *)mxGetPr(toMxArray), (void *)to, sizeof(to));
	engPutVariable(ep, "toVec", toMxArray);

	engEvalString(ep, "r = vrrotvec(fromVec, toVec);");
	engEvalString(ep, "m = vrrotvec2mat(r);");

	int pointNum = 0;
	double firstPoint[3]; //, firstIndex = 0;
	double points[20][3];
	engEvalString(ep, "aa = [];");
	for(int i = 0; i < fittingPointNum; ++i)
	{
		if(fittingPointStat[i])
		{
			for(int j = 0; j < 3; ++j)
			{
				if(pointNum == 0)
				{
					firstPoint[j] = fittingPoint[i][j];
					//firstIndex = j;
				}

				points[i][j] = fittingPoint[i][j] - firstPoint[j];
			}
			++pointNum;

			mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
			memcpy((void *)mxGetPr(tempArray), (void *)points[i], sizeof(points[i]));
			engPutVariable(ep, "tempVec", tempArray);

			engEvalString(ep, "b = tempVec * m';");
			engEvalString(ep, "aa = [aa; b];");

			mxDestroyArray(tempArray);
		}
	}

	double tranProjectA0[3];
	tranProjectA0[0] = planePoint[0] - firstPoint[0];
	tranProjectA0[1] = planePoint[1] - firstPoint[1];
	tranProjectA0[2] = planePoint[2] - firstPoint[2];
	mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
	memcpy((void *)mxGetPr(tempArray), (void *)tranProjectA0, sizeof(tranProjectA0));
	engPutVariable(ep, "ppTemp", tempArray);
	engEvalString(ep, "pp = ppTemp * m';");

	engEvalString(ep, "m1 = rotz(atand(aa(length(aa), 2) / aa(length(aa), 1)));");
	engEvalString(ep, "bb =aa * m1;");
	engEvalString(ep, "pp = pp * m1;");
	engEvalString(ep, "p = polyfit(bb(:,1),bb(:,2),5);");
	engEvalString(ep, "options = optimoptions('fminunc','Display','off');");
	engEvalString(ep, "x0 = 0;");

	QString pathString = QString("path(path, '").append(this->path).append("');");
	engEvalString(ep, qPrintable(pathString));
	engEvalString(ep, "[xmin, fval] = fminunc(@myfun,x0,options,pp,p);");
	engEvalString(ep, "y = p(1)*xmin^5 + p(2)*xmin^4 + p(3)*xmin^3 + p(4)*xmin^2 + p(5)*xmin + p(6);");
	engEvalString(ep, "temResult = [xmin, y, pp(3)];");
	engEvalString(ep, "result = temResult * ((m1)^-1);");
	engEvalString(ep, "result = result * ((m')^-1);");

	mxArray *resultMxArray = engGetVariable(ep, "result");
	double *result = mxGetPr(resultMxArray);
	planeCenter[0] = result[0] + firstPoint[0];
	planeCenter[1] = result[1] + firstPoint[1];
	planeCenter[2] = result[2] + firstPoint[2];

	double lineDirectionInOP[3];
	lineDirectionInOP[0] = planeCenter[0] - planePoint[0];
	lineDirectionInOP[1] = planeCenter[1] - planePoint[1];
	lineDirectionInOP[2] = planeCenter[2] - planePoint[2];
	vtkMath::Normalize(lineDirectionInOP);

	double a0[3];
	a0[0] = planeCenter[0] + planeSize * lineDirectionInOP[0];
	a0[1] = planeCenter[1] + planeSize * lineDirectionInOP[1];
	a0[2] = planeCenter[2] + planeSize * lineDirectionInOP[2];

	double a1[3];
	a1[0] = planeCenter[0] - planeSize * lineDirectionInOP[0];
	a1[1] = planeCenter[1] - planeSize * lineDirectionInOP[1];
	a1[2] = planeCenter[2] - planeSize * lineDirectionInOP[2];

	double a2[3];
	a2[0] = planeCenter[0] + planeSize * curvePlaneNormal[0];
	a2[1] = planeCenter[1] + planeSize * curvePlaneNormal[1];
	a2[2] = planeCenter[2] + planeSize * curvePlaneNormal[2];

	double a3[3];
	a3[0] = planeCenter[0] - planeSize * curvePlaneNormal[0];
	a3[1] = planeCenter[1] - planeSize * curvePlaneNormal[1];
	a3[2] = planeCenter[2] - planeSize * curvePlaneNormal[2];

	planePoints[0][0] = a1[0] + planeSize * curvePlaneNormal[0];
	planePoints[0][1] = a1[1] + planeSize * curvePlaneNormal[1];
	planePoints[0][2] = a1[2] + planeSize * curvePlaneNormal[2];
	planePoints[1][0] = a1[0] - planeSize * curvePlaneNormal[0];
	planePoints[1][1] = a1[1] - planeSize * curvePlaneNormal[1];
	planePoints[1][2] = a1[2] - planeSize * curvePlaneNormal[2];
	planePoints[2][0] = a0[0] + planeSize * curvePlaneNormal[0];
	planePoints[2][1] = a0[1] + planeSize * curvePlaneNormal[1];
	planePoints[2][2] = a0[2] + planeSize * curvePlaneNormal[2];

	double lineOADirect[3];
	lineOADirect[0] = planeCenter[0] - planePoint[0];
	lineOADirect[1] = planeCenter[1] - planePoint[1];
	lineOADirect[2] = planeCenter[2] - planePoint[2];
	vtkMath::Cross(curvePlaneNormal, lineOADirect, planeNrmal);
	vtkMath::Normalize(planeNrmal);
}

void MainWindow::initFitPlane()
{
	if(isCurrentSelectPointShow == 1)
	{
		switchCurrentSelectPointShow();
	}

	vtkSmartPointer<vtkRANSACPlane> rANSACPlane = vtkSmartPointer<vtkRANSACPlane>::New();
	rANSACPlane->SetInlierThreshold(0.01);
	rANSACPlane->SetMaxIterations(1000);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();

	vtkBoundingBox boundingBox;
	for(int i = 0; i < 20; ++i)
	{
		if(fitPlanePointStat[i])
		{
			vtkIdType pid[1];
			pid[0] = points->InsertNextPoint(fitPlanePointPosition[i][0], 
				fitPlanePointPosition[i][1], fitPlanePointPosition[i][2]);

			vertices->InsertNextCell(1, pid );
			boundingBox.AddPoint(fitPlanePointPosition[i]);
		}
	}

	vtkSmartPointer<vtkPlane> bestPlane = vtkSmartPointer<vtkPlane>::New();
	BestFitPlane(points, bestPlane);
	for(int i = 0; i < 3; ++i)
	{
		this->fitPlaneCenter[i] = bestPlane->GetOrigin()[i];
		this->fitPlaneNormal[i] = bestPlane->GetNormal()[i];
	}

	double fitPointBound[6];
	boundingBox.GetBounds(fitPointBound);
	double fitPointBoundLength[3] = {
		fitPointBound[1] -fitPointBound[0],
		fitPointBound[3] -fitPointBound[2],
		fitPointBound[5] -fitPointBound[4]
	};
	int minLengthIndex = 0;
	if(fitPointBoundLength[0] < fitPointBoundLength[1] && fitPointBoundLength[0] < fitPointBoundLength[2])
		minLengthIndex = 0;
	else if(fitPointBoundLength[1] < fitPointBoundLength[0] && fitPointBoundLength[1] < fitPointBoundLength[2])
		minLengthIndex = 1;
	else
		minLengthIndex = 2;
	//QMessageBox::information(NULL, tr("Info"), QString::number(minLengthIndex));

	double origin[3];
	double direction1[3], direction2[3];
	cout<<"minLengthIndex : "<<minLengthIndex<<endl;
	if(minLengthIndex == 0)
	{
		//
		// 合平面几乎平行于yoz面
		//

		double pointToProject[3];

		// 原点
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];
		bestPlane->ProjectPoint(pointToProject, origin);
		fitPlanePoint[0][0] = origin[0];
		fitPlanePoint[0][1] = origin[1];
		fitPlanePoint[0][2] = origin[2];

		// 轴一的方向
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[5];	
		bestPlane->ProjectPoint(pointToProject, direction1);
		direction1[0] = direction1[0] - origin[0];
		direction1[1] = direction1[1] - origin[1];
		direction1[2] = direction1[2] - origin[2];
		vtkMath::Normalize(direction1);

		// 轴二的方向
		vtkMath::Cross(fitPlaneNormal, direction1, direction2);
		vtkMath::Normalize(direction2);
		if(direction2[1] < 0)
		{
			direction2[0] = -direction2[0];
			direction2[1] = -direction2[1];
			direction2[2] = -direction2[2];
		}
		double point1[3], point2[3];
		point1[0] = origin[0] + 150 * pointSize * direction1[0];
		point1[1] = origin[1] + 150 * pointSize * direction1[1];
		point1[2] = origin[2] + 150 * pointSize * direction1[2];
		point2[0] = origin[0] + 150 * pointSize * direction2[0];
		point2[1] = origin[1] + 150 * pointSize * direction2[1];
		point2[2] = origin[2] + 150 * pointSize * direction2[2];

		fitPlanePoint[1][0] = point1[0];
		fitPlanePoint[1][1] = point1[1];
		fitPlanePoint[1][2] = point1[2];
		fitPlanePoint[2][0] = point2[0];
		fitPlanePoint[2][1] = point2[1];
		fitPlanePoint[2][2] = point2[2];
	}
	else if(minLengthIndex == 1)
	{
		//
		// 合平面几乎平行于xoz面
		//

		double pointToProject[3];

		// 原点
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];
		bestPlane->ProjectPoint(pointToProject, origin);
		fitPlanePoint[0][0] = origin[0];
		fitPlanePoint[0][1] = origin[1];
		fitPlanePoint[0][2] = origin[2];

		// 轴一的方向
		pointToProject[0] = modelBound[1];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];	
		bestPlane->ProjectPoint(pointToProject, direction1);
		direction1[0] = direction1[0] - origin[0];
		direction1[1] = direction1[1] - origin[1];
		direction1[2] = direction1[2] - origin[2];
		cout<<"t:"<<direction1[0]<<endl;
		vtkMath::Normalize(direction1);
		cout<<direction1[0]<<endl;
		// 轴二的方向
		vtkMath::Cross(fitPlaneNormal, direction1, direction2);
		vtkMath::Normalize(direction2);
		if(direction2[2] < 0)
		{
			direction2[0] = -direction2[0];
			direction2[1] = -direction2[1];
			direction2[2] = -direction2[2];
		}

		double point1[3], point2[3];
		point1[0] = origin[0] + 150 * pointSize * direction1[0];
		point1[1] = origin[1] + 150 * pointSize * direction1[1];
		point1[2] = origin[2] + 150 * pointSize * direction1[2];
		point2[0] = origin[0] + 150 * pointSize * direction2[0];
		point2[1] = origin[1] + 150 * pointSize * direction2[1];
		point2[2] = origin[2] + 150 * pointSize * direction2[2];

		fitPlanePoint[1][0] = point1[0];
		fitPlanePoint[1][1] = point1[1];
		fitPlanePoint[1][2] = point1[2];
		fitPlanePoint[2][0] = point2[0];
		fitPlanePoint[2][1] = point2[1];
		fitPlanePoint[2][2] = point2[2];
	}
	else if(minLengthIndex == 2)
	{
		//
		// 合平面平行于xoy面
		//

		double pointToProject[3];

		// 原点
		pointToProject[0] = modelBound[0];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];
		bestPlane->ProjectPoint(pointToProject, origin);
		fitPlanePoint[0][0] = origin[0];
		fitPlanePoint[0][1] = origin[1];
		fitPlanePoint[0][2] = origin[2];

		// 轴一的方向
		pointToProject[0] = modelBound[1];
		pointToProject[1] = modelBound[2];
		pointToProject[2] = modelBound[4];	
		bestPlane->ProjectPoint(pointToProject, direction1);
		direction1[0] = direction1[0] - origin[0];
		direction1[1] = direction1[1] - origin[1];
		direction1[2] = direction1[2] - origin[2];
		vtkMath::Normalize(direction1);

		// 轴二的方向
		vtkMath::Cross(fitPlaneNormal, direction1, direction2);
		vtkMath::Normalize(direction2);
		if(direction2[1] < 0)
		{
			direction2[0] = -direction2[0];
			direction2[1] = -direction2[1];
			direction2[2] = -direction2[2];
		}
		double point1[3], point2[3];
		point1[0] = origin[0] + 150 * pointSize * direction1[0];
		point1[1] = origin[1] + 150 * pointSize * direction1[1];
		point1[2] = origin[2] + 150 * pointSize * direction1[2];
		point2[0] = origin[0] + 150 * pointSize * direction2[0];
		point2[1] = origin[1] + 150 * pointSize * direction2[1];
		point2[2] = origin[2] + 150 * pointSize * direction2[2];

		fitPlanePoint[1][0] = point1[0];
		fitPlanePoint[1][1] = point1[1];
		fitPlanePoint[1][2] = point1[2];
		fitPlanePoint[2][0] = point2[0];
		fitPlanePoint[2][1] = point2[1];
		fitPlanePoint[2][2] = point2[2];
	}

	double planeColor[3] = {0.5647, 0.6235, 0.5647};
	double frameColor[3] = {0.5882, 0.7569, 0.5843};
	cout<<fitPlanePoint[0][0]<<' '<<fitPlanePoint[0][1]<<' '<<fitPlanePoint[0][2]<<endl;
	drawPlane(fitPlanePoint[0], fitPlanePoint[1], fitPlanePoint[2], planeColor, 0.4, fitPlaneActor, frameColor, 2, 2, fitPlaneFrameActor);
	isFitPlaneShow = 1;
}
void MainWindow::additem()
{
	if (selectOptionTreeIndex == 137)
	{
		int i = diSideOpenCloseToothNum;
		QString itemText = QString(tr("Grind Tooth")).append(QString::number(i + 1));
		QTreeWidgetItem *diSideOpenCloseGrindToothItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		diSideOpenCloseGrindToothItem->setData(0, Qt::UserRole, 140 * 100 + i);
		currentOptionTreeWidgetItem->addChild(diSideOpenCloseGrindToothItem);

		itemText = tr("Point");
		QTreeWidgetItem *diSideOpenClosePointItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenClosePointItem->setData(0, Qt::UserRole, 141 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenClosePointItem);
		itemText = tr("Intersect Plane O");
		QTreeWidgetItem *diSideOpenClosePlaneOItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenClosePlaneOItem->setData(0, Qt::UserRole, 142 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenClosePlaneOItem);
		itemText = tr("Contour Curve");
		QTreeWidgetItem *diSideOpenCloseContourCurveItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenCloseContourCurveItem->setData(0, Qt::UserRole, 143 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenCloseContourCurveItem);
		itemText = tr("Score");
		QTreeWidgetItem *diSideOpenCloseGrindToothScoreItem = new QTreeWidgetItem(diSideOpenCloseGrindToothItem, QStringList(itemText));
		diSideOpenCloseGrindToothScoreItem->setData(0, Qt::UserRole, 144 * 100 + i);
		diSideOpenCloseGrindToothItem->addChild(diSideOpenCloseGrindToothScoreItem);

		itemText = tr("Point");
		QTreeWidgetItem *diSideOpenClosePlaneOPointItem = new QTreeWidgetItem(diSideOpenClosePlaneOItem, QStringList(itemText));
		diSideOpenClosePlaneOPointItem->setData(0, Qt::UserRole, 145 * 100 + i);
		diSideOpenClosePlaneOItem->addChild(diSideOpenClosePlaneOPointItem);
		itemText = tr("Normal");
		QTreeWidgetItem *diSideOpenClosePlaneONormalItem = new QTreeWidgetItem(diSideOpenClosePlaneOItem, QStringList(itemText));
		diSideOpenClosePlaneONormalItem->setData(0, Qt::UserRole, 146 * 100 + i);
		diSideOpenClosePlaneOItem->addChild(diSideOpenClosePlaneONormalItem);

		itemText = tr("Point");
		QTreeWidgetItem *diSideOpenCloseCurvePointItem = new QTreeWidgetItem(diSideOpenCloseContourCurveItem, QStringList(itemText));
		diSideOpenCloseCurvePointItem->setData(0, Qt::UserRole, 147 * 100 + i);
		diSideOpenCloseContourCurveItem->addChild(diSideOpenCloseCurvePointItem);
		itemText = tr("Distance");
		QTreeWidgetItem *diSideOpenCloseCurveDistanceItem = new QTreeWidgetItem(diSideOpenCloseContourCurveItem, QStringList(itemText));
		diSideOpenCloseCurveDistanceItem->setData(0, Qt::UserRole, 148 * 100 + i);
		diSideOpenCloseContourCurveItem->addChild(diSideOpenCloseCurveDistanceItem);

		diSideOpenCloseToothNum++;
	}
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 7;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
	
}
void MainWindow::score()
{
	if(selectOptionTreeIndex == 903)
	{
		// 得分(后牙反合->得分)

		diBackToothOppositeScoreDlg *dlg;
		dlg = new diBackToothOppositeScoreDlg(this);
		dlg->mainWindow = this;
		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 907)
	{
		// 得分(后牙反合->得分)

		diBackToothLockScoreDlg *dlg;
		dlg = new diBackToothLockScoreDlg(this);
		dlg->mainWindow = this;
		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 15)
	{
		// 得分(前牙区不齐->得分)

		for(int i = 0; i < 10; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(!gapPointStat[i][j])
				{
					QMessageBox::information(NULL, tr("Info"), QString(tr("Can not get score...")));
					return;
				}
			}
		}

		frontToothDistance = 0;
		for(int i = 0; i < 10; ++i)
		{
			gapPointScoreT[i] = sqrt(vtkMath::Distance2BetweenPoints(gapProjecPointPosition[i][0], gapProjecPointPosition[i][1]));
			frontToothDistance += gapPointScoreT[i];
		}
		if (frontToothDistance <= 1)
		{
			gapPointScore = 0;
		}else if (frontToothDistance > 1 && frontToothDistance <= 2)
		{
			gapPointScore = 1;
		}else if (frontToothDistance > 2 && frontToothDistance <= 4)
		{
			gapPointScore = 2;
		}else if (frontToothDistance > 4 && frontToothDistance <= 8)
		{
			gapPointScore = 3;
		}else if (frontToothDistance > 8)
		{
			gapPointScore = 4;
		}
		setScore(gapPointScore);

		//FitPlaneDlg dlg(this);
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 1009)
	{
		// 得分(DI其他->得分)

		setOperatePreCondition(selectOptionTreeIndex, 7);

		DiOtherScoreDlg *dlg;
		dlg = new DiOtherScoreDlg(this);
		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 1371)
	{
		// 得分(DI合关系->得分)

		DiCloseRelationDlg *dlg;
		dlg = new DiCloseRelationDlg(this);
		//dlg->setModal(false);
		//dlg->show();
		dlg->exec();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 1381)
	{
		// 得分(DI合关系->得分)

		DiCloseRelationDlg *dlg;
		dlg = new DiCloseRelationDlg(this);
// 		dlg->setModal(false);
// 		dlg->show();
		dlg->exec();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 1023)
	{
		// 得分(DI拥挤度->得分)

		setOperatePreCondition(selectOptionTreeIndex, 7);

		bool canScore = true;

		for(int i = 0; i < 10; ++i)
		{
			if(diGrindToothScrowdPointState[i] == false)
			{
				canScore = false;
				break;
			}
		}
		for(int i = 0; i < 4; ++i)
		{
			if(diToothBowScrowdPointState[i] == false)
			{
				canScore = false;
				break;
			}
		}

		if(canScore)
		{
			removeChildItem(currentOptionTreeWidgetItem);

			diScrowdDistance = diGrindToothScrowdSumDistance - diToothBowScrowdSumDistance;
			if (diScrowdDistance < 1)
			{
				diScrowdScore = 0;
			}else if (diScrowdDistance <= 3)
			{
				diScrowdScore = 1;
			}else if (diScrowdDistance <= 5)
			{
				diScrowdScore = 2;
			}else if (diScrowdDistance <= 7)
			{
				diScrowdScore = 4;
			}else if (diScrowdDistance > 7)
			{
				diScrowdScore = 7;
			}
			QString itemText = QString::number(diScrowdScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
			currentOptionTreeWidgetItem->addChild(scoreInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}
		}
	}
	else if(selectOptionTreeIndex == 19)
	{
		VerticalPlaneScoreDlg *dlg;
		dlg = new VerticalPlaneScoreDlg(this);
		dlg->mainWindow = this;

		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 22)
	{
		FaceSideScoreDlg *dlg;
		dlg = new FaceSideScoreDlg(this);
		dlg->mainWindow = this;

		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 42)
	{
		// 得分(中线->得分)

		MiddleLineScoreDlg *dlg;
		dlg = new MiddleLineScoreDlg(this);
		dlg->mainWindow = this;

		dlg->ui.lineEditDistancePlane->setText(QString::number(distanceBetweenTwoVerticalPlane));
		//dlg.ui.lineEditDistancePoint->setText(QString::number(distanceBetweenTwoProjectOnFitPlanePoint));

		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 48)
	{
		ParfuheScoreDlg *dlg;
		dlg = new ParfuheScoreDlg(this);
		dlg->mainWindow = this;

		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else if(selectOptionTreeIndex == 55)
	{
		OverbiteLowerPairPointScoreDlg *dlg;
		dlg = new OverbiteLowerPairPointScoreDlg(this);
		dlg->mainWindow = this;

		dlg->setModal(false);
		dlg->show();
		//dlg.exec();
	}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
	}
}

void MainWindow::calFittingCurve()
{
	if(selectOptionTreeIndex == 63)
	{
		//
		// 开合->后牙开合->拟合曲线
		//


		if(isFitPlaneShow == 0)
			switchFitPlaneShow();

		mxArray *fromMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(fromMxArray), (void *)fitPlaneNormal, sizeof(fitPlaneNormal));
		engPutVariable(ep, "fromVec", fromMxArray);

		double to[3] = {0, 0, 1};
		mxArray *toMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(toMxArray), (void *)to, sizeof(to));
		engPutVariable(ep, "toVec", toMxArray);

		engEvalString(ep, "r = vrrotvec(fromVec, toVec);");
		engEvalString(ep, "m = vrrotvec2mat(r);");

		int pointNum = 0;
		double firstPoint[3], firstIndex = 0;
		double points[20][3];
		engEvalString(ep, "aa = [];");
		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i])
			{
				vtkPlane::ProjectPoint(backToothOpenCloseFittingPointPosition[i],
					fitPlaneCenter, fitPlaneNormal,
					backToothOpenCloseFittingProjectPointPosition[i]);

				for(int j = 0; j < 3; ++j)
				{
					if(pointNum == 0)
					{
						firstPoint[j] = backToothOpenCloseFittingProjectPointPosition[i][j];
						firstIndex = j;
					}

					points[i][j] = backToothOpenCloseFittingProjectPointPosition[i][j] - firstPoint[j];
				}
				++pointNum;

				double color[3] = {0, 1, 0};
				drawPoint(backToothOpenCloseFittingProjectPointPosition[i], color, backToothOpenCloseFittingProjectPointActor[i]);

				mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
				memcpy((void *)mxGetPr(tempArray), (void *)points[i], sizeof(points[i]));
				engPutVariable(ep, "tempVec", tempArray);

				engEvalString(ep, "b = tempVec * m';");
				engEvalString(ep, "aa = [aa; b];");

				mxDestroyArray(tempArray);
			}
		}

		engEvalString(ep, "m1 = rotz(atand(aa(length(aa), 2) / aa(length(aa), 1)));");
		engEvalString(ep, "bb =aa * m1;");
		engEvalString(ep, "p = polyfit(bb(:,1),bb(:,2),5);");///////////////change the power
		engEvalString(ep, "x = bb(:, 1);");
		engEvalString(ep, "xx = x(1):(x(length(x))-x(1))/40:x(length(x));");
		engEvalString(ep, "f = polyval(p,xx);");
		engEvalString(ep, "t=zeros(1,length(xx));");
		engEvalString(ep, "r=[xx;f;t];");
		engEvalString(ep, "for i=1:length(xx) r(3,i)=aa(1,3);");
		engEvalString(ep, "end");
		engEvalString(ep, "r =r';");
		engEvalString(ep, "r = r * (m1^ - 1);");
		engEvalString(ep, "points=r * ((m')^ - 1);");

		mxArray *resultMxArray = engGetVariable(ep, "points");
		double *result = mxGetPr(resultMxArray);

		for(int i = 0; i < 40; ++i)
		{
			double point1[3], point2[3];
			for(int j = 0; j < 3; ++j)
			{
				point1[j] = *(result + j * 41 + i) + firstPoint[j];
				point2[j] = *(result + j * 41 + i + 1) + firstPoint[j];
			}

			vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
			lineSource->SetPoint1(point1);
			lineSource->SetPoint2(point2);
			lineSource->Update();

			vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			lineMapper->SetInputConnection(lineSource->GetOutputPort());

			backToothOpenCloseFittingCurveActor[i]->SetMapper(lineMapper);
			backToothOpenCloseFittingCurveActor[i]->GetProperty()->SetLineWidth(2);
			backToothOpenCloseFittingCurveActor[i]->GetProperty()->SetColor(1, 0, 0);

			this->ren->AddActor(backToothOpenCloseFittingCurveActor[i]);
			isBackToothOpenCloseFittingCurveShow = 1;
		}
	}
	else if(selectOptionTreeIndex == 66)
	{
		// 拟合曲线(覆盖->拟合曲线)

		this->pickStyle->mode = 0;
		setOperatePreCondition(66, 4);

		mxArray *fromMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(fromMxArray), (void *)fitPlaneNormal, sizeof(fitPlaneNormal));
		engPutVariable(ep, "fromVec", fromMxArray);


		double to[3] = {0, 0, 1};
		mxArray *toMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(toMxArray), (void *)to, sizeof(to));
		engPutVariable(ep, "toVec", toMxArray);

		engEvalString(ep, "r = vrrotvec(fromVec, toVec);");
		engEvalString(ep, "m = vrrotvec2mat(r);");

		int pointNum = 0;
		double firstPoint[3], firstIndex = 0;
		double points[20][3];
		engEvalString(ep, "aa = [];");
		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])
			{
				vtkPlane::ProjectPoint(splintPointPosition[i],
					fitPlaneCenter, fitPlaneNormal,
					splineProjectPointPosition[i]);

				for(int j = 0; j < 3; ++j)
				{
					if(pointNum == 0)
					{
						firstPoint[j] = splineProjectPointPosition[i][j];
						firstIndex = j;
					}

					points[i][j] = splineProjectPointPosition[i][j] - firstPoint[j];
				}
				++pointNum;

				mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
				memcpy((void *)mxGetPr(tempArray), (void *)points[i], sizeof(points[i]));
				engPutVariable(ep, "tempVec", tempArray);

				engEvalString(ep, "b = tempVec * m';");
				engEvalString(ep, "aa = [aa; b];");

				mxDestroyArray(tempArray);
				
				double color[3] = {0, 1, 0};
				drawPoint(splineProjectPointPosition[i], color, splineProjectPointActor[i]);
				isSplineProjectPointShow = 1;
			}
		}

		engEvalString(ep, "m1 = rotz(atand(aa(length(aa), 2) / aa(length(aa), 1)));");
		engEvalString(ep, "bb =aa * m1;");
		engEvalString(ep, "p = polyfit(bb(:,1),bb(:,2),5);");
		engEvalString(ep, "x = bb(:, 1);");
		engEvalString(ep, "xx = x(1):(x(length(x))-x(1))/40:x(length(x));");
		engEvalString(ep, "f = polyval(p,xx);");
		engEvalString(ep, "t=zeros(1,length(xx));");
		engEvalString(ep, "r=[xx;f;t];");
		engEvalString(ep, "for i=1:length(xx) r(3,i)=aa(1,3);");
		engEvalString(ep, "end");
		engEvalString(ep, "r =r';");
		engEvalString(ep, "r = r * (m1^ - 1);");
		engEvalString(ep, "points=r * ((m')^ - 1);");

		mxArray *resultMxArray = engGetVariable(ep, "points");
		double *result = mxGetPr(resultMxArray);

		for(int i = 0; i < 40; ++i)
		{
			double point1[3], point2[3];
			for(int j = 0; j < 3; ++j)
			{
				point1[j] = *(result + j * 41 + i) + firstPoint[j];
				point2[j] = *(result + j * 41 + i + 1) + firstPoint[j];
			}

			vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
			lineSource->SetPoint1(point1);
			lineSource->SetPoint2(point2);
			lineSource->Update();

			vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			lineMapper->SetInputConnection(lineSource->GetOutputPort());

			fittingCurveActor[i]->SetMapper(lineMapper);
			fittingCurveActor[i]->GetProperty()->SetLineWidth(3);
			fittingCurveActor[i]->GetProperty()->SetColor(1, 0, 0);

			this->ren->AddActor(fittingCurveActor[i]);
			isFittingCurveShow = 1;
		}
	}
	else if(selectOptionTreeIndex == 89)
	{
		// 拟合曲线(开合->前牙开合->拟合曲线)

		this->pickStyle->mode = 0;
		setOperatePreCondition(89, 4);

		mxArray *fromMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(fromMxArray), (void *)fitPlaneNormal, sizeof(fitPlaneNormal));
		engPutVariable(ep, "fromVec", fromMxArray);

		double to[3] = {0, 0, 1};
		mxArray *toMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(toMxArray), (void *)to, sizeof(to));
		engPutVariable(ep, "toVec", toMxArray);

		engEvalString(ep, "r = vrrotvec(fromVec, toVec);");
		engEvalString(ep, "m = vrrotvec2mat(r);");

		int pointNum = 0;
		double firstPoint[3], firstIndex = 0;
		double points[20][3];
		engEvalString(ep, "aa = [];");
		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])///////////////////////////////////////
			{
				vtkPlane::ProjectPoint(splintPointPosition[i],
					fitPlaneCenter, fitPlaneNormal,
					frontToothOpenCloseFittingProjectPointPosition[i]);

				for(int j = 0; j < 3; ++j)
				{
					if(pointNum == 0)
					{
						firstPoint[j] = frontToothOpenCloseFittingProjectPointPosition[i][j];
						firstIndex = j;
					}

					points[i][j] = frontToothOpenCloseFittingProjectPointPosition[i][j] - firstPoint[j];
				}
				++pointNum;

				double color[3] = {0, 1, 0};
				//drawPoint(frontToothOpenCloseFittingProjectPointPosition[i], color, frontToothOpenCloseFittingProjectPointActor[i]);

				mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
				memcpy((void *)mxGetPr(tempArray), (void *)points[i], sizeof(points[i]));
				engPutVariable(ep, "tempVec", tempArray);

				engEvalString(ep, "b = tempVec * m';");
				engEvalString(ep, "aa = [aa; b];");

				mxDestroyArray(tempArray);
			}
		}

		engEvalString(ep, "m1 = rotz(atand(aa(length(aa), 2) / aa(length(aa), 1)));");
		engEvalString(ep, "bb =aa * m1;");
		engEvalString(ep, "p = polyfit(bb(:,1),bb(:,2),5);");
		engEvalString(ep, "x = bb(:, 1);");
		engEvalString(ep, "xx = x(1):(x(length(x))-x(1))/40:x(length(x));");
		engEvalString(ep, "f = polyval(p,xx);");
		engEvalString(ep, "t=zeros(1,length(xx));");
		engEvalString(ep, "r=[xx;f;t];");
		engEvalString(ep, "for i=1:length(xx) r(3,i)=aa(1,3);");
		engEvalString(ep, "end");
		engEvalString(ep, "r =r';");
		engEvalString(ep, "r = r * (m1^ - 1);");
		engEvalString(ep, "points=r * ((m')^ - 1);");

		mxArray *resultMxArray = engGetVariable(ep, "points");
		double *result = mxGetPr(resultMxArray);

		for(int i = 0; i < 40; ++i)
		{
			double point1[3], point2[3];
			for(int j = 0; j < 3; ++j)
			{
				point1[j] = *(result + j * 41 + i) + firstPoint[j];
				point2[j] = *(result + j * 41 + i + 1) + firstPoint[j];
			}

			vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
			lineSource->SetPoint1(point1);
			lineSource->SetPoint2(point2);
			lineSource->Update();

			vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			lineMapper->SetInputConnection(lineSource->GetOutputPort());

			frontToothOpenCloseFittingCurveActor[i]->SetMapper(lineMapper);
			frontToothOpenCloseFittingCurveActor[i]->GetProperty()->SetLineWidth(2);
			frontToothOpenCloseFittingCurveActor[i]->GetProperty()->SetColor(1, 0, 0);

			this->ren->AddActor(frontToothOpenCloseFittingCurveActor[i]);
			isFrontToothOpenCloseFittingCurveShow = 1;
		}

		QTreeWidgetItem *frontToothOpenCloseItem = currentOptionTreeWidgetItem->parent();
		QTreeWidgetItem *frontToothOpenCloseIntersectPlaneItem = getChildItem(frontToothOpenCloseItem,90);
		if(!frontToothOpenCloseIntersectPlaneItem->isExpanded())
			frontToothOpenCloseIntersectPlaneItem->setExpanded(true);
		QTreeWidgetItem *pointItem = getChildItem(frontToothOpenCloseIntersectPlaneItem, 92);

		removeChildItem(pointItem);
		QString itemText = QString("(").
			append(QString::number(frontToothToProjectPointPositionA0[0])).append(", ").
			append(QString::number(frontToothToProjectPointPositionA0[1])).append(", ").
			append(QString::number(frontToothToProjectPointPositionA0[2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(itemText));
		pointItem->addChild(pointInfoItem);
		if(!pointItem->isExpanded())
			pointItem->setExpanded(true);

		firstIndex = 0;
		engEvalString(ep, "aa = [];");
		for(int i = 0; i < 20; ++i)
		{
			if(frontToothOpenCloseFittingPointStat[i])
			{
				for(int j = 0; j < 3; ++j)
				{
					if(pointNum == 0)
					{
						firstPoint[j] = frontToothOpenCloseFittingProjectPointPosition[i][j];
						firstIndex = j;
					}

					points[i][j] = frontToothOpenCloseFittingProjectPointPosition[i][j] - firstPoint[j];
				}
				++pointNum;

				mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
				memcpy((void *)mxGetPr(tempArray), (void *)points[i], sizeof(points[i]));
				engPutVariable(ep, "tempVec", tempArray);

				engEvalString(ep, "b = tempVec * m';");
				engEvalString(ep, "aa = [aa; b];");

				mxDestroyArray(tempArray);
			}
		}

		vtkPlane::ProjectPoint(frontToothToProjectPointPositionA0,
			fitPlaneCenter, fitPlaneNormal,
			frontToothProjectPointPositionA0);
		double color[3] = {0, 1, 0};
		drawPoint(frontToothProjectPointPositionA0, color, frontToothProjectPointA0Actor);

		double tranProjectA0[3];
		tranProjectA0[0] = frontToothProjectPointPositionA0[0] - firstPoint[0];
		tranProjectA0[1] = frontToothProjectPointPositionA0[1] - firstPoint[1];
		tranProjectA0[2] = frontToothProjectPointPositionA0[2] - firstPoint[2];
		mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(tempArray), (void *)tranProjectA0, sizeof(tranProjectA0));
		engPutVariable(ep, "ppTemp", tempArray);
		engEvalString(ep, "pp = ppTemp * m';");

		engEvalString(ep, "m1 = rotz(atand(aa(length(aa), 2) / aa(length(aa), 1)));");
		engEvalString(ep, "bb =aa * m1;");
		engEvalString(ep, "pp = pp * m1;");
		engEvalString(ep, "p = polyfit(bb(:,1),bb(:,2),5);");
		engEvalString(ep, "options = optimoptions('fminunc','Display','off');");
		engEvalString(ep, "x0 = 0;");

		QString pathString = QString("path(path, '").append(this->path).append("');");
		//engEvalString(ep, "path(path, 'd:');");
		engEvalString(ep, qPrintable(pathString));
		engEvalString(ep, "[xmin, fval] = fminunc(@myfun,x0,options,pp,p);");
		engEvalString(ep, "y = p(1)*xmin^5 + p(2)*xmin^4 + p(3)*xmin^3 + p(4)*xmin^2 + p(5)*xmin + p(6);");
		engEvalString(ep, "temResult = [xmin, y, pp(3)];");
		engEvalString(ep, "result = temResult * ((m1)^-1);");
		engEvalString(ep, "result = result * ((m')^-1);");

		mxArray *resultCenterMxArray = engGetVariable(ep, "result");
		double *resultCenter = mxGetPr(resultCenterMxArray);
		frontToothPlaneOCenter[0] = resultCenter[0] + firstPoint[0];
		frontToothPlaneOCenter[1] = resultCenter[1] + firstPoint[1];
		frontToothPlaneOCenter[2] = resultCenter[2] + firstPoint[2];

		color[0] = 0;
		color[1] = 0;
		color[2] = 1;
		drawPoint(frontToothPlaneOCenter, color, frontToothPlaneCenterActor);

		double lineDirectionInOP[3];
		lineDirectionInOP[0] = frontToothPlaneOCenter[0] - frontToothProjectPointPositionA0[0];
		lineDirectionInOP[1] = frontToothPlaneOCenter[1] - frontToothProjectPointPositionA0[1];
		lineDirectionInOP[2] = frontToothPlaneOCenter[2] - frontToothProjectPointPositionA0[2];
		vtkMath::Normalize(lineDirectionInOP);

		double a0[3];
		a0[0] = frontToothPlaneOCenter[0] + 25 * pointSize * lineDirectionInOP[0];
		a0[1] = frontToothPlaneOCenter[1] + 25 * pointSize * lineDirectionInOP[1];
		a0[2] = frontToothPlaneOCenter[2] + 25 * pointSize * lineDirectionInOP[2];

		double a1[3];
		a1[0] = frontToothPlaneOCenter[0] - 25 * pointSize * lineDirectionInOP[0];
		a1[1] = frontToothPlaneOCenter[1] - 25 * pointSize * lineDirectionInOP[1];
		a1[2] = frontToothPlaneOCenter[2] - 25 * pointSize * lineDirectionInOP[2];

		double a2[3];
		a2[0] = frontToothPlaneOCenter[0] + 25 * pointSize * fitPlaneNormal[0];
		a2[1] = frontToothPlaneOCenter[1] + 25 * pointSize * fitPlaneNormal[1];
		a2[2] = frontToothPlaneOCenter[2] + 25 * pointSize * fitPlaneNormal[2];

		double a3[3];
		a3[0] = frontToothPlaneOCenter[0] - 25 * pointSize * fitPlaneNormal[0];
		a3[1] = frontToothPlaneOCenter[1] - 25 * pointSize * fitPlaneNormal[1];
		a3[2] = frontToothPlaneOCenter[2] - 25 * pointSize * fitPlaneNormal[2];

		double a4[3], a5[3], a6[3];
		frontToothPlaneOPoint[0][0] = a1[0] + 25 * pointSize * fitPlaneNormal[0];
		frontToothPlaneOPoint[0][1] = a1[1] + 25 * pointSize * fitPlaneNormal[1];
		frontToothPlaneOPoint[0][2] = a1[2] + 25 * pointSize * fitPlaneNormal[2];
		frontToothPlaneOPoint[1][0] = a1[0] - 25 * pointSize * fitPlaneNormal[0];
		frontToothPlaneOPoint[1][1] = a1[1] - 25 * pointSize * fitPlaneNormal[1];
		frontToothPlaneOPoint[1][2] = a1[2] - 25 * pointSize * fitPlaneNormal[2];
		frontToothPlaneOPoint[2][0] = a0[0] + 25 * pointSize * fitPlaneNormal[0];
		frontToothPlaneOPoint[2][1] = a0[1] + 25 * pointSize * fitPlaneNormal[1];
		frontToothPlaneOPoint[2][2] = a0[2] + 25 * pointSize * fitPlaneNormal[2];

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {1, 1, 0};
		drawPlane(frontToothPlaneOPoint[0], frontToothPlaneOPoint[1], frontToothPlaneOPoint[2], planeColor, 0.4, frontToothPlaneOActor, frameColor, 2, 2, frontToothPlaneOWireframeActor);
		isFrontToothPlaneOShow = 1;

		double lineOADirect[3];
		lineOADirect[0] = frontToothPlaneOCenter[0] - frontToothProjectPointPositionA0[0];
		lineOADirect[1] = frontToothPlaneOCenter[1] - frontToothProjectPointPositionA0[1];
		lineOADirect[2] = frontToothPlaneOCenter[2] - frontToothProjectPointPositionA0[2];
		vtkMath::Cross(fitPlaneNormal, lineOADirect, frontToothPlaneONormal);
		vtkMath::Normalize(frontToothPlaneONormal);

		QTreeWidgetItem *normalItem = getChildItem(frontToothOpenCloseIntersectPlaneItem, 93);

		removeChildItem(normalItem);
		itemText = QString("(").
			append(QString::number(frontToothPlaneONormal[0])).append(", ").
			append(QString::number(frontToothPlaneONormal[1])).append(", ").
			append(QString::number(frontToothPlaneONormal[2])).append(")");
		QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(itemText));
		normalItem->addChild(normalInfoItem);
		if(!normalItem->isExpanded())
			normalItem->setExpanded(true);

		vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
		plane->SetOrigin(frontToothPlaneOCenter);
		plane->SetNormal(frontToothPlaneONormal);

		vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
		cutter->SetCutFunction(plane);
		cutter->SetInput(lowerToothData);
		cutter->Update();

		vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cutterMapper->SetInputConnection(cutter->GetOutputPort());

		frontToothContourCurveActor->GetProperty()->SetColor(0, 0, 1);
		frontToothContourCurveActor->GetProperty()->SetLineWidth(2);
		frontToothContourCurveActor->SetMapper(cutterMapper);

		vtkSmartPointer<vtkPlane> plane1 = vtkSmartPointer<vtkPlane>::New();
		plane1->SetOrigin(frontToothPlaneOCenter);
		plane1->SetNormal(frontToothPlaneONormal);

		vtkSmartPointer<vtkCutter> cutter1 = vtkSmartPointer<vtkCutter>::New();
		cutter1->SetCutFunction(plane1);
		cutter1->SetInput(topToothData);
		cutter1->Update();

		vtkSmartPointer<vtkPolyDataMapper> cutterMapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
		cutterMapper1->SetInputConnection(cutter1->GetOutputPort());

		frontToothTopContourCurveActor->GetProperty()->SetColor(1.0, 0, 0);
		frontToothTopContourCurveActor->GetProperty()->SetLineWidth(2);
		frontToothTopContourCurveActor->SetMapper(cutterMapper1);

		this->ren->AddActor(frontToothContourCurveActor);
		this->ren->AddActor(frontToothTopContourCurveActor);
		isFrontToothContourCurveShaow = 1;
	}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
		return;
	}
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 6;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
}

void MainWindow::calFittingPlane()
{
	if(selectOptionTreeIndex == 4)
	{
		// 生成合平面(前牙区不齐->合平面)

		int fitPointCount = 0;
		for(int i = 0; i < 20; ++i)
		{
			if(fitPlanePointStat[i])
				++fitPointCount;
		}
		if(fitPointCount < 3)
		{
			QMessageBox::information(NULL, tr("Info"), QString(tr("Fit point is not enough. Please select more...")));
			return;
		}

		setOperatePreCondition(4, 3);
		this->pickStyle->mode = 0;

		initFitPlane();

		QTreeWidgetItem *fitPlaneNormalItem = getChildItem(currentOptionTreeWidgetItem, 6);
		QString itemText = QString("(").
			append(QString::number(fitPlaneNormal[0])).append(", ").
			append(QString::number(fitPlaneNormal[1])).append(", ").
			append(QString::number(fitPlaneNormal[2])).append(")");
		QTreeWidgetItem *fitPlaneNormalInfoItem = new QTreeWidgetItem(fitPlaneNormalItem, QStringList(itemText));
		this->removeChildItem(fitPlaneNormalItem);
		fitPlaneNormalItem->addChild(fitPlaneNormalInfoItem);
		if(!fitPlaneNormalItem->isExpanded())
			fitPlaneNormalItem->setExpanded(true);

		QTreeWidgetItem *fitPlaneCenterItem = getChildItem(currentOptionTreeWidgetItem, 7);
		itemText = QString("(").
			append(QString::number(fitPlaneCenter[0])).append(", ").
			append(QString::number(fitPlaneCenter[1])).append(", ").
			append(QString::number(fitPlaneCenter[2])).append(")");
		QTreeWidgetItem *fitPlaneCenterInfoItem = new QTreeWidgetItem(fitPlaneCenterItem, QStringList(itemText));
		this->removeChildItem(fitPlaneCenterItem);
		fitPlaneCenterItem->addChild(fitPlaneCenterInfoItem);
		if(!fitPlaneCenterItem->isExpanded())
			fitPlaneCenterItem->setExpanded(true);
	}

	else if(selectOptionTreeIndex == 2501)
	{
		//
		// ABO-OGS颊舌向倾斜度
		//		合平面
		//

		//QMessageBox::information(NULL, tr("Info"), QString(tr("Fit touch plane...")));

		vtkSmartPointer<vtkKdTree> pointTree = vtkSmartPointer<vtkKdTree>::New();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkIdType topToothPointNum = topToothData->GetNumberOfPoints();
		//QMessageBox::information(NULL, tr("Info"), QString::number(topToothPointNum));
		for(vtkIdType i = 0; i < topToothPointNum; ++i)
		{
			double pointPosition[3];
			topToothData->GetPoint(i, pointPosition);
			points->InsertNextPoint(pointPosition);
		}
		vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
		polydata->SetPoints(points);
		vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
		kDTree->SetDataSet(polydata);
		kDTree->BuildLocator();

		vtkSmartPointer<vtkRANSACPlane> rANSACPlane = vtkSmartPointer<vtkRANSACPlane>::New();
		rANSACPlane->SetInlierThreshold(0.1);
		rANSACPlane->SetMaxIterations(1000);

		vtkSmartPointer<vtkPoints> fittedPoints = vtkSmartPointer<vtkPoints>::New();
		vtkBoundingBox boundingBox;

		vtkIdType lowToothPointNum = lowerToothData->GetNumberOfPoints();
		//QMessageBox::information(NULL, tr("Info"), QString::number(lowToothPointNum));
		for(vtkIdType i = 0; i < lowToothPointNum; ++i)
		{
			double pointPosition[3];
			lowerToothData->GetPoint(i, pointPosition);

			double closestPointDist;
			vtkIdType id = kDTree->FindClosestPoint(pointPosition);
			double closestPoint[3];
			kDTree->GetDataSet()->GetPoint(id,closestPoint);

			double distance = sqrt(vtkMath::Distance2BetweenPoints(closestPoint, pointPosition));
			//double distance = vtkMath::Distance2BetweenPoints(closestPoint, pointPosition);
			//QMessageBox::information(NULL, tr("Info"), QString::number(distance));
			if(distance < 0.3)
			{				
				fittedPoints->InsertNextPoint(pointPosition[0], pointPosition[1], pointPosition[2]);
				fittedPoints->InsertNextPoint(closestPoint[0], closestPoint[1], closestPoint[2]);

				boundingBox.AddPoint(pointPosition);
				boundingBox.AddPoint(closestPoint);
			}
		}

		vtkIdType fitPointNum = fittedPoints->GetNumberOfPoints();
		//QMessageBox::information(NULL, tr("Info"), QString::number(fitPointNum));
		if(fitPointNum > 4)
		{
			turnoffAllResult();

			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 0)
				switchLowToothShow();

			// 生成拟合平面
			vtkSmartPointer<vtkPlane> bestPlane = vtkSmartPointer<vtkPlane>::New();
			BestFitPlane(fittedPoints, bestPlane);

			bestPlane->GetNormal(aboChapFitPlaneNormal);
			bestPlane->GetOrigin(aboChapFitPlaneCenter);

			currentOptionTreeWidgetItem->setExpanded(true);
			QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem, 2502);
			removeChildItem(normalItem);
			QString text = QString("(").
				append(QString::number(aboChapFitPlaneNormal[0])).append(", ").
				append(QString::number(aboChapFitPlaneNormal[1])).append(", ").
				append(QString::number(aboChapFitPlaneNormal[2])).append(")");
			QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
			normalItem->addChild(normalInfoItem);
			normalItem->setExpanded(true);

			// 绘制拟合平面
			double fitPointBound[6];
			boundingBox.GetBounds(fitPointBound);
			double fitPointBoundLength[3] = {
				fitPointBound[1] -fitPointBound[0],
				fitPointBound[3] -fitPointBound[2],
				fitPointBound[5] -fitPointBound[4]
			};
			int minLengthIndex = 0;
			if(fitPointBoundLength[0] < fitPointBoundLength[1] && fitPointBoundLength[0] < fitPointBoundLength[2])
				minLengthIndex = 0;
			else if(fitPointBoundLength[1] < fitPointBoundLength[0] && fitPointBoundLength[1] < fitPointBoundLength[2])
				minLengthIndex = 1;
			else if(fitPointBoundLength[2] < fitPointBoundLength[0] && fitPointBoundLength[2] < fitPointBoundLength[1])
				minLengthIndex = 2;

			double origin[3];
			double direction1[3], direction2[3];
			if(minLengthIndex == 0)
			{
				//
				// 合平面几乎平行于yoz面
				//

				double pointToProject[3];

				// 原点
				pointToProject[0] = modelBound[0];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[4];
				bestPlane->ProjectPoint(pointToProject, origin);
				aboChapFitPlanePoint[0][0] = origin[0];
				aboChapFitPlanePoint[0][1] = origin[1];
				aboChapFitPlanePoint[0][2] = origin[2];

				// 轴一的方向
				pointToProject[0] = modelBound[0];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[5];	
				bestPlane->ProjectPoint(pointToProject, direction1);
				direction1[0] = direction1[0] - origin[0];
				direction1[1] = direction1[1] - origin[1];
				direction1[2] = direction1[2] - origin[2];
				vtkMath::Normalize(direction1);

				// 轴二的方向
				vtkMath::Cross(fitPlaneNormal, direction1, direction2);
				vtkMath::Normalize(direction2);
				if(direction2[1] < 0)
					direction2[1] = -direction2[1];

				double point1[3], point2[3];
				point1[0] = origin[0] + 150 * pointSize * direction1[0];
				point1[1] = origin[1] + 150 * pointSize * direction1[1];
				point1[2] = origin[2] + 150 * pointSize * direction1[2];
				point2[0] = origin[0] + 150 * pointSize * direction2[0];
				point2[1] = origin[1] + 150 * pointSize * direction2[1];
				point2[2] = origin[2] + 150 * pointSize * direction2[2];

				aboChapFitPlanePoint[1][0] = point1[0];
				aboChapFitPlanePoint[1][1] = point1[1];
				aboChapFitPlanePoint[1][2] = point1[2];
				aboChapFitPlanePoint[2][0] = point2[0];
				aboChapFitPlanePoint[2][1] = point2[1];
				aboChapFitPlanePoint[2][2] = point2[2];
			}
			else if(minLengthIndex == 1)
			{
				//
				// 合平面几乎平行于xoz面
				//

				double pointToProject[3];

				// 原点
				pointToProject[0] = modelBound[0];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[4];
				bestPlane->ProjectPoint(pointToProject, origin);
				aboChapFitPlanePoint[0][0] = origin[0];
				aboChapFitPlanePoint[0][1] = origin[1];
				aboChapFitPlanePoint[0][2] = origin[2];

				pointToProject[0] = modelBound[1];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[4];
				bestPlane->ProjectPoint(pointToProject, origin);
				aboChapFitPlanePoint[1][0] = origin[0];
				aboChapFitPlanePoint[1][1] = origin[1];
				aboChapFitPlanePoint[1][2] = origin[2];

				pointToProject[0] = modelBound[0];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[5];
				bestPlane->ProjectPoint(pointToProject, origin);
				aboChapFitPlanePoint[2][0] = origin[0];
				aboChapFitPlanePoint[2][1] = origin[1];
				aboChapFitPlanePoint[2][2] = origin[2];
			}
			else if(minLengthIndex == 2)
			{
				//
				// 合平面平行于xoy面
				//

				double pointToProject[3];

				// 原点
				pointToProject[0] = modelBound[0];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[4];
				bestPlane->ProjectPoint(pointToProject, origin);
				aboChapFitPlanePoint[0][0] = origin[0];
				aboChapFitPlanePoint[0][1] = origin[1];
				aboChapFitPlanePoint[0][2] = origin[2];

				// 轴一的方向
				pointToProject[0] = modelBound[1];
				pointToProject[1] = modelBound[2];
				pointToProject[2] = modelBound[4];	
				bestPlane->ProjectPoint(pointToProject, direction1);
				direction1[0] = direction1[0] - origin[0];
				direction1[1] = direction1[1] - origin[1];
				direction1[2] = direction1[2] - origin[2];
				vtkMath::Normalize(direction1);

				// 轴二的方向
				vtkMath::Cross(fitPlaneNormal, direction1, direction2);
				vtkMath::Normalize(direction2);
				if(direction2[1] < 0)
					direction2[1] = -direction2[1];

				double point1[3], point2[3];
				point1[0] = origin[0] + 150 * pointSize * direction1[0];
				point1[1] = origin[1] + 150 * pointSize * direction1[1];
				point1[2] = origin[2] + 150 * pointSize * direction1[2];
				point2[0] = origin[0] + 150 * pointSize * direction2[0];
				point2[1] = origin[1] + 150 * pointSize * direction2[1];
				point2[2] = origin[2] + 150 * pointSize * direction2[2];

				aboChapFitPlanePoint[1][0] = point1[0];
				aboChapFitPlanePoint[1][1] = point1[1];
				aboChapFitPlanePoint[1][2] = point1[2];
				aboChapFitPlanePoint[2][0] = point2[0];
				aboChapFitPlanePoint[2][1] = point2[1];
				aboChapFitPlanePoint[2][2] = point2[2];
			}

			double planeColor[3] = {0.5647, 0.6235, 0.5647};
			double frameColor[3] = {0.5882, 0.7569, 0.5843};
			drawPlane(aboChapFitPlanePoint[0], aboChapFitPlanePoint[1], aboChapFitPlanePoint[2], planeColor, 0.4, aboChapFitPlaneActor, frameColor, 2, 2, aboChapFitPlaneWireframeActor);
			isChapFitPlaneShow = 1;
		}
		else
		{
			QMessageBox::information(NULL, tr("Info"), QString(tr("Fit point is not enough...")));
			return;
		}
	}
	else
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select option object...")));
		return;
	}
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 5;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
}

void MainWindow::segment()
{
	ofstream fout;
	fout.open("1.txt");
	for (vector<pair<int,double*>>::iterator it = SegPointMap.begin();it != SegPointMap.end();it ++)
	{
		double *P = it->second;
		fout<<P[0]<<' '<<P[1]<<' '<<P[2]<<endl;
	}
	fout.close();


// 	vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
// 	vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();			
// 	reader->SetFileName(qPrintable("ISO-5_1_Maxillar.stl"));
// 	reader->Update();
// 
// 	triangleFilter->SetInputConnection(reader->GetOutputPort());
// 	triangleFilter->Update();
// 	lowerToothData = triangleFilter->GetOutput();
// 	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
// 	mapper->SetInputConnection(triangleFilter->GetOutputPort());
// 
// 	ren->RemoveActor(lowerToothActor);
// 	lowerToothActor = vtkSmartPointer<vtkActor>::New();
// 	lowerToothActor->SetMapper(mapper);
// 	ren->AddActor(lowerToothActor);
// 	ren->Render();
}

void MainWindow::startsegment()
{
	cout<<"enter"<<endl;
	if(isLowToothShow == 1)
		DrawOnWhich = 0;
	else
		DrawOnWhich = 1;
	double ap[3][3];
	int j = 0;
	for (vector<pair<int,double*>>::iterator it = SegPointMap.begin();it != SegPointMap.end();it ++)
	{
		double *P = it->second;
		for (int i = 0;i < 3;i ++)
		{
			ap[j][i] = P[i];
		}
		j++;
	}
	mxArray *anchor_points = mxCreateDoubleMatrix(3, 3, mxREAL);
	memcpy((void *)mxGetPr(anchor_points), (void *)ap, sizeof(ap));
	engPutVariable(ep, "anchor_points", anchor_points);

	string filename;
	if (DrawOnWhich == 0)
	{
		filename = lowerModelName;	
	}
	else
		filename = topModelName;

	mxArray *filepath = mxCreateString(filename.c_str());
	engPutVariable(ep, "filepath", filepath);

	string s1 = "[frap_f] = annotate_teeth(filepath,anchor_points)";

	engEvalString(ep, "path(path,'G:\Project\2D&3D\SourceCode\ToothPoint04')");
	engEvalString(ep,"[frap_f] = annotate_teeth(filepath,anchor_points)");

	mxArray *resultMxArray = engGetVariable(ep, "frap_f");
	int row = mxGetM(resultMxArray);
	int col = mxGetN(resultMxArray);
	double *result = mxGetPr(resultMxArray);
	
	for (int i = 0;i < col;i++)
	{
		addCellId(4,i,result[i]);
	}

}

void MainWindow::setDiCloseRelationType(int T)
{
	DiCloseRelationType = T;
}

void MainWindow::startBigPick()
{
	this->pickStyle->mode = 2;
}

void MainWindow::startMiddlePick()
{
	this->pickStyle->mode = 3;
}
void MainWindow::startSmallPick()
{
	this->pickStyle->mode = 4;
}

void MainWindow::savePick()
{
	QFile targetFile("01.txt");
	targetFile.open(QFile::WriteOnly);
	QTextStream out(&targetFile);

	std::set<int>::iterator pos = pointSet.begin();
	std::set<int>::iterator end = pointSet.end();
	while(pos != end)
	{
		double pointPosition[3];
		data->GetPoint(*pos, pointPosition);
		out << pointPosition[0] << " " << pointPosition[1] << " " << pointPosition[2] << endl;

		++pos;
	}

	targetFile.close();
}

bool MainWindow::isCellIdInSet(int cellId)
{
	std::set<int>::iterator end = this->cellIdSet.end();
	std::set<int>::iterator pos = cellIdSet.find(cellId);
	if(pos == end)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void MainWindow::addCellId(int selectMode, int cellId,int celltype)
{
	if (celltype == 0)
	{
		return;
	}
	if(!isCellIdInSet(cellId))
	{
		this->cellIdSet.insert(cellId);

		switch(selectMode)
		{
		case 2:
			{
				this->drawTriCell(cellId,celltype);

				vtkSmartPointer<vtkIdList> pointIdList = vtkSmartPointer<vtkIdList>::New();
				data->GetCellPoints(cellId, pointIdList);

				std::set<vtkIdType> neighbors;
				for(vtkIdType i = 0; i < pointIdList->GetNumberOfIds(); i++)
				{
					vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
					idList->InsertNextId(pointIdList->GetId(i));

					vtkSmartPointer<vtkIdList> neighborCellIds = vtkSmartPointer<vtkIdList>::New();
					data->GetCellNeighbors(cellId, idList, neighborCellIds);

					for(vtkIdType j = 0; j < neighborCellIds->GetNumberOfIds(); j++)
					{
						neighbors.insert(neighborCellIds->GetId(j));
					}
				}

				std::set<int>::iterator pos = neighbors.begin();
				std::set<int>::iterator end = neighbors.end();
				while(pos != end)
				{
					if(!isCellIdInSet(*pos))
					{
						drawTriCell(*pos,celltype);

						this->cellIdSet.insert(*pos);
					}

					++pos;
				}
				this->ren->Render();
				break;
			}
		case 3:
			{
				this->drawTriCell(cellId,celltype);

				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				data->GetCellPoints(cellId, cellPointIds);

				std::list<vtkIdType> neighbors;
				for(vtkIdType i = 0; i < cellPointIds->GetNumberOfIds(); i++)
				{
					vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();

					idList->InsertNextId(cellPointIds->GetId(i));
					if(i + 1 == cellPointIds->GetNumberOfIds())
					{
						idList->InsertNextId(cellPointIds->GetId(0));
					}
					else
					{
						idList->InsertNextId(cellPointIds->GetId(i + 1));
					}

					vtkSmartPointer<vtkIdList> neighborCellIds = vtkSmartPointer<vtkIdList>::New();
					data->GetCellNeighbors(cellId, idList, neighborCellIds);

					for(vtkIdType j = 0; j < neighborCellIds->GetNumberOfIds(); j++)
					{
						neighbors.push_back(neighborCellIds->GetId(j));
					}
				}

				std::list<int>::iterator pos = neighbors.begin();
				std::list<int>::iterator end = neighbors.end();
				while(pos != end)
				{
					if(!isCellIdInSet(*pos))
					{
						drawTriCell(*pos,celltype);
						this->cellIdSet.insert(*pos);
					}

					++pos;
				}
				this->ren->Render();
				break;
			}
		case 4:
			{
				this->drawTriCell(cellId,celltype);
				this->ren->Render();
				break;
			}
		}
		this->ui.qvtkObjectViewerWidget->GetRenderWindow()->Render();
	}	
}

void MainWindow::drawTriCell(int cellId,int celltype)
{
	vtkSmartPointer<vtkIdList> pointIdList = vtkSmartPointer<vtkIdList>::New();
	data->GetCellPoints(cellId, pointIdList);

	double triPointPosition[3][3];
	for(int i = 0; i < pointIdList->GetNumberOfIds(); ++i)
	{
		pointSet.insert(pointIdList->GetId(i));
		data->GetPoint(pointIdList->GetId(i), triPointPosition[i]);
	}

	//
	// Draw triangle...
	//

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for(int i = 0; i < 3; ++i)
	{
		points->InsertNextPoint(triPointPosition[i]);
	}

	vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
	triangle->GetPointIds()->SetId ( 0, 0 );
	triangle->GetPointIds()->SetId ( 1, 1 );
	triangle->GetPointIds()->SetId ( 2, 2 );

	vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();
	triangles->InsertNextCell ( triangle );

	vtkSmartPointer<vtkPolyData> trianglePolyData = vtkSmartPointer<vtkPolyData>::New();
	trianglePolyData->SetPoints ( points );
	trianglePolyData->SetPolys ( triangles );

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInput(trianglePolyData);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(1, 0, 0);
	actor->GetProperty()->SetOpacity(0.7);

	this->ren->AddActor(actor);
	this->ren->Render();
}

void MainWindow::setSegPickPointPoisition(double x, double y, double z)
{

	if (selectOptionTreeIndex != 1111)
	{
		return;
	}
	double point1[3];
	double pointColor[3] = {1, 0, 0};
	point1[0] = x;
	point1[1] = y;
	point1[2] = z;
	QString text = QString("(").
		append(QString::number(x)).append(", ").
		append(QString::number(y)).append(", ").
		append(QString::number(z)).append(")");
	QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
	pointInfoItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
	currentOptionTreeWidgetItem->addChild(pointInfoItem);
	currentOptionTreeWidgetItem->setExpanded(true);
	for (int i = 0;i < 300;i ++)
	{
		if (PickPointState[i] == 0)
		{
			pickerCount = i;
			PickPointState[i] = 1;
			break;
		}
	}
	PickPointPosition[pickerCount][0] = x;
	PickPointPosition[pickerCount][1] = y;
	PickPointPosition[pickerCount][2] = z;
	pointInfoItem->setData(0, Qt::UserRole, 1111 * 1000 + pickerCount + 1);
	SegPointMap.push_back(pair<int,double*>(1111 * 1000 + pickerCount + 1 ,PickPointPosition[pickerCount] ));
	drawPoint(point1,pointColor,SegPointActor[pickerCount]);
	ScanTree();
	//drawPoint(point1, pointColor,fitPlanePointActor[0]);
	//QMessageBox::information(NULL, tr("Info"), QString::number(pickerCount));

}


void MainWindow::hideSelect()
{
	//QMessageBox::information(NULL, tr("Info"), QString::number(selectOptionTreeIndex));
	if(selectOptionTreeIndex == 0 || selectOptionTreeIndex == 3 || selectOptionTreeIndex == 6 || selectOptionTreeIndex == 7)
	{
		QMessageBox::information(NULL, tr("Info"), QString(tr("Please select hide things...")));
		return;
	}
	else if((selectOptionTreeIndex - 2010) >= 0 && (selectOptionTreeIndex - 2010) < 500 && (selectOptionTreeIndex - 2010) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2010) / 25;

		if(isAboEdgeVertebralHeighPointShow[pointPareIndex][0] == 1 || isAboEdgeVertebralHeighPointShow[pointPareIndex][1] == 1)
			switchAboEdgeVertebralHeighPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 2013) >= 0 && (selectOptionTreeIndex - 2013) < 500 && (selectOptionTreeIndex - 2013) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2013) / 25;

		if(isAboEdgeVertebralHeighProjectPointShow[pointPareIndex][0] == 1 || isAboEdgeVertebralHeighProjectPointShow[pointPareIndex][1] == 1)
			switchAboEdgeVertebralHeighProjectPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 2512) >= 0 && (selectOptionTreeIndex - 2512) < 500 && (selectOptionTreeIndex - 2512) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2512) / 25;

		if(isChapProjectPointShow[pointPareIndex][0] == 1 && isChapProjectPointShow[pointPareIndex][1] == 1)
			switchChapProjectPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 2511) >= 0 && (selectOptionTreeIndex - 2511) < 500 && (selectOptionTreeIndex - 2511) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 2511) / 25;
		for(int i = 0; i < aboChapPointPickNum[pointPareIndex]; ++i)
		{
			if(isChapPointShow[pointPareIndex][i] == 1)
				switchChapPointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 3012) >= 0 && (selectOptionTreeIndex - 3012) < 500 && (selectOptionTreeIndex - 3012) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3012) / 25;
		if(isAboToothCloseRelationPointShow[pointPareIndex][0] == 1)
			switchAboToothColoseRelationLowPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3013) >= 0 && (selectOptionTreeIndex - 3013) < 500 && (selectOptionTreeIndex - 3013) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3013) / 25;
		if(isAboToothCloseRelationPlaneOShow[pointPareIndex] == 1)
			switchAboToothCloseRelationPlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3017) >= 0 && (selectOptionTreeIndex - 3017) < 500 && (selectOptionTreeIndex - 3017) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3017) / 25;
		if(isAboToothCloseRelationPointShow[pointPareIndex][1] == 1)
			switchAboToothCloseRelationTopPointShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3511) >= 0 && (selectOptionTreeIndex - 3511) < 500 && (selectOptionTreeIndex - 3511) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3511) / 25;
		if(isAboToothCloseTouchPointShow[pointPareIndex][0] == 1)
			switchAboToothCloseTouchPointAShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3516) >= 0 && (selectOptionTreeIndex - 3516) < 500 && (selectOptionTreeIndex - 3516) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3516) / 25;
		if(isAboToothCloseTouchPointShow[pointPareIndex][1] == 1)
			switchAboToothCloseTouchPointBShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3512) >= 0 && (selectOptionTreeIndex - 3512) < 500 && (selectOptionTreeIndex - 3512) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3512) / 25;
		if(isAboToothCloseTouchPlaneOShow[pointPareIndex] == 1)
			switchAboToothCloseTouchPlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 3515) >= 0 && (selectOptionTreeIndex - 3515) < 500 && (selectOptionTreeIndex - 3515) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 3515) / 25;
		if(isAboToothCloseTouchIntersectCurveShow[pointPareIndex] == 1)
			switchAboToothCloseTouchIntersectCurveShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4011) >= 0 && (selectOptionTreeIndex - 4011) < 500 && (selectOptionTreeIndex - 4011) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4011) / 25;
		if(isAboCoverPointShow[pointPareIndex][1] == 1)
			switchAboCoverPointAShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4018) >= 0 && (selectOptionTreeIndex - 4018) < 500 && (selectOptionTreeIndex - 4018) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4018) / 25;
		if(isAboCoverPointShow[pointPareIndex][0] == 1)
			switchAboCoverPointBShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4012) >= 0 && (selectOptionTreeIndex - 4012) < 500 && (selectOptionTreeIndex - 4012) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4012) / 25;
		if(isAboCoverPlaneOShow[pointPareIndex] == 1)
			switchAboCoverPlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4015) >= 0 && (selectOptionTreeIndex - 4015) < 500 && (selectOptionTreeIndex - 4015) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4015) / 25;
		if(isCoverIntersectCurveShow[pointPareIndex][0] == 1 && isCoverIntersectCurveShow[pointPareIndex][1] == 1)
			switchisCoverIntersectCurveShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 4511) >= 0 && (selectOptionTreeIndex - 4511) < 500 && (selectOptionTreeIndex - 4511) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4511) / 25;
		for(int i = 0; i < aboTouchRelationPointNum[pointPareIndex]; ++i)
		{
			if(isAboTouchRelationPointShow[pointPareIndex][i] == 1)
				switchboTouchRelationPointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 4512) >= 0 && (selectOptionTreeIndex - 4512) < 500 && (selectOptionTreeIndex - 4512) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 4512) / 25;
		for(int i = 0; i < aboTouchRelationPointNum[pointPareIndex]; ++i)
		{
			if(isAboTouchRelationProjectPointShow[pointPareIndex][i] == 1)
				switchAboTouchRelationProjectPointShow(pointPareIndex);
		}
	}
	else if(selectOptionTreeIndex == 1)
	{
		if(isTopToothShow == 1)
		{
			isTopToothShow = 0;
			this->ren->RemoveActor(topToothActor);
		}
	}
	else if(selectOptionTreeIndex == 2)
	{
		if(isLowToothShow == 1)
		{
			isLowToothShow = 0;
			this->ren->RemoveActor(lowerToothActor);
		}
	}
	else if(selectOptionTreeIndex == 4)
	{
		if(isFitPlaneShow == 1)
		{
			isFitPlaneShow = 0;
			this->ren->RemoveActor(fitPlaneActor);
			this->ren->RemoveActor(fitPlaneFrameActor);
		}
	}
	else if(selectOptionTreeIndex == 1373)
	{
		if(isDiCloseRelationPointAShow == 1)
			switchDiCloseRelationPointAShow();
	}
	else if(selectOptionTreeIndex == 1374)
	{
		if(isDiCloseRelationPointBShow == 1)
			switchDiCloseRelationPointBShow();
	}
	else if(selectOptionTreeIndex == 1375)
	{
		if(isDiCloseRelationPlaneOShow == 1)
			switchDiCloseRelationPlaneOShow();
	}
	/*
	else if(selectOptionTreeIndex == 118)
	{
		if(isDiPointA0Show == 1)
			switchDiPointA0Show();
	}
	else if(selectOptionTreeIndex == 119)
	{
		if(isDiPlaneOShow == 1)
			switchDiPlaneOShow();
	}
	else if(selectOptionTreeIndex == 120)
	{
		if(isDiPlanePShow == 1)
			switchDiPlanePShow();
	}
	else if(selectOptionTreeIndex == 116)
	{
		if(isDiCoverIntersectLineShow == 1)
			switchDiCoverIntersectLineShow();
	}
	*/
	else if(selectOptionTreeIndex == 69)
	{
		if(isDiCoverPointShow == 1)
			switchDiCoverPointShow();
	}
	else if(selectOptionTreeIndex == 70)
	{
		if(isDiCoverPlaneOShow == 1)
			switchDiCoverPlaneOShow();
	}
	else if(selectOptionTreeIndex == 71)
	{
		if(isDiCoverIntersectCurveShow == 1)
			switchDiCoverIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 75)
	{
		if(isDiCoverIntersectCurvePointShow == 1)
			switchDiCoverIntersectCurvePointShow();
	}
	else if(selectOptionTreeIndex == 119)
	{
		if(isDiCoverPointShow == 1)
			switchDiCoverPointShow();
	}
	else if(selectOptionTreeIndex == 120)
	{
		if(isDiCoverPlaneOShow == 1)
			switchDiCoverPlaneOShow();
	}
	else if(selectOptionTreeIndex == 121)
	{
		if(isDiCoverIntersectCurveShow == 1)
			switchDiCoverIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 125)
	{
		if(isDiCoverIntersectCurvePointShow == 1)
			switchDiCoverIntersectCurvePointShow();
	}

	else if(selectOptionTreeIndex == 129)
	{
		if(isDiOverbitePointShow == 1)
			switchDiOverbitePointShow();
	}
	else if(selectOptionTreeIndex == 130)
	{
		if(isDiOverbitePlaneOShow == 1)
			switchDiOverbitePlaneOShow();
	}
	else if(selectOptionTreeIndex == 131)
	{
		if(isDiOverbiteIntersectCurveShow == 1)
			switchDiOverbiteIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 135)
	{
		if(isDiOverbiteIntersectCurvePointShow == 1)
			switchDiOverbiteIntersectCurvePointShow();
	}
	else if(selectOptionTreeIndex == 141 || selectOptionTreeIndex == 161 || selectOptionTreeIndex == 181)
	{
		int grindToothIndex = (selectOptionTreeIndex - 141) / 20;
		if(isDiSideOpenClosePointShow[grindToothIndex] == 1)
			switchDiSideOpenClosePointShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex / 100 == 142)
	{
		int grindToothIndex = selectOptionTreeIndex - 14200;
		if(isDiSideOpenClosePlaneOShow[grindToothIndex] == 1)
			switchDiSideOpenClosePlaneOShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex / 100 == 143)
	{
		int grindToothIndex = selectOptionTreeIndex - 14300;
		if(isDiSideOpenCloseIntersectCurveShow[grindToothIndex] == 1)
			switchDiSideOpenCloseIntersectCurveShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex / 100 == 147)
	{
		int grindToothIndex = selectOptionTreeIndex - 14700;
		if(isDiSideOpenCloseIntersectCurvePointShow[grindToothIndex] == 1)
			switchDiSideOpenCloseIntersectCurvePointShow(grindToothIndex);
	}
	else if(selectOptionTreeIndex == 401)
	{
		for(int i = 0; i < 20; ++i)
		{
			if(isDiFrontOpenClosePointShow[i] == 1)
				switchDiFrontOpenClosePointShow(i);
		}
	}
	else if(selectOptionTreeIndex > 420 && selectOptionTreeIndex < 820)
	{
		if((( selectOptionTreeIndex - 421) % 20) == 0)
		{
			int measureIndex = ( selectOptionTreeIndex - 421) / 20;
			if(isDiFrontOpenClosePointShow[measureIndex] == 1)
				switchDiFrontOpenClosePointShow(measureIndex);
		}
	}
	else if(selectOptionTreeIndex == 901)
	{
		if(isDiBackToothOppositeClosePointShow == 1)
			switchDiBackToothOppositeClosePointShow();
	}
	else if(selectOptionTreeIndex == 902)
	{
		if(isDiBackToothOppositeCloseProjectPointShow == 1)
			switchDiBackToothOppositeCloseProjectPointShow();
	}
	else if(selectOptionTreeIndex == 905)
	{
		if(isDiBackToothLockClosePointShow == 1)
			switchDiBackToothLockClosePointShow();
	}
	else if(selectOptionTreeIndex == 906)
	{
		if(isDiBackToothLockCloseProjectPointShow == 1)
			switchDiBackToothLockCloseProjectPointShow();
	}
	else if(selectOptionTreeIndex == 1002)
	{
		if(diMiddleUnfitPointShow == 1)
			switchDiMiddleUnfitPointShow();
	}
	else if(selectOptionTreeIndex == 1006)
	{
		if(isDiMiddleCutToothGapShow == 1)
			switchDiMiddleCutToothGapShow();
	}
	else if(selectOptionTreeIndex == 5)
	{
		if(isFitPlanePointShow == 1)
			switchFitPlanePointShow();
	}
	else if(selectOptionTreeIndex == 8)
	{
		if(isGapPointShow == 1)
			switchGapPointShow();
	}
	else if(selectOptionTreeIndex == 14)
	{
		if(isGapProjectPointShow == 1)
			switchGapProjectPointShow();
	}
	else if(selectOptionTreeIndex == 23)
	{
		if(isVerticalPlaneShow == 1)
			switchVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 26)
	{
		if(isPointInVerticalPlaneShow == 1)
			switchPointInVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 28)
	{
		if(isTopToothPointForVerticalPlaneShow == 1)
			switchTopToothPointForVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 30)
	{
		if(isTopToothProjectPointForVerticalPlane == 1)
			switchTopToothProjectPointForVerticalPlane();
	}
	else if(selectOptionTreeIndex == 29)
	{
		if(isTopToothPointForFitPlaneShow == 1)
			switchTopToothPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 31)
	{
		if(isTopToothProjectPointForFitPlaneShow == 1)
			switchTopToothProjectPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 24)
	{
		if(isLowToothPointForVerticalPlaneShow == 1)
			switchLowToothPointForVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 32)
	{
		if(isLowToothProjectPointForVerticalPlaneShow == 1)
			switchLowToothProjectPointForVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 25)
	{
		if(isLowToothPointForFitPlaneShow == 1)
			switchLowToothPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 33)
	{
		if(isLowToothProjectPointForFitPlaneShow == 1)
			switchLowToothProjectPointForFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 35)
	{
		// 隐藏(中线->矢状平行面1)
		if(isVerticalParallelPlaneShow == 1)
			switchVerticalParallelPlaneShow();
	}
	else if(selectOptionTreeIndex == 97)
	{
		// 隐藏(中线->矢状平行面2)
		if(isVerticalParallelPlane2Show == 1)
			switchVerticalParallelPlane2Show();
	}
	else if(selectOptionTreeIndex == 36)
	{
		// 隐藏(中线->矢状平行面1->初始点)
		if(isVerticalParallelPlaneInitPointShow == 1)
			switchVerticalParallelPlaneInitPointShow();
	}
	else if(selectOptionTreeIndex == 99)
	{
		// 隐藏(中线->矢状平行面2->初始点)
		if(isVerticalParallelPlaneInitPoint2Show == 1)
			switchVerticalParallelPlaneInitPoint2Show();
	}
	else if(selectOptionTreeIndex == 39)
	{
		if(isTwoPointToProjectFitPlaneShow == 1)
			switchTwoPointToProjectFitPlaneShow();
	}
	else if(selectOptionTreeIndex == 44)
	{
		if(isOverbiteIntersectCurveShow == 1)
			switchOverbiteIntersectCurveShow();
	}
	else if(selectOptionTreeIndex == 45)
	{
		if(isOverbiteParrelPlaneShow == 1)
			switchOverbiteParrelPlaneShow();
	}
	else if(selectOptionTreeIndex == 46)
	{
		if(isPointInOverbiteParallelPlaneShow == 1)
			switchPointInOverbiteParallelPlaneShow();
	}
	else if(selectOptionTreeIndex == 63)
	{
		if(isBackToothOpenCloseFittingCurveShow == 1)
			switchBackToothOpenCloseFittingCurveShow();
	}
	else if(selectOptionTreeIndex == 64)
	{
		if(isBackToothOpenCloseFittingPointShow == 1)
			switchBackToothOpenCloseFittingPointShow();
	}
	else if(selectOptionTreeIndex == 66)
	{
		if(isFittingCurveShow == 1)
			switchFittingCurveShow();
	}
	else if(selectOptionTreeIndex == 67)
	{
		if(isSplinePointShow == 1)
			switchSplinePointShow();
	}
	else if(selectOptionTreeIndex == 78)
	{
		if(isBackToothPlaneOShow == 1)
			switchBackToothPlaneOShow();
	}
	else if(selectOptionTreeIndex == 79)
	{
		if(isBackToothPointA0Show == 1)
			switchBackToothPointA0Show();
	}
	else if(selectOptionTreeIndex == 81)
	{
		if(isBackToothContourCurveShaow == 1)
			switchBackToothContourCurveShaow();
	}
	else if(selectOptionTreeIndex == 82)
	{
		if(isBackToothContiourCurvePointShow == 1)\
			switchBackToothContiourCurvePointShow();
	}
	else if(selectOptionTreeIndex == 86)
	{
		if(isPointForArticlePlaneShow == 1)
			switchPointForArticlePlaneShow();
	}
	else if(selectOptionTreeIndex == 85)
	{
		if(isArticlePlaneShow == 1)
			switchArticlePlaneShow();
	}
	else if(selectOptionTreeIndex == 91)
	{
		if(isFrontToothOpenCloseFittingPointShow == 1)
			switchFrontToothOpenCloseFittingPointShow();
	}
	else if(selectOptionTreeIndex == 89)
	{
		if(isFrontToothOpenCloseFittingCurveShow == 1)
			switchFrontToothOpenCloseFittingCurveShow();
	}
	else if(selectOptionTreeIndex == 90)
	{
		if(isFrontToothPlaneOShow == 1)
			switchFrontToothPlaneOShow();
	}
	else if(selectOptionTreeIndex == 94)
	{
		if(isFrontToothContourCurveShaow == 1)
			switchFrontToothContourCurveShaow();
	}
	else if(selectOptionTreeIndex == 95)
	{
		if(isFrontToothContiourCurvePointShow == 1)
			switchFrontToothContiourCurvePointShow();
	}
	else if(selectOptionTreeIndex == 49)
	{
		if(isInverseFitCoverPointShow == 1 && isInverseFitCoverProjectShow == 1)
			switchInverseFitCoverShow();
	}
	else if(selectOptionTreeIndex == 88)
	{
		if(isProjectArticlePlane == 1)
			switchProjectArticlePlane();
	}
	else if(selectOptionTreeIndex == 92)
	{
		if(idFrontToothToProjectPointPositionA0Show == 1)
			switchFrontToothToProjectPointPositionA0Show();
	}
	else if((selectOptionTreeIndex - 1510) >= 0 && (selectOptionTreeIndex - 1510) < 500 && (selectOptionTreeIndex - 1510) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1510) / 25;

		for(int i = 0; i < aboArrangePointPickNum[pointPareIndex]; ++i)
		{
			if(isAboArrangePointShow[pointPareIndex][i] == 1)
				switchAboArrangePointShow(pointPareIndex);
		}
	}
	else if((selectOptionTreeIndex - 1513) >= 0 && (selectOptionTreeIndex - 1513) < 500 && (selectOptionTreeIndex - 1513) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1513) / 25;

		if(isAboArrangePlaneOShow[pointPareIndex] == 1)
			switchAboArrangePlaneOShow(pointPareIndex);
	}
	else if((selectOptionTreeIndex - 1516) >= 0 && (selectOptionTreeIndex - 1516) < 500 && (selectOptionTreeIndex - 1516) % 25 == 0)
	{
		int pointPareIndex = (selectOptionTreeIndex - 1516) / 25;

		for(int i = 0; i < aboArrangePointPickNum[pointPareIndex]; ++i)
		{
			if(isAboArrangeProjectPointShow[pointPareIndex][i] == 1)
				switchAboArrangeProjectPointShow(pointPareIndex);
		}
	}
	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::clickOptionTreeItem(QTreeWidgetItem* item, int column)
{
	//QString selectNodeText = item->text(column);
	QVariant data = item->data(0, Qt::UserRole);
	selectOptionTreeIndex = getSelectOptionTreeIndex(data);
	//QMessageBox::information(NULL, tr("Info"), selectNodeText.append(", ").append(QString::number(selectOptionTreeIndex)));
	cout<<selectOptionTreeIndex<<endl;
	currentOptionTreeWidgetItem = item;
	if(!currentOptionTreeWidgetItem || !(currentOptionTreeWidgetItem->parent()))
	{
		return;
	}

	if(currentOptionTreeWidgetItem)
	{
		// 隐藏选中点
		if(isCurrentSelectPointShow == 1)
		{
			switchCurrentSelectPointShow();

			if(isFitPlanePointShow == 1)
			{
				isFitPlanePointShow = 0;
				switchFitPlanePointShow();
			}
			if(isPointInVerticalPlaneShow == 1)
			{
				isPointInVerticalPlaneShow = 0;
				switchPointInVerticalPlaneShow();
			}
			if(isPointForArticlePlaneShow == 1)
			{
				isPointForArticlePlaneShow = 0;
				switchPointForArticlePlaneShow();
			}
			if(isGapPointShow == 1)
			{
				isGapPointShow = 0;
				switchGapPointShow();
			}			
			if(isLowToothPointForVerticalPlaneShow == 1)
			{
				isLowToothPointForVerticalPlaneShow = 0;
				switchLowToothPointForVerticalPlaneShow();
			}
			if(isLowToothPointForFitPlaneShow == 1)
			{
				isLowToothPointForFitPlaneShow = 0;
				switchLowToothPointForFitPlaneShow();
			}
			if(isVerticalParallelPlaneInitPointShow == 1)
			{
				isVerticalParallelPlaneInitPointShow = 0;
				switchVerticalParallelPlaneInitPointShow();
			}
			if(isVerticalParallelPlaneInitPoint2Show == 1)
			{
				isVerticalParallelPlaneInitPoint2Show = 0;
				switchVerticalParallelPlaneInitPoint2Show();
			}
			if(isSplinePointShow == 1)
			{
				isSplinePointShow = 0;
				switchSplinePointShow();
			}
		}
	}

	if(selectOptionTreeIndex == 8)
	{
		if(isTopToothShow == 1)
		{
			switchTopToothShow();
		}

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}

	//if(selectOptionTreeIndex == 23)
	//{
	//	if(!currentOptionTreeWidgetItem->isExpanded())
	//	{
	//		currentOptionTreeWidgetItem->setExpanded(true);
	//	}
	//}

	if(selectOptionTreeIndex == 35)
	{
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}

	if(selectOptionTreeIndex == 97)
	{
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}

	QTreeWidgetItem *parentItem = currentOptionTreeWidgetItem->parent();
	data = parentItem->data(0, Qt::UserRole);
	if(5 == getSelectOptionTreeIndex(data))
	{
		if(isFitPlanePointShow == 0)
		{
			switchFitPlanePointShow();
		}

		if(isFitPlanePointShow == 1)
		{
			// 显示选中的合平面拟合点

			QString poingInfo = currentOptionTreeWidgetItem->text(0);
			poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
			QStringList stringList = poingInfo.split(", ");
			double position[3];
			for(int i = 0; i < stringList.length(); ++i)
			{
				position[i] = stringList.at(i).toDouble();
			}

			int fitPointIndex = this->getFitPointIndex(position[0], position[1], position[2]);
			if(fitPointIndex > -1)
			{
				isFitPlanePointShow = 0;
				switchFitPlanePointShow();

				this->ren->RemoveActor(fitPlanePointActor[fitPointIndex]);
			}

			double color[3] = {0, 0, 1};
			this->drawPoint(position, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	else if(26 == getSelectOptionTreeIndex(data))
	{
		if(isPointInVerticalPlaneShow == 0)
		{
			switchPointInVerticalPlaneShow();
		}

		// 显示选中点(颊侧区咬合->下牙列->正中矢状面->初始点)
		if(isPointInVerticalPlaneShow == 1)
		{
			QString poingInfo = currentOptionTreeWidgetItem->text(0);
			poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
			QStringList stringList = poingInfo.split(", ");
			double position[3];
			for(int i = 0; i < stringList.length(); ++i)
			{
				position[i] = stringList.at(i).toDouble();
			}

			int index = this->getPointInVerticalPlaneIndex(position[0], position[1], position[2]);
			if(index > -1)
			{
				isPointInVerticalPlaneShow = 0;
				switchPointInVerticalPlaneShow();

				this->ren->RemoveActor(pointInVerticalPlaneActor[index]);
			}

			double color[3] = {0, 0, 1};
			this->drawPoint(position, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	else if(86 == getSelectOptionTreeIndex(data))
	{
		// 显示选中点(冠状面初始点)

		if(isPointForArticlePlaneShow == 0)
		{
			switchPointForArticlePlaneShow();
		}

		QString poingInfo = currentOptionTreeWidgetItem->text(0);
		poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
		QStringList stringList = poingInfo.split(", ");
		double position[3];
		for(int i = 0; i < stringList.length(); ++i)
		{
			position[i] = stringList.at(i).toDouble();
		}

		this->ren->RemoveActor(pointForArticleActor);

		double color[3] = {0, 0, 1};
		this->drawPoint(position, color, currentSelectPointActor);
		isCurrentSelectPointShow = 1;
	}
	
	else if(67 == getSelectOptionTreeIndex(data))
	{
		// 显示选中点(覆盖->拟合曲线->点)

		if(isSplinePointShow == 0)
		{
			switchSplinePointShow();
		}

		if(isSplinePointShow == 1)
		{
			QString poingInfo = currentOptionTreeWidgetItem->text(0);
			poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
			QStringList stringList = poingInfo.split(", ");
			double position[3];
			for(int i = 0; i < stringList.length(); ++i)
			{
				position[i] = stringList.at(i).toDouble();
			}

			int index = this->getSplinePointIndex(position[0], position[1], position[2]);
			if(index > -1)
			{
				isSplinePointShow = 0;
				switchSplinePointShow();

				this->ren->RemoveActor(splinePointActor[index]);
			}

			double color[3] = {0, 0, 1};
			this->drawPoint(position, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	
// 	else if(9 == getSelectOptionTreeIndex(data) || 10 == getSelectOptionTreeIndex(data) || 11 == getSelectOptionTreeIndex(data) ||
// 		12 == getSelectOptionTreeIndex(data) || 13 == getSelectOptionTreeIndex(data))
	else if(getSelectOptionTreeIndex(data) / 1000 == 9 && getSelectOptionTreeIndex(data) % 1000 < 10)
	{
		// 显示选中点(前牙区不齐->接触点->缝隙x)

		if(isGapPointShow == 1)
		{
			QString poingInfo = currentOptionTreeWidgetItem->text(0);
			poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
			QStringList stringList = poingInfo.split(", ");
			double position[3];
			for(int i = 0; i < stringList.length(); ++i)
			{
				position[i] = stringList.at(i).toDouble();
			}

			int gapPointIndex[2];
			this->getGapPointIndex(position[0], position[1], position[2], gapPointIndex);
			if(gapPointIndex[0] > -1 && gapPointIndex[1] > -1)
			{
				isGapPointShow = 0;
				switchGapPointShow();

				this->ren->RemoveActor(gapPointActor[gapPointIndex[0]][gapPointIndex[1]]);
			}

			double color[3] = {0, 0, 1};
			this->drawPoint(position, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	else if(24 == getSelectOptionTreeIndex(data))
	{
		// 显示下牙列矢状向选中的选点

		if(isLowToothPointForVerticalPlaneShow == 1)
		{
			QString poingInfo = currentOptionTreeWidgetItem->text(0);
			poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
			QStringList stringList = poingInfo.split(", ");
			double position[3];
			for(int i = 0; i < stringList.length(); ++i)
			{
				position[i] = stringList.at(i).toDouble();
			}

			int index = this->getLowToothPointForVerticalPlaneIndex(position[0], position[1], position[2]);
			if(index > -1)
			{
				isLowToothPointForVerticalPlaneShow = 0;
				switchLowToothPointForVerticalPlaneShow();

				this->ren->RemoveActor(lowToothPointForVerticalPlaneActor[index]);
			}

			double color[3] = {0, 0, 1};
			this->drawPoint(position, projectToFittingPlanePointColot, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	else if(25 == getSelectOptionTreeIndex(data))
	{
		// 显示下牙列水平向选中的选点

		if(isLowToothPointForFitPlaneShow == 1)
		{
			QString poingInfo = currentOptionTreeWidgetItem->text(0);
			poingInfo = poingInfo.mid(1, poingInfo.length() - 2);
			QStringList stringList = poingInfo.split(", ");
			double position[3];
			for(int i = 0; i < stringList.length(); ++i)
			{
				position[i] = stringList.at(i).toDouble();
			}

			int index = this->getLowToothPointForFitPlaneIndex(position[0], position[1], position[2]);
			if(index > -1)
			{
				isLowToothPointForFitPlaneShow = 0;
				switchLowToothPointForFitPlaneShow();

				this->ren->RemoveActor(lowToothPointForFitPlaneActor[index]);
			}

			double color[3] = {0, 0, 1};
			this->drawPoint(position, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	else if(36 == getSelectOptionTreeIndex(data))
	{
		// 显示选中点(中线->矢状平行面1->初始点)
		if(isVerticalParallelPlaneInitPointShow == 1)
		{
			this->ren->RemoveActor(verticalParallelInitPointActor);

			double color[3] = {0, 0, 1};
			this->drawPoint(verticalParallelPlaneInitPointPosition, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	else if(99 == getSelectOptionTreeIndex(data))
	{
		// 显示选中点(中线->矢状平行面2->初始点)
		if(isVerticalParallelPlaneInitPoint2Show == 1)
		{
			this->ren->RemoveActor(verticalParallelInitPoint2Actor);

			double color[3] = {0, 0, 1};
			this->drawPoint(verticalParallelPlaneInitPoint2Position, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}	
	/*
	else if(72 == getSelectOptionTreeIndex(data))
	{
		// 显示选中点(覆盖->平面->牙弓放射状相交平面->点)

		if(isPointA0Show == 1)
		{
			//this->ren->RemoveActor(pointA0Actor);

			double color[3] = {0, 0, 1};
			this->drawPoint(pointPositionA0, color, currentSelectPointActor);
			isCurrentSelectPointShow = 1;
		}
	}
	*/
	else
	{
		// 隐藏选中点
		if(isCurrentSelectPointShow == 1)
			switchCurrentSelectPointShow();
	}
}

int MainWindow::getFitPointIndex(double x, double y, double z)
{
	int index = -1;
	double position[3] = {x, y, z};

	for(int i = 0; i < 20; ++i)
	{
		if(fitPlanePointStat[i])
		{
			double distance = sqrt(vtkMath::Distance2BetweenPoints(position, fitPlanePointPosition[i]));
			if(distance < 0.0001)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}

int MainWindow::getSplinePointIndex(double x, double y, double z)
{
	int index = -1;
	double position[3] = {x, y, z};
	for(int i = 0; i < 20; ++i)
	{
		if(splinePointState[i])
		{
			double distance = sqrt(vtkMath::Distance2BetweenPoints(position, splintPointPosition[i]));
			if(distance < 0.0001)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}

int  MainWindow::getLowToothPointForFitPlaneIndex(double x, double y, double z)
{
	int index = -1;
	double position[3] = {x, y, z};
	for(int i = 0; i < 10; ++i)
	{
		if(lowToothPointForFitPlaneStat[i])
		{
			double distance = sqrt(vtkMath::Distance2BetweenPoints(position, lowToothPointForFitPlanePosition[i]));
			if(distance < 0.0001)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}

int MainWindow::getLowToothPointForVerticalPlaneIndex(double x, double y, double z)
{
	int index = -1;
	double position[3] = {x, y, z};
	for(int i = 0; i < 10; ++i)
	{
		if(lowToothPointForVerticalPlaneStat[i])
		{
			double distance = sqrt(vtkMath::Distance2BetweenPoints(position, lowToothPointForVerticalPlanePosition[i]));
			if(distance < 0.0001)
			{
				index = i;
				break;
			}
		}
	}

	return index;
}

int MainWindow::getPointInVerticalPlaneIndex(double x, double y, double z)
{
	int index = -1;
	double position[3] = {x, y, z};

	for(int i = 0; i < 2; ++i)
	{
		double distance = sqrt(vtkMath::Distance2BetweenPoints(position, pointInVerticalPlanePosition[i]));
		if(distance < 0.0001)
		{
			index = i;
			break;
		}
	}

	return index;
}

void MainWindow::getGapPointIndex(double x, double y, double z, int *index)
{
	index[0] = index[1] = -1;
	double position[3] = {x, y, z};

	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 2; ++j)
		{
			if(gapPointStat[i][j] == true)
			{
				double distance = sqrt(vtkMath::Distance2BetweenPoints(position, gapPointPosition[i][j]));
				if(distance < 0.0001)
				{
					index[0] = i;
					index[1] = j;

					return;
				}
			}
		}
	}
}

int MainWindow::getSelectOptionTreeIndex(QVariant selectNodeData)
{
	int result = 0;
	int temp = selectNodeData.toInt();
	if(temp > 0)
	{
		result = temp;
	}

	return result;
}

QTreeWidgetItem* MainWindow::getOptionTreeByData(int targetData)
{
	QTreeWidgetItemIterator it(this->ui.optionTreeWidget);
	while (*it) {
		//do something like
		//->text(0);
		QVariant data = (*it)->data(0, Qt::UserRole);
		if (targetData ==  getSelectOptionTreeIndex(data))
		{
			currentOptionTreeWidgetItem = *it;
			break;
		}
		++it;
	}
	return *it;
}

void MainWindow::removeChildItemExcept(QTreeWidgetItem *parent, int *exceptDatas, int exceptNum)
{
	if(parent)
	{		
		bool condition = true;
		while(condition)
		{
			condition = false;
			int childCount = parent->childCount();
			for(int i = 0; i < childCount; ++i)
			{
				QTreeWidgetItem *child = parent->child(i);
				int data = getSelectOptionTreeIndex(child->data(0, Qt::UserRole));

				bool shouldNotDelete = false;
				for(int j = 0; j < exceptNum; ++j)
				{
					if(exceptDatas[j] == data)
					{
						shouldNotDelete = true;
						break;
					}
				}

				if(shouldNotDelete)

					continue;
				else
				{
					parent->removeChild(child);
					condition = true;
					break;
				}
			}
		}
	}
}

void MainWindow::removeChildItemExcept(QTreeWidgetItem *parent, int exceptData)
{
	if(parent)
	{		
		bool condition = true;
		while(condition)
		{
			condition = false;
			int childCount = parent->childCount();
			for(int i = 0; i < childCount; ++i)
			{
				QTreeWidgetItem *child = parent->child(i);
				int data = getSelectOptionTreeIndex(child->data(0, Qt::UserRole));
				if(exceptData == data)
					continue;
				else
				{
					parent->removeChild(child);
					condition = true;
					break;
				}
			}
		}
	}
}

void MainWindow::removeChildItem(QTreeWidgetItem *parent)
{
	if(parent)
	{
		bool condition = true;
		while(condition)
		{
			condition = false;
			int childCount = parent->childCount();
			for(int i = 0; i < childCount; ++i)
			{
				parent->removeChild(parent->child(i));

				condition = true;
				break;
			}
		}
	}
}

QTreeWidgetItem* MainWindow::getChildItem(QTreeWidgetItem *parent, int itemData)
{
	QTreeWidgetItem *result = NULL;
	if(parent)
	{
		int childCount = parent->childCount();
		QTreeWidgetItem *temp;
		for(int i = 0; i < childCount; ++i)
		{
			temp = parent->child(i);
			int tempData = getSelectOptionTreeIndex(temp->data(0, Qt::UserRole));
			if(tempData == itemData)
			{
				result = temp;
				break;
			}
		}
	}

	return result;
}

void MainWindow::drawPoint(double *pointPosition, double pointColor[3], vtkSmartPointer<vtkActor> pointActor)
{
	//QMessageBox::information(NULL, tr("Info"), QString::number(pointPosition[0]));
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(pointPosition);	
	sphereSource->SetRadius(pointSize);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	pointActor->SetMapper(mapper);
	pointActor->GetProperty()->SetColor(pointColor);

	this->ren->AddActor(pointActor);

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::drawLine(double *point1, double *point1Color, vtkSmartPointer<vtkActor> point1Actor,
		double *point2, double *point2Color, vtkSmartPointer<vtkActor> point2Actor, 
		float lineWidth, double *lineColor, vtkSmartPointer<vtkActor> lineActor)
{
	if(point1Color)
		drawPoint(point1, point1Color,point1Actor);
	if(point2Color)
		drawPoint(point2, point2Color, point2Actor);

	vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(point1);
	lineSource->SetPoint2(point2);
	lineSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	lineMapper->SetInputConnection(lineSource->GetOutputPort());

	lineActor->SetMapper(lineMapper);
	lineActor->GetProperty()->SetLineWidth(lineWidth);
	lineActor->GetProperty()->SetColor(lineColor);

	this->ren->AddActor(lineActor);

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::drawPlane(double *origin, double *point1, double *point2, double *color, double opacity, vtkSmartPointer<vtkActor> planeActor,
						   double *frameColor, int xResolution, int yResolution, vtkSmartPointer<vtkActor> planeFrameActor)
{
	vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
	planeSource->SetOrigin(origin);
	planeSource->SetPoint1(point1);
	planeSource->SetPoint2(point2);
	planeSource->SetXResolution(20);
	planeSource->SetYResolution(20);
	planeSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
	mapper->SetInput(planeSource->GetOutput());
#else
	mapper->SetInputData(planeSource->GetOutput());
#endif

	planeActor->SetMapper(mapper);
	planeActor->GetProperty()->SetColor(color);
	planeActor->GetProperty()->SetOpacity(opacity);

	this->ren->AddActor(planeActor);

	if(frameColor)
	{
		vtkSmartPointer<vtkPlaneSource> wireFramePlaneSource = vtkSmartPointer<vtkPlaneSource>::New();
		wireFramePlaneSource->SetOrigin(origin);
		wireFramePlaneSource->SetPoint1(point1);
		wireFramePlaneSource->SetPoint2(point2);
		wireFramePlaneSource->SetXResolution(xResolution);
		wireFramePlaneSource->SetYResolution(yResolution);
		wireFramePlaneSource->Update();

		vtkPolyData* fitWireFramePlanePolyData = wireFramePlaneSource->GetOutput();

		// Create a mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> wireFramePlaneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
		wireFramePlaneMapper->SetInput(fitWireFramePlanePolyData);
#else
		wireFramePlaneMapper->SetInputData(fitWireFramePlanePolyData);
#endif

		planeFrameActor->SetMapper(wireFramePlaneMapper);
		planeFrameActor->GetProperty()->SetColor(frameColor);
		planeFrameActor->GetProperty()->SetRepresentationToWireframe();

		this->ren->AddActor(planeFrameActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::findLowerCuttingPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB,double *ProCenterB)
{
	vtkSmartPointer<vtkDataSet> vP = CurveActor->GetMapper()->GetInput();
/*	double pB[3] = {};*/
	double centerB[3] = {};
	for (int i = 0;i < vP->GetNumberOfPoints();i ++)
	{
		double tP[3];
		vP->GetPoint(i,tP);
		centerB[0] += tP[0];
		centerB[1] += tP[1];
		centerB[2] += tP[2];
	}
	for (int i = 0;i < 3;i ++)
	{
		centerB[i] = centerB[i] / vP->GetNumberOfPoints();
	}
	//double ProCenterB[3] = {};
	vtkPlane::ProjectPoint(centerB, 
		fitPlaneCenter, fitPlaneNormal,
		ProCenterB);
	double maxDis = 0;
	for (int i = 0;i < vP->GetNumberOfPoints();i ++)// max distance on fitting plane
	{
		double tP[3];
		vP->GetPoint(i,tP);
		double tPro[3];
		vtkPlane::ProjectPoint(tP, 
			fitPlaneCenter, fitPlaneNormal,
			tPro);
		double m = (tPro[0] - ProCenterB[0]) * (pA[0] - ProCenterB[0]);
		double v1[3],v2[3];
		for (int j = 0;j < 3;j ++)
		{
			v1[j] = ProCenterB[j] - centerB[j];
			v2[j] = tP[j] - centerB[j];
		}
		double Ang = acos(vtkMath::Dot(v1,v2)/(vtkMath::Norm(v1) * vtkMath::Norm(v2)));
		if (m > 0 && Ang < 1.57)
		{
			/*double dis = sqrt(vtkMath::Distance2BetweenPoints(tPro, ProCenterB));*/
			double dis = tP[1];
			if (dis > maxDis)
			{
				maxDis = dis;
				pB[0] = tP[0];
				pB[1] = tP[1];
				pB[2] = tP[2];
			}
		}
	}
}

void MainWindow::findLowerClosestPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB)
{
	vtkSmartPointer<vtkDataSet> vP = CurveActor->GetMapper()->GetInput();
	/*	double pB[3] = {};*/
	double minDis = 10000;
	for (int i = 0;i < vP->GetNumberOfPoints();i ++)
	{
		double tP[3];
		vP->GetPoint(i,tP);
		double dis = sqrt(vtkMath::Distance2BetweenPoints(pA, tP));
		if (dis < minDis)
		{
			minDis = dis;
			for (int j = 0;j < 3;j ++)
			{
				pB[j] = tP[j];
			}
		}
	}
}

void MainWindow::findUpperConvexPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB,double *ProCenterB)
{
	//xoy平面最近点->求上凸极值点
	vtkSmartPointer<vtkDataSet> vP = CurveActor->GetMapper()->GetInput();
	/*	double pB[3] = {};*/
	double centerB[3] = {};
	for (int i = 0;i < vP->GetNumberOfPoints();i ++)
	{
		double tP[3];
		vP->GetPoint(i,tP);
		centerB[0] += tP[0];
		centerB[1] += tP[1];
		centerB[2] += tP[2];
	}
	for (int i = 0;i < 3;i ++)
	{
		centerB[i] = centerB[i] / vP->GetNumberOfPoints();
	}
	//double ProCenterB[3] = {};
	vtkPlane::ProjectPoint(centerB, 
		fitPlaneCenter, fitPlaneNormal,
		ProCenterB);//计算轮廓中心
	double maxDis = 0;
	for (int i = 0;i < vP->GetNumberOfPoints();i ++)// 求最近投影点
	{
		double tP[3];
		vP->GetPoint(i,tP);
		double tPro[3];
		vtkPlane::ProjectPoint(tP, 
			fitPlaneCenter, fitPlaneNormal,
			tPro);
		double m = (tPro[0] - ProCenterB[0]) * (pA[0] - ProCenterB[0]);
		double v1[3],v2[3];
		for (int j = 0;j < 3;j ++)
		{
			v1[j] = ProCenterB[j] - centerB[j];
			v2[j] = tP[j] - centerB[j];
		}
		double Ang = acos(vtkMath::Dot(v1,v2)/(vtkMath::Norm(v1) * vtkMath::Norm(v2)));
		if (m > 0 && Ang < 1.57)
		{
			/*double dis = sqrt(vtkMath::Distance2BetweenPoints(tPro, ProCenterB));*/
			double dis = tP[1];
			if (dis > maxDis)
			{
				maxDis = dis;
				pB[0] = tP[0];
				pB[1] = tP[1];
				pB[2] = tP[2];
			}
		}
	}
}

void MainWindow::setScore(double score)
{
	if(selectOptionTreeIndex == 903)
	{
		diBackToothOppositeCloseScore = score;

		removeChildItem(currentOptionTreeWidgetItem);
		ScoreSet[0][6] = diBackToothOppositeCloseScore;
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->diBackToothOppositeCloseScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 907)
	{
		diBakcToothLockCloseScore = score;

		removeChildItem(currentOptionTreeWidgetItem);
		ScoreSet[0][7] = diBakcToothLockCloseScore;
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->diBakcToothLockCloseScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 15)
	{
		// 得分(前牙区不齐->得分)

		this->frontToothScore = score;

		//
		// 更新操作框信息
		//
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		QTreeWidgetItem *frontToothDistanceItem = this->getChildItem(currentOptionTreeWidgetItem, 16);
		this->removeChildItem(frontToothDistanceItem);
		QTreeWidgetItem *frontToothDistanceInfoItem = new QTreeWidgetItem(frontToothDistanceItem, QStringList(QString::number(frontToothDistance))); 
		if(!frontToothDistanceItem->isExpanded())
		{
			frontToothDistanceItem->setExpanded(true);
		}

		QTreeWidgetItem *frontToothScoreItem = this->getChildItem(currentOptionTreeWidgetItem, 17);
		this->removeChildItem(frontToothScoreItem);
		QTreeWidgetItem *frontToothScoreInfoItem = new QTreeWidgetItem(frontToothScoreItem, QStringList(QString::number(frontToothScore))); 
		if(!frontToothScoreItem->isExpanded())
		{
			frontToothScoreItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 1009)
	{
		// 得分(DI其他->得分)

		diOtherScore = score;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString::number(diOtherScore);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(scoreInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 1371)
	{
		// 得分(DI合关系->得分)

		diCloseRelationScore += score;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString::number(diCloseRelationScore);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(scoreInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
		cout<<"1 complete"<<endl;
	}
	else if(selectOptionTreeIndex == 1381)
	{
		// 得分(DI合关系->得分)

		diCloseRelationScore += score;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString::number(diCloseRelationScore);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(scoreInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
		cout<<"2 complete"<<endl;
	}
	else if(selectOptionTreeIndex == 19)
	{
		// 得分(颊侧区咬合->矢装向得分)

		this->faceSideSagittalScore = score;

		this->removeChildItem(currentOptionTreeWidgetItem);
		QTreeWidgetItem *facesideVerticalScoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->faceSideSagittalScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 20)
	{
		// 得分(颊侧区咬合->矢装向得分)

		this->faceSideVerticalScore = score;

		//this->removeChildItem(currentOptionTreeWidgetItem);
		QTreeWidgetItem *facesideVerticalScoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->faceSideVerticalScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 61)
	{
		// 得分(开合)

		this->overbitOpenCloseScore = score;

		//this->removeChildItem(currentOptionTreeWidgetItem);
		QTreeWidgetItem *facesideVerticalScoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->overbitOpenCloseScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 22)
	{
		// 得分(颊侧区咬合->水平向得分)

		this->faceSideHorizonalScore = score;
		ScoreSet[2][1] += faceSideHorizonalScore;
		this->removeChildItem(currentOptionTreeWidgetItem);
		QTreeWidgetItem *facesideHorizonalScoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->faceSideHorizonalScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 42)
	{
		middleLineScore = score;

		removeChildItem(currentOptionTreeWidgetItem);

		QTreeWidgetItem *middleLneScoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->middleLineScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 48)
	{
		overbitIntersectCurveScore = score;
		ScoreSet[2][3] = overbitIntersectCurveScore;
		removeChildItem(currentOptionTreeWidgetItem);

		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->overbitIntersectCurveScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
	else if(selectOptionTreeIndex == 55)//反合
	{
		inverseFitCoverScore = score;
		ScoreSet[2][2] = inverseFitCoverScore;
		removeChildItem(currentOptionTreeWidgetItem);

		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, 
			QStringList(QString::number(this->inverseFitCoverScore))); 
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
	}
}

double MainWindow::getFrontToothDistance()
{
	return this->frontToothDistance;
}

void MainWindow::setPickPointPosition(double x, double y, double z)
{
	//QMessageBox::information(NULL, tr("Info"), QString::number(x));
	if(selectOptionTreeIndex == 4)
	{
		// 合平面选点

		currentOptionTreeWidgetItem->setExpanded(true);

		for(int i = 0; i < 20; ++i)
		{
			if(fitPlanePointStat[i] == false)
			{
				fitPlanePointStat[i] = true;

				fitPlanePointPosition[i][0] = x;
				fitPlanePointPosition[i][1] = y;
				fitPlanePointPosition[i][2] = z;

				double pointColor[3] = {1, 0, 0};
				drawPoint(fitPlanePointPosition[i], pointColor, fitPlanePointActor[i]);
				isFitPlanePointShow = 1;

				QTreeWidgetItem *fitPlanePointItem = getChildItem(currentOptionTreeWidgetItem, 5);

				QString pointInfoItemText = QString("(").
					append(QString::number(x)).append(", ").
					append(QString::number(y)).append(", ").
					append(QString::number(z)).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(fitPlanePointItem, QStringList(pointInfoItemText));
				fitPlanePointItem->addChild(pointInfoItem);

				if(!fitPlanePointItem->isExpanded())
				{
					fitPlanePointItem->setExpanded(true);
				}

				break;
			}
		}
	}
	else if((selectOptionTreeIndex - 2510) >= 0 && (selectOptionTreeIndex - 2510) < 500 && (selectOptionTreeIndex - 2510) % 25 == 0)
	{
		//
		// ABO-OGS颊舌向倾斜度
		//		选点
		//

		int pointPareIndex = (selectOptionTreeIndex - 2510) / 25;

		currentOptionTreeWidgetItem->setExpanded(true);

		// 点操作

		aboChapPoints[pointPareIndex][aboChapPointPickNum[pointPareIndex]][0] = x;
		aboChapPoints[pointPareIndex][aboChapPointPickNum[pointPareIndex]][1] = y;
		aboChapPoints[pointPareIndex][aboChapPointPickNum[pointPareIndex]][2] = z;

		vtkPlane::ProjectPoint(aboChapPoints[pointPareIndex][aboChapPointPickNum[pointPareIndex]],
			aboChapFitPlaneCenter, aboChapFitPlaneNormal,
			aboChapProjectPoint[pointPareIndex][aboChapPointPickNum[pointPareIndex]]);

		drawPoint(aboChapPoints[pointPareIndex][aboChapPointPickNum[pointPareIndex]], 
			pickPointColor, 
			aboChapPointAcotor[pointPareIndex][aboChapPointPickNum[pointPareIndex]]);
		isChapPointShow[pointPareIndex][aboChapPointPickNum[pointPareIndex]] = 1;

		QTreeWidgetItem *chapPointItem = getChildItem(currentOptionTreeWidgetItem, 2511 + pointPareIndex * 25);
		removeChildItem(chapPointItem);
		for(int i = 0; i <= aboChapPointPickNum[pointPareIndex]; ++i)
		{		
			QString text = QString("(").
				append(QString::number(aboChapPoints[pointPareIndex][i][0])).append(", ").
				append(QString::number(aboChapPoints[pointPareIndex][i][1])).append(", ").
				append(QString::number(aboChapPoints[pointPareIndex][i][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(chapPointItem, QStringList(text));
			chapPointItem->addChild(pointInfoItem);
		}
		chapPointItem->setExpanded(true);

		if(aboChapPointPickNum[pointPareIndex] == 1)
		{
			if(isChapFitPlaneShow == 0)
				switchChapFitPlaneShow();

			double dist1 = vtkMath::Distance2BetweenPoints(aboChapPoints[pointPareIndex][0], aboChapPoints[pointPareIndex][1]);
			double dist2 = vtkMath::Distance2BetweenPoints(aboChapProjectPoint[pointPareIndex][0], aboChapProjectPoint[pointPareIndex][1]);

			//double dist1 = sqrt(vtkMath::Distance2BetweenPoints(aboChapPoints[pointPareIndex][0], aboChapProjectPoint[pointPareIndex][0]));
			//double dist2 = sqrt(vtkMath::Distance2BetweenPoints(aboChapPoints[pointPareIndex][1], aboChapProjectPoint[pointPareIndex][1]));
			
			aboChapPointPareDistance[pointPareIndex] = sqrt(abs(dist1 - dist2));
			aboChapPointPareScore[pointPareIndex] = aboChapPointPareDistance[pointPareIndex];
			aboChapScore = 0;
			for(int i = 0; i < 14; ++i)
			{
				int temp = 0;
				if (aboChapPointPareScore[i] < 1)
				{
					temp = 0;
				}else
					if (aboChapPointPareScore[i] >= 1 && aboChapPointPareScore[i] <= 2)
					{
						temp = 1;
					}else
						if (aboChapPointPareScore[i] > 2)
						{
							temp = 2;
						}
				aboChapPointPareScore[i] = temp;
				aboChapScore += temp;
			}

			QTreeWidgetItem *chapProjectPointItem = getChildItem(currentOptionTreeWidgetItem, 2512 + pointPareIndex * 25);
			removeChildItem(chapProjectPointItem);

			for(int i = 0; i < 2; ++i)
			{
				drawPoint(aboChapProjectPoint[pointPareIndex][i], 
					projectToFittingPlanePointColot, 
					aboChapProjectPointAcotr[pointPareIndex][i]);
				isChapProjectPointShow[pointPareIndex][i] = 1;

				QString text = QString("(").
					append(QString::number(aboChapProjectPoint[pointPareIndex][i][0])).append(", ").
					append(QString::number(aboChapProjectPoint[pointPareIndex][i][1])).append(", ").
					append(QString::number(aboChapProjectPoint[pointPareIndex][i][2])).append(")");
				QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(chapProjectPointItem, QStringList(text));
				chapProjectPointItem->addChild(projectPointInfoItem);
				chapProjectPointItem->setExpanded(true);
			}

			QTreeWidgetItem *chapPointDistanceItem = getChildItem(currentOptionTreeWidgetItem, 2513 + pointPareIndex * 25);
			removeChildItem(chapPointDistanceItem);
			QString text = QString::number(aboChapPointPareDistance[pointPareIndex]);
			QTreeWidgetItem *chapPointDistanceInfoItem = new QTreeWidgetItem(chapPointDistanceItem, QStringList(text));
			chapPointDistanceItem->addChild(chapPointDistanceInfoItem);
			chapPointDistanceItem->setExpanded(true);

			QTreeWidgetItem *chapPointScoreItem = getChildItem(currentOptionTreeWidgetItem, 2514 + pointPareIndex * 25);
			removeChildItem(chapPointScoreItem);
			text = QString::number(aboChapPointPareScore[pointPareIndex]);
			QTreeWidgetItem *chapPointScoreInfoItem = new QTreeWidgetItem(chapPointScoreItem, QStringList(text));
			chapPointScoreItem->addChild(chapPointScoreInfoItem);
			chapPointScoreItem->setExpanded(true);

			QTreeWidgetItem *chapScoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 2503);
			removeChildItem(chapScoreItem);
			text = QString::number(aboChapScore);
			QTreeWidgetItem *chapScoreInfoItem = new QTreeWidgetItem(chapScoreItem, QStringList(text));
			chapScoreItem->addChild(chapScoreInfoItem);
			chapScoreItem->setExpanded(true);

			QTreeWidgetItem *aboSumScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 5000);
			updateAboSumScoreInfo(aboSumScoreItem);
		}

		++aboChapPointPickNum[pointPareIndex];

		this->pickStyle->mode = 0;
	}
	else if((selectOptionTreeIndex - 4510) >= 0 && (selectOptionTreeIndex - 4510) < 650 && (selectOptionTreeIndex - 4510) % 25 == 0)
	{
		//
		// ABO-OGS邻牙接触关系
		//		选点
		//

		int pointPareIndex = (selectOptionTreeIndex - 4510) / 25;

		// 点操作

		aboTouchRelationPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]][0] = x;
		aboTouchRelationPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]][1] = y;
		aboTouchRelationPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]][2] = z;

		vtkPlane::ProjectPoint(aboTouchRelationPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]],
			fitPlaneCenter, fitPlaneNormal,
			aboTouchRelationProjectPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]]);

		drawPoint(aboTouchRelationPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]], 
			pickPointColor, 
			aboTouchRelationPointAcotor[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]]);
		isAboTouchRelationPointShow[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]] = 1;

		drawPoint(aboTouchRelationProjectPoints[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]], 
			projectToFittingPlanePointColot, 
			aboTouchRelationProjectPointAcotor[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]]);
		isAboTouchRelationProjectPointShow[pointPareIndex][aboTouchRelationPointNum[pointPareIndex]] = 1;

		QTreeWidgetItem *pointItem = getChildItem(currentOptionTreeWidgetItem, 4511 + pointPareIndex * 25); 
		removeChildItem(pointItem);

		QTreeWidgetItem *measureProjectPointItem = getChildItem(currentOptionTreeWidgetItem, 4512 + pointPareIndex * 25); 
		removeChildItem(measureProjectPointItem);
		//measureProjectPointItem->setExpanded(true);

		currentOptionTreeWidgetItem->setExpanded(true);

		for(int i = 0; i <= aboTouchRelationPointNum[pointPareIndex]; ++i)
		{
			QString pointInfoItemText = QString("(").
				append(QString::number(aboTouchRelationPoints[pointPareIndex][i][0])).append(", ").
				append(QString::number(aboTouchRelationPoints[pointPareIndex][i][1])).append(", ").
				append(QString::number(aboTouchRelationPoints[pointPareIndex][i][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(pointInfoItemText));
			pointItem->addChild(pointInfoItem);
			pointItem->setExpanded(true);

			QString projectPointInfoItemText = QString("(").
				append(QString::number(aboTouchRelationProjectPoints[pointPareIndex][i][0])).append(", ").
				append(QString::number(aboTouchRelationProjectPoints[pointPareIndex][i][1])).append(", ").
				append(QString::number(aboTouchRelationProjectPoints[pointPareIndex][i][2])).append(")");
			//QMessageBox::information(NULL, tr("Info"), projectPointInfoItemText);
			QTreeWidgetItem *measureProjectPointInfoItem = new QTreeWidgetItem(measureProjectPointItem, QStringList(projectPointInfoItemText));
			measureProjectPointItem->addChild(measureProjectPointInfoItem);
			measureProjectPointItem->setExpanded(true);
		}

		if(aboTouchRelationPointNum[pointPareIndex] == 1)
		{
			aboTouchRelationMeasureDistance[pointPareIndex] = sqrt(vtkMath::Distance2BetweenPoints(aboTouchRelationProjectPoints[pointPareIndex][0], aboTouchRelationProjectPoints[pointPareIndex][1]));
			//aboTouchRelationMeasureScore[pointPareIndex] = aboTouchRelationMeasureDistance[pointPareIndex];
			
			double St = aboTouchRelationMeasureDistance[pointPareIndex];
			double SF = 0;
			if (St < 0.5)
			{
				SF = 0;
			}else if (St <= 1)
			{
				SF = 1;
			}else if (St > 1)
			{
				SF = 2;
			}
			//double ScoreT[28] = {};
			if (pointPareIndex <13)
			{
				aboTouchRelationScoreT[pointPareIndex] += SF;
				aboTouchRelationScoreT[pointPareIndex + 1] += SF;
			}else if (pointPareIndex >= 13)
			{
				aboTouchRelationScoreT[pointPareIndex + 1] += SF;
				aboTouchRelationScoreT[pointPareIndex + 2] += SF;
			}

			aboTouchRelationMeasureScore[pointPareIndex] = SF;


			aboTouchRelationScore = 0;
			for(int i = 0; i < 28; ++i)
			{
				if (aboTouchRelationScoreT[i] > 2)
				{
					aboTouchRelationScoreT[i] = 2;
				}
				aboTouchRelationScore += aboTouchRelationScoreT[i];
			}

			QTreeWidgetItem *measureDistanceItem = getChildItem(currentOptionTreeWidgetItem, 4513 + pointPareIndex * 25);
			removeChildItem(measureDistanceItem);
			QString measureDistanceItemText = QString::number(aboTouchRelationMeasureDistance[pointPareIndex]);
			//QMessageBox::information(NULL, tr("Info"), projectPointInfoItemText);
			QTreeWidgetItem *measureDistanceInfoItem = new QTreeWidgetItem(measureDistanceItem, QStringList(measureDistanceItemText));
			measureDistanceItem->addChild(measureDistanceInfoItem);
			measureDistanceItem->setExpanded(true);

			QTreeWidgetItem *measureScoreItem = getChildItem(currentOptionTreeWidgetItem, 4514 + pointPareIndex * 25);
			removeChildItem(measureScoreItem);
			QString measureScoreItemText = QString::number(aboTouchRelationMeasureScore[pointPareIndex]);
			QTreeWidgetItem *measureScoreInfoItem = new QTreeWidgetItem(measureScoreItem, QStringList(measureScoreItemText));
			measureScoreItem->addChild(measureScoreInfoItem);
			measureScoreItem->setExpanded(true);

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 4501);
			removeChildItem(scoreItem);
			QString scoreItemText = QString::number(aboTouchRelationScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(scoreItemText));
			scoreItem->addChild(scoreInfoItem);
			scoreItem->setExpanded(true);
		}

		++aboTouchRelationPointNum[pointPareIndex];

		this->pickStyle->mode = 0;
	}
	else if((selectOptionTreeIndex - 4018) >= 0 && (selectOptionTreeIndex - 4018) < 500 && (selectOptionTreeIndex - 4018) % 25 == 0)
	{
		//
		// ABO-OGS覆盖
		//		上牙选点
		//

		int pointPareIndex = (selectOptionTreeIndex - 4018) / 25;

		// 点操作

		aboCoverPoints[pointPareIndex][0][0] = x;
		aboCoverPoints[pointPareIndex][0][1] = y;
		aboCoverPoints[pointPareIndex][0][2] = z;

		drawPoint(aboCoverPoints[pointPareIndex][0], pickPointColor, aboCoverAcotor[pointPareIndex][0]);
		isAboCoverPointShow[pointPareIndex][0] = 1;

		removeChildItem(currentOptionTreeWidgetItem);
		QString pointInfoItemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(pointInfoItemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);

		aboTCoverMeasureDistance[pointPareIndex] = sqrt(vtkMath::Distance2BetweenPoints(aboCoverPoints[pointPareIndex][0], aboCoverPoints[pointPareIndex][1]));

		double St = aboTCoverMeasureDistance[pointPareIndex];
		if (St <= 0.5)
		{
			St = 0;
		}else if (St <= 1)
		{
			St = 1;
		}else if (St > 1)
		{
			St = 2;
		}

		aboCoverMeasureScore[pointPareIndex] = St;		
		aboCoverScore = 0;
		for(int i = 0; i < 18; ++i)
		{
			aboCoverScore += aboCoverMeasureScore[i];
		}

		QTreeWidgetItem *measureDistanceItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent()->parent(), 4019 + pointPareIndex * 25);
		removeChildItem(measureDistanceItem);
		pointInfoItemText = QString::number(aboTCoverMeasureDistance[pointPareIndex]);
		QTreeWidgetItem *measureDistanceInfoItem = new QTreeWidgetItem(measureDistanceItem, QStringList(pointInfoItemText));
		measureDistanceItem->addChild(measureDistanceInfoItem);
		measureDistanceItem->setExpanded(true);

		QTreeWidgetItem *measureScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent()->parent(), 4020 + pointPareIndex * 25);
		removeChildItem(measureScoreItem);
		pointInfoItemText = QString::number(aboCoverMeasureScore[pointPareIndex]);
		QTreeWidgetItem *measureScoreInfoItem = new QTreeWidgetItem(measureScoreItem, QStringList(pointInfoItemText));
		measureScoreItem->addChild(measureDistanceInfoItem);
		measureScoreItem->setExpanded(true);

		QTreeWidgetItem *coverScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent()->parent()->parent(), 4001);
		removeChildItem(coverScoreItem);
		pointInfoItemText = QString::number(aboCoverScore);
		QTreeWidgetItem *coverScoreInfoItem = new QTreeWidgetItem(coverScoreItem, QStringList(pointInfoItemText));
		coverScoreItem->addChild(coverScoreInfoItem);
		coverScoreItem->setExpanded(true);
	}
	else if((selectOptionTreeIndex - 4011) >= 0 && (selectOptionTreeIndex - 4011) < 500 && (selectOptionTreeIndex - 4011) % 25 == 0)
	{
		//
		// ABO-OGS覆盖
		//		下牙选点
		//

		int pointPareIndex = (selectOptionTreeIndex - 4011) / 25;

		// 点操作

		aboCoverPoints[pointPareIndex][1][0] = x;
		aboCoverPoints[pointPareIndex][1][1] = y;
		aboCoverPoints[pointPareIndex][1][2] = z;

		drawPoint(aboCoverPoints[pointPareIndex][1], pickPointColor, aboCoverAcotor[pointPareIndex][1]);
		isAboCoverPointShow[pointPareIndex][1] = 1;

		removeChildItem(currentOptionTreeWidgetItem);
		QString pointInfoItemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(pointInfoItemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		currentOptionTreeWidgetItem->setExpanded(true);

		// 面操作

		double fitPlaneProjectPoint[3];
		vtkPlane::ProjectPoint(aboCoverPoints[pointPareIndex][1],
			fitPlaneCenter, fitPlaneNormal,
			fitPlaneProjectPoint);
		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			fitPlaneProjectPoint,
			25 * pointSize,
			aboCoverPlaneOCenter[pointPareIndex], aboCoverPlaneONormal[pointPareIndex], aboCoverPlaneOPoint[pointPareIndex]);
		drawPlane(aboCoverPlaneOPoint[pointPareIndex][0], aboCoverPlaneOPoint[pointPareIndex][1], aboCoverPlaneOPoint[pointPareIndex][2], 
			planeColor, 0.4, aboCoverPlaneOActor[pointPareIndex], frameColor, 2, 2, aboCoverPlaneOWireframeActor[pointPareIndex]);
		isAboCoverPlaneOShow[pointPareIndex] = 1;

		QTreeWidgetItem *planeOItem = getChildItem(currentOptionTreeWidgetItem->parent(), 4012 + pointPareIndex * 25);
		planeOItem->setExpanded(true);

		QTreeWidgetItem *planeOPointItem = getChildItem(planeOItem, 4013 + pointPareIndex * 25);
		removeChildItem(planeOPointItem);
		pointInfoItemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *planeOPointInfoItem = new QTreeWidgetItem(planeOPointItem, QStringList(pointInfoItemText));
		planeOPointItem->addChild(planeOPointInfoItem);
		planeOPointItem->setExpanded(true);

		QTreeWidgetItem *planeONormalItem = getChildItem(planeOItem, 4014 + pointPareIndex * 25);
		removeChildItem(planeONormalItem);
		pointInfoItemText = QString("(").
			append(QString::number(aboCoverPlaneONormal[pointPareIndex][0])).append(", ").
			append(QString::number(aboCoverPlaneONormal[pointPareIndex][1])).append(", ").
			append(QString::number(aboCoverPlaneONormal[pointPareIndex][2])).append(")");
		QTreeWidgetItem *planeONormalInfoItem = new QTreeWidgetItem(planeONormalItem, QStringList(pointInfoItemText));
		planeONormalItem->addChild(planeONormalInfoItem);
		planeONormalItem->setExpanded(true);

		// 轮廓线
		drawIntersectCurve(lowerToothData, 
			aboCoverPlaneOCenter[pointPareIndex], aboCoverPlaneONormal[pointPareIndex], 
			intersectCurveColor, 2, 
			aboCoverLowIntersectCurveActor[pointPareIndex]);
		drawIntersectCurve(topToothData, 
			aboCoverPlaneOCenter[pointPareIndex], aboCoverPlaneONormal[pointPareIndex], 
			intersectCurveColor, 2, 
			aboCoverIntersectCurveActor[pointPareIndex]);
		isCoverIntersectCurveShow[pointPareIndex][0] = isCoverIntersectCurveShow[pointPareIndex][1] = 1;

		this->pickStyle->mode = 0;

		double pB[3] = {};
		double ProCenterB[3] = {};
		findUpperConvexPoint(aboCoverIntersectCurveActor[pointPareIndex],aboCoverPoints[pointPareIndex][1],pB,ProCenterB);//最凹点求解	（最近点）	

		currentOptionTreeWidgetItem = getOptionTreeByData(4018 + 25 * pointPareIndex);
		selectOptionTreeIndex = 4018 + 25 * pointPareIndex;
		setPickPointPosition(pB[0],pB[1],pB[2]);
	}
	else if((selectOptionTreeIndex - 3510) >= 0 && (selectOptionTreeIndex - 3510) < 500 && (selectOptionTreeIndex - 3510) % 25 == 0)
	{
		//
		// ABO-OGS牙合接触
		//		选点
		//

		currentOptionTreeWidgetItem->setExpanded(true);

		int pointPareIndex = (selectOptionTreeIndex - 3510) / 25;

		// 点操作

		aboToothCloseTouchPoints[pointPareIndex][0][0] = x;
		aboToothCloseTouchPoints[pointPareIndex][0][1] = y;
		aboToothCloseTouchPoints[pointPareIndex][0][2] = z;

		drawPoint(aboToothCloseTouchPoints[pointPareIndex][0], pickPointColor, aboToothCloseTouchPointAcotor[pointPareIndex][0]);
		isAboToothCloseTouchPointShow[pointPareIndex][0] = 1;

		QTreeWidgetItem *pointItem = getChildItem(currentOptionTreeWidgetItem, 3511 + pointPareIndex * 25);
		removeChildItem(pointItem);
		QString pointInfoItemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(pointInfoItemText));
		pointItem->addChild(pointInfoItem);
		pointItem->setExpanded(true);

		// 面操作

		double fitPlaneProjectPoint[3];
		vtkPlane::ProjectPoint(aboToothCloseTouchPoints[pointPareIndex][0],
			fitPlaneCenter, fitPlaneNormal,
			fitPlaneProjectPoint);
		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			fitPlaneProjectPoint,
			25 * pointSize,
			aboToothCloseTouchPlaneOCenter[pointPareIndex], aboToothCloseTouchPlaneONormal[pointPareIndex], aboToothCloseTouchPlaneOPoint[pointPareIndex]);
		drawPlane(aboToothCloseTouchPlaneOPoint[pointPareIndex][0], aboToothCloseTouchPlaneOPoint[pointPareIndex][1], aboToothCloseTouchPlaneOPoint[pointPareIndex][2], 
			planeColor, 0.4, aboToothCloseTouchPlaneOActor[pointPareIndex], frameColor, 2, 2, aboToothCloseTouchPlaneOWireframeActor[pointPareIndex]);
		isAboToothCloseTouchPlaneOShow[pointPareIndex] = 1;

		QTreeWidgetItem *planeOItem = getChildItem(currentOptionTreeWidgetItem, 3512 + pointPareIndex * 25);
		planeOItem->setExpanded(true);

		QTreeWidgetItem *planeOPointItem = getChildItem(planeOItem, 3513 + pointPareIndex * 25);
		removeChildItem(planeOPointItem);
		pointInfoItemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *planeOPointInfoItem = new QTreeWidgetItem(planeOPointItem, QStringList(pointInfoItemText));
		planeOPointItem->addChild(planeOPointInfoItem);
		planeOPointItem->setExpanded(true);

		QTreeWidgetItem *planeONormalItem = getChildItem(planeOItem, 3514 + pointPareIndex * 25);
		removeChildItem(planeONormalItem);
		pointInfoItemText = QString("(").
			append(QString::number(aboToothCloseTouchPlaneONormal[pointPareIndex][0])).append(", ").
			append(QString::number(aboToothCloseTouchPlaneONormal[pointPareIndex][1])).append(", ").
			append(QString::number(aboToothCloseTouchPlaneONormal[pointPareIndex][2])).append(")");
		QTreeWidgetItem *planeONormalInfoItem = new QTreeWidgetItem(planeONormalItem, QStringList(pointInfoItemText));
		planeONormalItem->addChild(planeONormalInfoItem);
		planeONormalItem->setExpanded(true);

		// 轮廓线
		drawIntersectCurve(lowerToothData, 
			aboToothCloseTouchPlaneOCenter[pointPareIndex], aboToothCloseTouchPlaneONormal[pointPareIndex], 
			intersectCurveColor, 2, 
			aboToothCloseTouchLowIntersectCurveActor[pointPareIndex]);
		drawIntersectCurve(topToothData, 
			aboToothCloseTouchPlaneOCenter[pointPareIndex], aboToothCloseTouchPlaneONormal[pointPareIndex], 
			intersectCurveColor, 2, 
			aboToothCloseTouchTopIntersectCurveActor[pointPareIndex]);
		isAboToothCloseTouchIntersectCurveShow[pointPareIndex] = 1;

		vtkSmartPointer<vtkKdTree> pointTree = vtkSmartPointer<vtkKdTree>::New();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkDataSet *lowCurveDataSet = aboToothCloseTouchLowIntersectCurveActor[pointPareIndex]->GetMapper()->GetInput();
		vtkIdType pointNum = lowCurveDataSet->GetNumberOfPoints();
		//QMessageBox::information(NULL, tr("Info"), QString::number(pointNum));
		for(vtkIdType i = 0; i < pointNum; ++i)
		{
			double pointPosition[3];
			lowCurveDataSet->GetPoint(i, pointPosition);
			points->InsertNextPoint(pointPosition);
		}
		vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
		polydata->SetPoints(points);
		vtkSmartPointer<vtkKdTreePointLocator> kDTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
		kDTree->SetDataSet(polydata);
		kDTree->BuildLocator();
		double closestPointDist;
		vtkIdType id = kDTree->FindClosestPoint(aboToothCloseTouchPoints[pointPareIndex][0]);
		double closestPoint[3];
		kDTree->GetDataSet()->GetPoint(id,closestPoint);
		aboToothCloseTouchPoints[pointPareIndex][1][0] = closestPoint[0];
		aboToothCloseTouchPoints[pointPareIndex][1][1] = closestPoint[1];
		aboToothCloseTouchPoints[pointPareIndex][1][2] = closestPoint[2];
		drawPoint(aboToothCloseTouchPoints[pointPareIndex][1], pickPointColor, aboToothCloseTouchPointAcotor[pointPareIndex][1]);
		isAboToothCloseTouchPointShow[pointPareIndex][1] = 1;

		QTreeWidgetItem *pointBItem = getChildItem(currentOptionTreeWidgetItem, 3516 + pointPareIndex * 25);
		removeChildItem(pointBItem);
		pointInfoItemText = QString("(").
			append(QString::number(aboToothCloseTouchPoints[pointPareIndex][1][0])).append(", ").
			append(QString::number(aboToothCloseTouchPoints[pointPareIndex][1][1])).append(", ").
			append(QString::number(aboToothCloseTouchPoints[pointPareIndex][1][2])).append(")");
		QTreeWidgetItem *pointBInfoItem = new QTreeWidgetItem(pointBItem, QStringList(pointInfoItemText));
		pointBItem->addChild(pointBInfoItem);
		pointBItem->setExpanded(true);

		aboToothCloseTouchMeasureDistance[pointPareIndex] = sqrt(vtkMath::Distance2BetweenPoints(aboToothCloseTouchPoints[pointPareIndex][0], aboToothCloseTouchPoints[pointPareIndex][1]));
		double St = aboToothCloseTouchMeasureDistance[pointPareIndex];
		if (St <= 0.5)
		{
			St = 0;
		}else if (St <= 1)
		{
			St = 1;
		}else if (St > 1)
		{
			St = 2;
		}
		aboToothCloseTouchMeasureScore[pointPareIndex] = St;
		aboToothCloseTouchScore = 0;
		for(int i = 0; i < 20; ++i)
		{
			aboToothCloseTouchScore += aboToothCloseTouchMeasureScore[i];;
		}

		QTreeWidgetItem *measureDistanceItem = getChildItem(currentOptionTreeWidgetItem, 3517 + pointPareIndex * 25);
		removeChildItem(measureDistanceItem);
		pointInfoItemText = QString::number(aboToothCloseTouchMeasureDistance[pointPareIndex]);
		QTreeWidgetItem *measureDistanceInfoItem = new QTreeWidgetItem(measureDistanceItem, QStringList(pointInfoItemText));
		measureDistanceItem->addChild(measureDistanceInfoItem);
		measureDistanceItem->setExpanded(true);

		QTreeWidgetItem *measureScoreItem = getChildItem(currentOptionTreeWidgetItem, 3518 + pointPareIndex * 25);
		removeChildItem(measureScoreItem);
		pointInfoItemText = QString::number(aboToothCloseTouchMeasureScore[pointPareIndex]);
		QTreeWidgetItem *measureScoreInfoItem = new QTreeWidgetItem(measureScoreItem, QStringList(pointInfoItemText));
		measureScoreItem->addChild(measureScoreInfoItem);
		measureScoreItem->setExpanded(true);

		QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 3501);
		removeChildItem(scoreItem);
		pointInfoItemText = QString::number(aboToothCloseTouchScore);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(pointInfoItemText));
		scoreItem->addChild(scoreInfoItem);
		scoreItem->setExpanded(true);

		this->pickStyle->mode = 0;
	}
	else if((selectOptionTreeIndex - 3010) >= 0 && (selectOptionTreeIndex - 3010) < 500 && (selectOptionTreeIndex - 3010) % 25 == 0)
	{
		//
		// ABO-OGS牙合关系
		//		选点
		//

		currentOptionTreeWidgetItem->setExpanded(true);

		int pointPareIndex = (selectOptionTreeIndex - 3010) / 25;

		// 点操作

		aboToothCloseRelationPoints[pointPareIndex][aboToothCloseRelationPointPickNum[pointPareIndex]][0] = x;
		aboToothCloseRelationPoints[pointPareIndex][aboToothCloseRelationPointPickNum[pointPareIndex]][1] = y;
		aboToothCloseRelationPoints[pointPareIndex][aboToothCloseRelationPointPickNum[pointPareIndex]][2] = z;

		drawPoint(aboToothCloseRelationPoints[pointPareIndex][aboToothCloseRelationPointPickNum[pointPareIndex]], pickPointColor, 
			aboToothCloseRelationPointAcotor[pointPareIndex][aboToothCloseRelationPointPickNum[pointPareIndex]]);
		isAboToothCloseRelationPointShow[pointPareIndex][aboToothCloseRelationPointPickNum[pointPareIndex]] = 1;

		QTreeWidgetItem *toothItem = NULL;
		QTreeWidgetItem *pointItem = NULL;
		if(aboToothCloseRelationPointPickNum[pointPareIndex] == 0)
		{
			toothItem = getChildItem(currentOptionTreeWidgetItem, 3011 + pointPareIndex * 25);
			pointItem = getChildItem(toothItem, 3012 + pointPareIndex * 25);
		}
		else if(aboToothCloseRelationPointPickNum[pointPareIndex] == 1)
		{
			toothItem = getChildItem(currentOptionTreeWidgetItem, 3016 + pointPareIndex * 25);
			pointItem = getChildItem(toothItem, 3017 + pointPareIndex * 25);
		}

		toothItem->setExpanded(true);
		removeChildItem(pointItem);
		QString pointInfoItemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(pointInfoItemText));
		pointItem->addChild(pointInfoItem);
		pointItem->setExpanded(true);

		if(aboToothCloseRelationPointPickNum[pointPareIndex] == 0)
		{
			// 面
			QTreeWidgetItem *planeOItem = getChildItem(toothItem, 3013 + pointPareIndex * 25);
			planeOItem->setExpanded(true);

			QTreeWidgetItem *initPointItem = getChildItem(planeOItem, 3014 + pointPareIndex * 25);
			removeChildItem(initPointItem);
			pointInfoItemText = QString("(").
				append(QString::number(x)).append(", ").
				append(QString::number(y)).append(", ").
				append(QString::number(z)).append(")");
			QTreeWidgetItem *initPointInfoItem = new QTreeWidgetItem(initPointItem, QStringList(pointInfoItemText));
			initPointItem->addChild(initPointInfoItem);
			initPointItem->setExpanded(true);

			double fitPlaneProjectPoint[3];
			vtkPlane::ProjectPoint(aboToothCloseRelationPoints[pointPareIndex][0],
				fitPlaneCenter, fitPlaneNormal,
				fitPlaneProjectPoint);
			initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
				splineProjectPointPosition, &splinePointState[0], 20,
				fitPlaneProjectPoint,
				25 * pointSize,
				aboToothCloseRelationPlaneOCenter[pointPareIndex], aboToothCloseRelationPlaneONormal[pointPareIndex], aboToothCloseRelationPlaneOPoint[pointPareIndex]);
			drawPlane(aboToothCloseRelationPlaneOPoint[pointPareIndex][0], aboToothCloseRelationPlaneOPoint[pointPareIndex][1], aboToothCloseRelationPlaneOPoint[pointPareIndex][2], 
				planeColor, 0.4, aboToothCloseRelationPlaneOActor[pointPareIndex], frameColor, 2, 2, aboToothCloseRelationPlaneOWireframeActor[pointPareIndex]);
			isAboToothCloseRelationPlaneOShow[pointPareIndex] = 1;

			QTreeWidgetItem *normalItem = getChildItem(planeOItem, 3015 + pointPareIndex * 25);
			removeChildItem(normalItem);
			pointInfoItemText = QString("(").
				append(QString::number(aboToothCloseRelationPlaneONormal[pointPareIndex][0])).append(", ").
				append(QString::number(aboToothCloseRelationPlaneONormal[pointPareIndex][1])).append(", ").
				append(QString::number(aboToothCloseRelationPlaneONormal[pointPareIndex][2])).append(")");
			QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(pointInfoItemText));
			normalItem->addChild(normalInfoItem);
			normalItem->setExpanded(true);
		}

		if(aboToothCloseRelationPointPickNum[pointPareIndex] == 1)
		{
			double projectPoint[3];
			vtkPlane::ProjectPoint(aboToothCloseRelationPoints[pointPareIndex][1],
				aboToothCloseRelationPlaneOCenter[pointPareIndex], aboToothCloseRelationPlaneONormal[pointPareIndex],
				projectPoint);
			aboToothCloseRelationPointPlaneDistance[pointPareIndex] = sqrt(vtkMath::Distance2BetweenPoints(aboToothCloseRelationPoints[pointPareIndex][1], projectPoint));
			double St = aboToothCloseRelationPointPlaneDistance[pointPareIndex];
			if (St < 1)
			{
				St = 0;
			}else if (St < 2)
			{
				St = 1;
			}else if (St >= 2)
			{
				St = 2;
			}
			aboToothCloseRelationMeasureScore[pointPareIndex] = St;
			aboToothCloseRelationScore = 0;
			for(int i = 0; i < 10; ++i)
			{
				aboToothCloseRelationScore += aboToothCloseRelationMeasureScore[i];
			}

			QTreeWidgetItem *measureDistanceItem = getChildItem(currentOptionTreeWidgetItem, 3018 + pointPareIndex * 25);
			removeChildItem(measureDistanceItem);
			pointInfoItemText = QString::number(aboToothCloseRelationPointPlaneDistance[pointPareIndex]);
			QTreeWidgetItem *measureDistanceInfoItem = new QTreeWidgetItem(measureDistanceItem, QStringList(pointInfoItemText));
			measureDistanceItem->addChild(measureDistanceInfoItem);
			measureDistanceItem->setExpanded(true);

			QTreeWidgetItem *measureScoreItem = getChildItem(currentOptionTreeWidgetItem, 3019 + pointPareIndex * 25);
			removeChildItem(measureScoreItem);
			pointInfoItemText = QString::number(aboToothCloseRelationMeasureScore[pointPareIndex]);
			QTreeWidgetItem *measureScoreInfoItem = new QTreeWidgetItem(measureScoreItem, QStringList(pointInfoItemText));
			measureScoreItem->addChild(measureScoreInfoItem);
			measureScoreItem->setExpanded(true);

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 3001);
			removeChildItem(scoreItem);
			pointInfoItemText = QString::number(aboToothCloseRelationScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(pointInfoItemText));
			scoreItem->addChild(scoreInfoItem);
			scoreItem->setExpanded(true);
		}

		++aboToothCloseRelationPointPickNum[pointPareIndex];

		this->pickStyle->mode = 0;
	}
	else if((selectOptionTreeIndex - 2010) >= 0 && (selectOptionTreeIndex - 2010) < 500 && (selectOptionTreeIndex - 2010) % 25 == 0)
	{
		//
		// ABO-OGS边缘嵴高度
		//		选点
		//

		currentOptionTreeWidgetItem->setExpanded(true);

		int pointPareIndex = (selectOptionTreeIndex - 2010) / 25;

		// 点操作

		aboEdgeVertebralHeighPoints[pointPareIndex][aboEdgeVertebralHeighPointPickNum[pointPareIndex]][0] = x;
		aboEdgeVertebralHeighPoints[pointPareIndex][aboEdgeVertebralHeighPointPickNum[pointPareIndex]][1] = y;
		aboEdgeVertebralHeighPoints[pointPareIndex][aboEdgeVertebralHeighPointPickNum[pointPareIndex]][2] = z;

		drawPoint(aboEdgeVertebralHeighPoints[pointPareIndex][aboEdgeVertebralHeighPointPickNum[pointPareIndex]], pickPointColor, 
			aboEdgeVertebralHeighPointAcotor[pointPareIndex][aboEdgeVertebralHeighPointPickNum[pointPareIndex]]);
		isAboEdgeVertebralHeighPointShow[pointPareIndex][aboEdgeVertebralHeighPointPickNum[pointPareIndex]] = 1;

		QTreeWidgetItem *pointItem = NULL;
		if(aboEdgeVertebralHeighPointPickNum[pointPareIndex] == 0)
		{
			pointItem = getChildItem(currentOptionTreeWidgetItem, 2011 + pointPareIndex * 25);
		}
		else if(aboEdgeVertebralHeighPointPickNum[pointPareIndex] == 1)
		{
			pointItem = getChildItem(currentOptionTreeWidgetItem, 2012 + pointPareIndex * 25);
		}
		if(pointItem)
		{
			removeChildItem(pointItem);
			QString pointInfoItemText = QString("(").
				append(QString::number(x)).append(", ").
				append(QString::number(y)).append(", ").
				append(QString::number(z)).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(pointInfoItemText));
			pointItem->addChild(pointInfoItem);
			pointItem->setExpanded(true);

			if(aboEdgeVertebralHeighPointPickNum[pointPareIndex] == 1) 
			{
				// 投影操作

				QTreeWidgetItem *projectResultItem = getChildItem(currentOptionTreeWidgetItem, 2013 + pointPareIndex * 25);
				projectResultItem->setExpanded(true);
				QTreeWidgetItem *projectResultMiddlePlaneItem = getChildItem(projectResultItem, 2014 + pointPareIndex * 25);
				projectResultMiddlePlaneItem->setExpanded(true);
				QTreeWidgetItem *projectResultArticlePlaneItem = getChildItem(projectResultItem, 2017 + pointPareIndex * 25);
				projectResultArticlePlaneItem->setExpanded(true);
				QTreeWidgetItem *middleProjectPointItem = NULL;
				QTreeWidgetItem *articleProjectPointItem = NULL;

				for(int i = 0; i < 2; ++i)
				{
					vtkPlane::ProjectPoint(aboEdgeVertebralHeighPoints[pointPareIndex][i],
						verticalPlaneCenter, verticalPlaneNormal,
						aboEdgeVertebralHeighPointProjectToMiddlePlane[pointPareIndex][i]);
					vtkPlane::ProjectPoint(aboEdgeVertebralHeighPointProjectToMiddlePlane[pointPareIndex][i],
						articlePlanePoint[0], articlePlaneNormal,
						aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][i]);

					drawPoint(aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][i], 
						projectToFittingPlanePointColot, 
						aboEdgeVertebralHeighPointProjectToArticlePlaneActor[pointPareIndex][i]);
					isAboEdgeVertebralHeighProjectPointShow[pointPareIndex][i] = 1;

					if(isArticlePlaneShow == 0)
						switchArticlePlaneShow();

					if(i == 0)
					{
						middleProjectPointItem = getChildItem(projectResultMiddlePlaneItem, 2015 + pointPareIndex * 25);
						articleProjectPointItem = getChildItem(projectResultArticlePlaneItem, 2018 + pointPareIndex * 25);
					}
					else if(i == 1)
					{
						middleProjectPointItem = getChildItem(projectResultMiddlePlaneItem, 2016 + pointPareIndex * 25);
						articleProjectPointItem = getChildItem(projectResultArticlePlaneItem, 2019 + pointPareIndex * 25);
					}

					removeChildItem(middleProjectPointItem);
					pointInfoItemText = QString("(").
						append(QString::number(aboEdgeVertebralHeighPointProjectToMiddlePlane[pointPareIndex][i][0])).append(", ").
						append(QString::number(aboEdgeVertebralHeighPointProjectToMiddlePlane[pointPareIndex][i][1])).append(", ").
						append(QString::number(aboEdgeVertebralHeighPointProjectToMiddlePlane[pointPareIndex][i][2])).append(")");
					QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(middleProjectPointItem, QStringList(pointInfoItemText));
					middleProjectPointItem->addChild(projectPointInfoItem);
					middleProjectPointItem->setExpanded(true);

					removeChildItem(articleProjectPointItem);
					pointInfoItemText = QString("(").
						append(QString::number(aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][i][0])).append(", ").
						append(QString::number(aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][i][1])).append(", ").
						append(QString::number(aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][i][2])).append(")");
					QTreeWidgetItem *articleProjectPointInfoItem = new QTreeWidgetItem(articleProjectPointItem, QStringList(pointInfoItemText));
					articleProjectPointItem->addChild(articleProjectPointInfoItem);
					articleProjectPointItem->setExpanded(true);
				}

				aboEdgeVertebralHeighPointPareDistance[pointPareIndex] = sqrt(vtkMath::Distance2BetweenPoints(aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][0], aboEdgeVertebralHeighPointProjectToArticlePlane[pointPareIndex][1]));
				double St = aboEdgeVertebralHeighPointPareDistance[pointPareIndex];
				double SF = 0;
				if (St < 0.5)
				{
					SF = 0;
				}else if (St <= 1)
				{
					SF = 1;
				}else if (St > 1)
				{
					SF = -2;
				}
				//double ScoreT[28] = {};
				int aboEdgeToothIndex[24] = {0,1,1,2,2,3,10,11,11,12,12,13,14,15,15,16,16,17,	24,25,25,26,26,27};
				aboEdgeVertebralScoreT[aboEdgeToothIndex[pointPareIndex * 2]] += SF;
				aboEdgeVertebralScoreT[aboEdgeToothIndex[pointPareIndex * 2 + 1]] += SF;

				aboEdgeVertebralHeighPointPareScore[pointPareIndex] = SF;
				aboEdgeVertebralHeighScore = 0;
				for(int i = 0; i < 20; ++i)
				{
					if (aboEdgeVertebralHeighPointPareScore[i] < -2)
					{
						aboEdgeVertebralHeighPointPareScore[i] = -2;
					}
					aboEdgeVertebralHeighScore += aboEdgeVertebralHeighPointPareScore[i];
				}

				QTreeWidgetItem *pointPairDistanceItem = getChildItem(projectResultArticlePlaneItem, 2020 + pointPareIndex * 25);
				removeChildItem(pointPairDistanceItem);
				pointInfoItemText = QString::number(aboEdgeVertebralHeighPointPareDistance[pointPareIndex]);
				QTreeWidgetItem *pointPairDistanceInfoItem = new QTreeWidgetItem(pointPairDistanceItem, QStringList(pointInfoItemText));
				pointPairDistanceItem->addChild(pointPairDistanceInfoItem);
				pointPairDistanceItem->setExpanded(true);

				QTreeWidgetItem *pointPairScoreItem = getChildItem(currentOptionTreeWidgetItem, 2021 + pointPareIndex * 25);
				removeChildItem(pointPairScoreItem);
				pointInfoItemText = QString::number(aboEdgeVertebralHeighPointPareScore[pointPareIndex]);
				QTreeWidgetItem *pointPairScoreItemInfoItem = new QTreeWidgetItem(pointPairScoreItem, QStringList(pointInfoItemText));
				pointPairScoreItem->addChild(pointPairScoreItemInfoItem);
				pointPairScoreItem->setExpanded(true);

				QTreeWidgetItem *edgeVertebralHeighScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 2002);
				removeChildItem(edgeVertebralHeighScoreItem);
				pointInfoItemText = QString::number(aboEdgeVertebralHeighScore);
				QTreeWidgetItem *edgeVertebralHeighScoreInfoItem = new QTreeWidgetItem(edgeVertebralHeighScoreItem, QStringList(pointInfoItemText));
				edgeVertebralHeighScoreItem->addChild(edgeVertebralHeighScoreInfoItem);
				edgeVertebralHeighScoreItem->setExpanded(true);

				QTreeWidgetItem *aboSumScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent()->parent(), 5000);
				updateAboSumScoreInfo(aboSumScoreItem);
			}
		}

		++aboEdgeVertebralHeighPointPickNum[pointPareIndex];

		this->pickStyle->mode = 0;
	}
	/*else if((selectOptionTreeIndex - 1510) >= 0 && (selectOptionTreeIndex - 1510) < 500 && (selectOptionTreeIndex - 1510) % 25 == 0)*/
	else if(selectOptionTreeIndex / 100 == 1510)
	{
		//
		// ABO-OGS排列
		//		选点
		//

		currentOptionTreeWidgetItem->setExpanded(true);

		int pointPareIndex = selectOptionTreeIndex % 100;

		// 点操作

		aboPoints[pointPareIndex][aboArrangePointPickNum[pointPareIndex]][0] = x;
		aboPoints[pointPareIndex][aboArrangePointPickNum[pointPareIndex]][1] = y;
		aboPoints[pointPareIndex][aboArrangePointPickNum[pointPareIndex]][2] = z;

		drawPoint(aboPoints[pointPareIndex][aboArrangePointPickNum[pointPareIndex]], pickPointColor, aboPointAcotor[pointPareIndex][aboArrangePointPickNum[pointPareIndex]]);
		isAboArrangePointShow[pointPareIndex][aboArrangePointPickNum[pointPareIndex]] = 1;

		QTreeWidgetItem *pointItem = NULL;
		if(aboArrangePointPickNum[pointPareIndex] == 0)
		{
			pointItem = getChildItem(currentOptionTreeWidgetItem, 1511 * 100 + pointPareIndex);
		}
		else if(aboArrangePointPickNum[pointPareIndex] == 1)
		{
			pointItem = getChildItem(currentOptionTreeWidgetItem, 1512 * 100 + pointPareIndex);
		}
		if(pointItem)
		{
			removeChildItem(pointItem);

			QString pointInfoItemText = QString("(").
				append(QString::number(x)).append(", ").
				append(QString::number(y)).append(", ").
				append(QString::number(z)).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(pointInfoItemText));
			pointItem->addChild(pointInfoItem);

			pointItem->setExpanded(true);

			if(aboArrangePointPickNum[pointPareIndex] == 1)
			{
				// 面操作

				QTreeWidgetItem *planeItem = getChildItem(currentOptionTreeWidgetItem, 1513 * 100 + pointPareIndex);
				if(planeItem)
				{
					planeItem->setExpanded(true);
					pointItem = getChildItem(planeItem, 1514 * 100 + pointPareIndex);
					if(pointItem)
					{
						removeChildItem(pointItem);

						pointInfoItemText = QString("(").
							append(QString::number(aboPoints[pointPareIndex][0][0])).append(", ").
							append(QString::number(aboPoints[pointPareIndex][0][1])).append(", ").
							append(QString::number(aboPoints[pointPareIndex][0][2])).append(")");
						QTreeWidgetItem *initInfoItem = new QTreeWidgetItem(pointItem, QStringList(pointInfoItemText));
						pointItem->addChild(initInfoItem);

						pointItem->setExpanded(true);
					}

					double fitPlaneProjectPoint[3];
					vtkPlane::ProjectPoint(aboPoints[pointPareIndex][0],
						fitPlaneCenter, fitPlaneNormal,
						fitPlaneProjectPoint);

					initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
						splineProjectPointPosition, &splinePointState[0], 20,
						fitPlaneProjectPoint,
						25 * pointSize,
						aboArrangePlaneOCenter[pointPareIndex], aboArrangePlaneONormal[pointPareIndex], aboArrangePlaneOPoint[pointPareIndex]);
					drawPlane(aboArrangePlaneOPoint[pointPareIndex][0], aboArrangePlaneOPoint[pointPareIndex][1], aboArrangePlaneOPoint[pointPareIndex][2], 
						planeColor, 0.4, aboArrangePlaneOActor[pointPareIndex], frameColor, 2, 2, aboArrangePlaneOWireframeActor[pointPareIndex]);
					isAboArrangePlaneOShow[pointPareIndex] = 1;

					QTreeWidgetItem *planeNormalItem = getChildItem(planeItem, 1515 * 100 + pointPareIndex);
					if(planeNormalItem)
					{
						removeChildItem(planeNormalItem);

						pointInfoItemText = QString("(").
							append(QString::number(aboArrangePlaneONormal[pointPareIndex][0])).append(", ").
							append(QString::number(aboArrangePlaneONormal[pointPareIndex][1])).append(", ").
							append(QString::number(aboArrangePlaneONormal[pointPareIndex][2])).append(")");
						QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(planeNormalItem, QStringList(pointInfoItemText));
						planeNormalItem->addChild(normalInfoItem);

						planeNormalItem->setExpanded(true);
					}

					// 投影操作

					for(int i = 0; i < 2; ++i)
					{
						vtkPlane::ProjectPoint(aboPoints[pointPareIndex][i],
							aboArrangePlaneOCenter[pointPareIndex], aboArrangePlaneONormal[pointPareIndex],
							aboArrangePointProjectToPlaneO[pointPareIndex][i]);
						vtkPlane::ProjectPoint(aboArrangePointProjectToPlaneO[pointPareIndex][i],
							fitPlaneCenter, fitPlaneNormal,
							aboArrangePointProjectToFitPlanePoint[pointPareIndex][i]);

						drawPoint(aboArrangePointProjectToFitPlanePoint[pointPareIndex][i], projectToFittingPlanePointColot, aboArrangePointProjectToFitPlane[pointPareIndex][i]);
						isAboArrangeProjectPointShow[pointPareIndex][i] = 1;

						if(isFitPlaneShow == 0)
							switchFitPlaneShow();
					}

					aboArrangePointPareDistance[pointPareIndex] = sqrt(vtkMath::Distance2BetweenPoints(aboArrangePointProjectToFitPlanePoint[pointPareIndex][0], aboArrangePointProjectToFitPlanePoint[pointPareIndex][1]));
					double St = aboArrangePointPareDistance[pointPareIndex];
					double SF = 0;
					
					if (St < 0.5)
					{
						SF = 0;
					}else if (St < 1)
					{
						SF = 1;
					}else if (St >= 1)
					{
						SF = 2;
					}
					if (pointPareIndex <13)
					{
						aboArrangeScoreT[pointPareIndex] += SF;
						aboArrangeScoreT[pointPareIndex + 1] += SF;
					}else if (pointPareIndex >= 13)
					{
						aboArrangeScoreT[pointPareIndex + 1] += SF;
						aboArrangeScoreT[pointPareIndex + 2] += SF;
					}

					aboArrangePointPareScore[pointPareIndex] = SF;
					aboArrangeScore = 0;
					for(int i = 0; i < 28; ++i)
					{
						if (aboArrangeScoreT[i] > 2)
						{
							aboArrangeScoreT[i] = 2;
						}
						aboArrangeScore += aboArrangeScoreT[i];
					}

					QTreeWidgetItem *planeProjectItem = getChildItem(planeItem, 1516 * 100 + pointPareIndex);
					planeProjectItem->setExpanded(true);

					QTreeWidgetItem *planeProjectPointAItem = getChildItem(planeProjectItem, 1517 * 100 + pointPareIndex);
					removeChildItem(planeProjectPointAItem);
					pointInfoItemText = QString("(").
						append(QString::number(aboArrangePointProjectToPlaneO[pointPareIndex][0][0])).append(", ").
						append(QString::number(aboArrangePointProjectToPlaneO[pointPareIndex][0][1])).append(", ").
						append(QString::number(aboArrangePointProjectToPlaneO[pointPareIndex][0][2])).append(")");
					QTreeWidgetItem *planeProjectPointAInfoItem = new QTreeWidgetItem(planeProjectPointAItem, QStringList(pointInfoItemText));
					planeProjectPointAItem->addChild(planeProjectPointAInfoItem);
					planeProjectPointAItem->setExpanded(true);

					QTreeWidgetItem *planeProjectPointBItem = getChildItem(planeProjectItem, 1518 * 100 + pointPareIndex);
					removeChildItem(planeProjectPointBItem);
					pointInfoItemText = QString("(").
						append(QString::number(aboArrangePointProjectToPlaneO[pointPareIndex][1][0])).append(", ").
						append(QString::number(aboArrangePointProjectToPlaneO[pointPareIndex][1][1])).append(", ").
						append(QString::number(aboArrangePointProjectToPlaneO[pointPareIndex][1][2])).append(")");
					QTreeWidgetItem *planeProjectPointBInfoItem = new QTreeWidgetItem(planeProjectPointBItem, QStringList(pointInfoItemText));
					planeProjectPointBItem->addChild(planeProjectPointBInfoItem);
					planeProjectPointBItem->setExpanded(true);

					QTreeWidgetItem *projectPointItem = getChildItem(currentOptionTreeWidgetItem, 1519 * 100 + pointPareIndex);
					projectPointItem->setExpanded(true);

					QTreeWidgetItem *projectPointAItem = getChildItem(projectPointItem, 1520 * 100 + pointPareIndex);
					removeChildItem(projectPointAItem);
					pointInfoItemText = QString("(").
						append(QString::number(aboArrangePointProjectToFitPlanePoint[pointPareIndex][0][0])).append(", ").
						append(QString::number(aboArrangePointProjectToFitPlanePoint[pointPareIndex][0][1])).append(", ").
						append(QString::number(aboArrangePointProjectToFitPlanePoint[pointPareIndex][0][2])).append(")");
					QTreeWidgetItem *projectPointAInfoItem = new QTreeWidgetItem(projectPointAItem, QStringList(pointInfoItemText));
					projectPointAItem->addChild(projectPointAInfoItem);
					projectPointAItem->setExpanded(true);

					QTreeWidgetItem *projectPointBItem = getChildItem(projectPointItem, 1521 * 100 + pointPareIndex);
					removeChildItem(projectPointBItem);
					pointInfoItemText = QString("(").
						append(QString::number(aboArrangePointProjectToFitPlanePoint[pointPareIndex][1][0])).append(", ").
						append(QString::number(aboArrangePointProjectToFitPlanePoint[pointPareIndex][1][1])).append(", ").
						append(QString::number(aboArrangePointProjectToFitPlanePoint[pointPareIndex][1][2])).append(")");
					QTreeWidgetItem *projectPointBInfoItem = new QTreeWidgetItem(projectPointBItem, QStringList(pointInfoItemText));
					projectPointBItem->addChild(projectPointBInfoItem);
					projectPointBItem->setExpanded(true);

					QTreeWidgetItem *projectPointDistanceItem = getChildItem(projectPointItem, 1522 * 100 + pointPareIndex);
					removeChildItem(projectPointDistanceItem);
					pointInfoItemText = QString::number(aboArrangePointPareDistance[pointPareIndex]);
					QTreeWidgetItem *projectPointDistanceInfoItem = new QTreeWidgetItem(projectPointDistanceItem, QStringList(pointInfoItemText));
					projectPointDistanceItem->addChild(projectPointDistanceInfoItem);
					projectPointDistanceItem->setExpanded(true);

					QTreeWidgetItem *pointPareScoreItem = getChildItem(currentOptionTreeWidgetItem, 1523 * 100 + pointPareIndex);
					removeChildItem(pointPareScoreItem);
					pointInfoItemText = QString::number(aboArrangePointPareScore[pointPareIndex]);
					QTreeWidgetItem *pointPareScoreInfoItem = new QTreeWidgetItem(pointPareScoreItem, QStringList(pointInfoItemText));
					pointPareScoreItem->addChild(pointPareScoreInfoItem);
					pointPareScoreItem->setExpanded(true);

					QTreeWidgetItem *aboArrangeScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 1503);
					removeChildItem(aboArrangeScoreItem);
					pointInfoItemText = QString::number(aboArrangeScore);
					QTreeWidgetItem *paboArrangeScoreItemInfoItem = new QTreeWidgetItem(aboArrangeScoreItem, QStringList(pointInfoItemText));
					aboArrangeScoreItem->addChild(paboArrangeScoreItemInfoItem);
					aboArrangeScoreItem->setExpanded(true);

					QTreeWidgetItem *aboSumScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent()->parent(), 5000);
					updateAboSumScoreInfo(aboSumScoreItem);
				}
			}
		}

		++aboArrangePointPickNum[pointPareIndex];

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 6900)
	{
		//覆盖选点
		JCoverPoint[0] = x;
		JCoverPoint[1] = y;
		JCoverPoint[2] = z;

		drawPoint(JCoverPoint, pickPointColor, JCoverPointActor);
		isJCoverPointShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		//
		// 牙弓放射状相交平面
		//

		QTreeWidgetItem *JCoverPlaneItem = getChildItem(currentOptionTreeWidgetItem->parent(), 7000);
		if(!JCoverPlaneItem->isExpanded())
		{
			JCoverPlaneItem->setExpanded(true);
		}

		QTreeWidgetItem *JCoverPlanePointItem = getChildItem(JCoverPlaneItem, 7300);
		removeChildItem(JCoverPlanePointItem);
		QTreeWidgetItem *JCoverPlaneNormalItem = getChildItem(JCoverPlaneItem, 7400);
		removeChildItem(JCoverPlaneNormalItem);

		vtkPlane::ProjectPoint(JCoverPoint,
			fitPlaneCenter, fitPlaneNormal,
			JCoverProjectPoint);
		drawPoint(JCoverProjectPoint, projectToFittingPlanePointColot, JCoverProjectPointActor);

		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			JCoverProjectPoint,
			50 * pointSize,
			JCoverPlaneOCenter, JCoverPlaneONormal, JCoverPlaneOPoint);
		drawPoint(JCoverPlaneOCenter, projectToFittingPlanePointColot, JCoverCenterPointActor);
		drawPlane(JCoverPlaneOPoint[0], JCoverPlaneOPoint[1], JCoverPlaneOPoint[2], planeColor, 0.4, JCoverPlaneOActor, frameColor, 2, 2, JCoverPlaneOWireframeActor);
		isJCoverPlaneOShow = 1;

		itemText = QString("(").
			append(QString::number(JCoverPlaneOCenter[0])).append(", ").
			append(QString::number(JCoverPlaneOCenter[1])).append(", ").
			append(QString::number(JCoverPlaneOCenter[2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(JCoverPlanePointItem, QStringList(itemText));
		JCoverPlanePointItem->addChild(planePointInfoItem);
		if(!JCoverPlanePointItem->isExpanded())
		{
			JCoverPlanePointItem->setExpanded(true);
		}

		itemText = QString("(").
			append(QString::number(JCoverPlaneONormal[0])).append(", ").
			append(QString::number(JCoverPlaneONormal[1])).append(", ").
			append(QString::number(JCoverPlaneONormal[2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(JCoverPlaneNormalItem, QStringList(itemText));
		JCoverPlaneNormalItem->addChild(planeNormalInfoItem);
		if(!JCoverPlaneNormalItem->isExpanded())
		{
			JCoverPlaneNormalItem->setExpanded(true);
		}

		//
		// 轮廓线
		//
		drawIntersectCurve(lowerToothData, JCoverPlaneOCenter, JCoverPlaneONormal, intersectCurveColor, 2, JCoverIntersectCurveActor);
		drawIntersectCurve(topToothData, JCoverPlaneOCenter, JCoverPlaneONormal, topIntersectCurveColor, 2, TopJCoverIntersectCurveActor);
		isParJCoverIntersectCurveShow = 1;

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		selectOptionTreeIndex = 7500;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setPickPointPosition(JCoverPoint[0],JCoverPoint[1],JCoverPoint[2]);
		double pB[3];
		findLowerClosestPoint(JCoverIntersectCurveActor,JCoverPoint,pB);
		setPickPointPosition(pB[0],pB[1],pB[2]);
		this->pickStyle->mode = 0;		
	}

	else if(selectOptionTreeIndex == 69)
	{
		//覆盖选点
		CoverPoint[0] = x;
		CoverPoint[1] = y;
		CoverPoint[2] = z;

		drawPoint(CoverPoint, pickPointColor, CoverPointActor);
		isCoverPointShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		//
		// 牙弓放射状相交平面
		//

		QTreeWidgetItem *CoverPlaneItem = getChildItem(currentOptionTreeWidgetItem->parent(), 70);
		if(!CoverPlaneItem->isExpanded())
		{
			CoverPlaneItem->setExpanded(true);
		}

		QTreeWidgetItem *CoverPlanePointItem = getChildItem(CoverPlaneItem, 73);
		removeChildItem(CoverPlanePointItem);
		QTreeWidgetItem *CoverPlaneNormalItem = getChildItem(CoverPlaneItem, 74);
		removeChildItem(CoverPlaneNormalItem);

		vtkPlane::ProjectPoint(CoverPoint,
			fitPlaneCenter, fitPlaneNormal,
			CoverProjectPoint);
		drawPoint(CoverProjectPoint, projectToFittingPlanePointColot, CoverProjectPointActor);

		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			CoverProjectPoint,
			50 * pointSize,
			CoverPlaneOCenter, CoverPlaneONormal, CoverPlaneOPoint);
		drawPoint(CoverPlaneOCenter, projectToFittingPlanePointColot, CoverCenterPointActor);
		drawPlane(CoverPlaneOPoint[0], CoverPlaneOPoint[1], CoverPlaneOPoint[2], planeColor, 0.4, CoverPlaneOActor, frameColor, 2, 2, CoverPlaneOWireframeActor);
		isCoverPlaneOShow = 1;

		itemText = QString("(").
			append(QString::number(CoverPlaneOCenter[0])).append(", ").
			append(QString::number(CoverPlaneOCenter[1])).append(", ").
			append(QString::number(CoverPlaneOCenter[2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(CoverPlanePointItem, QStringList(itemText));
		CoverPlanePointItem->addChild(planePointInfoItem);
		if(!CoverPlanePointItem->isExpanded())
		{
			CoverPlanePointItem->setExpanded(true);
		}

		itemText = QString("(").
			append(QString::number(CoverPlaneONormal[0])).append(", ").
			append(QString::number(CoverPlaneONormal[1])).append(", ").
			append(QString::number(CoverPlaneONormal[2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(CoverPlaneNormalItem, QStringList(itemText));
		CoverPlaneNormalItem->addChild(planeNormalInfoItem);
		if(!CoverPlaneNormalItem->isExpanded())
		{
			CoverPlaneNormalItem->setExpanded(true);
		}

		//
		// 轮廓线
		//
		drawIntersectCurve(lowerToothData, CoverPlaneOCenter, CoverPlaneONormal, intersectCurveColor, 2, CoverIntersectCurveActor);
		drawIntersectCurve(topToothData, CoverPlaneOCenter, CoverPlaneONormal, topIntersectCurveColor, 2, TopCoverIntersectCurveActor);
		isParCoverIntersectCurveShow = 1;

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		selectOptionTreeIndex = 75;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setPickPointPosition(CoverPoint[0],CoverPoint[1],CoverPoint[2]);
		double pB[3];
		double ProCenterB[3] = {}; 
		findLowerCuttingPoint(CoverIntersectCurveActor,CoverPoint,pB,ProCenterB);
		setPickPointPosition(pB[0],pB[1],pB[2]);

		this->pickStyle->mode = 0;		
	}
	else if(selectOptionTreeIndex == 119)
	{
		// DI覆盖选点

		diCoverPoint[0] = x;
		diCoverPoint[1] = y;
		diCoverPoint[2] = z;

		drawPoint(diCoverPoint, pickPointColor, diCoverPointActor);
		isDiCoverPointShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		//
		// 牙弓放射状相交平面
		//

		QTreeWidgetItem *diCoverPlaneItem = getChildItem(currentOptionTreeWidgetItem->parent(), 120);
		if(!diCoverPlaneItem->isExpanded())
		{
			diCoverPlaneItem->setExpanded(true);
		}

		QTreeWidgetItem *diCoverPlanePointItem = getChildItem(diCoverPlaneItem, 123);
		removeChildItem(diCoverPlanePointItem);
		QTreeWidgetItem *diCoverPlaneNormalItem = getChildItem(diCoverPlaneItem, 124);
		removeChildItem(diCoverPlaneNormalItem);

		vtkPlane::ProjectPoint(diCoverPoint,
			fitPlaneCenter, fitPlaneNormal,
			diCoverProjectPoint);
		drawPoint(diCoverProjectPoint, projectToFittingPlanePointColot, diCoverProjectPointActor);

		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			diCoverProjectPoint,
			50 * pointSize,
			diCoverPlaneOCenter, diCoverPlaneONormal, diCoverPlaneOPoint);
		//drawPoint(diCoverPlaneOCenter, projectToFittingPlanePointColot, diCoverCenterPointActor);
		//drawPlane(diCoverPlaneOPoint[0], diCoverPlaneOPoint[1], diCoverPlaneOPoint[2], planeColor, 0.4, diCoverPlaneOActor, frameColor, 2, 2, diCoverPlaneOWireframeActor);
		isDiCoverPlaneOShow = 1;

		itemText = QString("(").
			append(QString::number(diCoverPlaneOCenter[0])).append(", ").
			append(QString::number(diCoverPlaneOCenter[1])).append(", ").
			append(QString::number(diCoverPlaneOCenter[2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(diCoverPlanePointItem, QStringList(itemText));
		diCoverPlanePointItem->addChild(planePointInfoItem);
		if(!diCoverPlanePointItem->isExpanded())
		{
			diCoverPlanePointItem->setExpanded(true);
		}

		itemText = QString("(").
			append(QString::number(diCoverPlaneONormal[0])).append(", ").
			append(QString::number(diCoverPlaneONormal[1])).append(", ").
			append(QString::number(diCoverPlaneONormal[2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(diCoverPlaneNormalItem, QStringList(itemText));
		diCoverPlaneNormalItem->addChild(planeNormalInfoItem);
		if(!diCoverPlaneNormalItem->isExpanded())
		{
			diCoverPlaneNormalItem->setExpanded(true);
		}

		//
		// 轮廓线 max value in y-axis
		//
		drawIntersectCurve(lowerToothData, diCoverPlaneOCenter, diCoverPlaneONormal, intersectCurveColor, 2, diCoverIntersectCurveActor);
		drawIntersectCurve(topToothData, diCoverPlaneOCenter, diCoverPlaneONormal, topIntersectCurveColor, 2, diTopCoverIntersectCurveActor);
		double pB[3] = {};
		double ProCenterB[3] = {};
		findLowerCuttingPoint(diCoverIntersectCurveActor,diCoverProjectPoint,pB,ProCenterB);
// 		vtkSmartPointer<vtkDataSet> vP = diCoverIntersectCurveActor->GetMapper()->GetInput();
// 		
// 		double centerB[3] = {};
// 		for (int i = 0;i < vP->GetNumberOfPoints();i ++)
// 		{
// 			double tP[3];
// 			vP->GetPoint(i,tP);
// 			centerB[0] += tP[0];
// 			centerB[1] += tP[1];
// 			centerB[2] += tP[2];
// 		}
// 		for (int i = 0;i < 3;i ++)
// 		{
// 			centerB[i] = centerB[i] / vP->GetNumberOfPoints();
// 		}
// 		double ProCenterB[3] = {};
// 		vtkPlane::ProjectPoint(centerB, 
// 			fitPlaneCenter, fitPlaneNormal,
// 			ProCenterB);
// 		double maxDis = 0;
// 		for (int i = 0;i < vP->GetNumberOfPoints();i ++)// max distance on fitting plane
// 		{
// 			double tP[3];
// 			vP->GetPoint(i,tP);
// 			double tPro[3];
// 			vtkPlane::ProjectPoint(tP, 
// 				fitPlaneCenter, fitPlaneNormal,
// 				tPro);
// 			double m = (tPro[0] - ProCenterB[0]) * (diCoverProjectPoint[0] - ProCenterB[0]);
// 			if (m > 0)
// 			{
// 				/*double dis = sqrt(vtkMath::Distance2BetweenPoints(tPro, ProCenterB));*/
// 				double dis = tP[1];
// 				if (dis > maxDis)
// 				{
// 					maxDis = dis;
// 					pB[0] = tP[0];
// 					pB[1] = tP[1];
// 					pB[2] = tP[2];
// 				}
// 			}
// 		}

		isDiCoverIntersectCurveShow = 1;
		diCoverIntersectCurvePointIndex = 0;
		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][0] = x;
		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][1] = y;
		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][2] = z;

// 		QString itemText = QString("(").
// 			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][0])).append(", ").
// 			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][1])).append(", ").
// 			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][2])).append(")");
// 		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
// 		currentOptionTreeWidgetItem->addChild(pointInfoItem);
// 		if(!currentOptionTreeWidgetItem->isExpanded())
// 		{
// 			currentOptionTreeWidgetItem->setExpanded(true);
// 		}

		drawPoint(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex], pickPointColor, diCoverIntersectCurvePointActor[diCoverIntersectCurvePointIndex]);
		isDiCoverIntersectCurvePointShow = 1;

		++diCoverIntersectCurvePointIndex;

		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][0] = pB[0];
		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][1] = pB[1];
		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][2] = pB[2];

		itemText = QString("(").
			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][0])).append(", ").
			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][1])).append(", ").
			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem1 = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem1);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		drawPoint(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex], pickPointColor, diCoverIntersectCurvePointActor[diCoverIntersectCurvePointIndex]);
		isDiCoverIntersectCurvePointShow = 1;

		++diCoverIntersectCurvePointIndex;

		if(diCoverIntersectCurvePointIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				vtkPlane::ProjectPoint(diCoverIntersectCurvePoint[i], 
					fitPlaneCenter, fitPlaneNormal,
					diCoverIntersectCurveProjectFitPlanePoint[i]);
			}
			double diD1 = sqrt(vtkMath::Distance2BetweenPoints(diCoverIntersectCurveProjectFitPlanePoint[0], ProCenterB));
			double diD2 = sqrt(vtkMath::Distance2BetweenPoints(diCoverIntersectCurveProjectFitPlanePoint[1], ProCenterB));
			diCoverDistance = diD1 - diD2;

			currentOptionTreeWidgetItem = getChildItem(currentOptionTreeWidgetItem->parent(), 121);
			currentOptionTreeWidgetItem = getChildItem(currentOptionTreeWidgetItem, 126);
			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 126);
			removeChildItem(distanceItem);

			itemText = QString::number(diCoverDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}
			//QMessageBox::information(NULL, tr("Info"), QString::number(diCoverDistance));

			//
			// DI覆盖得分
			//

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 122);
			removeChildItem(scoreItem);

			if(diCoverDistance >= 0 && diCoverDistance < 1)
			{
				diCoverScore = 1;
			}
			else if(diCoverDistance >= 1 && diCoverDistance < 3)
			{
				diCoverScore = 0;
			}
			else if(diCoverDistance >= 3 && diCoverDistance < 5)
			{
				diCoverScore = 2;
			}
			else if(diCoverDistance >= 5 && diCoverDistance < 7)
			{
				diCoverScore = 3;
			}
			else if(diCoverDistance >= 7 && diCoverDistance < 9)
			{
				diCoverScore = 4;
			}
			else if(diCoverDistance >= 9)
			{
				diCoverScore = 5;
			}else if (diCoverDistance < 0)
			{
				diCoverScore = abs(diCoverDistance);
			}
			ScoreSet[0][0] = diCoverScore;
			itemText = QString::number(diCoverScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}

			if(isLowToothShow == 1)
				switchLowToothShow();
			//switchDiCoverIntersectCurveShow();
			switchDiCoverIntersectCurveShow();
			switchDiCoverIntersectCurvePointShow();
			switchDiCoverIntersectCurvePointShow();
		}
		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		this->pickStyle->mode = 0;		
	}
	else if(selectOptionTreeIndex == 1373)
	{
		// 选点(DI合关系->测量->点A)

		diCloseRelationPointA[0] = x;
		diCloseRelationPointA[1] = y;
		diCloseRelationPointA[2] = z;

		drawPoint(diCloseRelationPointA, pickPointColor, diCloseRelationPointAActor);
		isDiCloseRelationPointAShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 1383)
	{
		// 选点(DI合关系->测量->点A)

		diCloseRelationPointA[0] = x;
		diCloseRelationPointA[1] = y;
		diCloseRelationPointA[2] = z;

		drawPoint(diCloseRelationPointA, pickPointColor, diCloseRelationPointAActor);
		isDiCloseRelationPointAShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 1374)
	{
		// 选点(DI合关系->测量->点B)

		diCloseRelationPointB[0] = x;
		diCloseRelationPointB[1] = y;
		diCloseRelationPointB[2] = z;

		drawPoint(diCloseRelationPointB, pickPointColor, diCloseRelationPointBActor);
		isDiCloseRelationPointBShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
		cout<<"stage 1"<<endl;
		//
		// 牙弓放射状相交平面
		//
		vtkPlane::ProjectPoint(diCloseRelationPointB,
			fitPlaneCenter, fitPlaneNormal,
			diCloseRelationProjectPointB);
		drawPoint(diCloseRelationProjectPointB, projectToFittingPlanePointColot, diCloseRelationProjectPointBActor);

		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			diCloseRelationProjectPointB,
			50 * pointSize,
			diCloseRelationPlaneOCenter, diCloseRelationPlaneONormal, diCloseRelationPlaneOPoint);
		drawPoint(diCloseRelationPlaneOCenter, projectToFittingPlanePointColot, diCloseRelationPlanePointAActor);
		drawPlane(diCloseRelationPlaneOPoint[0], diCloseRelationPlaneOPoint[1], diCloseRelationPlaneOPoint[2], planeColor, 0.4, diCloseRelationPlaneOActor, 
			frameColor, 2, 2, diCloseRelationPlaneOWireframeActor);
		isDiCloseRelationPlaneOShow = 1;
		cout<<"stage 2"<<endl;
		QTreeWidgetItem *planeItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1375);
		planeItem->setExpanded(true);

		QTreeWidgetItem *planePointItem = getChildItem(planeItem, 1377);
		removeChildItem(planePointItem);
		QTreeWidgetItem *planeNormalItem = getChildItem(planeItem, 1378);
		removeChildItem(planeNormalItem);
		cout<<"stage 3"<<endl;
		itemText = QString("(").
			append(QString::number(diCloseRelationPlaneOCenter[0])).append(", ").
			append(QString::number(diCloseRelationPlaneOCenter[1])).append(", ").
			append(QString::number(diCloseRelationPlaneOCenter[2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(planePointItem, QStringList(itemText));
		planePointItem->addChild(planePointInfoItem);
		if(!planePointItem->isExpanded())
		{
			planePointItem->setExpanded(true);
		}

		itemText = QString("(").
			append(QString::number(diCloseRelationPlaneONormal[0])).append(", ").
			append(QString::number(diCloseRelationPlaneONormal[1])).append(", ").
			append(QString::number(diCloseRelationPlaneONormal[2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(planeNormalItem, QStringList(itemText));
		planeNormalItem->addChild(planeNormalInfoItem);
		if(!planeNormalItem->isExpanded())
		{
			planeNormalItem->setExpanded(true);
		}
		cout<<"stage 4"<<endl;
		// 合关系距离
		double projectPoint[3];
		vtkPlane::ProjectPoint(diCloseRelationPointA,
			diCloseRelationPlaneOCenter, diCloseRelationPlaneONormal,
			projectPoint);
		diCloseRelationDistance = sqrt(vtkMath::Distance2BetweenPoints(diCloseRelationPointA, projectPoint));
		diCloseRelationDistance = diCloseRelationDistance - (int)diCloseRelationDistance >= 0.5 ? (int)diCloseRelationDistance + 1 : (int)diCloseRelationDistance;
		diCloseRelationScore += (int)diCloseRelationDistance + 4;

		QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1376);
		removeChildItem(distanceItem);
		cout<<"stage 5"<<endl;
		itemText = QString::number(diCloseRelationDistance);
		QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
		distanceItem->addChild(distanceInfoItem);
		if(!distanceItem->isExpanded())
		{
			distanceItem->setExpanded(true);
		}

		/*QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 1371);*/
		QTreeWidgetItem *scoreItem = currentOptionTreeWidgetItem->parent()->parent();
		//removeChildItem(scoreItem);

		itemText = QString::number(diCloseRelationScore);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
		scoreItem->addChild(scoreInfoItem);
		if(!scoreItem->isExpanded())
		{
			scoreItem->setExpanded(true);
		}

		switchDiCloseRelationPointAShow();
		switchFitPlaneShow();
		switchFittingCurveShow();

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 1384)
	{
		// 选点(DI合关系->测量->点B)

		diCloseRelationPointB[0] = x;
		diCloseRelationPointB[1] = y;
		diCloseRelationPointB[2] = z;

		drawPoint(diCloseRelationPointB, pickPointColor, diCloseRelationPointBActor);
		isDiCloseRelationPointBShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		//
		// 牙弓放射状相交平面
		//
		vtkPlane::ProjectPoint(diCloseRelationPointB,
			fitPlaneCenter, fitPlaneNormal,
			diCloseRelationProjectPointB);
		drawPoint(diCloseRelationProjectPointB, projectToFittingPlanePointColot, diCloseRelationProjectPointBActor);

		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			diCloseRelationProjectPointB,
			50 * pointSize,
			diCloseRelationPlaneOCenter, diCloseRelationPlaneONormal, diCloseRelationPlaneOPoint);
		drawPoint(diCloseRelationPlaneOCenter, projectToFittingPlanePointColot, diCloseRelationPlanePointAActor);
		drawPlane(diCloseRelationPlaneOPoint[0], diCloseRelationPlaneOPoint[1], diCloseRelationPlaneOPoint[2], planeColor, 0.4, diCloseRelationPlaneOActor, 
			frameColor, 2, 2, diCloseRelationPlaneOWireframeActor);
		isDiCloseRelationPlaneOShow = 1;

		QTreeWidgetItem *planeItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1385);
		planeItem->setExpanded(true);

		QTreeWidgetItem *planePointItem = getChildItem(planeItem, 1387);
		removeChildItem(planePointItem);
		QTreeWidgetItem *planeNormalItem = getChildItem(planeItem, 1388);
		removeChildItem(planeNormalItem);

		itemText = QString("(").
			append(QString::number(diCloseRelationPlaneOCenter[0])).append(", ").
			append(QString::number(diCloseRelationPlaneOCenter[1])).append(", ").
			append(QString::number(diCloseRelationPlaneOCenter[2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(planePointItem, QStringList(itemText));
		planePointItem->addChild(planePointInfoItem);
		if(!planePointItem->isExpanded())
		{
			planePointItem->setExpanded(true);
		}

		itemText = QString("(").
			append(QString::number(diCloseRelationPlaneONormal[0])).append(", ").
			append(QString::number(diCloseRelationPlaneONormal[1])).append(", ").
			append(QString::number(diCloseRelationPlaneONormal[2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(planeNormalItem, QStringList(itemText));
		planeNormalItem->addChild(planeNormalInfoItem);
		if(!planeNormalItem->isExpanded())
		{
			planeNormalItem->setExpanded(true);
		}

		// 合关系距离
		double projectPoint[3];
		vtkPlane::ProjectPoint(diCloseRelationPointA,
			diCloseRelationPlaneOCenter, diCloseRelationPlaneONormal,
			projectPoint);
		diCloseRelationDistance = sqrt(vtkMath::Distance2BetweenPoints(diCloseRelationPointA, projectPoint));
		diCloseRelationDistance = diCloseRelationDistance - (int)diCloseRelationDistance >= 0.5 ? (int)diCloseRelationDistance + 1 : (int)diCloseRelationDistance;
		diCloseRelationScore += (int)diCloseRelationDistance + 4;

		QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1386);
		removeChildItem(distanceItem);

		itemText = QString::number(diCloseRelationDistance);
		QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
		distanceItem->addChild(distanceInfoItem);
		if(!distanceItem->isExpanded())
		{
			distanceItem->setExpanded(true);
		}

// 		QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 1381);
// 		removeChildItem(scoreItem);
		QTreeWidgetItem *scoreItem = currentOptionTreeWidgetItem->parent()->parent();
		itemText = QString::number(diCloseRelationScore);
		QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
		scoreItem->addChild(scoreInfoItem);
		if(!scoreItem->isExpanded())
		{
			scoreItem->setExpanded(true);
		}

		switchDiCloseRelationPointAShow();
		switchFitPlaneShow();
		switchFittingCurveShow();

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 129)
	{
		// DI覆合选点

		// 		else  
		// 		{  
		// 			qDebug()<<"clicked no\n";  
		// 		}  
		diOverbitePoint[0] = x;
		diOverbitePoint[1] = y;
		diOverbitePoint[2] = z;

		drawPoint(diOverbitePoint, pickPointColor, diOverbitePointActor);
		isDiOverbitePointShow = 1;

		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		//
		// 牙弓放射状相交平面
		//

		QTreeWidgetItem *diOverbitePlaneItem = getChildItem(currentOptionTreeWidgetItem->parent(), 130);
		if(!diOverbitePlaneItem->isExpanded())
		{
			diOverbitePlaneItem->setExpanded(true);
		}

		QTreeWidgetItem *diOverbitePlanePointItem = getChildItem(diOverbitePlaneItem, 133);
		removeChildItem(diOverbitePlanePointItem);
		QTreeWidgetItem *diOverbitePlaneNormalItem = getChildItem(diOverbitePlaneItem, 134);
		removeChildItem(diOverbitePlaneNormalItem);

		vtkPlane::ProjectPoint(diOverbitePoint,
			fitPlaneCenter, fitPlaneNormal,
			diOverbiteProjectPoint);
		drawPoint(diOverbiteProjectPoint, projectToFittingPlanePointColot, diOverbiteProjectPointActor);

		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			diOverbiteProjectPoint,
			50 * pointSize,
			diOverbitePlaneOCenter, diOverbitePlaneONormal, diOverbitePlaneOPoint);
		//drawPoint(diOverbitePlaneOCenter, projectToFittingPlanePointColot, diOverbiteCenterPointActor);
		//drawPlane(diOverbitePlaneOPoint[0], diOverbitePlaneOPoint[1], diOverbitePlaneOPoint[2], planeColor, 0.4, diOverbitePlaneOActor, frameColor, 2, 2, diOverbitePlaneOWireframeActor);
		isDiOverbitePlaneOShow = 1;

		itemText = QString("(").
			append(QString::number(diOverbitePlaneOCenter[0])).append(", ").
			append(QString::number(diOverbitePlaneOCenter[1])).append(", ").
			append(QString::number(diOverbitePlaneOCenter[2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(diOverbitePlanePointItem, QStringList(itemText));
		diOverbitePlanePointItem->addChild(planePointInfoItem);
		if(!diOverbitePlanePointItem->isExpanded())
		{
			diOverbitePlanePointItem->setExpanded(true);
		}

		itemText = QString("(").
			append(QString::number(diOverbitePlaneONormal[0])).append(", ").
			append(QString::number(diOverbitePlaneONormal[1])).append(", ").
			append(QString::number(diOverbitePlaneONormal[2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(diOverbitePlaneNormalItem, QStringList(itemText));
		diOverbitePlaneNormalItem->addChild(planeNormalInfoItem);
		if(!diOverbitePlaneNormalItem->isExpanded())
		{
			diOverbitePlaneNormalItem->setExpanded(true);
		}

		//
		// 轮廓线
		//
		drawIntersectCurve(lowerToothData, diOverbitePlaneOCenter, diOverbitePlaneONormal, intersectCurveColor, 2, diOverbiteIntersectCurveActor);
		drawIntersectCurve(topToothData, diOverbitePlaneOCenter, diOverbitePlaneONormal, topIntersectCurveColor, 2, diTopOverbiteIntersectCurveActor);
		isDiOverbiteIntersectCurveShow = 1;

		double pB[3] = {};
		double ProCenterB[3] = {};
		findLowerCuttingPoint(diOverbiteIntersectCurveActor,diOverbitePoint,pB,ProCenterB);		






		diOverbiteIntersectCurvePointIndex = 0;
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][0] = x;
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][1] = y;
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][2] = z;



		drawPoint(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex], pickPointColor, diOverbiteIntersectCurvePointActor[diOverbiteIntersectCurvePointIndex]);
		isDiOverbiteIntersectCurvePointShow = 1;

		++diOverbiteIntersectCurvePointIndex;

		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][0] = pB[0];
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][1] = pB[1];
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][2] = pB[2];

		itemText = QString("(").
			append(QString::number(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][0])).append(", ").
			append(QString::number(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][1])).append(", ").
			append(QString::number(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem1 = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem1);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
		drawPoint(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex], pickPointColor, diOverbiteIntersectCurvePointActor[diOverbiteIntersectCurvePointIndex]);
		isDiOverbiteIntersectCurvePointShow = 1;
		++diOverbiteIntersectCurvePointIndex;
		cout<<"phase 1"<<endl;
		if(diOverbiteIntersectCurvePointIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				vtkPlane::ProjectPoint(diOverbiteIntersectCurvePoint[i], 
					verticalPlaneCenter, verticalPlaneNormal,
					diOverbiteIntersectCurveProjectArticlePlanePoint[i]);
				vtkPlane::ProjectPoint(diOverbiteIntersectCurveProjectArticlePlanePoint[i], 
					pointPositionForArticlePlane, articlePlaneNormal,
					diOverbiteIntersectCurveProjectFitPlanePoint[i]);
			}
			cout<<"phase 2"<<endl;
			diOverbiteDistance =sqrt(vtkMath::Distance2BetweenPoints(diOverbiteIntersectCurveProjectFitPlanePoint[0], diOverbiteIntersectCurveProjectFitPlanePoint[1]));
			currentOptionTreeWidgetItem = getChildItem(currentOptionTreeWidgetItem->parent(), 131);
			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem, 136);
			removeChildItem(distanceItem);

			itemText = QString::number(diOverbiteDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}
			cout<<"phase 3"<<endl;
			//QMessageBox::information(NULL, tr("Info"), QString::number(diOverbiteDistance));

			//
			// DI覆合得分
			//

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 132);
			removeChildItem(scoreItem);
			QMessageBox message(QMessageBox::Warning,"判断","是否闭锁合?",QMessageBox::Yes|QMessageBox::No,NULL);  
			if (message.exec()==QMessageBox::Yes)  
			{  
				diOverbiteScore = 5;
			}else
			{
				if(diOverbiteDistance >= 0 && diOverbiteDistance < 3)
				{
					diOverbiteScore = 0;
				}
				else if(diOverbiteDistance >= 3 && diOverbiteDistance < 5)
				{
					diOverbiteScore = 2;
				}
				else if(diOverbiteDistance >= 5 && diOverbiteDistance < 7)
				{
					diOverbiteScore = 3;
				}
				// 				else if(diOverbiteDistance >= 5 && diOverbiteDistance < 7)
				// 				{
				// 					diOverbiteScore = 3;
				// 				}
				// 				else if(diOverbiteDistance >= 7 && diOverbiteDistance < 9)
				// 				{
				// 					diOverbiteScore = 4;
				// 				}
				// 				else if(diOverbiteDistance >= 9)
				// 				{
				// 					diOverbiteScore = 5;
				// 				}
			}
			ScoreSet[0][1] = diOverbiteScore;
			itemText = QString::number(diOverbiteScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}

			if(isLowToothShow == 1)
				switchLowToothShow();
			//switchDiOverbiteIntersectCurveShow();
			switchDiOverbiteIntersectCurveShow();
			switchDiOverbiteIntersectCurvePointShow();
			switchDiOverbiteIntersectCurvePointShow();
		}

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 1002)
	{
		// 选点(DI其他->中线不调->点)

		diMiddleUnfitPoint[diMiddleUnfitPointIndex][0] = x;
		diMiddleUnfitPoint[diMiddleUnfitPointIndex][1] = y;
		diMiddleUnfitPoint[diMiddleUnfitPointIndex][2] = z;

		drawPoint(diMiddleUnfitPoint[diMiddleUnfitPointIndex], pickPointColor, diMiddleUnfitPointActor[diMiddleUnfitPointIndex]);
		diMiddleUnfitPointShow = 1;

		QString itemText = QString("(").
			append(QString::number(diMiddleUnfitPoint[diMiddleUnfitPointIndex][0])).append(", ").
			append(QString::number(diMiddleUnfitPoint[diMiddleUnfitPointIndex][1])).append(", ").
			append(QString::number(diMiddleUnfitPoint[diMiddleUnfitPointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		++diMiddleUnfitPointIndex;
		if(diMiddleUnfitPointIndex == 2)
		{
			double planePoint[2][3][3];
			for(int i = 0; i < 2; ++i)
			{
				for(int j = 0; j < 3; ++j)
				{
					vtkPlane::ProjectPoint(verticalPlanePoint[j],
						diMiddleUnfitPoint[i], verticalPlaneNormal,
						planePoint[i][j]);
				}
				drawPlane(planePoint[i][0], planePoint[i][1], planePoint[i][2], planeColor, 0.4, diMiddleUnfitPlaneActor[i], 
					frameColor, 2, 2, diMiddleUnfitPlaneWireframeActor[i]);
			}

			// 距离
			double projectPoint[3];
			vtkPlane::ProjectPoint(diMiddleUnfitPoint[0],
				diMiddleUnfitPoint[1], verticalPlaneNormal,
				projectPoint);
			diMiddleUnfitDistance = sqrt(vtkMath::Distance2BetweenPoints(diMiddleUnfitPoint[0], projectPoint));
			
			diMiddleUnfitScore = 0;
			if(diMiddleUnfitDistance > 3)
			{
				diMiddleUnfitScore = 2;
			}

			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1003);
			removeChildItem(distanceItem);
			itemText = QString::number(diMiddleUnfitDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1004);
			removeChildItem(scoreItem);
			itemText = QString::number(diMiddleUnfitScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 1006)
	{
		// 选点(DI其他->中线不调->点)

		diMiddleCutToothGapPoint[diMiddleCutToothGapIndex][0] = x;
		diMiddleCutToothGapPoint[diMiddleCutToothGapIndex][1] = y;
		diMiddleCutToothGapPoint[diMiddleCutToothGapIndex][2] = z;

		drawPoint(diMiddleCutToothGapPoint[diMiddleCutToothGapIndex], pickPointColor, diMiddleCutToothGapPointActor[diMiddleCutToothGapIndex]);
		isDiMiddleCutToothGapShow = 1;

		QString itemText = QString("(").
			append(QString::number(diMiddleCutToothGapPoint[diMiddleCutToothGapIndex][0])).append(", ").
			append(QString::number(diMiddleCutToothGapPoint[diMiddleCutToothGapIndex][1])).append(", ").
			append(QString::number(diMiddleCutToothGapPoint[diMiddleCutToothGapIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		++diMiddleCutToothGapIndex;
		if(diMiddleCutToothGapIndex == 2)
		{
			diMiddleCutToothGapDistance = sqrt(vtkMath::Distance2BetweenPoints(diMiddleCutToothGapPoint[0], diMiddleCutToothGapPoint[1]));

			diMiddleCutToothGapScore = 0;
			if(diMiddleCutToothGapDistance >= 2)
			{
				diMiddleCutToothGapScore = 2;
			}

			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1007);
			removeChildItem(distanceItem);
			itemText = QString::number(diMiddleCutToothGapDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent(), 1008);
			removeChildItem(scoreItem);
			itemText = QString::number(diMiddleCutToothGapScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 901)
	{
		// 选点(DI后牙反合->点)

		int index = -1;
		for(int i = 0; i < 16; ++i)
		{
			if(!diBackToothOppositeClosePointState[i])
			{
				index = i; 
				break;
			}
		}

		if(index > -1)
		{
			diBackToothOppositeClosePoint[index][0] = x;
			diBackToothOppositeClosePoint[index][1] = y;
			diBackToothOppositeClosePoint[index][2] = z;
			diBackToothOppositeClosePointState[index] = true;

			drawPoint(diBackToothOppositeClosePoint[index], pickPointColor, diBackToothOppositeClosePointActor[index]);
			isDiBackToothOppositeClosePointShow = 1;

			removeChildItem(currentOptionTreeWidgetItem);

			for(int i = 0; i < 16; ++i)
			{
				if(diBackToothOppositeClosePointState[i])
				{
					QString itemText = QString("(").
						append(QString::number(diBackToothOppositeClosePoint[i][0])).append(", ").
						append(QString::number(diBackToothOppositeClosePoint[i][1])).append(", ").
						append(QString::number(diBackToothOppositeClosePoint[i][2])).append(")");
					QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
					currentOptionTreeWidgetItem->addChild(pointInfoItem);

					if(!currentOptionTreeWidgetItem->isExpanded())
					{
						currentOptionTreeWidgetItem->setExpanded(true);
					}
				}
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 905)
	{
		// 选点(DI后牙正锁合->点)

		int index = -1;
		for(int i = 0; i < 10; ++i)
		{
			if(!diBackToothLockClosePointState[i])
			{
				index = i; 
				break;
			}
		}

		if(index > -1)
		{
			diBackToothLockClosePoint[index][0] = x;
			diBackToothLockClosePoint[index][1] = y;
			diBackToothLockClosePoint[index][2] = z;
			diBackToothLockClosePointState[index] = true;

			drawPoint(diBackToothLockClosePoint[index], pickPointColor, diBackToothLockClosePointActor[index]);
			isDiBackToothLockClosePointShow = 1;

			removeChildItem(currentOptionTreeWidgetItem);

			for(int i = 0; i < 10; ++i)
			{
				if(diBackToothLockClosePointState[i])
				{
					QString itemText = QString("(").
						append(QString::number(diBackToothLockClosePoint[i][0])).append(", ").
						append(QString::number(diBackToothLockClosePoint[i][1])).append(", ").
						append(QString::number(diBackToothLockClosePoint[i][2])).append(")");
					QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
					currentOptionTreeWidgetItem->addChild(pointInfoItem);

					if(!currentOptionTreeWidgetItem->isExpanded())
					{
						currentOptionTreeWidgetItem->setExpanded(true);
					}
				}
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex / 100 == 141)
	{
		// 选点(DI侧方开合->磨牙1/2/3->点)

		int grindToothIndex = selectOptionTreeIndex - 14100;

		diSideOpenClosePoint[grindToothIndex][0] = x;
		diSideOpenClosePoint[grindToothIndex][1] = y;
		diSideOpenClosePoint[grindToothIndex][2] = z;
		cout<<"draw Cefangkaihe!!"<<grindToothIndex<<endl;
		drawPoint(diSideOpenClosePoint[grindToothIndex], pickPointColor, diSideOpenClosePointActor[grindToothIndex]);
		isDiSideOpenClosePointShow[grindToothIndex] = 1;
		cout<<"draw Cefangkaiheend!!"<<endl;
		removeChildItem(currentOptionTreeWidgetItem);

		QString itemText = QString("(").
			append(QString::number(x)).append(", ").
			append(QString::number(y)).append(", ").
			append(QString::number(z)).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		cout<<x<<' '<<y<<endl;
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		//
		// 牙弓放射状相交平面
		//

		vtkPlane::ProjectPoint(diSideOpenClosePoint[grindToothIndex],
			fitPlaneCenter, fitPlaneNormal,
			diSideOpenCloseProjectPoint[grindToothIndex]);
		drawPoint(diSideOpenCloseProjectPoint[grindToothIndex], projectToFittingPlanePointColot, diSideOpenCloseProjectPointActor[grindToothIndex]);
		cout<<1<<endl;
		initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
			splineProjectPointPosition, &splinePointState[0], 20,
			diSideOpenCloseProjectPoint[grindToothIndex],
			50 * pointSize,
			diSideOpenClosePlaneOCenter[grindToothIndex], diSideOpenClosePlaneONormal[grindToothIndex], diSideOpenClosePlaneOPoint[grindToothIndex]);
		drawPoint(diSideOpenClosePlaneOCenter[grindToothIndex], projectToFittingPlanePointColot, diSideOpenCloseCenterPointActor[grindToothIndex]);
		drawPlane(diSideOpenClosePlaneOPoint[grindToothIndex][0], diSideOpenClosePlaneOPoint[grindToothIndex][1], diSideOpenClosePlaneOPoint[grindToothIndex][2], planeColor, 0.4, diSideOpenClosePlaneOActor[grindToothIndex], frameColor, 2, 2, diSideOpenClosePlaneOWireframeActor[grindToothIndex]);
		isDiSideOpenClosePlaneOShow[grindToothIndex] = 1;
		cout<<2<<endl;
		QTreeWidgetItem *diSideOpenClosePlaneItem = getChildItem(currentOptionTreeWidgetItem->parent(), 14200 + grindToothIndex);
		if(!diSideOpenClosePlaneItem->isExpanded())
		{
			diSideOpenClosePlaneItem->setExpanded(true);
		}

		QTreeWidgetItem *diSideOpenClosePlanePointItem = getChildItem(diSideOpenClosePlaneItem, 14500 + grindToothIndex);
		removeChildItem(diSideOpenClosePlanePointItem);
		QTreeWidgetItem *diSideOpenClosePlaneNormalItem = getChildItem(diSideOpenClosePlaneItem, 14600 + grindToothIndex);
		removeChildItem(diSideOpenClosePlaneNormalItem);
		cout<<3<<endl;
		itemText = QString("(").
			append(QString::number(diSideOpenClosePlaneOCenter[grindToothIndex][0])).append(", ").
			append(QString::number(diSideOpenClosePlaneOCenter[grindToothIndex][1])).append(", ").
			append(QString::number(diSideOpenClosePlaneOCenter[grindToothIndex][2])).append(")");
		QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(diSideOpenClosePlanePointItem, QStringList(itemText));
		diSideOpenClosePlanePointItem->addChild(planePointInfoItem);
		if(!diSideOpenClosePlanePointItem->isExpanded())
		{
			diSideOpenClosePlanePointItem->setExpanded(true);
		}
		cout<<4<<endl;
		itemText = QString("(").
			append(QString::number(diSideOpenClosePlaneONormal[grindToothIndex][0])).append(", ").
			append(QString::number(diSideOpenClosePlaneONormal[grindToothIndex][1])).append(", ").
			append(QString::number(diSideOpenClosePlaneONormal[grindToothIndex][2])).append(")");
		QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(diSideOpenClosePlaneNormalItem, QStringList(itemText));
		diSideOpenClosePlaneNormalItem->addChild(planeNormalInfoItem);
		if(!diSideOpenClosePlaneNormalItem->isExpanded())
		{
			diSideOpenClosePlaneNormalItem->setExpanded(true);
		}
		cout<<5<<endl;
		//
		// 轮廓线
		//
		drawIntersectCurve(lowerToothData, diSideOpenClosePlaneOCenter[grindToothIndex], diSideOpenClosePlaneONormal[grindToothIndex], intersectCurveColor, 2, diSideOpenCloseIntersectCurveActor[grindToothIndex]);
		drawIntersectCurve(topToothData, diSideOpenClosePlaneOCenter[grindToothIndex], diSideOpenClosePlaneONormal[grindToothIndex], topIntersectCurveColor, 2, diTopSideOpenCloseIntersectCurveActor[grindToothIndex]);
		isDiSideOpenCloseIntersectCurveShow[grindToothIndex] = 1;

		double pB[3] = {};
		double ProCenterB[3] = {};
		findLowerClosestPoint(diSideOpenCloseIntersectCurveActor[grindToothIndex],diSideOpenClosePoint[grindToothIndex],pB);		

		selectOptionTreeIndex = 14700 + grindToothIndex;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setPickPointPosition(diSideOpenClosePoint[grindToothIndex][0],diSideOpenClosePoint[grindToothIndex][1],diSideOpenClosePoint[grindToothIndex][2]);
		setPickPointPosition(pB[0],pB[1],pB[2]);

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		this->pickStyle->mode = 0;
	}

	else if(selectOptionTreeIndex == 7500)
	{
		JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex][0] = x;
		JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex][1] = y;
		JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex][2] = z;

		QString itemText = QString("(").
			append(QString::number(JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex][0])).append(", ").
			append(QString::number(JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex][1])).append(", ").
			append(QString::number(JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		drawPoint(JCoverIntersectCurvePoint[JCoverIntersectCurvePointIndex], pickPointColor, JCoverIntersectCurvePointActor[JCoverIntersectCurvePointIndex]);
		isJCoverIntersectCurvePointShow = 1;

		++JCoverIntersectCurvePointIndex;
		if(JCoverIntersectCurvePointIndex == 2)
		{
// 			for(int i = 0; i < 2; ++i)
// 			{
// 				double temp[3] = {};
// 				vtkPlane::ProjectPoint(JCoverIntersectCurvePoint[i], 
// 					pointPositionForArticlePlane, articlePlaneNormal,
// 					temp);
// 				vtkPlane::ProjectPoint(temp, 
// 					verticalPlaneCenter, verticalPlaneNormal,
// 					JCoverIntersectCurveProjectFitPlanePoint[i]);
// 			}
			cout<<1<<endl;
			JCoverDistance =sqrt(vtkMath::Distance2BetweenPoints(JCoverIntersectCurvePoint[0], JCoverIntersectCurvePoint[1]));

			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 7600);
			removeChildItem(distanceItem);

			cout<<2<<endl;

			itemText = QString::number(JCoverDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}
			//QMessageBox::information(NULL, tr("Info"), QString::number(diJCoverDistance));

			//
			// 覆盖得分
			//

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 7200);
			removeChildItem(scoreItem);

			if(JCoverDistance < 2)
			{
				faceSideVerticalScore = 0;
			}
			else 
			{
				faceSideVerticalScore = 2;
			}

			itemText = QString::number(faceSideVerticalScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}

			if(isLowToothShow == 1)
				switchLowToothShow();
			//switchDiJCoverIntersectCurveShow();
			switchJCoverIntersectCurveShow();
			switchJCoverIntersectCurvePointShow();
			switchJCoverIntersectCurvePointShow();
		}

		this->pickStyle->mode = 0;
	}

	else if(selectOptionTreeIndex == 75)
	{
		// 选点(覆盖->轮廓曲线->点)


		CoverIntersectCurvePoint[CoverIntersectCurvePointIndex][0] = x;
		CoverIntersectCurvePoint[CoverIntersectCurvePointIndex][1] = y;
		CoverIntersectCurvePoint[CoverIntersectCurvePointIndex][2] = z;

		QString itemText = QString("(").
			append(QString::number(CoverIntersectCurvePoint[CoverIntersectCurvePointIndex][0])).append(", ").
			append(QString::number(CoverIntersectCurvePoint[CoverIntersectCurvePointIndex][1])).append(", ").
			append(QString::number(CoverIntersectCurvePoint[CoverIntersectCurvePointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		drawPoint(CoverIntersectCurvePoint[CoverIntersectCurvePointIndex], pickPointColor, CoverIntersectCurvePointActor[CoverIntersectCurvePointIndex]);
		isCoverIntersectCurvePointShow = 1;

		++CoverIntersectCurvePointIndex;
		if(CoverIntersectCurvePointIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				vtkPlane::ProjectPoint(CoverIntersectCurvePoint[i], 
					fitPlaneCenter, fitPlaneNormal,
					CoverIntersectCurveProjectFitPlanePoint[i]);
			}
			cout<<1<<endl;
			CoverDistance =sqrt(vtkMath::Distance2BetweenPoints(CoverIntersectCurveProjectFitPlanePoint[0], CoverIntersectCurveProjectFitPlanePoint[1]));

			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 76);
			removeChildItem(distanceItem);

			cout<<2<<endl;

			itemText = QString::number(CoverDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}
			//QMessageBox::information(NULL, tr("Info"), QString::number(diCoverDistance));

			//
			// 覆盖得分
			//

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 72);
			removeChildItem(scoreItem);

			if(CoverDistance >= 0 && CoverDistance < 3)
			{
				CoverScore = 0;
			}
			else if(CoverDistance >= 3 && CoverDistance < 5)
			{
				CoverScore = 1;
			}
			else if(CoverDistance >= 5 && CoverDistance < 7)
			{
				CoverScore = 2;
			}
			else if(CoverDistance >= 7 && CoverDistance < 9)
			{
				CoverScore = 3;
			}
			else if(CoverDistance >= 9)
			{
				CoverScore = 4;
			}
			ScoreSet[2][2] += CoverScore;
			itemText = QString::number(CoverScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}

			if(isLowToothShow == 1)
				switchLowToothShow();
			//switchDiCoverIntersectCurveShow();
			switchCoverIntersectCurveShow();
			switchCoverIntersectCurvePointShow();
			switchCoverIntersectCurvePointShow();
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 125)
	{
		// 选点(DI覆盖->轮廓曲线->点)


// 		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][0] = x;
// 		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][1] = y;
// 		diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][2] = z;
// 
// 		QString itemText = QString("(").
// 			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][0])).append(", ").
// 			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][1])).append(", ").
// 			append(QString::number(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex][2])).append(")");
// 		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
// 		currentOptionTreeWidgetItem->addChild(pointInfoItem);
// 		if(!currentOptionTreeWidgetItem->isExpanded())
// 		{
// 			currentOptionTreeWidgetItem->setExpanded(true);
// 		}
// 
// 		drawPoint(diCoverIntersectCurvePoint[diCoverIntersectCurvePointIndex], pickPointColor, diCoverIntersectCurvePointActor[diCoverIntersectCurvePointIndex]);
// 		isDiCoverIntersectCurvePointShow = 1;
// 
// 		++diCoverIntersectCurvePointIndex;
// 		if(diCoverIntersectCurvePointIndex == 2)
// 		{
// 			for(int i = 0; i < 2; ++i)
// 			{
// 				vtkPlane::ProjectPoint(diCoverIntersectCurvePoint[i], 
// 					fitPlaneCenter, fitPlaneNormal,
// 					diCoverIntersectCurveProjectFitPlanePoint[i]);
// 			}
// 
// 			diCoverDistance =sqrt(vtkMath::Distance2BetweenPoints(diCoverIntersectCurveProjectFitPlanePoint[0], diCoverIntersectCurveProjectFitPlanePoint[1]));
// 
// 			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 126);
// 			removeChildItem(distanceItem);
// 
// 			itemText = QString::number(diCoverDistance);
// 			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
// 			distanceItem->addChild(distanceInfoItem);
// 			if(!distanceItem->isExpanded())
// 			{
// 				distanceItem->setExpanded(true);
// 			}
// 			//QMessageBox::information(NULL, tr("Info"), QString::number(diCoverDistance));
// 
// 			//
// 			// DI覆盖得分
// 			//
// 
// 			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 122);
// 			removeChildItem(scoreItem);
// 
// 			if(diCoverDistance >= 0 && diCoverDistance < 1)
// 			{
// 				diCoverScore = 1;
// 			}
// 			else if(diCoverDistance >= 1 && diCoverDistance < 3)
// 			{
// 				diCoverScore = 0;
// 			}
// 			else if(diCoverDistance >= 3 && diCoverDistance < 5)
// 			{
// 				diCoverScore = 2;
// 			}
// 			else if(diCoverDistance >= 5 && diCoverDistance < 7)
// 			{
// 				diCoverScore = 3;
// 			}
// 			else if(diCoverDistance >= 7 && diCoverDistance < 9)
// 			{
// 				diCoverScore = 4;
// 			}
// 			else if(diCoverDistance >= 9)
// 			{
// 				diCoverScore = 5;
// 			}
// 
// 			itemText = QString::number(diCoverScore);
// 			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
// 			scoreItem->addChild(scoreInfoItem);
// 			if(!scoreItem->isExpanded())
// 			{
// 				scoreItem->setExpanded(true);
// 			}
// 
// 			if(isLowToothShow == 1)
// 				switchLowToothShow();
// 			//switchDiCoverIntersectCurveShow();
// 			switchDiCoverIntersectCurveShow();
// 			switchDiCoverIntersectCurvePointShow();
// 			switchDiCoverIntersectCurvePointShow();
// 		}
// 
// 		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 135)
	{
		// 选点(DI覆合->轮廓曲线->点)

		//if(diOverbiteIntersectCurvePointIndex == 2)
		//{
		//	diOverbiteIntersectCurvePointIndex = 0;

		//	for(int i = 0; i < 2; ++i)
		//	{
		//		this->ren->RemoveActor(diOverbiteIntersectCurvePointActor[i]);
		//	}
		//}

		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][0] = x;
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][1] = y;
		diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][2] = z;

		QString itemText = QString("(").
			append(QString::number(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][0])).append(", ").
			append(QString::number(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][1])).append(", ").
			append(QString::number(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		drawPoint(diOverbiteIntersectCurvePoint[diOverbiteIntersectCurvePointIndex], pickPointColor, diOverbiteIntersectCurvePointActor[diOverbiteIntersectCurvePointIndex]);
		isDiOverbiteIntersectCurvePointShow = 1;

		++diOverbiteIntersectCurvePointIndex;
		if(diOverbiteIntersectCurvePointIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				vtkPlane::ProjectPoint(diOverbiteIntersectCurvePoint[i], 
					pointPositionForArticlePlane, articlePlaneNormal,
					diOverbiteIntersectCurveProjectArticlePlanePoint[i]);
				vtkPlane::ProjectPoint(diOverbiteIntersectCurveProjectArticlePlanePoint[i], 
					verticalPlaneCenter, verticalPlaneNormal,
					diOverbiteIntersectCurveProjectFitPlanePoint[i]);
			}

			diOverbiteDistance =sqrt(vtkMath::Distance2BetweenPoints(diOverbiteIntersectCurveProjectFitPlanePoint[0], diOverbiteIntersectCurveProjectFitPlanePoint[1]));

			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 136);
			removeChildItem(distanceItem);

			itemText = QString::number(diOverbiteDistance);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}
			//QMessageBox::information(NULL, tr("Info"), QString::number(diOverbiteDistance));

			//
			// DI覆合得分
			//

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 132);
			removeChildItem(scoreItem);
			QMessageBox message(QMessageBox::Warning,"判断","是否闭锁合?",QMessageBox::Yes|QMessageBox::No,NULL);  
			if (message.exec()==QMessageBox::Yes)  
			{  
				diOverbiteScore = 5;
			}else
			{
				if(diOverbiteDistance >= 0 && diOverbiteDistance < 3)
				{
					diOverbiteScore = 0;
				}
				else if(diOverbiteDistance >= 3 && diOverbiteDistance < 5)
				{
					diOverbiteScore = 2;
				}
				else if(diOverbiteDistance >= 5 && diOverbiteDistance < 7)
				{
					diOverbiteScore = 3;
				}
// 				else if(diOverbiteDistance >= 5 && diOverbiteDistance < 7)
// 				{
// 					diOverbiteScore = 3;
// 				}
// 				else if(diOverbiteDistance >= 7 && diOverbiteDistance < 9)
// 				{
// 					diOverbiteScore = 4;
// 				}
// 				else if(diOverbiteDistance >= 9)
// 				{
// 					diOverbiteScore = 5;
// 				}
			}

			itemText = QString::number(diOverbiteScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}

			if(isLowToothShow == 1)
				switchLowToothShow();
			//switchDiOverbiteIntersectCurveShow();
			switchDiOverbiteIntersectCurveShow();
			switchDiOverbiteIntersectCurvePointShow();
			switchDiOverbiteIntersectCurvePointShow();
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex / 100 == 147)
	{
		// 选点(DI侧方开合->磨牙1/2/3->轮廓曲线->点)

		int grindToothIndex = selectOptionTreeIndex - 14700;

		diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]][0] = x;
		diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]][1] = y;
		diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]][2] = z;

		QString itemText = QString("(").
			append(QString::number(diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]][0])).append(", ").
			append(QString::number(diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]][1])).append(", ").
			append(QString::number(diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		drawPoint(diSideOpenCloseIntersectCurvePoint[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]], 
			pickPointColor, 
			diSideOpenCloseIntersectCurvePointActor[grindToothIndex][diSideOpenCloseIntersectCurvePointIndex[grindToothIndex]]);
		isDiSideOpenCloseIntersectCurvePointShow[grindToothIndex] = 1;

		++diSideOpenCloseIntersectCurvePointIndex[grindToothIndex];
		if(diSideOpenCloseIntersectCurvePointIndex[grindToothIndex] == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				vtkPlane::ProjectPoint(diSideOpenCloseIntersectCurvePoint[grindToothIndex][i], 
					pointPositionForArticlePlane, articlePlaneNormal,
					diSideOpenCloseIntersectCurveProjectArticlePlanePoint[grindToothIndex][i]);
				vtkPlane::ProjectPoint(diSideOpenCloseIntersectCurveProjectArticlePlanePoint[grindToothIndex][i], 
					verticalPlaneCenter, verticalPlaneNormal,
					diSideOpenCloseIntersectCurveProjectFitPlanePoint[grindToothIndex][i]);
			}

			diSideOpenCloseDistance[grindToothIndex] =sqrt(vtkMath::Distance2BetweenPoints(diSideOpenCloseIntersectCurveProjectFitPlanePoint[grindToothIndex][0], 
				diSideOpenCloseIntersectCurveProjectFitPlanePoint[grindToothIndex][1]));

			QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 14800 + grindToothIndex);
			removeChildItem(distanceItem);

			itemText = QString::number(diSideOpenCloseDistance[grindToothIndex]);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}

			//
			// DI侧方开合得分
			//

			QTreeWidgetItem *grindScoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 14400 + grindToothIndex);
			removeChildItem(grindScoreItem);

			diSideOpenCloseGrindToothScore[grindToothIndex] = ((int)diSideOpenCloseDistance[grindToothIndex]) * 2;
			diSideOpenCloseScore = 0;
			for(int i = 0; i < 8; ++i)
				diSideOpenCloseScore += diSideOpenCloseGrindToothScore[i];

			itemText = QString::number(diSideOpenCloseGrindToothScore[grindToothIndex]);
			QTreeWidgetItem *grindScoreInfoItem = new QTreeWidgetItem(grindScoreItem, QStringList(itemText));
			grindScoreItem->addChild(grindScoreInfoItem);
			if(!grindScoreItem->isExpanded())
			{
				grindScoreItem->setExpanded(true);
			}

			QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent()->parent(), 200);
			removeChildItem(scoreItem);

			itemText = QString::number(diSideOpenCloseScore);
			QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
			scoreItem->addChild(scoreInfoItem);
			if(!scoreItem->isExpanded())
			{
				scoreItem->setExpanded(true);
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 401)
	{
		// 选点(DI前牙开合->点)

		int measureIndex = -1;
		for(int i = 0; i < 20; ++i)
		{
			if(diFrontOpenClosePointStat[i] == false)
			{
				measureIndex = i;
				break;
			}
		}

		if(measureIndex > -1 && diFrontOpenCloseFlag[measureIndex] == 0)
		{
			diFrontOpenClosePoint[measureIndex][0] = x;
			diFrontOpenClosePoint[measureIndex][1] = y;
			diFrontOpenClosePoint[measureIndex][2] = z;
			diFrontOpenClosePointStat[measureIndex] = true;

			drawPoint(diFrontOpenClosePoint[measureIndex], 
				pickPointColor, 
				diFrontOpenClosePointActor[measureIndex]);
			isDiFrontOpenClosePointShow[measureIndex] = 1;

			//
			// 牙弓放射状相交平面
			//

			vtkPlane::ProjectPoint(diFrontOpenClosePoint[measureIndex],
				fitPlaneCenter, fitPlaneNormal,
				diFrontOpenCloseProjectPoint[measureIndex]);
			drawPoint(diFrontOpenCloseProjectPoint[measureIndex], projectToFittingPlanePointColot, diFrontOpenCloseProjectPointActor[measureIndex]);

			initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
				splineProjectPointPosition, &splinePointState[0], 20,
				diFrontOpenCloseProjectPoint[measureIndex],
				50 * pointSize,
				diFrontOpenClosePlaneOCenter[measureIndex], diFrontOpenClosePlaneONormal[measureIndex], diFrontOpenClosePlaneOPoint[measureIndex]);
			drawPoint(diFrontOpenClosePlaneOCenter[measureIndex], projectToFittingPlanePointColot, diFrontOpenCloseCenterPointActor[measureIndex]);
			drawPlane(diFrontOpenClosePlaneOPoint[measureIndex][0], diFrontOpenClosePlaneOPoint[measureIndex][1], diFrontOpenClosePlaneOPoint[measureIndex][2], 
				planeColor, 0.4, diFrontOpenClosePlaneOActor[measureIndex], frameColor, 2, 2, diFrontOpenClosePlaneOWireframeActor[measureIndex]);
			isDiFrontOpenClosePlaneOShow[measureIndex] = 1;

			//
			// 轮廓线
			//

			drawIntersectCurve(lowerToothData, diFrontOpenClosePlaneOCenter[measureIndex], diFrontOpenClosePlaneONormal[measureIndex], intersectCurveColor, 2, diFrontOpenCloseIntersectCurveActor[measureIndex]);
			drawIntersectCurve(topToothData, diFrontOpenClosePlaneOCenter[measureIndex], diFrontOpenClosePlaneONormal[measureIndex], topIntersectCurveColor, 2, diTopFrontOpenCloseIntersectCurveActor[measureIndex]);
			isDiFrontOpenCloseIntersectCurveShow[measureIndex] = 1;

			initDiFrontOpenClosePointItem(currentOptionTreeWidgetItem);
			initDiFrontOpenCloseMeasure(currentOptionTreeWidgetItem->parent(), measureIndex);

			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 0)
				switchLowToothShow();

			double pB[3] = {};
			double ProCenterB[3] = {};
			findLowerCuttingPoint(diFrontOpenCloseIntersectCurveActor[measureIndex],diFrontOpenClosePoint[measureIndex],pB,ProCenterB);		

			selectOptionTreeIndex = 427 + 20 * measureIndex;
			currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
			//A点
			diFrontOpenCloseIntersectCurvePoint[measureIndex][0][0] = x;
			diFrontOpenCloseIntersectCurvePoint[measureIndex][0][1] = y;
			diFrontOpenCloseIntersectCurvePoint[measureIndex][0][2] = z;

			QString itemText = QString("(").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][0][0])).append(", ").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][0][1])).append(", ").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][0][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
			currentOptionTreeWidgetItem->addChild(pointInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			drawPoint(diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]], 
				pickPointColor, 
				diFrontOpenCloseIntersectCurvePointActor[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]]);
			isDiFrontOpenCloseIntersectCurvePointShow[measureIndex] = 1;
			diFrontOpenCloseIntersectCurvePointIndex[measureIndex] ++;
			//B点
			diFrontOpenCloseIntersectCurvePoint[measureIndex][1][0] = pB[0];
			diFrontOpenCloseIntersectCurvePoint[measureIndex][1][1] = pB[1];
			diFrontOpenCloseIntersectCurvePoint[measureIndex][1][2] = pB[2];

			itemText = QString("(").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][1][0])).append(", ").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][1][1])).append(", ").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][1][2])).append(")");
			pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
			currentOptionTreeWidgetItem->addChild(pointInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			drawPoint(diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]], 
				pickPointColor, 
				diFrontOpenCloseIntersectCurvePointActor[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]]);
			diFrontOpenCloseIntersectCurvePointIndex[measureIndex] ++;


			//++diFrontOpenCloseIntersectCurvePointIndex[measureIndex];
			if(diFrontOpenCloseIntersectCurvePointIndex[measureIndex] == 2)
			{
				for(int i = 0; i < 2; ++i)
				{
					vtkPlane::ProjectPoint(diFrontOpenCloseIntersectCurvePoint[measureIndex][i], 
						verticalPlaneCenter, verticalPlaneNormal,
						diFrontOpenCloseIntersectCurveProjectArticlePlanePoint[measureIndex][i]);
					vtkPlane::ProjectPoint(diFrontOpenCloseIntersectCurveProjectArticlePlanePoint[measureIndex][i], 
						pointPositionForArticlePlane, articlePlaneNormal,
						diFrontOpenCloseIntersectCurveProjectFitPlanePoint[measureIndex][i]);
				}

				diFrontOpenCloseDistance[measureIndex] =sqrt(vtkMath::Distance2BetweenPoints(diFrontOpenCloseIntersectCurveProjectFitPlanePoint[measureIndex][0], 
					diFrontOpenCloseIntersectCurveProjectFitPlanePoint[measureIndex][1]));

				QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 428 + measureIndex * 20);
				removeChildItem(distanceItem);

				itemText = QString::number(diFrontOpenCloseDistance[measureIndex]);
				QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
				distanceItem->addChild(distanceInfoItem);
				if(!distanceItem->isExpanded())
				{
					distanceItem->setExpanded(true);
				}

				//
				// DI前牙开合得分
				//
				diFrontOpenCloseMeasureScore[measureIndex] = (int)diFrontOpenCloseDistance[measureIndex] + 1;
				QTreeWidgetItem *scoreItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 424 + measureIndex * 20);
				removeChildItem(scoreItem);

				itemText = QString::number(diFrontOpenCloseMeasureScore[measureIndex]);
				QTreeWidgetItem *scoreInfoItem = new QTreeWidgetItem(scoreItem, QStringList(itemText));
				distanceItem->addChild(scoreInfoItem);
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex / 100 > 1030 && selectOptionTreeIndex / 100 < 1270)
	{
		if((selectOptionTreeIndex - 1031 * 100) < 100)
		{
			// 选点(DI拥挤度->磨牙->测量1/../12->点)

			int measureIndex = (selectOptionTreeIndex - 1031 * 100);
			cout<<"拥挤度磨牙"<<endl;
			diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]][0] = x;
			diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]][1] = y;
			diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]][2] = z;

			QString itemText = QString("(").
				append(QString::number(diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]][0])).append(", ").
				append(QString::number(diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]][1])).append(", ").
				append(QString::number(diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
			currentOptionTreeWidgetItem->addChild(pointInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			drawPoint(diGrindToothScrowdPoint[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]], 
				pickPointColor, 
				diGrindToothScrowdPointActor[measureIndex * 2 + diGrindToothScrowdPointIndex[measureIndex]]);
			isDiGrindToothScrowdToothShow[measureIndex] = 1;

			++diGrindToothScrowdPointIndex[measureIndex];
			if(diGrindToothScrowdPointIndex[measureIndex] == 2)
			{
				diGrindToothScrowdPointState[measureIndex] = true;
				diGrindToothScrowdDistance[measureIndex] = sqrt(vtkMath::Distance2BetweenPoints(diGrindToothScrowdPoint[2 * measureIndex], diGrindToothScrowdPoint[2 * measureIndex + 1]));

				QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), measureIndex + 1032 * 100);
				removeChildItem(distanceItem);

				itemText = QString::number(diGrindToothScrowdDistance[measureIndex]);
				QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
				distanceItem->addChild(distanceInfoItem);
				if(!distanceItem->isExpanded())
				{
					distanceItem->setExpanded(true);
				}

				updateDiGrindToothScrowdSumDistance(getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 1028));
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex > 1270 && selectOptionTreeIndex < 1350)
	{
		if(((selectOptionTreeIndex - 1271) % 20) == 0)
		{
			// 选点(DI拥挤度->牙弓->测量1/../4->点)

			int measureIndex = (selectOptionTreeIndex - 1271) / 20;

			diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]][0] = x;
			diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]][1] = y;
			diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]][2] = z;

			QString itemText = QString("(").
				append(QString::number(diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]][0])).append(", ").
				append(QString::number(diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]][1])).append(", ").
				append(QString::number(diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
			currentOptionTreeWidgetItem->addChild(pointInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			drawPoint(diToothBowScrowdPoint[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]], 
				pickPointColor, 
				diToothBowScrowdPointActor[measureIndex * 2 + diToothBowScrowdPointIndex[measureIndex]]);
			isDiToothBowScrowdShow[measureIndex] = 1;

			++diToothBowScrowdPointIndex[measureIndex];
			if(diToothBowScrowdPointIndex[measureIndex] == 2)
			{
				diToothBowScrowdPointState[measureIndex] = true;
				diToothBowScrowdDistance[measureIndex] = sqrt(vtkMath::Distance2BetweenPoints(diToothBowScrowdPoint[2 * measureIndex], diToothBowScrowdPoint[2 * measureIndex + 1]));

				QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), measureIndex * 20 + 1272);
				removeChildItem(distanceItem);

				itemText = QString::number(diToothBowScrowdDistance[measureIndex]);
				QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
				distanceItem->addChild(distanceInfoItem);
				if(!distanceItem->isExpanded())
				{
					distanceItem->setExpanded(true);
				}

				updateDiToothBowScrowdSumDistance(getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 1029));
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex > 420 && selectOptionTreeIndex < 820)
	{
		if(((selectOptionTreeIndex - 427) % 20) == 0)
		{
			// 选点(DI前牙开合->测量1/.../20->轮廓曲线->点)

			int measureIndex = (selectOptionTreeIndex - 427) / 20;

			diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]][0] = x;
			diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]][1] = y;
			diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]][2] = z;

			QString itemText = QString("(").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]][0])).append(", ").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]][1])).append(", ").
				append(QString::number(diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(itemText));
			currentOptionTreeWidgetItem->addChild(pointInfoItem);
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			drawPoint(diFrontOpenCloseIntersectCurvePoint[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]], 
				pickPointColor, 
				diFrontOpenCloseIntersectCurvePointActor[measureIndex][diFrontOpenCloseIntersectCurvePointIndex[measureIndex]]);
			isDiFrontOpenCloseIntersectCurvePointShow[measureIndex] = 1;

			++diFrontOpenCloseIntersectCurvePointIndex[measureIndex];
			if(diFrontOpenCloseIntersectCurvePointIndex[measureIndex] == 2)
			{
				for(int i = 0; i < 2; ++i)
				{
					vtkPlane::ProjectPoint(diFrontOpenCloseIntersectCurvePoint[measureIndex][i], 
						pointPositionForArticlePlane, articlePlaneNormal,
						diFrontOpenCloseIntersectCurveProjectArticlePlanePoint[measureIndex][i]);
					vtkPlane::ProjectPoint(diFrontOpenCloseIntersectCurveProjectArticlePlanePoint[measureIndex][i], 
						verticalPlaneCenter, verticalPlaneNormal,
						diFrontOpenCloseIntersectCurveProjectFitPlanePoint[measureIndex][i]);
				}

				diFrontOpenCloseDistance[measureIndex] =sqrt(vtkMath::Distance2BetweenPoints(diFrontOpenCloseIntersectCurveProjectFitPlanePoint[measureIndex][0], 
					diFrontOpenCloseIntersectCurveProjectFitPlanePoint[measureIndex][1]));

				QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 428 + measureIndex * 20);
				removeChildItem(distanceItem);

				itemText = QString::number(diFrontOpenCloseDistance[measureIndex]);
				QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(itemText));
				distanceItem->addChild(distanceInfoItem);
				if(!distanceItem->isExpanded())
				{
					distanceItem->setExpanded(true);
				}

				//
				// DI前牙开合得分
				//
			}

			this->pickStyle->mode = 0;
		}
		else if(((selectOptionTreeIndex - 421) % 20) == 0)
		{
			// 选点(DI前牙开合->测量1/.../20->点)

			int measureIndex = (selectOptionTreeIndex - 421) / 20;

			diFrontOpenClosePoint[measureIndex][0] = x;
			diFrontOpenClosePoint[measureIndex][1] = y;
			diFrontOpenClosePoint[measureIndex][2] = z;
			diFrontOpenClosePointStat[measureIndex] = true;

			drawPoint(diFrontOpenClosePoint[measureIndex], 
				pickPointColor, 
				diFrontOpenClosePointActor[measureIndex]);
			isDiFrontOpenClosePointShow[measureIndex] = 1;

			//
			// 牙弓放射状相交平面
			//

			vtkPlane::ProjectPoint(diFrontOpenClosePoint[measureIndex],
				fitPlaneCenter, fitPlaneNormal,
				diFrontOpenCloseProjectPoint[measureIndex]);
			drawPoint(diFrontOpenCloseProjectPoint[measureIndex], projectToFittingPlanePointColot, diFrontOpenCloseProjectPointActor[measureIndex]);

			initPlaneByFittingCurve(fitPlaneCenter, fitPlaneNormal, 
				splineProjectPointPosition, &splinePointState[0], 20,
				diFrontOpenCloseProjectPoint[measureIndex],
				50 * pointSize,
				diFrontOpenClosePlaneOCenter[measureIndex], diFrontOpenClosePlaneONormal[measureIndex], diFrontOpenClosePlaneOPoint[measureIndex]);
			drawPoint(diFrontOpenClosePlaneOCenter[measureIndex], projectToFittingPlanePointColot, diFrontOpenCloseCenterPointActor[measureIndex]);
			drawPlane(diFrontOpenClosePlaneOPoint[measureIndex][0], diFrontOpenClosePlaneOPoint[measureIndex][1], diFrontOpenClosePlaneOPoint[measureIndex][2], 
				planeColor, 0.4, diFrontOpenClosePlaneOActor[measureIndex], frameColor, 2, 2, diFrontOpenClosePlaneOWireframeActor[measureIndex]);
			isDiFrontOpenClosePlaneOShow[measureIndex] = 1;

			//
			// 轮廓线
			//

			drawIntersectCurve(lowerToothData, diFrontOpenClosePlaneOCenter[measureIndex], diFrontOpenClosePlaneONormal[measureIndex], intersectCurveColor, 2, diFrontOpenCloseIntersectCurveActor[measureIndex]);
			drawIntersectCurve(topToothData, diFrontOpenClosePlaneOCenter[measureIndex], diFrontOpenClosePlaneONormal[measureIndex], topIntersectCurveColor, 2, diTopFrontOpenCloseIntersectCurveActor[measureIndex]);
			isDiFrontOpenCloseIntersectCurveShow[measureIndex] = 1;

			QTreeWidgetItem *pointItem = getChildItem(currentOptionTreeWidgetItem->parent()->parent(), 401);
			initDiFrontOpenClosePointItem(pointItem);
			initDiFrontOpenCloseMeasure(pointItem->parent(), measureIndex);

			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 0)
				switchLowToothShow();

			this->pickStyle->mode = 0;
		}
	}
	else if(selectOptionTreeIndex / 1000 == 9 && selectOptionTreeIndex % 1000 < 10)//前牙区不齐
	{
		int gapIndex = selectOptionTreeIndex % 1000;
		for(int i = 0; i < 2; ++i)
		{
			if(gapPointStat[gapIndex][i] == false)
			{
				gapPointStat[gapIndex][i] = true;

				gapPointPosition[gapIndex][i][0] = x;
				gapPointPosition[gapIndex][i][1] = y;
				gapPointPosition[gapIndex][i][2] = z;

				double color[3] = {1, 0, 0};
				drawPoint(gapPointPosition[gapIndex][i], color, gapPointActor[gapIndex][i]);
				isGapPointShow = 1;

				QString pointInfoItemText = QString("(").
					append(QString::number(x)).append(", ").
					append(QString::number(y)).append(", ").
					append(QString::number(z)).append(")");
				QTreeWidgetItem *gapPointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(pointInfoItemText));
				currentOptionTreeWidgetItem->addChild(gapPointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 103 || selectOptionTreeIndex == 104
		|| selectOptionTreeIndex == 105 || selectOptionTreeIndex == 106)
	{
		int index = selectOptionTreeIndex % 103;

		for(int i = 0; i < 2; ++i)
		{
			if(topPointToProjectArticlePlaneStat[index][i] == false)
			{
				topPointToProjectArticlePlaneStat[index][i] = true;

				topPointToProjectArticlePlanePosition[index][i][0] = x;
				topPointToProjectArticlePlanePosition[index][i][1] = y;
				topPointToProjectArticlePlanePosition[index][i][2] = z;

				double color[3] = {0.6549, 0.9569, 0.647};
				drawPoint(topPointToProjectArticlePlanePosition[index][i], color, topPointToProjectArticlePlaneActor[index][i]);
				isTopPointToProjectArticlePlaneShow = 1;

				QString pointInfoItemText = QString("(").
					append(QString::number(x)).append(", ").
					append(QString::number(y)).append(", ").
					append(QString::number(z)).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(pointInfoItemText));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 107 || selectOptionTreeIndex == 108
		|| selectOptionTreeIndex == 109 || selectOptionTreeIndex == 110)
	{
		int index = selectOptionTreeIndex % 107;

		for(int i = 0; i < 2; ++i)
		{
			if(lowPointToProjectArticlePlaneStat[index][i] == false)
			{
				lowPointToProjectArticlePlaneStat[index][i] = true;

				lowPointToProjectArticlePlanePosition[index][i][0] = x;
				lowPointToProjectArticlePlanePosition[index][i][1] = y;
				lowPointToProjectArticlePlanePosition[index][i][2] = z;

				double color[3] = {0.6549, 0.9569, 0.647};
				drawPoint(lowPointToProjectArticlePlanePosition[index][i], color, lowPointToProjectArticlePlaneActor[index][i]);
				isLowPointToProjectArticlePlaneShow = 1;

				QString pointInfoItemText = QString("(").
					append(QString::number(x)).append(", ").
					append(QString::number(y)).append(", ").
					append(QString::number(z)).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(pointInfoItemText));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 23)
	{
		// 选取正中矢状面初始点

		pointInVerticalPlaneIndex++;
		if(pointInVerticalPlaneIndex == 0 || pointInVerticalPlaneIndex == 1)
		{
			if(!currentOptionTreeWidgetItem->isExpanded())
			{
				currentOptionTreeWidgetItem->setExpanded(true);
			}

			pointInVerticalPlanePosition[pointInVerticalPlaneIndex][0] = x;
			pointInVerticalPlanePosition[pointInVerticalPlaneIndex][1] = y;
			pointInVerticalPlanePosition[pointInVerticalPlaneIndex][2] = z;
			pointInVerticalPlaneStat[pointInVerticalPlaneIndex] = true;

			double pointColor[3] = {1, 0, 0};
			drawPoint(pointInVerticalPlanePosition[pointInVerticalPlaneIndex], pointColor, 
				pointInVerticalPlaneActor[pointInVerticalPlaneIndex]);
			isPointInVerticalPlaneShow = 1;

			if(pointInVerticalPlaneIndex == 1 || (pointInVerticalPlaneStat[0] && pointInVerticalPlaneStat[1]))
			{
				if(pointInVerticalPlaneIndex == 1)
				{
					pointInVerticalPlaneIndex = -1;
				}
				this->pickStyle->mode = 0;

				if(isFitPlaneShow == 0)
				{
					switchFitPlaneShow();
				}

				verticalPlaneCenter[0] = (pointInVerticalPlanePosition[0][0] + pointInVerticalPlanePosition[1][0]) / 2.0;
				verticalPlaneCenter[1] = (pointInVerticalPlanePosition[0][1] + pointInVerticalPlanePosition[1][1]) / 2.0;
				verticalPlaneCenter[2] = (pointInVerticalPlanePosition[0][2] + pointInVerticalPlanePosition[1][2]) / 2.0;

				double direct[2][3];
				direct[0][0] = pointInVerticalPlanePosition[0][0] - pointInVerticalPlanePosition[1][0];
				direct[0][1] = pointInVerticalPlanePosition[0][1] - pointInVerticalPlanePosition[1][1];
				direct[0][2] = pointInVerticalPlanePosition[0][2] - pointInVerticalPlanePosition[1][2];
				vtkMath::Normalize(direct[0]);

				vtkMath::Cross(fitPlaneNormal, direct[0], verticalPlaneNormal);
				vtkMath::Normalize(verticalPlaneNormal);

				vtkMath::Cross(direct[0], verticalPlaneNormal, direct[1]);
				vtkMath::Normalize(direct[1]);

				verticalPlanePoint[0][0] = verticalPlaneCenter[0] - 75 * pointSize * direct[0][0] - 75 * pointSize * direct[1][0];
				verticalPlanePoint[0][1] = verticalPlaneCenter[1] - 75 * pointSize * direct[0][1] - 75 * pointSize * direct[1][1];
				verticalPlanePoint[0][2] = verticalPlaneCenter[2] - 75 * pointSize * direct[0][2] - 75 * pointSize * direct[1][2];

				verticalPlanePoint[1][0] = verticalPlanePoint[0][0] + 150 * pointSize * direct[1][0];
				verticalPlanePoint[1][1] = verticalPlanePoint[0][1] + 150 * pointSize * direct[1][1];
				verticalPlanePoint[1][2] = verticalPlanePoint[0][2] + 150 * pointSize * direct[1][2];

				verticalPlanePoint[2][0] = verticalPlanePoint[0][0] + 150 * pointSize * direct[0][0];
				verticalPlanePoint[2][1] = verticalPlanePoint[0][1] + 150 * pointSize * direct[0][1];
				verticalPlanePoint[2][2] = verticalPlanePoint[0][2] + 150 * pointSize * direct[0][2];

				double planeColor[3] = {0.5647, 0.6235, 0.5647};
				double frameColor[3] = {0.5882, 0.7569, 0.5843};
				drawPlane(verticalPlanePoint[0], verticalPlanePoint[1], verticalPlanePoint[2], planeColor, 
					0.4, verticalPlaneActor, frameColor, 2, 2, verticalWireFramePlaneActor);
				isVerticalPlaneShow = 1;

				QTreeWidgetItem *initPointItem = getChildItem(currentOptionTreeWidgetItem, 26);
				this->removeChildItem(initPointItem);
				for(int i = 0; i < 2; ++i)
				{
					if(pointInVerticalPlaneStat[i])
					{
						QString text = QString("(").
							append(QString::number(pointInVerticalPlanePosition[i][0])).append(", ").
							append(QString::number(pointInVerticalPlanePosition[i][1])).append(", ").
							append(QString::number(pointInVerticalPlanePosition[i][2])).append(")");

						QTreeWidgetItem *initPointInfoItem = new QTreeWidgetItem(initPointItem, QStringList(text));
						initPointItem->insertChild(i, initPointInfoItem);
					}
				}
				if(!initPointItem->isExpanded())
				{
					initPointItem->setExpanded(true);
				}

				QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem, 27);
				QString text = QString("(").
						append(QString::number(verticalPlaneNormal[0])).append(", ").
						append(QString::number(verticalPlaneNormal[1])).append(", ").
						append(QString::number(verticalPlaneNormal[2])).append(")");
				QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
				normalItem->addChild(normalInfoItem);
				if(!normalItem->isExpanded())
				{
					normalItem->setExpanded(true);
				}
			}
		}
	}
	else if(selectOptionTreeIndex == 28)//选点 颊侧区咬合->上牙列->点(矢状向)
	{
		topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][0] = x;
		topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][1] = y;
		topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][2] = z;

		double pointColor[3] = {1, 0, 0};
		drawPoint(topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex], 
			pointColor, 
			topToothPointForVerticalPlaneActor[topToothPointForVerticalPlaneIndex]);
		isTopToothPointForVerticalPlaneShow = 1;

		vtkPlane::ProjectPoint(topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex],
			pointInVerticalPlanePosition[0], verticalPlaneNormal,
			topToothProjectPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex]);
		double projectPointColor[3] = {0, 1, 0};
		drawPoint(topToothProjectPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex], 
			projectPointColor, 
			topToothProjectPointForVerticalPlaneActor[topToothPointForVerticalPlaneIndex]);
		isTopToothProjectPointForVerticalPlane = 1;

		if(!currentOptionTreeWidgetItem->isExpanded())
			currentOptionTreeWidgetItem->setExpanded(true);

		QString text = QString("(").
			append(QString::number(topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][0])).append(", ").
			append(QString::number(topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][1])).append(", ").
			append(QString::number(topToothPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);

		QTreeWidgetItem *projectItem = getChildItem(currentOptionTreeWidgetItem, 30);
		text = QString("(").
			append(QString::number(topToothProjectPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][0])).append(", ").
			append(QString::number(topToothProjectPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][1])).append(", ").
			append(QString::number(topToothProjectPointForVerticalPlanePosition[topToothPointForVerticalPlaneIndex][2])).append(")");
		QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(projectItem, QStringList(text));
		projectItem->addChild(projectPointInfoItem);
		if(!projectItem->isExpanded())
			projectItem->setExpanded(true);

		topToothPointForVerticalPlaneIndex++;
		if(topToothPointForVerticalPlaneIndex == 10)
		{
			topToothPointForVerticalPlaneIndex = 0;
		}
	}
	else if(selectOptionTreeIndex == 29)//水平向选点
	{
		for(int i = 0; i < 16; ++i)
		{
			if(topToothPointForFitPlaneStat[i] == false)
			{
				cout<<"水平向选点"<<i<<endl;
				topToothPointForFitPlaneStat[i] = true;

				topToothPointForFitPlanePosition[i][0] = x;
				topToothPointForFitPlanePosition[i][1] = y;
				topToothPointForFitPlanePosition[i][2] = z;

				double pointColor[3] = {1, 0, 0};
				drawPoint(topToothPointForFitPlanePosition[i], 
					pointColor, 
					topToothPointForFitPlaneActor[i]);
				isTopToothPointForFitPlaneShow = 1;

				QString text = QString("(").
					append(QString::number(topToothPointForFitPlanePosition[i][0])).append(", ").
					append(QString::number(topToothPointForFitPlanePosition[i][1])).append(", ").
					append(QString::number(topToothPointForFitPlanePosition[i][2])).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
					currentOptionTreeWidgetItem->setExpanded(true);

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 24)
	{
		// 选点(颊侧区咬合->下牙列->点(矢状向))

// 		for(int i = 0; i < 10; ++i)
// 		{
// 			if(lowToothPointForVerticalPlaneStat[i] == false)
// 			{
// 				lowToothPointForVerticalPlaneStat[i] = true;
// 
// 				lowToothPointForVerticalPlanePosition[i][0] = x;
// 				lowToothPointForVerticalPlanePosition[i][1] = y;
// 				lowToothPointForVerticalPlanePosition[i][2] = z;
// 
// 				double pointColor[3] = {0, 1, 0};
// 				drawPoint(lowToothPointForVerticalPlanePosition[i], 
// 					pointColor, 
// 					lowToothPointForVerticalPlaneActor[i]);
// 				isLowToothPointForVerticalPlaneShow = 1;
// 				//switchLowToothPointForVerticalPlaneShow();
// 
// 				QString text = QString("(").
// 					append(QString::number(lowToothPointForVerticalPlanePosition[i][0])).append(", ").
// 					append(QString::number(lowToothPointForVerticalPlanePosition[i][1])).append(", ").
// 					append(QString::number(lowToothPointForVerticalPlanePosition[i][2])).append(")");
// 				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
// 				currentOptionTreeWidgetItem->addChild(pointInfoItem);
// 
// 				if(!currentOptionTreeWidgetItem->isExpanded())
// 					currentOptionTreeWidgetItem->setExpanded(true);
// 
// 				break;
// 			}
// 		}
		lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][0] = x;
		lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][1] = y;
		lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][2] = z;
		cout<<1<<endl;
		double pointColor[3] = {1, 0, 0};
		drawPoint(lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex], 
			pointColor, 
			lowToothPointForVerticalPlaneActor[lowToothPointForVerticalPlaneIndex]);
		isLowToothPointForVerticalPlaneShow = 1;
		cout<<2<<endl;
		vtkPlane::ProjectPoint(lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex],
			pointInVerticalPlanePosition[0], verticalPlaneNormal,
			lowToothProjectPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex]);
		double projectPointColor[3] = {1, 0, 0};
		drawPoint(lowToothProjectPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex], 
			projectPointColor, 
			lowToothProjectPointForVerticalPlaneActor[lowToothPointForVerticalPlaneIndex]);
		isLowToothProjectPointForVerticalPlaneShow = 1;
		cout<<3<<endl;
		if(!currentOptionTreeWidgetItem->isExpanded())
			currentOptionTreeWidgetItem->setExpanded(true);

		QString text = QString("(").
			append(QString::number(lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][0])).append(", ").
			append(QString::number(lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][1])).append(", ").
			append(QString::number(lowToothPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		cout<<4<<endl;
		QTreeWidgetItem *projectItem = getChildItem(currentOptionTreeWidgetItem, 32);
		text = QString("(").
			append(QString::number(lowToothProjectPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][0])).append(", ").
			append(QString::number(lowToothProjectPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][1])).append(", ").
			append(QString::number(lowToothProjectPointForVerticalPlanePosition[lowToothPointForVerticalPlaneIndex][2])).append(")");
		QTreeWidgetItem *projectPointInfoItem = new QTreeWidgetItem(projectItem, QStringList(text));
		projectItem->addChild(projectPointInfoItem);
		if(!projectItem->isExpanded())
			projectItem->setExpanded(true);
		cout<<5<<endl;
		lowToothPointForVerticalPlaneIndex++;
		if(lowToothPointForVerticalPlaneIndex == 10)
		{
			lowToothPointForVerticalPlaneIndex = 0;
		}
	}
	else if(selectOptionTreeIndex == 25)
	{
		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForFitPlaneStat[i] == false)
			{
				lowToothPointForFitPlaneStat[i] = true;

				lowToothPointForFitPlanePosition[i][0] = x;
				lowToothPointForFitPlanePosition[i][1] = y;
				lowToothPointForFitPlanePosition[i][2] = z;

				double pointColor[3] = {1, 0, 0};
				drawPoint(lowToothPointForFitPlanePosition[i], 
					pointColor, 
					lowToothPointForFitPlaneActor[i]);
				isLowToothPointForFitPlaneShow = 1;

				QString text = QString("(").
					append(QString::number(lowToothPointForFitPlanePosition[i][0])).append(", ").
					append(QString::number(lowToothPointForFitPlanePosition[i][1])).append(", ").
					append(QString::number(lowToothPointForFitPlanePosition[i][2])).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);

				if(!currentOptionTreeWidgetItem->isExpanded())
					currentOptionTreeWidgetItem->setExpanded(true);

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 35)
	{
		// 选点(中线->矢状平行面1)
		verticalParallelPlaneInitPointPosition[0] = x;
		verticalParallelPlaneInitPointPosition[1] = y;
		verticalParallelPlaneInitPointPosition[2] = z;

		double pointColor[3] = {1, 0, 0};
		drawPoint(verticalParallelPlaneInitPointPosition, 
			pointColor, 
			verticalParallelInitPointActor);
		isVerticalParallelPlaneInitPointShow = 1;

		QTreeWidgetItem *initPointItem = getChildItem(currentOptionTreeWidgetItem, 36);
		this->removeChildItem(initPointItem);
		QString text = QString("(").
			append(QString::number(verticalParallelPlaneInitPointPosition[0])).append(", ").
			append(QString::number(verticalParallelPlaneInitPointPosition[1])).append(", ").
			append(QString::number(verticalParallelPlaneInitPointPosition[2])).append(")");
		QTreeWidgetItem *initPointInfoItem = new QTreeWidgetItem(initPointItem, QStringList(text));
		initPointItem->addChild(initPointInfoItem);
		if(!initPointItem->isExpanded())
		{
			initPointItem->setExpanded(true);
		}

		double projectPoint[3];
		vtkPlane::ProjectPoint(verticalParallelPlaneInitPointPosition,
			verticalPlaneCenter, verticalPlaneNormal,
			projectPoint);
		distanceToVerticalPlane = sqrt(vtkMath::Distance2BetweenPoints(projectPoint, verticalParallelPlaneInitPointPosition));

		//QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem, 38);
		//text = QString::number(distanceToVerticalPlane);
		//QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(text));
		//distanceItem->addChild(distanceInfoItem);
		//if(!distanceItem->isExpanded())
		//{
		//	distanceItem->setExpanded(true);
		//}

		double point[3][3];
		for(int i = 0; i < 3; ++i)
		{
			vtkPlane::ProjectPoint(verticalPlanePoint[i],
				verticalParallelPlaneInitPointPosition, verticalPlaneNormal,
				point[i]);
		}

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {0.5882, 0.7569, 0.5843};
		drawPlane(point[0], point[1], point[2], planeColor, 0.4, verticalparallelPlaneActor, 
			frameColor, 2, 2, verticalParallelWireFramePlaneActor);
		
		isVerticalParallelPlaneShow = 1;
		verticalParallelPlaneStat[0] = true;

		QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem, 37);
		this->removeChildItem(normalItem);
		text = QString("(").
			append(QString::number(verticalPlaneNormal[0])).append(", ").
			append(QString::number(verticalPlaneNormal[1])).append(", ").
			append(QString::number(verticalPlaneNormal[2])).append(")");
		QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
		normalItem->addChild(normalInfoItem);
		if(!normalItem->isExpanded())
		{
			normalItem->setExpanded(true);
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 97)
	{
		// 选点(中线->矢状平行面1)

		verticalParallelPlaneInitPoint2Position[0] = x;
		verticalParallelPlaneInitPoint2Position[1] = y;
		verticalParallelPlaneInitPoint2Position[2] = z;

		double pointColor[3] = {1, 0, 0};
		drawPoint(verticalParallelPlaneInitPoint2Position, 
			pointColor, 
			verticalParallelInitPoint2Actor);
		isVerticalParallelPlaneInitPoint2Show = 1;

		QTreeWidgetItem *initPointItem = getChildItem(currentOptionTreeWidgetItem, 99);
		this->removeChildItem(initPointItem);
		QString text = QString("(").
			append(QString::number(verticalParallelPlaneInitPointPosition[0])).append(", ").
			append(QString::number(verticalParallelPlaneInitPointPosition[1])).append(", ").
			append(QString::number(verticalParallelPlaneInitPointPosition[2])).append(")");
		QTreeWidgetItem *initPointInfoItem = new QTreeWidgetItem(initPointItem, QStringList(text));
		initPointItem->addChild(initPointInfoItem);
		if(!initPointItem->isExpanded())
		{
			initPointItem->setExpanded(true);
		}

		double point[3][3];
		for(int i = 0; i < 3; ++i)
		{
			vtkPlane::ProjectPoint(verticalPlanePoint[i],
				verticalParallelPlaneInitPoint2Position, verticalPlaneNormal,
				point[i]);
		}

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {0.5882, 0.7569, 0.5843};
		drawPlane(point[0], point[1], point[2], planeColor, 0.4, verticalparallelPlane2Actor, 
			frameColor, 2, 2, verticalParallelWireFramePlane2Actor);

		isVerticalParallelPlane2Show = 1;
		verticalParallelPlaneStat[1] = true;

		QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem, 100);
		this->removeChildItem(normalItem);
		text = QString("(").
			append(QString::number(verticalPlaneNormal[0])).append(", ").
			append(QString::number(verticalPlaneNormal[1])).append(", ").
			append(QString::number(verticalPlaneNormal[2])).append(")");
		QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
		normalItem->addChild(normalInfoItem);
		if(!normalItem->isExpanded())
		{
			normalItem->setExpanded(true);
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 39)
	{
		if(indexOfTwoSelectPointOnLowerModel == 0)
		{
			if(isTwoPointToProjectFitPlaneShow == 1)
				switchTwoPointToProjectFitPlaneShow();

			removeChildItemExcept(currentOptionTreeWidgetItem, 40);
		}

		towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel][0] = x;
		towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel][1] = y;
		towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel][2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel],
			color,
			towPointToProjectFitPlaneActor[indexOfTwoSelectPointOnLowerModel]);
		isTwoPointToProjectFitPlaneShow = 1;

		QString text = QString("(").
			append(QString::number(towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel][0])).append(", ").
			append(QString::number(towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel][1])).append(", ").
			append(QString::number(towPointToProjectFitPlanePositon[indexOfTwoSelectPointOnLowerModel][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		++indexOfTwoSelectPointOnLowerModel;	
		if(indexOfTwoSelectPointOnLowerModel == 2)
		{
			indexOfTwoSelectPointOnLowerModel = 0;
			this->pickStyle->mode = 0;
		}
	}
	else if(selectOptionTreeIndex == 45)//覆合
	{
		this->pickStyle->mode = 0;

		pointInOverbiteParallelPlanePosition[0] = x;
		pointInOverbiteParallelPlanePosition[1] = y;
		pointInOverbiteParallelPlanePosition[2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(pointInOverbiteParallelPlanePosition,
			color,
			pointForOverbitParallelPlaneActor);
		isPointInOverbiteParallelPlaneShow = 1;

		QTreeWidgetItem *initPointItem = getChildItem(currentOptionTreeWidgetItem, 46);
		removeChildItem(initPointItem);
		QString text = QString("(").
			append(QString::number(pointInOverbiteParallelPlanePosition[0])).append(", ").
			append(QString::number(pointInOverbiteParallelPlanePosition[1])).append(", ").
			append(QString::number(pointInOverbiteParallelPlanePosition[2])).append(")");
		QTreeWidgetItem *initPointInfoItem = new QTreeWidgetItem(initPointItem, QStringList(text));
		initPointItem->addChild(initPointInfoItem);
		if(!initPointItem->isExpanded())
		{
			initPointItem->setExpanded(true);
		}

		QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem, 47);
		removeChildItem(normalItem);
		text = QString("(").
			append(QString::number(fitPlaneNormal[0])).append(", ").
			append(QString::number(fitPlaneNormal[1])).append(", ").
			append(QString::number(fitPlaneNormal[2])).append(")");
		QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
		normalItem->addChild(normalInfoItem);
		if(!normalItem->isExpanded())
		{
			normalItem->setExpanded(true);
		}

		double point[3][3];
		for(int i = 0; i < 3; ++i)
		{
			vtkPlane::ProjectPoint(fitPlanePoint[i],
				pointInOverbiteParallelPlanePosition, fitPlaneNormal,
				point[i]);
		}

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {0.5882, 0.7569, 0.5843};
		drawPlane(point[0], point[1], point[2], planeColor, 0.4, overbiteParallelPlaneActor, frameColor, 2, 2, overbiteParallelPlaneWireFrameActor);
		isOverbiteParrelPlaneShow = 1;

		vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
		plane->SetOrigin(pointInOverbiteParallelPlanePosition);
		plane->SetNormal(fitPlaneNormal);

		vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
		cutter->SetCutFunction(plane);
		cutter->SetInput(lowerToothData);
		cutter->Update();

		vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cutterMapper->SetInputConnection(cutter->GetOutputPort());

		overbiteIntersectCurveActor->GetProperty()->SetColor(1.0, 1, 0);
		overbiteIntersectCurveActor->GetProperty()->SetLineWidth(2);
		overbiteIntersectCurveActor->SetMapper(cutterMapper);

		vtkSmartPointer<vtkPlane> plane1 = vtkSmartPointer<vtkPlane>::New();
		plane1->SetOrigin(pointInOverbiteParallelPlanePosition);
		plane1->SetNormal(fitPlaneNormal);

		vtkSmartPointer<vtkCutter> cutter1 = vtkSmartPointer<vtkCutter>::New();
		cutter1->SetCutFunction(plane1);
		cutter1->SetInput(topToothData);
		cutter1->Update();

		vtkSmartPointer<vtkPolyDataMapper> cutterMapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
		cutterMapper1->SetInputConnection(cutter1->GetOutputPort());

		topOverbiteIntersectCurveActor->GetProperty()->SetColor(1.0, 1, 0);
		topOverbiteIntersectCurveActor->GetProperty()->SetLineWidth(2);
		topOverbiteIntersectCurveActor->SetMapper(cutterMapper1);

		this->ren->AddActor(overbiteIntersectCurveActor);
		this->ren->AddActor(topOverbiteIntersectCurveActor);
		if (isTopToothShow == 1)
		{
			switchTopToothShow();
		}
		isOverbiteIntersectCurveShow = 1;
	}
	else if(selectOptionTreeIndex == 51 || selectOptionTreeIndex == 52 || selectOptionTreeIndex == 53 || selectOptionTreeIndex == 54 ||
		selectOptionTreeIndex == 56 || selectOptionTreeIndex == 57 || selectOptionTreeIndex == 58 || selectOptionTreeIndex == 59)
	{
		int index = topLowIndex * 8 + pairIndex * 2;
		//QMessageBox::information(NULL, tr("Info"), QString::number(topLowIndex).append(", ").append(QString::number(pairIndex)).append(", ").
		//	append(QString::number(index)));
		
		if(inverseFitCoverPointStat[index] && inverseFitCoverPointStat[index + 1])
		{
			removeChildItem(currentOptionTreeWidgetItem);

			inverseFitCoverPointStat[index] = inverseFitCoverPointStat[index + 1] = false;

			this->ren->RemoveActor(inverseFitCoverPointActor[index]);
			this->ren->RemoveActor(inverseFitCoverPointActor[index + 1]);
		}

		for(int i = 0; i < 2; ++i)
		{
			if(inverseFitCoverPointStat[index + i] == false)
			{
				inverseFitCoverPointStat[index + i] = true;

				inverseFitCoverPointPosition[index + i][0] = x;
				inverseFitCoverPointPosition[index + i][1] = y;
				inverseFitCoverPointPosition[index + i][2] = z;				

				QString text = QString("(").
					append(QString::number(inverseFitCoverPointPosition[index + i][0])).append(", ").
					append(QString::number(inverseFitCoverPointPosition[index + i][1])).append(", ").
					append(QString::number(inverseFitCoverPointPosition[index + i][2])).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);
				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}

				break;
			}			
		}

		for(int i = 0; i < 16; ++i)
		{
			if(inverseFitCoverPointStat[i])
			{
				double color[3] = {1, 0, 0};
				drawPoint(inverseFitCoverPointPosition[i], 
					color,
					inverseFitCoverPointActor[i]);
				isInverseFitCoverPointShow = 1;
			}
		}

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 64)
	{
		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i] == false)
			{
				backToothOpenCloseFittingPointStat[i] = true;

				backToothOpenCloseFittingPointPosition[i][0] = x;
				backToothOpenCloseFittingPointPosition[i][1] = y;
				backToothOpenCloseFittingPointPosition[i][2] = z;

				double color[3] = {1, 0, 0};
				drawPoint(backToothOpenCloseFittingPointPosition[i], color, backToothOpenCloseFittingPointActor[i]);
				isBackToothOpenCloseFittingPointShow = 1;

				QString text = QString("(").
					append(QString::number(backToothOpenCloseFittingPointPosition[i][0])).append(", ").
					append(QString::number(backToothOpenCloseFittingPointPosition[i][1])).append(", ").
					append(QString::number(backToothOpenCloseFittingPointPosition[i][2])).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);
				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 66)
	{
		// 选点(覆盖->拟合曲线->点)

		currentOptionTreeWidgetItem->setExpanded(true);
		QTreeWidgetItem *pointItem = getChildItem(currentOptionTreeWidgetItem, 67);
		pointItem->setExpanded(true);

		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i] == false)
			{
				splinePointState[i] = true;

				splintPointPosition[i][0] = x;
				splintPointPosition[i][1] = y;
				splintPointPosition[i][2] = z;

				break;
			}
		}
		cout<<"FIT POINT"<<endl;
		removeChildItem(pointItem);

		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])
			{
				double color[3] = {1, 0, 0};
				drawPoint(splintPointPosition[i], color, splinePointActor[i]);
				isSplinePointShow = 1;

				QString text = QString("(").
					append(QString::number(splintPointPosition[i][0])).append(", ").
					append(QString::number(splintPointPosition[i][1])).append(", ").
					append(QString::number(splintPointPosition[i][2])).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(text));
				pointItem->addChild(pointInfoItem);
				if(!pointItem->isExpanded())
				{
					pointItem->setExpanded(true);
				}
			}
		}
 	}
	else if(selectOptionTreeIndex == 79)
	{
		backToothPointPositionA0[0] = x;
		backToothPointPositionA0[1] = y;
		backToothPointPositionA0[2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(backToothPointPositionA0, color, backToothPointA0Actor);
		isBackToothPointA0Show = 1;

		removeChildItem(currentOptionTreeWidgetItem);
		QString text = QString("(").
			append(QString::number(backToothPointPositionA0[0])).append(", ").
			append(QString::number(backToothPointPositionA0[1])).append(", ").
			append(QString::number(backToothPointPositionA0[2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		vtkPlane::ProjectPoint(backToothPointPositionA0,
			fitPlaneCenter, fitPlaneNormal,
			packToothProjectPointPositionA0);
		color[0] = 0;
		color[1] = 1;
		color[2] = 0;
		drawPoint(packToothProjectPointPositionA0, color, backToothProjectPointA0Actor);

		mxArray *fromMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(fromMxArray), (void *)fitPlaneNormal, sizeof(fitPlaneNormal));
		engPutVariable(ep, "fromVec", fromMxArray);

		double to[3] = {0, 0, 1};
		mxArray *toMxArray = mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(toMxArray), (void *)to, sizeof(to));
		engPutVariable(ep, "toVec", toMxArray);

		engEvalString(ep, "r = vrrotvec(fromVec, toVec);");
		engEvalString(ep, "m = vrrotvec2mat(r);");

		int pointNum = 0;
		double firstPoint[3], firstIndex = 0;
		double points[20][3];
		engEvalString(ep, "aa = [];");
		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i])
			{
				for(int j = 0; j < 3; ++j)
				{
					if(pointNum == 0)
					{
						firstPoint[j] = backToothOpenCloseFittingProjectPointPosition[i][j];
						firstIndex = j;
					}

					points[i][j] = backToothOpenCloseFittingProjectPointPosition[i][j] - firstPoint[j];
				}
				++pointNum;

				mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
				memcpy((void *)mxGetPr(tempArray), (void *)points[i], sizeof(points[i]));
				engPutVariable(ep, "tempVec", tempArray);

				engEvalString(ep, "b = tempVec * m';");
				engEvalString(ep, "aa = [aa; b];");

				mxDestroyArray(tempArray);
			}
		}

		double tranProjectA0[3];
		tranProjectA0[0] = packToothProjectPointPositionA0[0] - firstPoint[0];
		tranProjectA0[1] = packToothProjectPointPositionA0[1] - firstPoint[1];
		tranProjectA0[2] = packToothProjectPointPositionA0[2] - firstPoint[2];
		mxArray *tempArray =  mxCreateDoubleMatrix(1, 3, mxREAL);
		memcpy((void *)mxGetPr(tempArray), (void *)tranProjectA0, sizeof(tranProjectA0));
		engPutVariable(ep, "ppTemp", tempArray);
		engEvalString(ep, "pp = ppTemp * m';");

		engEvalString(ep, "m1 = rotz(atand(aa(length(aa), 2) / aa(length(aa), 1)));");
		engEvalString(ep, "bb =aa * m1;");
		engEvalString(ep, "pp = pp * m1;");
		engEvalString(ep, "p = polyfit(bb(:,1),bb(:,2),5);");
		engEvalString(ep, "options = optimoptions('fminunc','Display','off');");
		engEvalString(ep, "x0 = 0;");

		QString pathString = QString("path(path, '").append(this->path).append("');");
		//engEvalString(ep, "path(path, 'd:');");
		engEvalString(ep, qPrintable(pathString));
		engEvalString(ep, "[xmin, fval] = fminunc(@myfun,x0,options,pp,p);");
		engEvalString(ep, "y = p(1)*xmin^3 + p(2)*xmin^2 + p(3)*xmin + p(4);");
		engEvalString(ep, "temResult = [xmin, y, pp(3)];");
		engEvalString(ep, "result = temResult * ((m1)^-1);");
		engEvalString(ep, "result = result * ((m')^-1);");

		mxArray *resultMxArray = engGetVariable(ep, "result");
		double *result = mxGetPr(resultMxArray);
		backToothPlaneOCenter[0] = result[0] + firstPoint[0];
		backToothPlaneOCenter[1] = result[1] + firstPoint[1];
		backToothPlaneOCenter[2] = result[2] + firstPoint[2];

		color[0] = 0;
		color[1] = 0;
		color[2] = 1;
		drawPoint(backToothPlaneOCenter, color, backToothPlaneCenterActor);

		double lineDirectionInOP[3];
		lineDirectionInOP[0] = backToothPlaneOCenter[0] - packToothProjectPointPositionA0[0];
		lineDirectionInOP[1] = backToothPlaneOCenter[1] - packToothProjectPointPositionA0[1];
		lineDirectionInOP[2] = backToothPlaneOCenter[2] - packToothProjectPointPositionA0[2];
		vtkMath::Normalize(lineDirectionInOP);

		double a0[3];
		a0[0] = backToothPlaneOCenter[0] + 25 * pointSize * lineDirectionInOP[0];
		a0[1] = backToothPlaneOCenter[1] + 25 * pointSize * lineDirectionInOP[1];
		a0[2] = backToothPlaneOCenter[2] + 25 * pointSize * lineDirectionInOP[2];

		double a1[3];
		a1[0] = backToothPlaneOCenter[0] - 25 * pointSize * lineDirectionInOP[0];
		a1[1] = backToothPlaneOCenter[1] - 25 * pointSize * lineDirectionInOP[1];
		a1[2] = backToothPlaneOCenter[2] - 25 * pointSize * lineDirectionInOP[2];

		double a2[3];
		a2[0] = backToothPlaneOCenter[0] + 25 * pointSize * fitPlaneNormal[0];
		a2[1] = backToothPlaneOCenter[1] + 25 * pointSize * fitPlaneNormal[1];
		a2[2] = backToothPlaneOCenter[2] + 25 * pointSize * fitPlaneNormal[2];
		
		double a3[3];
		a3[0] = backToothPlaneOCenter[0] - 25 * pointSize * fitPlaneNormal[0];
		a3[1] = backToothPlaneOCenter[1] - 25 * pointSize * fitPlaneNormal[1];
		a3[2] = backToothPlaneOCenter[2] - 25 * pointSize * fitPlaneNormal[2];

		double a4[3], a5[3], a6[3];
		backToothPlaneOPoint[0][0] = a1[0] + 35 * pointSize * fitPlaneNormal[0];
		backToothPlaneOPoint[0][1] = a1[1] + 35 * pointSize * fitPlaneNormal[1];
		backToothPlaneOPoint[0][2] = a1[2] + 35 * pointSize * fitPlaneNormal[2];
		backToothPlaneOPoint[1][0] = a1[0] - 35 * pointSize * fitPlaneNormal[0];
		backToothPlaneOPoint[1][1] = a1[1] - 35 * pointSize * fitPlaneNormal[1];
		backToothPlaneOPoint[1][2] = a1[2] - 35 * pointSize * fitPlaneNormal[2];
		backToothPlaneOPoint[2][0] = a0[0] + 35 * pointSize * fitPlaneNormal[0];
		backToothPlaneOPoint[2][1] = a0[1] + 35 * pointSize * fitPlaneNormal[1];
		backToothPlaneOPoint[2][2] = a0[2] + 35 * pointSize * fitPlaneNormal[2];

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {1, 1, 0};
		drawPlane(backToothPlaneOPoint[0], backToothPlaneOPoint[1], backToothPlaneOPoint[2], 
			planeColor, 0.4, backToothPlaneOActor, frameColor, 2, 2, backToothPlaneOWireframeActor);
		isBackToothPlaneOShow = 1;

		double lineOADirect[3];
		lineOADirect[0] = backToothPlaneOCenter[0] - packToothProjectPointPositionA0[0];
		lineOADirect[1] = backToothPlaneOCenter[1] - packToothProjectPointPositionA0[1];
		lineOADirect[2] = backToothPlaneOCenter[2] - packToothProjectPointPositionA0[2];
		vtkMath::Cross(fitPlaneNormal, lineOADirect, backToothPlaneONormal);
		vtkMath::Normalize(backToothPlaneONormal);

		QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem->parent(), 80);
		removeChildItem(normalItem);
		text = QString("(").
			append(QString::number(backToothPlaneONormal[0])).append(", ").
			append(QString::number(backToothPlaneONormal[1])).append(", ").
			append(QString::number(backToothPlaneONormal[2])).append(")");
		QTreeWidgetItem *intersectPlaneNormalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
		normalItem->addChild(intersectPlaneNormalInfoItem);
		if(!normalItem->isExpanded())
		{
			normalItem->setExpanded(true);
		}

		vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
		plane->SetOrigin(backToothPlaneOCenter);
		plane->SetNormal(backToothPlaneONormal);

		vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
		cutter->SetCutFunction(plane);
		cutter->SetInput(lowerToothData);
		cutter->Update();

		vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cutterMapper->SetInputConnection(cutter->GetOutputPort());

		backToothContourCurveActor->GetProperty()->SetColor(1.0, 1, 0);
		backToothContourCurveActor->GetProperty()->SetLineWidth(2);
		backToothContourCurveActor->SetMapper(cutterMapper);

		vtkSmartPointer<vtkPlane> plane1 = vtkSmartPointer<vtkPlane>::New();
		plane1->SetOrigin(backToothPlaneOCenter);
		plane1->SetNormal(backToothPlaneONormal);

		vtkSmartPointer<vtkCutter> cutter1 = vtkSmartPointer<vtkCutter>::New();
		cutter1->SetCutFunction(plane1);
		cutter1->SetInput(topToothData);
		cutter1->Update();

		vtkSmartPointer<vtkPolyDataMapper> cutterMapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
		cutterMapper1->SetInputConnection(cutter1->GetOutputPort());

		backToothTopContourCurveActor->GetProperty()->SetColor(1.0, 1, 0);
		backToothTopContourCurveActor->GetProperty()->SetLineWidth(2);
		backToothTopContourCurveActor->SetMapper(cutterMapper1);

		this->ren->AddActor(backToothContourCurveActor);
		this->ren->AddActor(backToothTopContourCurveActor);
		isBackToothContourCurveShaow = 1;

		this->pickStyle->mode = 0;
	}
	else if(selectOptionTreeIndex == 82)
	{
		QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 83);
		if(backToothContourPointIndex == 0)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			removeChildItem(distanceItem);

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(backToothContourCurvePointActor[i]);
			}
			isBackToothContiourCurvePointShow = 0;
		}

		backToothContourCurvePointPosition[backToothContourPointIndex][0] = x;
		backToothContourCurvePointPosition[backToothContourPointIndex][1] = y;
		backToothContourCurvePointPosition[backToothContourPointIndex][2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(backToothContourCurvePointPosition[backToothContourPointIndex], color, backToothContourCurvePointActor[backToothContourPointIndex]);
		isBackToothContiourCurvePointShow = 1;

		QString text = QString("(").
			append(QString::number(backToothContourCurvePointPosition[backToothContourPointIndex][0])).append(", ").
			append(QString::number(backToothContourCurvePointPosition[backToothContourPointIndex][1])).append(", ").
			append(QString::number(backToothContourCurvePointPosition[backToothContourPointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		++backToothContourPointIndex;
		if(backToothContourPointIndex == 2)
		{
			distanceBackToothContourCurvePoint = sqrt(vtkMath::Distance2BetweenPoints(backToothContourCurvePointPosition[0], backToothContourCurvePointPosition[1]));
			text = QString::number(distanceBackToothContourCurvePoint);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(text));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}

			backToothContourPointIndex = 0;
		}
	}
	else if(selectOptionTreeIndex == 92)
	{
		// 选点(开合->前牙开合->牙弓放射状相交平面->点)

		frontToothToProjectPointPositionA0[0] = x;
		frontToothToProjectPointPositionA0[1] = y;
		frontToothToProjectPointPositionA0[2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(frontToothToProjectPointPositionA0, color, frontToothToProjectPointA0Actor);
		idFrontToothToProjectPointPositionA0Show = 1;

		removeChildItem(currentOptionTreeWidgetItem);
		QString text = QString("(").
			append(QString::number(frontToothToProjectPointPositionA0[0])).append(", ").
			append(QString::number(frontToothToProjectPointPositionA0[1])).append(", ").
			append(QString::number(frontToothToProjectPointPositionA0[2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}
		selectOptionTreeIndex = 89;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		calFittingCurve();

		selectOptionTreeIndex = 95;
		currentOptionTreeWidgetItem = getOptionTreeByData(selectOptionTreeIndex);
		setPickPointPosition(frontToothToProjectPointPositionA0[0],frontToothToProjectPointPositionA0[1],frontToothToProjectPointPositionA0[2]);
		double pB[3] = {};
		double ProCenterB[3] = {};
		findLowerCuttingPoint(frontToothContourCurveActor,frontToothToProjectPointPositionA0,pB,ProCenterB);		
		setPickPointPosition(pB[0],pB[1],pB[2]);

	}
	else if(selectOptionTreeIndex == 85)
	{
		// 冠状面

		this->pickStyle->mode = 0;

		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		pointPositionForArticlePlane[0] = x;
		pointPositionForArticlePlane[1] = y;
		pointPositionForArticlePlane[2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(pointPositionForArticlePlane, color, pointForArticleActor);
		isPointForArticlePlaneShow = 1;

		QTreeWidgetItem *tempItem = getChildItem(currentOptionTreeWidgetItem, 86);
		removeChildItem(tempItem);
		QString text = QString("(").
			append(QString::number(pointPositionForArticlePlane[0])).append(", ").
			append(QString::number(pointPositionForArticlePlane[1])).append(", ").
			append(QString::number(pointPositionForArticlePlane[2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(tempItem, QStringList(text));
		tempItem->addChild(pointInfoItem);
		if(!tempItem->isExpanded())
		{
			tempItem->setExpanded(true);
		}
		

		vtkMath::Cross(fitPlaneNormal, verticalPlaneNormal, articlePlaneNormal);
		vtkMath::Normalize(articlePlaneNormal);

		double origin[3], point1[3], point2[3];
		origin[0] = pointPositionForArticlePlane[0] - 45 * pointSize * fitPlaneNormal[0] + 45 * pointSize * verticalPlaneNormal[0];
		origin[1] = pointPositionForArticlePlane[1] - 45 * pointSize * fitPlaneNormal[1] + 45 * pointSize * verticalPlaneNormal[1]; 
		origin[2] = pointPositionForArticlePlane[2] - 45 * pointSize * fitPlaneNormal[2] + 45 * pointSize * verticalPlaneNormal[2];
		point1[0] = pointPositionForArticlePlane[0] + 45 * pointSize * fitPlaneNormal[0] + 45 * pointSize * verticalPlaneNormal[0];
		point1[1] = pointPositionForArticlePlane[1] + 45 * pointSize * fitPlaneNormal[1] + 45 * pointSize * verticalPlaneNormal[1];
		point1[2] = pointPositionForArticlePlane[2] + 45 * pointSize * fitPlaneNormal[2] + 45 * pointSize * verticalPlaneNormal[2];
		point2[0] = pointPositionForArticlePlane[0] - 45 * pointSize * fitPlaneNormal[0] - 45 * pointSize * verticalPlaneNormal[0];
		point2[1] = pointPositionForArticlePlane[1] - 45 * pointSize * fitPlaneNormal[1] - 45 * pointSize * verticalPlaneNormal[1];
		point2[2] = pointPositionForArticlePlane[2] - 45 * pointSize * fitPlaneNormal[2] - 45 * pointSize * verticalPlaneNormal[2];

		double planeColor[3] = {0.5647, 0.6235, 0.5647};
		double frameColor[3] = {0.5882, 0.7569, 0.5843};
		drawPlane(origin, point1, point2, planeColor, 0.4, articlePlaneActor, frameColor, 2, 2, articalPlaneWireframeActor);
		isArticlePlaneShow = 1;

		QTreeWidgetItem *normalItem = getChildItem(currentOptionTreeWidgetItem, 87);
		removeChildItem(normalItem);
		text = QString("(").
			append(QString::number(articlePlaneNormal[0])).append(", ").
			append(QString::number(articlePlaneNormal[1])).append(", ").
			append(QString::number(articlePlaneNormal[2])).append(")");
		QTreeWidgetItem *normalInfoItem = new QTreeWidgetItem(normalItem, QStringList(text));
		normalItem->addChild(normalInfoItem);
		if(!normalItem->isExpanded())
		{
			normalItem->setExpanded(true);
		}

		if(isFitPlaneShow == 0)
			switchFitPlaneShow();
		if(isVerticalPlaneShow == 0)
			switchVerticalPlaneShow();
	}
	else if(selectOptionTreeIndex == 91)
	{
		//
		// 选点(开合->前牙开合->拟合曲线->点)
		//

		for(int i = 0; i < 20; ++i)
		{
			if(frontToothOpenCloseFittingPointStat[i] == false)
			{
				frontToothOpenCloseFittingPointStat[i] = true;

				frontToothOpenCloseFittingPointPosition[i][0] = x;
				frontToothOpenCloseFittingPointPosition[i][1] = y;
				frontToothOpenCloseFittingPointPosition[i][2] = z;

				double color[3] = {1, 0, 0};
				drawPoint(frontToothOpenCloseFittingPointPosition[i], color, frontToothOpenCloseFittingPointActor[i]);
				isFrontToothOpenCloseFittingPointShow = 1;

				QString text = QString("(").
					append(QString::number(frontToothOpenCloseFittingPointPosition[i][0])).append(", ").
					append(QString::number(frontToothOpenCloseFittingPointPosition[i][1])).append(", ").
					append(QString::number(frontToothOpenCloseFittingPointPosition[i][2])).append(")");
				QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
				currentOptionTreeWidgetItem->addChild(pointInfoItem);
				if(!currentOptionTreeWidgetItem->isExpanded())
				{
					currentOptionTreeWidgetItem->setExpanded(true);
				}

				break;
			}
		}
	}
	else if(selectOptionTreeIndex == 95)//开合
	{
		QTreeWidgetItem *distanceItem = getChildItem(currentOptionTreeWidgetItem->parent(), 96);
		if(frontToothContourPointIndex == 0)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			removeChildItem(distanceItem);

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(frontToothContourCurvePointActor[i]);
			}
			isFrontToothContiourCurvePointShow = 0;
		}

		frontToothContourCurvePointPosition[frontToothContourPointIndex][0] = x;
		frontToothContourCurvePointPosition[frontToothContourPointIndex][1] = y;
		frontToothContourCurvePointPosition[frontToothContourPointIndex][2] = z;

		double color[3] = {1, 0, 0};
		drawPoint(frontToothContourCurvePointPosition[frontToothContourPointIndex], color, frontToothContourCurvePointActor[frontToothContourPointIndex]);
		isFrontToothContiourCurvePointShow = 1;

		QString text = QString("(").
			append(QString::number(frontToothContourCurvePointPosition[frontToothContourPointIndex][0])).append(", ").
			append(QString::number(frontToothContourCurvePointPosition[frontToothContourPointIndex][1])).append(", ").
			append(QString::number(frontToothContourCurvePointPosition[frontToothContourPointIndex][2])).append(")");
		QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(currentOptionTreeWidgetItem, QStringList(text));
		currentOptionTreeWidgetItem->addChild(pointInfoItem);
		if(!currentOptionTreeWidgetItem->isExpanded())
		{
			currentOptionTreeWidgetItem->setExpanded(true);
		}

		++frontToothContourPointIndex;
		if(frontToothContourPointIndex == 2)
		{
			double frontToothContourCurveProjectArticlePlanePoint[2][3];
			double frontToothContourCurveProjectFitPlanePoint[2][3];
			for(int i = 0; i < 2; ++i)
			{
				vtkPlane::ProjectPoint(frontToothContourCurvePointPosition[i], 
					verticalPlaneCenter, verticalPlaneNormal,
					frontToothContourCurveProjectArticlePlanePoint[i]);
				vtkPlane::ProjectPoint(frontToothContourCurveProjectArticlePlanePoint[i], 
					pointPositionForArticlePlane, articlePlaneNormal,
					frontToothContourCurveProjectFitPlanePoint[i]);
			}
			distanceFrontToothContourCurvePoint = sqrt(vtkMath::Distance2BetweenPoints(frontToothContourCurveProjectFitPlanePoint[0], frontToothContourCurveProjectFitPlanePoint[1]));
			if (distanceFrontToothContourCurvePoint <= 1)
			{
				FrontToothContourCurveScore = 1;
			}else if (distanceFrontToothContourCurvePoint <= 2)
			{
				FrontToothContourCurveScore = 2;
			}else if (distanceFrontToothContourCurvePoint <= 3)
			{
				FrontToothContourCurveScore = 3;
			}else 
			{
				FrontToothContourCurveScore = 4;
			}
			
			ScoreSet[2][3] += FrontToothContourCurveScore;
			text = QString::number(FrontToothContourCurveScore);
			QTreeWidgetItem *distanceInfoItem = new QTreeWidgetItem(distanceItem, QStringList(text));
			distanceItem->addChild(distanceInfoItem);
			if(!distanceItem->isExpanded())
			{
				distanceItem->setExpanded(true);
			}

			frontToothContourPointIndex = 0;
		}
	}
}

void MainWindow::switchLowToothShow()
{
	if(isLowToothShow == 0)
	{
		isLowToothShow = 1;
		this->ren->AddActor(lowerToothActor);
	}
	else if(isLowToothShow == 1)
	{
		isLowToothShow = 0;
		this->ren->RemoveActor(lowerToothActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchTopToothShow()
{
	if(isTopToothShow == 0)
	{
		isTopToothShow = 1;
		this->ren->AddActor(topToothActor);
	}
	else if(isTopToothShow == 1)
	{
		isTopToothShow = 0;
		this->ren->RemoveActor(topToothActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchGapPointShow()
{
	if(isGapPointShow == 0)
	{
		isGapPointShow = 1;
		for(int i = 0; i < 10; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(gapPointStat[i][j])
				{
					this->ren->AddActor(gapPointActor[i][j]);
				}
			}
		}
	}
	else if(isGapPointShow == 1)
	{
		isGapPointShow = 0;
		for(int i = 0; i < 5; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(gapPointStat[i][j])
				{
					this->ren->RemoveActor(gapPointActor[i][j]);
				}
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchVerticalPlaneShow()
{
	if(isVerticalPlaneShow == 0)
	{
		isVerticalPlaneShow = 1;
		this->ren->AddActor(verticalPlaneActor);
		this->ren->AddActor(verticalWireFramePlaneActor);
	}
	else if(isVerticalPlaneShow == 1)
	{
		isVerticalPlaneShow = 0;
		this->ren->RemoveActor(verticalPlaneActor);
		this->ren->RemoveActor(verticalWireFramePlaneActor);
	}

	//QMessageBox::information(NULL, tr("Action"), QString::number(isVerticalPlaneShow));

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchTopToothPointForVerticalPlaneShow()
{
	if(isTopToothPointForVerticalPlaneShow == 0)
	{
		isTopToothPointForVerticalPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			this->ren->AddActor(topToothPointForVerticalPlaneActor[i]);
		}
	}
	else if(isTopToothPointForVerticalPlaneShow == 1)
	{
		isTopToothPointForVerticalPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			this->ren->RemoveActor(topToothPointForVerticalPlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchTopToothProjectPointForVerticalPlane()
{
	if(isTopToothProjectPointForVerticalPlane == 0)
	{
		isTopToothProjectPointForVerticalPlane = 1;

		for(int i = 0; i < 10; ++i)
		{
			this->ren->AddActor(topToothProjectPointForVerticalPlaneActor[i]);
		}
	}
	else if(isTopToothProjectPointForVerticalPlane == 1)
	{
		isTopToothProjectPointForVerticalPlane = 0;

		for(int i = 0; i < 10; ++i)
		{
			this->ren->RemoveActor(topToothProjectPointForVerticalPlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchInverseFitCoverShow()
{
	if(isInverseFitCoverPointShow == 0 && isInverseFitCoverProjectShow == 0)
	{
		isInverseFitCoverPointShow = 1;
		isInverseFitCoverProjectShow = 1;

		for(int i = 0; i < 16; ++i)
		{
			this->ren->AddActor(inverseFitCoverPointActor[i]);
			this->ren->AddActor(inverseFitCoverProjectPointActor[i]);
		}

		for(int i = 0; i < 8; ++i)
		{
			this->ren->AddActor(inverseFitCoverProjectLineActor[i]);
		}
	}
	else if(isInverseFitCoverPointShow == 1 && isInverseFitCoverProjectShow == 1)
	{
		isInverseFitCoverPointShow = 0;
		isInverseFitCoverProjectShow = 0;

		for(int i = 0; i < 16; ++i)
		{
			this->ren->RemoveActor(inverseFitCoverPointActor[i]);
			this->ren->RemoveActor(inverseFitCoverProjectPointActor[i]);
		}

		for(int i = 0; i < 8; ++i)
		{
			this->ren->RemoveActor(inverseFitCoverProjectLineActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchPointForArticlePlaneShow()
{
	if(isPointForArticlePlaneShow == 0)
	{
		isPointForArticlePlaneShow = 1;
		this->ren->AddActor(pointForArticleActor);
	}
	else if(isPointForArticlePlaneShow == 1)
	{
		isPointForArticlePlaneShow = 0;
		this->ren->RemoveActor(pointForArticleActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchArticlePlaneShow()
{
	if(isArticlePlaneShow == 0)
	{
		isArticlePlaneShow = 1;
		this->ren->AddActor(articlePlaneActor);
		this->ren->AddActor(articalPlaneWireframeActor);
	}
	else if(isArticlePlaneShow == 1)
	{
		isArticlePlaneShow = 0;
		this->ren->RemoveActor(articlePlaneActor);
		this->ren->RemoveActor(articalPlaneWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFrontToothOpenCloseFittingPointShow()
{
	if(isFrontToothOpenCloseFittingPointShow == 0)
	{
		isFrontToothOpenCloseFittingPointShow = 1;

		for(int i = 0; i < 20; ++i)
		{
			if(frontToothOpenCloseFittingPointStat[i])
			{
				this->ren->AddActor(frontToothOpenCloseFittingPointActor[i]);
			}
		}
	}
	else if(isFrontToothOpenCloseFittingPointShow == 1)
	{
		isFrontToothOpenCloseFittingPointShow = 0;

		for(int i = 0; i < 20; ++i)
		{
			if(frontToothOpenCloseFittingPointStat[i])
			{
				this->ren->RemoveActor(frontToothOpenCloseFittingPointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFrontToothOpenCloseFittingCurveShow()
{
	if(isFrontToothOpenCloseFittingCurveShow == 0)
	{
		isFrontToothOpenCloseFittingCurveShow = 1;

		for(int i = 0; i < 20; ++i)
		{
			if(frontToothOpenCloseFittingPointStat[i])
			{
				this->ren->AddActor(frontToothOpenCloseFittingProjectPointActor[i]);
			}
		}
		for(int i = 0; i < 40; ++i)
		{
			this->ren->AddActor(frontToothOpenCloseFittingCurveActor[i]);
		}
	}
	else if(isFrontToothOpenCloseFittingCurveShow == 1)
	{
		isFrontToothOpenCloseFittingCurveShow = 0;

		for(int i = 0; i < 20; ++i)
		{
			if(frontToothOpenCloseFittingPointStat[i])
			{
				this->ren->RemoveActor(frontToothOpenCloseFittingProjectPointActor[i]);
			}
		}
		for(int i = 0; i < 40; ++i)
		{
			this->ren->RemoveActor(frontToothOpenCloseFittingCurveActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFrontToothPlaneOShow()
{
	if(isFrontToothPlaneOShow == 0)
	{
		isFrontToothPlaneOShow = 1;

		this->ren->AddActor(frontToothProjectPointA0Actor);
		this->ren->AddActor(frontToothPlaneCenterActor);
		this->ren->AddActor(frontToothPlaneOActor);
		this->ren->AddActor(frontToothPlaneOWireframeActor);
	}
	else if(isFrontToothPlaneOShow == 1)
	{
		isFrontToothPlaneOShow = 0;

		this->ren->RemoveActor(frontToothProjectPointA0Actor);
		this->ren->RemoveActor(frontToothPlaneCenterActor);
		this->ren->RemoveActor(frontToothPlaneOActor);
		this->ren->RemoveActor(frontToothPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFrontToothContourCurveShaow()
{
	if(isFrontToothContourCurveShaow == 0)
	{
		isFrontToothContourCurveShaow = 1;

		this->ren->AddActor(frontToothContourCurveActor);
		this->ren->AddActor(frontToothTopContourCurveActor);
	}
	else if(isFrontToothContourCurveShaow == 1)
	{
		isFrontToothContourCurveShaow = 0;

		this->ren->RemoveActor(frontToothContourCurveActor);
		this->ren->RemoveActor(frontToothTopContourCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFrontToothToProjectPointPositionA0Show()
{
	if(idFrontToothToProjectPointPositionA0Show == 0)
	{
		idFrontToothToProjectPointPositionA0Show = 1;

		this->ren->AddActor(frontToothToProjectPointA0Actor);
	}
	else if(idFrontToothToProjectPointPositionA0Show == 1)
	{
		idFrontToothToProjectPointPositionA0Show = 0;

		this->ren->RemoveActor(frontToothToProjectPointA0Actor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchProjectArticlePlane()
{
	if(isProjectArticlePlane == 0)
	{
		isProjectArticlePlane = 1;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				this->ren->AddActor(topPointProjectArticlePlaneActor[i][j]);
				this->ren->AddActor(lowPointProjectArticlePlaneActor[i][j]);
			}

			this->ren->AddActor(topLineProjectArticlePlaneActor[i]);
			this->ren->AddActor(lowLineProjectArticlePlaneActor[i]);
		}
	}
	else if(isProjectArticlePlane == 1)
	{
		isProjectArticlePlane = 0;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				this->ren->RemoveActor(topPointProjectArticlePlaneActor[i][j]);
				this->ren->RemoveActor(lowPointProjectArticlePlaneActor[i][j]);
			}

			this->ren->RemoveActor(topLineProjectArticlePlaneActor[i]);
			this->ren->RemoveActor(lowLineProjectArticlePlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchLowPointToProjectArticlePlaneShow()
{
	if(isLowPointToProjectArticlePlaneShow == 0)
	{
		isLowPointToProjectArticlePlaneShow = 1;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(lowPointToProjectArticlePlaneStat[i][j])
					this->ren->AddActor(lowPointToProjectArticlePlaneActor[i][j]);
			}
		}
	}
	else if(isLowPointToProjectArticlePlaneShow == 1)
	{
		isLowPointToProjectArticlePlaneShow = 0;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(lowPointToProjectArticlePlaneStat[i][j])
					this->ren->RemoveActor(lowPointToProjectArticlePlaneActor[i][j]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchTopPointToProjectArticlePlaneShow()
{
	if(isTopPointToProjectArticlePlaneShow == 0)
	{
		isTopPointToProjectArticlePlaneShow = 1;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(topPointToProjectArticlePlaneStat[i][j])
					this->ren->AddActor(topPointToProjectArticlePlaneActor[i][j]);
			}
		}
	}
	else if(isTopPointToProjectArticlePlaneShow == 1)
	{
		isTopPointToProjectArticlePlaneShow = 0;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(topPointToProjectArticlePlaneStat[i][j])
					this->ren->RemoveActor(topPointToProjectArticlePlaneActor[i][j]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFrontToothContiourCurvePointShow()
{
	if(isFrontToothContiourCurvePointShow == 0)
	{
		isFrontToothContiourCurvePointShow = 1;

		for(int i = 0; i < 2; ++i)
		{
			this->ren->AddActor(frontToothContourCurvePointActor[i]);
		}
	}
	else if(isFrontToothContiourCurvePointShow == 1)
	{
		isFrontToothContiourCurvePointShow = 0;

		for(int i = 0; i < 2; ++i)
		{
			this->ren->RemoveActor(frontToothContourCurvePointActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchBackToothOpenCloseFittingPointShow()
{
	if(isBackToothOpenCloseFittingPointShow == 0)
	{
		isBackToothOpenCloseFittingPointShow = 1;

		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i])
			{
				this->ren->AddActor(backToothOpenCloseFittingPointActor[i]);
			}
		}
	}
	else if(isBackToothOpenCloseFittingPointShow == 1)
	{
		isBackToothOpenCloseFittingPointShow = 0;

		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i])
			{
				this->ren->RemoveActor(backToothOpenCloseFittingPointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchBackToothOpenCloseFittingCurveShow()
{
	if(isBackToothOpenCloseFittingCurveShow == 0)
	{
		isBackToothOpenCloseFittingCurveShow = 1;
		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i])
			{
				this->ren->AddActor(backToothOpenCloseFittingProjectPointActor[i]);
			}
		}
		for(int i = 0; i < 40; ++i)
			this->ren->AddActor(backToothOpenCloseFittingCurveActor[i]);
	}
	else if(isBackToothOpenCloseFittingCurveShow == 1)
	{
		isBackToothOpenCloseFittingCurveShow = 0;
		for(int i = 0; i < 20; ++i)
		{
			if(backToothOpenCloseFittingPointStat[i])
			{
				this->ren->RemoveActor(backToothOpenCloseFittingProjectPointActor[i]);
			}
		}
		for(int i = 0; i < 40; ++i)
			this->ren->RemoveActor(backToothOpenCloseFittingCurveActor[i]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchBackToothPointA0Show()
{
	if(isBackToothPointA0Show == 0)
	{
		isBackToothPointA0Show = 1;
		this->ren->AddActor(backToothPointA0Actor);
	}
	else if(isBackToothPointA0Show == 1)
	{
		isBackToothPointA0Show = 0;
		this->ren->RemoveActor(backToothPointA0Actor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchBackToothPlaneOShow()
{
	if(isBackToothPlaneOShow == 0)
	{
		isBackToothPlaneOShow = 1;

		this->ren->AddActor(backToothProjectPointA0Actor);
		this->ren->AddActor(backToothPlaneCenterActor);
		this->ren->AddActor(backToothPlaneOActor);
		this->ren->AddActor(backToothPlaneOWireframeActor);
	}
	else if(isBackToothPlaneOShow == 1)
	{
		isBackToothPlaneOShow = 0;

		this->ren->RemoveActor(backToothProjectPointA0Actor);
		this->ren->RemoveActor(backToothPlaneCenterActor);
		this->ren->RemoveActor(backToothPlaneOActor);
		this->ren->RemoveActor(backToothPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchBackToothContourCurveShaow()
{
	if(isBackToothContourCurveShaow == 0)
	{
		isBackToothContourCurveShaow = 1;

		this->ren->AddActor(backToothContourCurveActor);
		this->ren->AddActor(backToothTopContourCurveActor);
	}
	else if(isBackToothContourCurveShaow == 1)
	{
		isBackToothContourCurveShaow = 0;

		this->ren->RemoveActor(backToothContourCurveActor);
		this->ren->RemoveActor(backToothTopContourCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchBackToothContiourCurvePointShow()
{
	if(isBackToothContiourCurvePointShow == 0)
	{
		isBackToothContiourCurvePointShow = 1;

		for(int i = 0; i < 2; ++i)
			this->ren->AddActor(backToothContourCurvePointActor[i]);
	}
	else if(isBackToothContiourCurvePointShow == 1)
	{
		isBackToothContiourCurvePointShow = 0;

		for(int i = 0; i < 2; ++i)
			this->ren->RemoveActor(backToothContourCurvePointActor[i]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchPointInOverbiteParallelPlaneShow()
{
	if(isPointInOverbiteParallelPlaneShow == 0)
	{
		isPointInOverbiteParallelPlaneShow = 1;
		this->ren->AddActor(pointForOverbitParallelPlaneActor);
	}
	else if(isPointInOverbiteParallelPlaneShow == 1)
	{
		isPointInOverbiteParallelPlaneShow = 0;
		this->ren->RemoveActor(pointForOverbitParallelPlaneActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchOverbiteParrelPlaneShow()
{
	if(isOverbiteParrelPlaneShow == 0)
	{
		isOverbiteParrelPlaneShow = 1;

		this->ren->AddActor(overbiteParallelPlaneActor);
		this->ren->AddActor(overbiteParallelPlaneWireFrameActor);
	}
	else if(isOverbiteParrelPlaneShow == 1)
	{
		isOverbiteParrelPlaneShow = 0;

		this->ren->RemoveActor(overbiteParallelPlaneActor);
		this->ren->RemoveActor(overbiteParallelPlaneWireFrameActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchOverbiteIntersectCurveShow()
{
	if(isOverbiteIntersectCurveShow == 0)
	{
		isOverbiteIntersectCurveShow = 1;

		this->ren->AddActor(overbiteIntersectCurveActor);
		this->ren->AddActor(topOverbiteIntersectCurveActor);
	}
	else if(isOverbiteIntersectCurveShow == 1)
	{
		isOverbiteIntersectCurveShow = 0;

		this->ren->RemoveActor(overbiteIntersectCurveActor);
		this->ren->RemoveActor(topOverbiteIntersectCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchVerticalParallelPlaneInitPoint2Show()
{
	if(isVerticalParallelPlaneInitPoint2Show == 0)
	{
		isVerticalParallelPlaneInitPoint2Show = 1;
		this->ren->AddActor(verticalParallelInitPoint2Actor);
	}
	else if(isVerticalParallelPlaneInitPoint2Show == 1)
	{
		isVerticalParallelPlaneInitPoint2Show = 0;
		this->ren->RemoveActor(verticalParallelInitPoint2Actor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchVerticalParallelPlaneInitPointShow()
{
	if(isVerticalParallelPlaneInitPointShow == 0)
	{
		isVerticalParallelPlaneInitPointShow = 1;
		this->ren->AddActor(verticalParallelInitPointActor);
	}
	else if(isVerticalParallelPlaneInitPointShow == 1)
	{
		isVerticalParallelPlaneInitPointShow = 0;
		this->ren->RemoveActor(verticalParallelInitPointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}
void MainWindow::switchTwoPointToProjectFitPlaneShow()
{
	if(isTwoPointToProjectFitPlaneShow == 0)
	{
		isTwoPointToProjectFitPlaneShow = 1;

		for(int i = 0; i < 2; ++i)
			this->ren->AddActor(towPointToProjectFitPlaneActor[i]);
	}
	else if(isTwoPointToProjectFitPlaneShow == 1)
	{
		isTwoPointToProjectFitPlaneShow = 0;

		for(int i = 0; i < 2; ++i)
			this->ren->RemoveActor(towPointToProjectFitPlaneActor[i]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchVerticalParallelPlane2Show()
{
	if(isVerticalParallelPlane2Show == 0)
	{
		isVerticalParallelPlane2Show = 1;
		this->ren->AddActor(verticalparallelPlane2Actor);
		this->ren->AddActor(verticalParallelWireFramePlane2Actor);
	}
	else if(isVerticalParallelPlane2Show == 1)
	{
		isVerticalParallelPlane2Show = 0;
		this->ren->RemoveActor(verticalparallelPlane2Actor);
		this->ren->RemoveActor(verticalParallelWireFramePlane2Actor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchVerticalParallelPlaneShow()
{
	if(isVerticalParallelPlaneShow == 0)
	{
		isVerticalParallelPlaneShow = 1;
		this->ren->AddActor(verticalparallelPlaneActor);
		this->ren->AddActor(verticalParallelWireFramePlaneActor);
	}
	else if(isVerticalParallelPlaneShow == 1)
	{
		isVerticalParallelPlaneShow = 0;
		this->ren->RemoveActor(verticalparallelPlaneActor);
		this->ren->RemoveActor(verticalParallelWireFramePlaneActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchLowToothProjectPointForFitPlaneShow()
{
	if(isLowToothProjectPointForFitPlaneShow == 0)
	{
		isLowToothProjectPointForFitPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForFitPlaneStat[i])
				this->ren->AddActor(lowToothProjectPointForFitPlaneActor[i]);
		}
	}
	else if(isLowToothProjectPointForFitPlaneShow == 1)
	{
		isLowToothProjectPointForFitPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForFitPlaneStat[i])
				this->ren->RemoveActor(lowToothProjectPointForFitPlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchLowToothPointForFitPlaneShow()
{
	if(isLowToothPointForFitPlaneShow == 0)
	{
		isLowToothPointForFitPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForFitPlaneStat[i])
				this->ren->AddActor(lowToothPointForFitPlaneActor[i]);
		}
	}
	else if(isLowToothPointForFitPlaneShow == 1)
	{
		isLowToothPointForFitPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForFitPlaneStat[i])
				this->ren->RemoveActor(lowToothPointForFitPlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchLowToothProjectPointForVerticalPlaneShow()
{
	if(isLowToothProjectPointForVerticalPlaneShow == 0)
	{
		isLowToothProjectPointForVerticalPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			this->ren->AddActor(lowToothProjectPointForVerticalPlaneActor[i]);
		}
	}
	else if(isLowToothProjectPointForVerticalPlaneShow == 1)
	{
		isLowToothProjectPointForVerticalPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			this->ren->RemoveActor(lowToothProjectPointForVerticalPlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchLowToothPointForVerticalPlaneShow()
{
	if(isLowToothPointForVerticalPlaneShow == 0)
	{
		isLowToothPointForVerticalPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForVerticalPlaneStat[i])
			{
				this->ren->AddActor(lowToothPointForVerticalPlaneActor[i]);
			}
		}
	}
	else if(isLowToothPointForVerticalPlaneShow == 1)
	{
		isLowToothPointForVerticalPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(lowToothPointForVerticalPlaneStat[i])
			{
				this->ren->RemoveActor(lowToothPointForVerticalPlaneActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchTopToothProjectPointForFitPlaneShow()
{
	if(isTopToothProjectPointForFitPlaneShow == 0)
	{
		isTopToothProjectPointForFitPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(topToothPointForFitPlaneStat[i])
			{
				this->ren->AddActor(topToothProjectPointForFitPlaneActor[i]);
			}
		}
	}
	else if(isTopToothProjectPointForFitPlaneShow == 1)
	{
		isTopToothProjectPointForFitPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(topToothPointForFitPlaneStat[i])
			{
				this->ren->RemoveActor(topToothProjectPointForFitPlaneActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchTopToothPointForFitPlaneShow()
{
	if(isTopToothPointForFitPlaneShow == 0)
	{
		isTopToothPointForFitPlaneShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(topToothPointForFitPlaneStat[i])
				this->ren->AddActor(topToothPointForFitPlaneActor[i]);
		}
	}
	else if(isTopToothPointForFitPlaneShow == 1)
	{
		isTopToothPointForFitPlaneShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(topToothPointForFitPlaneStat[i])
				this->ren->RemoveActor(topToothPointForFitPlaneActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchPointInVerticalPlaneShow()
{
	if(isPointInVerticalPlaneShow == 0)
	{
		isPointInVerticalPlaneShow = 1;
		for(int i = 0; i < 2; ++i)
		{
			if(pointInVerticalPlaneStat[i])
			{
				this->ren->AddActor(pointInVerticalPlaneActor[i]);
			}
		}
	}
	else if(isPointInVerticalPlaneShow == 1)
	{
		isPointInVerticalPlaneShow = 0;
		for(int i = 0; i < 2; ++i)
		{
			if(pointInVerticalPlaneStat[i])
			{
				this->ren->RemoveActor(pointInVerticalPlaneActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchGapProjectPointShow()
{
	if(isGapProjectPointShow == 0)
	{
		isGapProjectPointShow = 1;
		for(int i = 0; i < 5; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(gapPointStat[i][j])
				{
					this->ren->AddActor(gapProjectPointActor[i][j]);
				}
			}
		}
	}
	else if(isGapProjectPointShow == 1)
	{
		isGapProjectPointShow = 0;
		for(int i = 0; i < 5; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				if(gapPointStat[i][j])
				{
					this->ren->RemoveActor(gapProjectPointActor[i][j]);
				}
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFitPlaneShow()
{
	if(isFitPlaneShow == 0)
	{
		isFitPlaneShow = 1;
		this->ren->AddActor(fitPlaneActor);
		this->ren->AddActor(fitPlaneFrameActor);
	}
	else if(isFitPlaneShow == 1)
	{
		isFitPlaneShow = 0;
		this->ren->RemoveActor(fitPlaneActor);
		this->ren->RemoveActor(fitPlaneFrameActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}


void MainWindow::switchAboTouchRelationProjectPointShow(int index)
{
	for(int i = 0; i < aboTouchRelationPointNum[index]; ++i)
	{
		if(isAboTouchRelationProjectPointShow[index][i] == 1)
		{
			isAboTouchRelationProjectPointShow[index][i] = 0;
			this->ren->RemoveActor(aboTouchRelationProjectPointAcotor[index][i]);
		}
		else if(isAboTouchRelationProjectPointShow[index][i] == 0)
		{
			isAboTouchRelationProjectPointShow[index][i] = 1;
			this->ren->AddActor(aboTouchRelationProjectPointAcotor[index][i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchboTouchRelationPointShow(int index)
{
	for(int i = 0; i < aboTouchRelationPointNum[index]; ++i)
	{
		if(isAboTouchRelationPointShow[index][i] == 1)
		{
			isAboTouchRelationPointShow[index][i] = 0;
			this->ren->RemoveActor(aboTouchRelationPointAcotor[index][i]);
		}
		else if(isAboTouchRelationPointShow[index][i] == 0)
		{
			isAboTouchRelationPointShow[index][i] = 1;
			this->ren->AddActor(aboTouchRelationPointAcotor[index][i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchisCoverIntersectCurveShow(int index)
{
	//for(int i = 0; i < 2; ++i)
	//{
	if(isCoverIntersectCurveShow[index][0] == 1 && isCoverIntersectCurveShow[index][1] == 1)
	{
		isCoverIntersectCurveShow[index][0] = isCoverIntersectCurveShow[index][1] = 0;
		this->ren->RemoveActor(aboCoverIntersectCurveActor[index]);
		this->ren->RemoveActor(aboCoverLowIntersectCurveActor[index]);
	}
	else if(isCoverIntersectCurveShow[index][0] == 0 && isCoverIntersectCurveShow[index][1] == 0)
	{
		isCoverIntersectCurveShow[index][0] = isCoverIntersectCurveShow[index][1] = 1;
		this->ren->AddActor(aboCoverIntersectCurveActor[index]);
		this->ren->AddActor(aboCoverLowIntersectCurveActor[index]);
	}
	//}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboCoverPlaneOShow(int index)
{
	if(isAboCoverPlaneOShow[index] == 1)
	{
		isAboCoverPlaneOShow[index] = 0;
		this->ren->RemoveActor(aboCoverPlaneOActor[index]);
		this->ren->RemoveActor(aboCoverPlaneOWireframeActor[index]);
	}
	else if(isAboCoverPlaneOShow[index] == 0)
	{
		isAboCoverPlaneOShow[index] = 1;
		this->ren->AddActor(aboCoverPlaneOActor[index]);
		this->ren->AddActor(aboCoverPlaneOWireframeActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboCoverPointBShow(int index)
{
	if(isAboCoverPointShow[index][0] == 1)
	{
		isAboCoverPointShow[index][0] = 0;
		this->ren->RemoveActor(aboCoverAcotor[index][0]);
	}
	else if(isAboCoverPointShow[index][0] == 0)
	{
		isAboCoverPointShow[index][0] = 1;
		this->ren->AddActor(aboCoverAcotor[index][0]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboCoverPointAShow(int index)
{
	if(isAboCoverPointShow[index][1] == 1)
	{
		isAboCoverPointShow[index][1] = 0;
		this->ren->RemoveActor(aboCoverAcotor[index][1]);
	}
	else if(isAboCoverPointShow[index][1] == 0)
	{
		isAboCoverPointShow[index][1] = 1;
		this->ren->AddActor(aboCoverAcotor[index][1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboToothCloseTouchIntersectCurveShow(int index)
{
	if(isAboToothCloseTouchIntersectCurveShow[index] == 1)
	{
		isAboToothCloseTouchIntersectCurveShow[index] = 0;
		this->ren->RemoveActor(aboToothCloseTouchTopIntersectCurveActor[index]);
		this->ren->RemoveActor(aboToothCloseTouchLowIntersectCurveActor[index]);
	}
	else if(isAboToothCloseTouchIntersectCurveShow[index] == 0)
	{
		isAboToothCloseTouchIntersectCurveShow[index] = 1;
		this->ren->AddActor(aboToothCloseTouchTopIntersectCurveActor[index]);
		this->ren->AddActor(aboToothCloseTouchLowIntersectCurveActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboToothCloseTouchPlaneOShow(int index)
{
	if(isAboToothCloseTouchPlaneOShow[index] == 1)
	{
		isAboToothCloseTouchPlaneOShow[index] = 0;
		this->ren->RemoveActor(aboToothCloseTouchPlaneOActor[index]);
		this->ren->RemoveActor(aboToothCloseTouchPlaneOWireframeActor[index]);
	}
	else if(isAboToothCloseTouchPlaneOShow[index] == 0)
	{
		isAboToothCloseTouchPlaneOShow[index] = 1;
		this->ren->AddActor(aboToothCloseTouchPlaneOActor[index]);
		this->ren->AddActor(aboToothCloseTouchPlaneOWireframeActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboToothCloseTouchPointBShow(int index)
{
	if(isAboToothCloseTouchPointShow[index][1] == 1)
	{
		isAboToothCloseTouchPointShow[index][1] = 0;
		this->ren->RemoveActor(aboToothCloseTouchPointAcotor[index][1]);
	}
	else if(isAboToothCloseTouchPointShow[index][1] == 0)
	{
		isAboToothCloseTouchPointShow[index][1] = 1;
		this->ren->AddActor(aboToothCloseTouchPointAcotor[index][1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboToothCloseTouchPointAShow(int index)
{
	if(isAboToothCloseTouchPointShow[index][0] == 1)
	{
		isAboToothCloseTouchPointShow[index][0] = 0;
		this->ren->RemoveActor(aboToothCloseTouchPointAcotor[index][0]);
	}
	else if(isAboToothCloseTouchPointShow[index][0] == 0)
	{
		isAboToothCloseTouchPointShow[index][0] = 1;
		this->ren->AddActor(aboToothCloseTouchPointAcotor[index][0]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboToothCloseRelationTopPointShow(int index)
{
	if(isAboToothCloseRelationPointShow[index][1] == 1)
	{
		isAboToothCloseRelationPointShow[index][1] = 0;
		this->ren->RemoveActor(aboToothCloseRelationPointAcotor[index][1]);
	}
	else if(isAboToothCloseRelationPointShow[index][1] == 0)
	{
		isAboToothCloseRelationPointShow[index][1] = 1;
		this->ren->AddActor(aboToothCloseRelationPointAcotor[index][1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboToothCloseRelationPlaneOShow(int index)
{
	if(isAboToothCloseRelationPlaneOShow[index] == 1)
	{
		isAboToothCloseRelationPlaneOShow[index] = 0;
		this->ren->RemoveActor(aboToothCloseRelationPlaneOActor[index]);
		this->ren->RemoveActor(aboToothCloseRelationPlaneOWireframeActor[index]);
	}
	else if(isAboToothCloseRelationPlaneOShow[index] == 0)
	{
		isAboToothCloseRelationPlaneOShow[index] = 1;
		this->ren->AddActor(aboToothCloseRelationPlaneOActor[index]);
		this->ren->AddActor(aboToothCloseRelationPlaneOWireframeActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();

}

void MainWindow::switchAboToothColoseRelationLowPointShow(int index)
{
	if(isAboToothCloseRelationPointShow[index][0] == 1)
	{
		isAboToothCloseRelationPointShow[index][0] = 0;
		this->ren->RemoveActor(aboToothCloseRelationPointAcotor[index][0]);
	}
	else if(isAboToothCloseRelationPointShow[index][0] == 0)
	{
		isAboToothCloseRelationPointShow[index][0] = 1;
		this->ren->AddActor(aboToothCloseRelationPointAcotor[index][0]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchChapProjectPointShow(int index)
{
	if(aboChapPointPickNum[index] == 2)
	{
		for(int i = 0; i < aboChapPointPickNum[index]; ++i)
		{
			if(isChapProjectPointShow[index][i] == 1)
			{
				isChapProjectPointShow[index][i] = 0;
				this->ren->RemoveActor(aboChapProjectPointAcotr[index][i]);
			}
			else if(isChapProjectPointShow[index][i] == 0)
			{
				isChapProjectPointShow[index][i] = 1;
				this->ren->AddActor(aboChapProjectPointAcotr[index][i]);
			}
		}
	}
}

void MainWindow::switchChapPointShow(int index)
{
	for(int i = 0; i < aboChapPointPickNum[index]; ++i)
	{
		if(isChapPointShow[index][i] == 1)
		{
			isChapPointShow[index][i] = 0;
			this->ren->RemoveActor(aboChapPointAcotor[index][i]);
		}
		else if(isChapPointShow[index][i] == 0)
		{
			isChapPointShow[index][i] = 1;
			this->ren->AddActor(aboChapPointAcotor[index][i]);
		}
	}
}

void MainWindow::switchChapFitPlaneShow()
{
	if(isChapFitPlaneShow == 0)
	{
		isChapFitPlaneShow = 1;
		this->ren->AddActor(aboChapFitPlaneActor);
		this->ren->AddActor(aboChapFitPlaneWireframeActor);
	}
	else if(isChapFitPlaneShow == 1)
	{
		isChapFitPlaneShow = 0;
		this->ren->RemoveActor(aboChapFitPlaneActor);
		this->ren->RemoveActor(aboChapFitPlaneWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboEdgeVertebralHeighProjectPointShow(int index)
{
	for(int i = 0; i < aboEdgeVertebralHeighPointPickNum[index]; ++i)
	{
		if(isAboEdgeVertebralHeighProjectPointShow[index][i] == 1)
		{
			this->ren->RemoveActor(aboEdgeVertebralHeighPointProjectToArticlePlaneActor[index][i]);
			isAboEdgeVertebralHeighProjectPointShow[index][i] = 0;
		}
		else if(isAboEdgeVertebralHeighProjectPointShow[index][i] == 0)
		{
			this->ren->AddActor(aboEdgeVertebralHeighPointProjectToArticlePlaneActor[index][i]);
			isAboEdgeVertebralHeighProjectPointShow[index][i] = 1;
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboEdgeVertebralHeighPointShow(int index)
{
	for(int i = 0; i < aboEdgeVertebralHeighPointPickNum[index]; ++i)
	{
		if(isAboEdgeVertebralHeighPointShow[index][i] == 1)
		{
			this->ren->RemoveActor(aboEdgeVertebralHeighPointAcotor[index][i]);
			isAboEdgeVertebralHeighPointShow[index][i] = 0;
		}
		else if(isAboEdgeVertebralHeighPointShow[index][i] == 0)
		{
			this->ren->AddActor(aboEdgeVertebralHeighPointAcotor[index][i]);
			isAboEdgeVertebralHeighPointShow[index][i] = 1;
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboArrangePlaneOShow(int index)
{
	if(isAboArrangePlaneOShow[index] == 1)
	{
		this->ren->RemoveActor(aboArrangePlaneOActor[index]);
		this->ren->RemoveActor(aboArrangePlaneOWireframeActor[index]);
		isAboArrangePlaneOShow[index] = 0;
	}
	else if(isAboArrangePlaneOShow[index] == 0)
	{
		this->ren->AddActor(aboArrangePlaneOActor[index]);
		this->ren->AddActor(aboArrangePlaneOWireframeActor[index]);
		isAboArrangePlaneOShow[index] = 1;
	}
}

void MainWindow::switchAboArrangeProjectPointShow(int index)
{
	for(int i = 0; i < aboArrangePointPickNum[index]; ++i)
	{
		if(isAboArrangeProjectPointShow[index][i] == 1)
		{
			this->ren->RemoveActor(aboArrangePointProjectToFitPlane[index][i]);
			isAboArrangeProjectPointShow[index][i] = 0;
		}
		else if(isAboArrangeProjectPointShow[index][i] == 0)
		{
			this->ren->AddActor(aboArrangePointProjectToFitPlane[index][i]);
			isAboArrangeProjectPointShow[index][i] = 1;
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchAboArrangePointShow(int index)
{
	for(int i = 0; i < aboArrangePointPickNum[index]; ++i)
	{
		if(isAboArrangePointShow[index][i] == 1)
		{
			this->ren->RemoveActor(aboPointAcotor[index][i]);
			isAboArrangePointShow[index][i] = 0;
		}
		else if(isAboArrangePointShow[index][i] == 0)
		{
			this->ren->AddActor(aboPointAcotor[index][i]);
			isAboArrangePointShow[index][i] = 1;
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}


void MainWindow::switchDiToothBowScrowdShow(int index)
{
	if(isDiToothBowScrowdShow[index] == 0)
	{
		if(diToothBowScrowdPointIndex[index] == 1)
		{
			this->ren->AddActor(diToothBowScrowdPointActor[2 * index]);
			isDiToothBowScrowdShow[index] = 1;
		}
		else if(diToothBowScrowdPointIndex[index] == 2)
		{
			this->ren->AddActor(diToothBowScrowdPointActor[2 * index]);
			this->ren->AddActor(diToothBowScrowdPointActor[2 * index + 1]);
			isDiToothBowScrowdShow[index] = 1;
		}
	}
	else if(isDiToothBowScrowdShow[index] == 1)
	{
		if(diToothBowScrowdPointIndex[index] == 1)
		{
			this->ren->AddActor(diToothBowScrowdPointActor[2 * index]);
			isDiToothBowScrowdShow[index] = 0;
		}
		else if(diToothBowScrowdPointIndex[index] == 2)
		{
			this->ren->RemoveActor(diToothBowScrowdPointActor[2 * index]);
			this->ren->RemoveActor(diToothBowScrowdPointActor[2 * index + 1]);
			isDiToothBowScrowdShow[index] = 0;
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiGrindToothScrowdToothShow(int index)
{
	if(isDiGrindToothScrowdToothShow[index] == 0)
	{
		if(diGrindToothScrowdPointIndex[index] == 1)
		{
			this->ren->AddActor(diGrindToothScrowdPointActor[2 * index]);
			isDiGrindToothScrowdToothShow[index] = 1;
		}
		else if(diGrindToothScrowdPointIndex[index] == 2)
		{
			this->ren->AddActor(diGrindToothScrowdPointActor[2 * index]);
			this->ren->AddActor(diGrindToothScrowdPointActor[2 * index + 1]);
			isDiGrindToothScrowdToothShow[index] = 1;
		}
	}
	else if(isDiGrindToothScrowdToothShow[index] == 1)
	{
		if(diGrindToothScrowdPointIndex[index] == 1)
		{
			this->ren->AddActor(diGrindToothScrowdPointActor[2 * index]);
			isDiGrindToothScrowdToothShow[index] = 0;
		}
		else if(diGrindToothScrowdPointIndex[index] == 2)
		{
			this->ren->RemoveActor(diGrindToothScrowdPointActor[2 * index]);
			this->ren->RemoveActor(diGrindToothScrowdPointActor[2 * index + 1]);
			isDiGrindToothScrowdToothShow[index] = 0;
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiMiddleCutToothGapShow()
{
	if(isDiMiddleCutToothGapShow == 0)
	{
		isDiMiddleCutToothGapShow = 1;

		if(diMiddleCutToothGapIndex == 1)
		{
			this->ren->AddActor(diMiddleCutToothGapPointActor[0]);
		}
		else if(diMiddleCutToothGapIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				this->ren->AddActor(diMiddleCutToothGapPointActor[i]);
			}
		}
	}
	else if(isDiMiddleCutToothGapShow == 1)
	{
		isDiMiddleCutToothGapShow = 0;

		if(diMiddleCutToothGapIndex == 1)
		{
			this->ren->AddActor(diMiddleCutToothGapPointActor[0]);
		}
		else if(diMiddleCutToothGapIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(diMiddleCutToothGapPointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiMiddleUnfitPointShow()
{
	if(diMiddleUnfitPointShow == 0)
	{
		diMiddleUnfitPointShow = 1;

		if(diMiddleUnfitPointIndex == 1)
		{
			this->ren->AddActor(diMiddleUnfitPointActor[0]);
		}
		else if(diMiddleUnfitPointIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				this->ren->AddActor(diMiddleUnfitPointActor[i]);
				this->ren->AddActor(diMiddleUnfitPlaneActor[i]);
				this->ren->AddActor(diMiddleUnfitPlaneWireframeActor[i]);
			}
		}
	}
	else if(diMiddleUnfitPointShow == 1)
	{
		diMiddleUnfitPointShow = 0;

		if(diMiddleUnfitPointIndex == 1)
		{
			this->ren->AddActor(diMiddleUnfitPointActor[0]);
		}
		else if(diMiddleUnfitPointIndex == 2)
		{
			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(diMiddleUnfitPointActor[i]);
				this->ren->RemoveActor(diMiddleUnfitPlaneActor[i]);
				this->ren->RemoveActor(diMiddleUnfitPlaneWireframeActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiBackToothLockCloseProjectPointShow()
{
	if(isDiBackToothLockCloseProjectPointShow == 0)
	{
		isDiBackToothLockCloseProjectPointShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(diBackToothLockClosePointState[i])
			{
				this->ren->AddActor(diBackToothLockCloseProjectPointActor[i]);
			}
		}
	}
	else if(isDiBackToothLockCloseProjectPointShow == 1)
	{
		isDiBackToothLockCloseProjectPointShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(diBackToothLockClosePointState[i])
			{
				this->ren->RemoveActor(diBackToothLockCloseProjectPointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiBackToothLockClosePointShow()
{
	if(isDiBackToothLockClosePointShow == 0)
	{
		isDiBackToothLockClosePointShow = 1;

		for(int i = 0; i < 10; ++i)
		{
			if(diBackToothLockClosePointState[i])
			{
				this->ren->AddActor(diBackToothLockClosePointActor[i]);
			}
		}
	}
	else if(isDiBackToothLockClosePointShow == 1)
	{
		isDiBackToothLockClosePointShow = 0;

		for(int i = 0; i < 10; ++i)
		{
			if(diBackToothLockClosePointState[i])
			{
				this->ren->RemoveActor(diBackToothLockClosePointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiBackToothOppositeCloseProjectPointShow()
{
	if(isDiBackToothOppositeCloseProjectPointShow == 0)
	{
		isDiBackToothOppositeCloseProjectPointShow = 1;

		for(int i = 0; i < 16; ++i)
		{
			if(diBackToothOppositeClosePointState[i])
			{
				this->ren->AddActor(diBackToothOppositeCloseProjectPointActor[i]);
			}
		}
	}
	else if(isDiBackToothOppositeCloseProjectPointShow == 1)
	{
		isDiBackToothOppositeCloseProjectPointShow = 0;

		for(int i = 0; i < 16; ++i)
		{
			if(diBackToothOppositeClosePointState[i])
			{
				this->ren->RemoveActor(diBackToothOppositeCloseProjectPointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCloseRelationPlaneOShow()
{
	if(isDiCloseRelationPlaneOShow == 0)
	{
		isDiCloseRelationPlaneOShow = 1;

		this->ren->AddActor(diCloseRelationProjectPointBActor);
		this->ren->AddActor(diCloseRelationPlanePointAActor);
		this->ren->AddActor(diCloseRelationPlaneOActor);
		this->ren->AddActor(diCloseRelationPlaneOWireframeActor);
	}
	else if(isDiCloseRelationPlaneOShow == 1)
	{
		isDiCloseRelationPlaneOShow = 0;

		this->ren->RemoveActor(diCloseRelationProjectPointBActor);
		this->ren->RemoveActor(diCloseRelationPlanePointAActor);
		this->ren->RemoveActor(diCloseRelationPlaneOActor);
		this->ren->RemoveActor(diCloseRelationPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCloseRelationPointBShow()
{
	if(isDiCloseRelationPointBShow == 0)
	{
		isDiCloseRelationPointBShow = 1;

		this->ren->AddActor(diCloseRelationPointBActor);
	}
	else if(isDiCloseRelationPointBShow == 1)
	{
		isDiCloseRelationPointBShow = 0;

		this->ren->RemoveActor(diCloseRelationPointBActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCloseRelationPointAShow()
{
	if(isDiCloseRelationPointAShow == 0)
	{
		isDiCloseRelationPointAShow = 1;

		this->ren->AddActor(diCloseRelationPointAActor);
	}
	else if(isDiCloseRelationPointAShow == 1)
	{
		isDiCloseRelationPointAShow = 0;

		this->ren->RemoveActor(diCloseRelationPointAActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiBackToothOppositeClosePointShow()
{
	if(isDiBackToothOppositeClosePointShow == 0)
	{
		isDiBackToothOppositeClosePointShow = 1;

		for(int i = 0; i < 16; ++i)
		{
			if(diBackToothOppositeClosePointState[i])
			{
				this->ren->AddActor(diBackToothOppositeClosePointActor[i]);
			}
		}
	}
	else if(isDiBackToothOppositeClosePointShow == 1)
	{
		isDiBackToothOppositeClosePointShow = 0;

		for(int i = 0; i < 16; ++i)
		{
			if(diBackToothOppositeClosePointState[i])
			{
				this->ren->RemoveActor(diBackToothOppositeClosePointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiFrontOpenCloseIntersectCurvePointShow(int index)
{
	if(isDiFrontOpenCloseIntersectCurvePointShow[index] == 0)
	{
		isDiFrontOpenCloseIntersectCurvePointShow[index] = 1;

		this->ren->AddActor(diFrontOpenCloseIntersectCurvePointActor[index][0]);
		this->ren->AddActor(diFrontOpenCloseIntersectCurvePointActor[index][1]);
	}
	else if(isDiFrontOpenCloseIntersectCurvePointShow[index] == 1)
	{
		isDiFrontOpenCloseIntersectCurvePointShow[index] = 0;

		this->ren->RemoveActor(diFrontOpenCloseIntersectCurvePointActor[index][0]);
		this->ren->RemoveActor(diFrontOpenCloseIntersectCurvePointActor[index][1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiFrontOpenCloseIntersectCurveShow(int index)
{
	if(isDiFrontOpenCloseIntersectCurveShow[index] == 0)
	{
		isDiFrontOpenCloseIntersectCurveShow[index] = 1;

		this->ren->AddActor(diFrontOpenCloseIntersectCurveActor[index]);
		this->ren->AddActor(diTopFrontOpenCloseIntersectCurveActor[index]);
	}
	else if(isDiFrontOpenCloseIntersectCurveShow[index] == 1)
	{
		isDiFrontOpenCloseIntersectCurveShow[index] = 0;

		this->ren->RemoveActor(diFrontOpenCloseIntersectCurveActor[index]);
		this->ren->RemoveActor(diTopFrontOpenCloseIntersectCurveActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiFrontOpenClosePlaneOShow(int index)
{
	if(isDiFrontOpenClosePlaneOShow[index] == 0)
	{
		isDiFrontOpenClosePlaneOShow[index] = 1;

		this->ren->AddActor(diFrontOpenCloseProjectPointActor[index]);
		this->ren->AddActor(diFrontOpenCloseCenterPointActor[index]);
		this->ren->AddActor(diFrontOpenClosePlaneOActor[index]);
		this->ren->AddActor(diFrontOpenClosePlaneOWireframeActor[index]);
	}
	else if(isDiFrontOpenClosePlaneOShow[index] == 1)
	{
		isDiFrontOpenClosePlaneOShow[index] = 0;

		this->ren->RemoveActor(diFrontOpenCloseProjectPointActor[index]);
		this->ren->RemoveActor(diFrontOpenCloseCenterPointActor[index]);
		this->ren->RemoveActor(diFrontOpenClosePlaneOActor[index]);
		this->ren->RemoveActor(diFrontOpenClosePlaneOWireframeActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiFrontOpenClosePointShow(int index)
{
	if(diFrontOpenClosePointStat[index] && isDiFrontOpenClosePointShow[index] == 0)
	{
		isDiFrontOpenClosePointShow[index] = 1;

		this->ren->AddActor(diFrontOpenClosePointActor[index]);
	}
	else if(diFrontOpenClosePointStat[index] && isDiFrontOpenClosePointShow[index] == 1)
	{
		isDiFrontOpenClosePointShow[index] = 0;

		this->ren->RemoveActor(diFrontOpenClosePointActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiSideOpenCloseIntersectCurvePointShow(int index)
{
	if(isDiSideOpenCloseIntersectCurvePointShow[index] == 0)
	{
		isDiSideOpenCloseIntersectCurvePointShow[index] = 1;

		this->ren->AddActor(diSideOpenCloseIntersectCurvePointActor[index][0]);
		this->ren->AddActor(diSideOpenCloseIntersectCurvePointActor[index][1]);
	}
	else if(isDiSideOpenCloseIntersectCurvePointShow[index] == 1)
	{
		isDiSideOpenCloseIntersectCurvePointShow[index] = 0;

		this->ren->RemoveActor(diSideOpenCloseIntersectCurvePointActor[index][0]);
		this->ren->RemoveActor(diSideOpenCloseIntersectCurvePointActor[index][1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiSideOpenCloseIntersectCurveShow(int index)
{
	if(isDiSideOpenCloseIntersectCurveShow[index] == 0)
	{
		isDiSideOpenCloseIntersectCurveShow[index] = 1;

		this->ren->AddActor(diSideOpenCloseIntersectCurveActor[index]);
		this->ren->AddActor(diTopSideOpenCloseIntersectCurveActor[index]);
	}
	else if(isDiSideOpenCloseIntersectCurveShow[index] == 1)
	{
		isDiSideOpenCloseIntersectCurveShow[index] = 0;

		this->ren->RemoveActor(diSideOpenCloseIntersectCurveActor[index]);
		this->ren->RemoveActor(diTopSideOpenCloseIntersectCurveActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiSideOpenClosePlaneOShow(int index)
{
	if(isDiSideOpenClosePlaneOShow[index] == 0)
	{
		isDiSideOpenClosePlaneOShow[index] = 1;

		this->ren->AddActor(diSideOpenCloseProjectPointActor[index]);
		this->ren->AddActor(diSideOpenCloseCenterPointActor[index]);
		this->ren->AddActor(diSideOpenClosePlaneOActor[index]);
		this->ren->AddActor(diSideOpenClosePlaneOWireframeActor[index]);
	}
	else if(isDiSideOpenClosePlaneOShow[index] == 1)
	{
		isDiSideOpenClosePlaneOShow[index] = 0;

		this->ren->RemoveActor(diSideOpenCloseProjectPointActor[index]);
		this->ren->RemoveActor(diSideOpenCloseCenterPointActor[index]);
		this->ren->RemoveActor(diSideOpenClosePlaneOActor[index]);
		this->ren->RemoveActor(diSideOpenClosePlaneOWireframeActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiSideOpenClosePointShow(int index)
{
	if(isDiSideOpenClosePointShow[index] == 0)
	{
		isDiSideOpenClosePointShow[index] = 1;

		this->ren->AddActor(diSideOpenClosePointActor[index]);
	}
	else if(isDiSideOpenClosePointShow[index] == 1)
	{
		isDiSideOpenClosePointShow[index] = 0;

		this->ren->RemoveActor(diSideOpenClosePointActor[index]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiOverbiteIntersectCurvePointShow()
{
	if(isDiOverbiteIntersectCurvePointShow == 0)
	{
		isDiOverbiteIntersectCurvePointShow = 1;

		this->ren->AddActor(diOverbiteIntersectCurvePointActor[0]);
		this->ren->AddActor(diOverbiteIntersectCurvePointActor[1]);
	}
	else if(isDiOverbiteIntersectCurvePointShow == 1)
	{
		isDiOverbiteIntersectCurvePointShow = 0;

		this->ren->RemoveActor(diOverbiteIntersectCurvePointActor[0]);
		this->ren->RemoveActor(diOverbiteIntersectCurvePointActor[1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiOverbiteIntersectCurveShow()
{
	if(isDiOverbiteIntersectCurveShow == 0)
	{
		isDiOverbiteIntersectCurveShow = 1;

		this->ren->AddActor(diOverbiteIntersectCurveActor);
		this->ren->AddActor(diTopOverbiteIntersectCurveActor);
	}
	else if(isDiOverbiteIntersectCurveShow == 1)
	{
		isDiOverbiteIntersectCurveShow = 0;

		this->ren->RemoveActor(diOverbiteIntersectCurveActor);
		this->ren->RemoveActor(diTopOverbiteIntersectCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiOverbitePlaneOShow()
{
	if(isDiOverbitePlaneOShow == 0)
	{
		isDiOverbitePlaneOShow = 1;

		this->ren->AddActor(diOverbiteProjectPointActor);
		this->ren->AddActor(diOverbiteCenterPointActor);
		this->ren->AddActor(diOverbitePlaneOActor);
		this->ren->AddActor(diOverbitePlaneOWireframeActor);
	}
	else if(isDiOverbitePlaneOShow == 1)
	{
		isDiOverbitePlaneOShow = 0;

		this->ren->RemoveActor(diOverbiteProjectPointActor);
		this->ren->RemoveActor(diOverbiteCenterPointActor);
		this->ren->RemoveActor(diOverbitePlaneOActor);
		this->ren->RemoveActor(diOverbitePlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiOverbitePointShow()
{
	if(isDiOverbitePointShow == 0)
	{
		isDiOverbitePointShow = 1;

		this->ren->AddActor(diOverbitePointActor);
	}
	else if(isDiOverbitePointShow == 1)
	{
		isDiOverbitePointShow = 0;

		this->ren->RemoveActor(diOverbitePointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}
/*
void MainWindow::switchDiCoverIntersectLineShow()
{
	if(isDiCoverIntersectLineShow == 0)
	{
		isDiCoverIntersectLineShow = 1;

		this->ren->AddActor(diCoverIntersectLineActor);
		this->ren->AddActor(diCoverIntersectPointActor);
	}
	else if(isDiCoverIntersectLineShow == 1)
	{
		isDiCoverIntersectLineShow = 0;

		this->ren->RemoveActor(diCoverIntersectLineActor);
		this->ren->RemoveActor(diCoverIntersectPointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiPlanePShow()
{
	if(isDiPlanePShow == 0)
	{
		isDiPlanePShow = 1;

		this->ren->AddActor(diPlanePActor);
		this->ren->AddActor(diPlanePWireframeActor);
	}
	else if(isDiPlanePShow == 1)
	{
		isDiPlanePShow = 0;

		this->ren->RemoveActor(diPlanePActor);
		this->ren->RemoveActor(diPlanePWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiPlaneOShow()
{
	if(isDiPlaneOShow == 0)
	{
		isDiPlaneOShow = 1;

		this->ren->AddActor(diProjectPointA0Actor);
		this->ren->AddActor(diPointAActor);
		this->ren->AddActor(diPlaneOActor);
		this->ren->AddActor(diPlaneOWireframeActor);
	}
	else if(isDiPlaneOShow == 1)
	{
		isDiPlaneOShow = 0;

		this->ren->RemoveActor(diProjectPointA0Actor);
		this->ren->RemoveActor(diPointAActor);
		this->ren->RemoveActor(diPlaneOActor);
		this->ren->RemoveActor(diPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiPointA0Show()
{
	if(isDiPointA0Show == 0)
	{
		isDiPointA0Show = 1;
		this->ren->AddActor(diPointA0Actor);
	}
	else if(isDiPointA0Show == 1)
	{
		isDiPointA0Show = 0;
		this->ren->RemoveActor(diPointA0Actor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}
*/
void MainWindow::switchSplinePointShow()
{
	if(isSplinePointShow == 0)
	{
		isSplinePointShow = 1;

		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])
			{
				this->ren->AddActor(splinePointActor[i]);
			}
		}
	}
	else if(isSplinePointShow == 1)
	{
		isSplinePointShow = 0;

		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])
			{
				this->ren->RemoveActor(splinePointActor[i]);
			}
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFittingCurveShow()
{
	if(isFittingCurveShow == 0)
	{
		isFittingCurveShow = 1;
		isSplineProjectPointShow = 1;

		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])
			{
				this->ren->AddActor(splineProjectPointActor[i]);
			}
		}

		for(int i = 0; i < 40; ++i)
		{
			this->ren->AddActor(fittingCurveActor[i]);
		}
	}
	else if(isFittingCurveShow == 1)
	{
		isFittingCurveShow = 0;
		isSplineProjectPointShow = 0;

		for(int i = 0; i < 20; ++i)
		{
			if(splinePointState[i])
			{
				this->ren->RemoveActor(splineProjectPointActor[i]);
			}
		}

		for(int i = 0; i < 40; ++i)
		{
			this->ren->RemoveActor(fittingCurveActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchJCoverIntersectCurvePointShow()
{
	if(isJCoverIntersectCurvePointShow == 0)
	{
		isJCoverIntersectCurvePointShow = 1;

		this->ren->AddActor(JCoverIntersectCurvePointActor[0]);
		this->ren->AddActor(JCoverIntersectCurvePointActor[1]);
	}
	else if(isJCoverIntersectCurvePointShow == 1)
	{
		isJCoverIntersectCurvePointShow = 0;

		this->ren->RemoveActor(JCoverIntersectCurvePointActor[0]);
		this->ren->RemoveActor(JCoverIntersectCurvePointActor[1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchJCoverIntersectCurveShow()
{
	if(isParJCoverIntersectCurveShow == 0)
	{
		isParJCoverIntersectCurveShow = 1;

		this->ren->AddActor(JCoverIntersectCurveActor);
		this->ren->AddActor(TopJCoverIntersectCurveActor);
	}
	else if(isParJCoverIntersectCurveShow == 1)
	{
		isParJCoverIntersectCurveShow = 0;

		this->ren->RemoveActor(JCoverIntersectCurveActor);
		this->ren->RemoveActor(TopJCoverIntersectCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchJCoverPlaneOShow()
{
	if(isJCoverPlaneOShow == 0)
	{
		isJCoverPlaneOShow = 1;

		this->ren->AddActor(JCoverProjectPointActor);
		this->ren->AddActor(JCoverCenterPointActor);
		this->ren->AddActor(JCoverPlaneOActor);
		this->ren->AddActor(JCoverPlaneOWireframeActor);
	}
	else if(isJCoverPlaneOShow == 1)
	{
		isJCoverPlaneOShow = 0;

		this->ren->RemoveActor(JCoverProjectPointActor);
		this->ren->RemoveActor(JCoverCenterPointActor);
		this->ren->RemoveActor(JCoverPlaneOActor);
		this->ren->RemoveActor(JCoverPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchJCoverPointShow()
{
	if(isJCoverPointShow == 0)
	{
		isJCoverPointShow = 1;

		this->ren->AddActor(JCoverPointActor);
	}
	else if(isJCoverPointShow == 1)
	{
		isJCoverPointShow = 0;

		this->ren->RemoveActor(JCoverPointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchCoverIntersectCurvePointShow()
{
	if(isCoverIntersectCurvePointShow == 0)
	{
		isCoverIntersectCurvePointShow = 1;

		this->ren->AddActor(CoverIntersectCurvePointActor[0]);
		this->ren->AddActor(CoverIntersectCurvePointActor[1]);
	}
	else if(isCoverIntersectCurvePointShow == 1)
	{
		isCoverIntersectCurvePointShow = 0;

		this->ren->RemoveActor(CoverIntersectCurvePointActor[0]);
		this->ren->RemoveActor(CoverIntersectCurvePointActor[1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchCoverIntersectCurveShow()
{
	if(isParCoverIntersectCurveShow == 0)
	{
		isParCoverIntersectCurveShow = 1;

		this->ren->AddActor(CoverIntersectCurveActor);
		this->ren->AddActor(TopCoverIntersectCurveActor);
	}
	else if(isParCoverIntersectCurveShow == 1)
	{
		isParCoverIntersectCurveShow = 0;

		this->ren->RemoveActor(CoverIntersectCurveActor);
		this->ren->RemoveActor(TopCoverIntersectCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchCoverPlaneOShow()
{
	if(isCoverPlaneOShow == 0)
	{
		isCoverPlaneOShow = 1;

		this->ren->AddActor(CoverProjectPointActor);
		this->ren->AddActor(CoverCenterPointActor);
		this->ren->AddActor(CoverPlaneOActor);
		this->ren->AddActor(CoverPlaneOWireframeActor);
	}
	else if(isCoverPlaneOShow == 1)
	{
		isCoverPlaneOShow = 0;

		this->ren->RemoveActor(CoverProjectPointActor);
		this->ren->RemoveActor(CoverCenterPointActor);
		this->ren->RemoveActor(CoverPlaneOActor);
		this->ren->RemoveActor(CoverPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchCoverPointShow()
{
	if(isCoverPointShow == 0)
	{
		isCoverPointShow = 1;

		this->ren->AddActor(CoverPointActor);
	}
	else if(isCoverPointShow == 1)
	{
		isCoverPointShow = 0;

		this->ren->RemoveActor(CoverPointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCoverIntersectCurvePointShow()
{
	if(isDiCoverIntersectCurvePointShow == 0)
	{
		isDiCoverIntersectCurvePointShow = 1;

		this->ren->AddActor(diCoverIntersectCurvePointActor[0]);
		this->ren->AddActor(diCoverIntersectCurvePointActor[1]);
	}
	else if(isDiCoverIntersectCurvePointShow == 1)
	{
		isDiCoverIntersectCurvePointShow = 0;

		this->ren->RemoveActor(diCoverIntersectCurvePointActor[0]);
		this->ren->RemoveActor(diCoverIntersectCurvePointActor[1]);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCoverIntersectCurveShow()
{
	if(isDiCoverIntersectCurveShow == 0)
	{
		isDiCoverIntersectCurveShow = 1;

		this->ren->AddActor(diCoverIntersectCurveActor);
		this->ren->AddActor(diTopCoverIntersectCurveActor);
	}
	else if(isDiCoverIntersectCurveShow == 1)
	{
		isDiCoverIntersectCurveShow = 0;

		this->ren->RemoveActor(diCoverIntersectCurveActor);
		this->ren->RemoveActor(diTopCoverIntersectCurveActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCoverPlaneOShow()
{
	if(isDiCoverPlaneOShow == 0)
	{
		isDiCoverPlaneOShow = 1;

		this->ren->AddActor(diCoverProjectPointActor);
		this->ren->AddActor(diCoverCenterPointActor);
		this->ren->AddActor(diCoverPlaneOActor);
		this->ren->AddActor(diCoverPlaneOWireframeActor);
	}
	else if(isDiCoverPlaneOShow == 1)
	{
		isDiCoverPlaneOShow = 0;

		this->ren->RemoveActor(diCoverProjectPointActor);
		this->ren->RemoveActor(diCoverCenterPointActor);
		this->ren->RemoveActor(diCoverPlaneOActor);
		this->ren->RemoveActor(diCoverPlaneOWireframeActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchDiCoverPointShow()
{
	if(isDiCoverPointShow == 0)
	{
		isDiCoverPointShow = 1;

		this->ren->AddActor(diCoverPointActor);
	}
	else if(isDiCoverPointShow == 1)
	{
		isDiCoverPointShow = 0;

		this->ren->RemoveActor(diCoverPointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchCurrentSelectPointShow()
{
	if(isCurrentSelectPointShow == 0)
	{
		isCurrentSelectPointShow = 1;
		this->ren->AddActor(currentSelectPointActor);
	}
	else if(isCurrentSelectPointShow == 1)
	{
		isCurrentSelectPointShow = 0;
		this->ren->RemoveActor(currentSelectPointActor);
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::switchFitPlanePointShow()
{
	if(isFitPlanePointShow == 0)
	{
		isFitPlanePointShow = 1;

		for(int i = 0; i < 20; ++i)
		{
			if(fitPlanePointStat[i])
				this->ren->AddActor(fitPlanePointActor[i]);
		}
	}
	else if(isFitPlanePointShow == 1)
	{
		isFitPlanePointShow = 0;

		for(int i = 0; i < 20; ++i)
		{
			if(fitPlanePointStat[i])
				this->ren->RemoveActor(fitPlanePointActor[i]);
		}
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::turnoffFitPlaneResult()
{
	if(isFitPlanePointShow == 1)
		switchFitPlanePointShow();
	if(isFitPlaneShow == 1)
		switchFitPlaneShow();
	if(isGapPointShow == 1)
		switchGapPointShow();
	if(isGapProjectPointShow == 1)
		switchGapProjectPointShow();
}

void MainWindow::turnoffFaceSideResult()
{
	if(isPointInVerticalPlaneShow == 1)
		switchPointInVerticalPlaneShow();
	if(isVerticalPlaneShow == 1)
		switchVerticalPlaneShow();
	if(isTopToothPointForVerticalPlaneShow == 1)
		switchTopToothPointForVerticalPlaneShow();
	if(isTopToothProjectPointForVerticalPlane == 1)
		switchTopToothProjectPointForVerticalPlane();
	if(isTopToothPointForFitPlaneShow == 1)
		switchTopToothPointForFitPlaneShow();
	if(isTopToothProjectPointForFitPlaneShow == 1)
		switchTopToothProjectPointForFitPlaneShow();
	if(isLowToothPointForVerticalPlaneShow == 1)
		switchLowToothPointForVerticalPlaneShow();
	if(isLowToothProjectPointForVerticalPlaneShow == 1)
		switchLowToothProjectPointForVerticalPlaneShow();
	if(isLowToothPointForFitPlaneShow == 1)
		switchLowToothPointForFitPlaneShow();
	if(isLowToothProjectPointForFitPlaneShow == 1)
		switchLowToothProjectPointForFitPlaneShow();
}

void MainWindow::turnoffCoverResult()
{
	if(isSplinePointShow == 1)
		switchSplinePointShow();
	if(isFittingCurveShow == 1)
		switchFittingCurveShow();
	if(isCoverPointShow == 1)
		switchCoverPointShow();
	if(isCoverPlaneOShow == 1)
		switchCoverPlaneOShow();
	if(isParCoverIntersectCurveShow == 1)
		switchCoverIntersectCurveShow();
	if(isCoverIntersectCurvePointShow == 1)
		switchCoverIntersectCurvePointShow();
}

void MainWindow::turnoffJCoverResult()
{
	if(isSplinePointShow == 1)
		switchSplinePointShow();
	if(isFittingCurveShow == 1)
		switchFittingCurveShow();
	if(isJCoverPointShow == 1)
		switchJCoverPointShow();
	if(isJCoverPlaneOShow == 1)
		switchJCoverPlaneOShow();
	if(isParJCoverIntersectCurveShow == 1)
		switchJCoverIntersectCurveShow();
	if(isJCoverIntersectCurvePointShow == 1)
		switchJCoverIntersectCurvePointShow();
}

void MainWindow::turnoffInverseFitCoverResult()
{
	if(isInverseFitCoverPointShow == 1 && isInverseFitCoverProjectShow == 1)
		switchInverseFitCoverShow();
}

void MainWindow::turnoffOverBiteResult()
{
	if(isPointInOverbiteParallelPlaneShow == 1)
		switchPointInOverbiteParallelPlaneShow();
	if(isOverbiteParrelPlaneShow == 1)
		switchOverbiteParrelPlaneShow();
	if(isOverbiteIntersectCurveShow == 1)
		switchOverbiteIntersectCurveShow();
}


void MainWindow::turnoffAboToothTouchRelationResult()
{
	for(int i = 0; i < 26; ++i)
	{
		for(int j = 0; j < aboTouchRelationPointNum[i]; ++j)
		{
			if(isAboTouchRelationPointShow[i][j] == 1)
				switchboTouchRelationPointShow(i);
			if(isAboTouchRelationProjectPointShow[i][j] == 1)
				switchAboTouchRelationProjectPointShow(i);
		}
	}
}

void MainWindow::turnoffAboCoverResult()
{
	for(int i = 0; i < 18; ++i)
	{
		if(isAboCoverPointShow[i][1] == 1)
			switchAboCoverPointAShow(i);
		if(isAboCoverPointShow[i][0] == 1)
			switchAboCoverPointBShow(i);
		if(isAboCoverPlaneOShow[i] == 1)
			switchAboCoverPlaneOShow(i);
		if(isCoverIntersectCurveShow[i][0] == 1 && isCoverIntersectCurveShow[i][1] == 1)
			switchisCoverIntersectCurveShow(i);
	}
}

void MainWindow::turnoffToothCloseTouchResult()
{
	for(int i = 0; i < 20; ++i)
	{
		if(isAboToothCloseTouchPointShow[i][0] == 1)
			switchAboToothCloseTouchPointAShow(i);
		if(isAboToothCloseTouchPointShow[i][1] == 1)
			switchAboToothCloseTouchPointBShow(i);
		if(isAboToothCloseTouchPlaneOShow[i] == 1)
			switchAboToothCloseTouchPlaneOShow(i);
		if(isAboToothCloseTouchIntersectCurveShow[i] == 1)
			switchAboToothCloseTouchIntersectCurveShow(i);
	}
}

void MainWindow::turnoffToothCloseRelationResult()
{
	for(int i = 0; i < 10; ++i)
	{
		if(isAboToothCloseRelationPointShow[i][0] == 1)
			switchAboToothColoseRelationLowPointShow(i);
		if(isAboToothCloseRelationPlaneOShow[i] == 1)
			switchAboToothCloseRelationPlaneOShow(i);
		if(isAboToothCloseRelationPointShow[i][1] == 1)
			switchAboToothCloseRelationTopPointShow(i);
	}
}

void MainWindow::turnoffChapResult()
{
	if(isChapFitPlaneShow == 1)
		switchChapFitPlaneShow();

	for(int i = 0; i < 14; ++i)
	{
		if(isChapPointShow[i][0] == 1 || isChapPointShow[i][1] == 1)
			switchChapPointShow(i);
		if(isChapProjectPointShow[i][0] == 1 && isChapProjectPointShow[i][1] == 1)
			switchChapProjectPointShow(i);
	}
}

void MainWindow::turnoffAboEdgeVertebralHeighResult()
{
	for(int i = 0; i < 20; ++i)
	{
		if(isAboEdgeVertebralHeighPointShow[i][0] == 1 || isAboEdgeVertebralHeighPointShow[i][1] == 1)
		{
			switchAboEdgeVertebralHeighPointShow(i);
		}

		if(isAboEdgeVertebralHeighProjectPointShow[i][0] == 1 || isAboEdgeVertebralHeighProjectPointShow[i][1] == 1)
		{
			switchAboEdgeVertebralHeighProjectPointShow(i);
		}
	}
}

void MainWindow::turnoffAboArrangeResult()
{
	for(int i = 0; i < 26; ++i)
	{
		if(isAboArrangePointShow[i][0] == 1 || isAboArrangePointShow[i][1] == 1)
		{
			switchAboArrangePointShow(i);
		}
		if(isAboArrangeProjectPointShow[i][0] == 1 || isAboArrangeProjectPointShow[i][1] == 1)
		{
			switchAboArrangeProjectPointShow(i);
		}
		if(isAboArrangePlaneOShow[i] == 1)
		{
			switchAboArrangePlaneOShow(i);
		}
	}
}

void MainWindow::turnoffDiCloseRelationResult()
{
	if(isDiCloseRelationPointAShow == 1)
		switchDiCloseRelationPointAShow();
	if(isDiCloseRelationPointBShow == 1)
		switchDiCloseRelationPointBShow();
	if(isDiCloseRelationPlaneOShow == 1)
		switchDiCloseRelationPlaneOShow();
}

void MainWindow::turnoffDiScrowdResult()
{
	for(int i = 0; i < 12; ++i)
	{
		if(isDiGrindToothScrowdToothShow[i] == 1)
			switchDiGrindToothScrowdToothShow(i);
	}
	for(int i = 0; i < 4; ++i)
		if(isDiToothBowScrowdShow[i] == 1)
			switchDiToothBowScrowdShow(i);
}

void MainWindow::turnoffDiMiddleCutToothGapResult()
{
	if(isDiMiddleCutToothGapShow == 1)
		switchDiMiddleCutToothGapShow();
}

void MainWindow::turnoffDiMiddleUnfitResult()
{
	if(diMiddleUnfitPointShow == 1)
		switchDiMiddleUnfitPointShow();
}

void MainWindow::turnoffDiBackToothLockCloseResult()
{
	if(isDiBackToothLockClosePointShow == 1)
		switchDiBackToothLockClosePointShow();
	if(isDiBackToothLockCloseProjectPointShow == 1)
		switchDiBackToothLockCloseProjectPointShow();
}

void MainWindow::turnoffOpenCloseResult()
{
	if(isPointForArticlePlaneShow == 1)
		switchPointForArticlePlaneShow();
	if(isArticlePlaneShow == 1)
		switchArticlePlaneShow();
	if(isTopPointToProjectArticlePlaneShow == 1)
		switchTopPointToProjectArticlePlaneShow();
	if(isLowPointToProjectArticlePlaneShow == 1)
		switchLowPointToProjectArticlePlaneShow();
	if(isProjectArticlePlane == 1)
		switchProjectArticlePlane();
	if(idFrontToothToProjectPointPositionA0Show == 1)
		switchFrontToothToProjectPointPositionA0Show();
	if(isFrontToothOpenCloseFittingPointShow == 1)
		switchFrontToothOpenCloseFittingPointShow();
	if(isFrontToothOpenCloseFittingCurveShow == 1)
		switchFrontToothOpenCloseFittingCurveShow();
	if(isFrontToothPlaneOShow == 1)
		switchFrontToothPlaneOShow();
	if(isFrontToothContourCurveShaow == 1)
		switchFrontToothContourCurveShaow();
	if(isFrontToothContiourCurvePointShow == 1)
		switchFrontToothContiourCurvePointShow();
	if(isBackToothOpenCloseFittingPointShow == 1)
		switchBackToothOpenCloseFittingPointShow();
	if(isBackToothOpenCloseFittingCurveShow == 1)
		switchBackToothOpenCloseFittingCurveShow();
	if(isBackToothPointA0Show == 1)
		switchBackToothPointA0Show();
	if(isBackToothPlaneOShow == 1)
		switchBackToothPlaneOShow();
	if(isBackToothContourCurveShaow == 1)
		switchBackToothContourCurveShaow();
	if(isBackToothContiourCurvePointShow == 1)
		switchBackToothContiourCurvePointShow();
}

void MainWindow::turnoffMiddleLineResult()
{
	if(isVerticalParallelPlaneInitPointShow == 1)
		switchVerticalParallelPlaneInitPointShow();
	if(isVerticalParallelPlaneShow == 1)
		switchVerticalParallelPlaneShow();
	if(isVerticalParallelPlaneInitPoint2Show == 1)
		switchVerticalParallelPlaneInitPoint2Show();
	if(isVerticalParallelPlane2Show == 1)
		switchVerticalParallelPlane2Show();
	if(isTwoPointToProjectFitPlaneShow == 1)
		switchTwoPointToProjectFitPlaneShow();
	//if(isTwoPointProjectFitPlaneShow == 1)
	//	switchTwoPointProjectFitPlaneShow();
}

void MainWindow::turnoffDiBackToothOppositeCloseResult()
{
	if(isDiBackToothOppositeClosePointShow == 1)
		switchDiBackToothOppositeClosePointShow();
	if(isDiBackToothOppositeCloseProjectPointShow == 1)
		switchDiBackToothOppositeCloseProjectPointShow();
}

void MainWindow::turnoffDiFrontOpenCloseResult()
{
	for(int i = 0; i < 20; ++i)
	{
		if(isDiFrontOpenClosePointShow[i] == 1)
			switchDiFrontOpenClosePointShow(i);
		if(isDiFrontOpenClosePlaneOShow[i] == 1)
			switchDiFrontOpenClosePlaneOShow(i);
		if(isDiFrontOpenCloseIntersectCurveShow[i] == 1)
			switchDiFrontOpenCloseIntersectCurveShow(i);
		if(isDiFrontOpenCloseIntersectCurvePointShow[i] == 1)
			switchDiFrontOpenCloseIntersectCurvePointShow(i);
	}
}

void MainWindow::turnoffDiSideOpenCloseResult()
{
	for(int i = 0; i < 8; ++i)
	{
		if(isDiSideOpenClosePointShow[i] == 1)
			switchDiSideOpenClosePointShow(i);
		if(isDiSideOpenClosePlaneOShow[i] == 1)
			switchDiSideOpenClosePlaneOShow(i);
		if(isDiSideOpenCloseIntersectCurveShow[i] == 1)
			switchDiSideOpenCloseIntersectCurveShow(i);
		if(isDiSideOpenCloseIntersectCurvePointShow[i] == 1)
			switchDiSideOpenCloseIntersectCurvePointShow(i);
	}
}

void MainWindow::turnoffDiOverbiteResult()
{
	if(isDiOverbitePointShow == 1)
		switchDiOverbitePointShow();
	if(isDiOverbitePlaneOShow == 1)
		switchDiOverbitePlaneOShow();
	if(isDiOverbiteIntersectCurveShow == 1)
		switchDiOverbiteIntersectCurveShow();
	if(isDiOverbiteIntersectCurvePointShow == 1)
		switchDiOverbiteIntersectCurvePointShow();
}
/*
void MainWindow::turnoffDiCoverResult()
{
	if(isDiPointA0Show == 1)
		switchDiPointA0Show();
	if(isDiPlaneOShow == 1)
		switchDiPlaneOShow();
	if(isDiPlanePShow == 1)
		switchDiPlanePShow();
	if(isDiCoverIntersectLineShow == 1)
		switchDiCoverIntersectLineShow();
}
*/
void MainWindow::turnoffDiCoverResult()
{
	if(isDiCoverPointShow == 1)
		switchDiCoverPointShow();
	if(isDiCoverPlaneOShow == 1)
		switchDiCoverPlaneOShow();
	if(isDiCoverIntersectCurveShow == 1)
		switchDiCoverIntersectCurveShow();
	if(isDiCoverIntersectCurvePointShow == 1)
		switchDiCoverIntersectCurvePointShow();
}
void MainWindow::turnoffMeasureResult()
{
	if(ismeasureDistancePointToPointShow == 1)
		switchMeasureDistanceLineToLine();
}

void MainWindow::turnoffAllResult()
{
	turnoffFitPlaneResult();
	turnoffFaceSideResult();
	turnoffCoverResult();
	turnoffInverseFitCoverResult();
	turnoffOverBiteResult();
	turnoffOpenCloseResult();
	turnoffMiddleLineResult();
	turnoffJCoverResult();
	turnoffMeasureResult();
	
	turnoffAboToothTouchRelationResult();
	turnoffAboCoverResult();
	turnoffToothCloseTouchResult();
	turnoffToothCloseRelationResult();
	turnoffChapResult();
	turnoffAboEdgeVertebralHeighResult();
	turnoffAboArrangeResult();

	turnoffDiCoverResult();
	turnoffDiOverbiteResult();
	turnoffDiSideOpenCloseResult();
	turnoffDiFrontOpenCloseResult();
	turnoffDiBackToothOppositeCloseResult();
	turnoffDiBackToothLockCloseResult();
	turnoffDiMiddleUnfitResult();
	turnoffDiMiddleCutToothGapResult();
	turnoffDiScrowdResult();
	turnoffDiCloseRelationResult();
}

void MainWindow::setOperatePreCondition(int selectOperationTreeIndex, int operatorType)
{
	if(selectOperationTreeIndex == 4 && operatorType == 2)
	{
		// 选点(前牙区不齐->合平面)

		turnoffAllResult();

		//if(isTopToothShow == 0)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();

		// 显示已选中的点
		isFitPlanePointShow = 0;
		switchFitPlanePointShow();
	}
	else if (selectOperationTreeIndex == 1111 && operatorType == 3)
	{
		turnoffAllResult();
	}
	else if((selectOptionTreeIndex - 2510) >= 0 && (selectOptionTreeIndex - 2510) < 500 && (selectOptionTreeIndex - 2510) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS颊舌向倾斜度
		//		选点
		//

		turnoffAllResult();

		int pointPareIndex = (selectOptionTreeIndex - 2510) / 25;
		if(aboChapPointPickNum[pointPareIndex] == 2)
		{
			aboChapPointPickNum[pointPareIndex] = 0;
		}

		for(int i = 0; i < aboChapPointPickNum[pointPareIndex]; ++i)
		{
			this->ren->AddActor(aboChapPointAcotor[pointPareIndex][i]);
			isChapPointShow[pointPareIndex][i] = 1;
		}
	}
	else if((selectOptionTreeIndex - 4510) >= 0 && (selectOptionTreeIndex - 4510) < 650 && (selectOptionTreeIndex - 4510) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS邻牙接触关系
		//		选点
		//

		turnoffAllResult();

		int pointPareIndex = (selectOptionTreeIndex - 4510) / 25;
		if(aboTouchRelationPointNum[pointPareIndex] == 2)
		{
			aboTouchRelationPointNum[pointPareIndex] = 0;
		}

		for(int i = 0; i < aboTouchRelationPointNum[pointPareIndex]; ++i)
		{
			this->ren->AddActor(aboTouchRelationPointAcotor[pointPareIndex][i]);
			this->ren->AddActor(aboTouchRelationProjectPointAcotor[pointPareIndex][i]);
		}
	}
	else if((selectOptionTreeIndex - 4018) >= 0 && (selectOptionTreeIndex - 4018) < 500 && (selectOptionTreeIndex - 4018) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS覆盖
		//		上牙选点
		//

		turnoffAllResult();
		int pointPareIndex = (selectOptionTreeIndex - 4018) / 25;
		this->ren->AddActor(aboCoverIntersectCurveActor[pointPareIndex]);
		if (isCoverIntersectCurveShow[pointPareIndex][0] == 0)
		{
			switchisCoverIntersectCurveShow(pointPareIndex);
		}

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();
	}
	else if((selectOptionTreeIndex - 4011) >= 0 && (selectOptionTreeIndex - 4011) < 500 && (selectOptionTreeIndex - 4011) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS覆盖
		//		下牙选点
		//

		turnoffAllResult();

		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();
	}
	else if((selectOptionTreeIndex - 3510) >= 0 && (selectOptionTreeIndex - 3510) < 500 && (selectOptionTreeIndex - 3510) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS牙合接触
		//		选点
		//

		turnoffAllResult();

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();
	}
	else if((selectOptionTreeIndex - 3010) >= 0 && (selectOptionTreeIndex - 3010) < 500 && (selectOptionTreeIndex - 3010) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS牙合关系
		//		选点
		//

		turnoffAllResult();

		int pointPareIndex = (selectOptionTreeIndex - 3010) / 25;
		if(aboToothCloseRelationPointPickNum[pointPareIndex] == 2)
		{
			aboToothCloseRelationPointPickNum[pointPareIndex] = 0;
		}

		if(aboToothCloseRelationPointPickNum[pointPareIndex] == 0)
		{
			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 1)
				switchLowToothShow();
		}
		else if(aboToothCloseRelationPointPickNum[pointPareIndex] == 1)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			if(isLowToothShow == 0)
				switchLowToothShow();
		}

		for(int i = 0; i < aboToothCloseRelationPointPickNum[i]; ++i)
		{
			this->ren->AddActor(aboToothCloseRelationPointAcotor[pointPareIndex][i]);
		}
	}
	else if((selectOptionTreeIndex - 2010) >= 0 && (selectOptionTreeIndex - 2010) < 500 && (selectOptionTreeIndex - 2010) % 25 == 0 && operatorType == 2)
	{
		//
		// ABO-OGS边缘嵴高度
		//		选点
		//

		turnoffAllResult();

		int pointPareIndex = (selectOptionTreeIndex - 2010) / 25;
		if(aboEdgeVertebralHeighPointPickNum[pointPareIndex] == 2)
		{
			aboEdgeVertebralHeighPointPickNum[pointPareIndex] = 0;
		}
		for(int i = 0; i < aboEdgeVertebralHeighPointPickNum[i]; ++i)
		{
			this->ren->AddActor(aboEdgeVertebralHeighPointAcotor[pointPareIndex][i]);
			isAboEdgeVertebralHeighPointShow[pointPareIndex][i] = 1;
		}
	}
	else if(selectOptionTreeIndex / 100 == 1510 && operatorType == 2)
	{
		//
		// ABO-OGS排列
		//		选点
		//

		turnoffAllResult();

		int pointPareIndex = selectOptionTreeIndex % 100;
		if(aboArrangePointPickNum[pointPareIndex] == 2)
		{
			aboArrangePointPickNum[pointPareIndex] = 0;
		}
		for(int i = 0; i < aboArrangePointPickNum[i]; ++i)
		{
			this->ren->AddActor(aboPointAcotor[pointPareIndex][i]);
			isAboArrangePointShow[pointPareIndex][i] = 1;
		}
	}
	else if(selectOperationTreeIndex == 1009 && operatorType == 7)
	{
		// 得分(DI其他->得分)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 1024 && operatorType == 7)
	{
		// 距离(DI拥挤度->得分)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 1023 && operatorType == 7)
	{
		// 得分(DI拥挤度->得分)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 1373 && operatorType == 2)
	{
		// 选点(DI合关系->测量->点A)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 1374 && operatorType == 2)
	{
		// 选点(DI合关系->测量->点B)

		turnoffAllResult();
	}
	/*else if(selectOperationTreeIndex == 119 && operatorType == 2)
	{
		// DI覆盖选点

		turnoffAllResult();
	}*/
	else if(selectOperationTreeIndex == 6900 && operatorType == 2)
	{
		// 覆盖选点

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 7500 && operatorType == 2)
	{
		// 选点(覆盖->轮廓曲线->点)

		turnoffAllResult();

		if(JCoverIntersectCurvePointIndex == 2)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			JCoverIntersectCurvePointIndex = 0;

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(JCoverIntersectCurvePointActor[i]);
			}
		}
		if(JCoverIntersectCurvePointIndex < 2)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			this->ren->AddActor(TopJCoverIntersectCurveActor);
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->AddActor(JCoverIntersectCurveActor);
			if (isParJCoverIntersectCurveShow == 0)
			{
				switchJCoverIntersectCurveShow();
			}
		}
	}
	else if(selectOperationTreeIndex == 69 && operatorType == 2)
	{
		// 覆盖选点

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 75 && operatorType == 2)
	{
		// 选点(覆盖->轮廓曲线->点)

		turnoffAllResult();

		if(CoverIntersectCurvePointIndex == 2)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			CoverIntersectCurvePointIndex = 0;

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(CoverIntersectCurvePointActor[i]);
			}
		}
		if(CoverIntersectCurvePointIndex < 2)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			this->ren->AddActor(TopCoverIntersectCurveActor);
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->AddActor(CoverIntersectCurveActor);
			if (isParCoverIntersectCurveShow == 0)
			{
				switchCoverIntersectCurveShow();
			}
		}
	}
	else if(selectOperationTreeIndex == 119 && operatorType == 2)
	{
		// DI覆盖选点

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 125 && operatorType == 2)
	{
		// 选点(DI覆盖->轮廓曲线->点)

		turnoffAllResult();

		if(diCoverIntersectCurvePointIndex == 2)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			diCoverIntersectCurvePointIndex = 0;

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(diCoverIntersectCurvePointActor[i]);
			}
		}
		if(diCoverIntersectCurvePointIndex < 2)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			this->ren->AddActor(diTopCoverIntersectCurveActor);
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->AddActor(diCoverIntersectCurveActor);
			if (isDiCoverIntersectCurveShow == 0)
			{
				switchDiCoverIntersectCurveShow();
			}
		}
	}
	else if(selectOperationTreeIndex == 129 && operatorType == 2)
	{
		// DI覆合选点

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 135 && operatorType == 2)
	{
		// 选点(DI覆合->轮廓曲线->点)

		turnoffAllResult();

		if(diOverbiteIntersectCurvePointIndex == 2)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			diOverbiteIntersectCurvePointIndex = 0;

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(diOverbiteIntersectCurvePointActor[i]);
			}
		}
		/////////////修改
		/*
		if(diOverbiteIntersectCurvePointIndex == 0)
		{
			if(isTopToothShow == 0)
				switchTopToothShow();
			this->ren->AddActor(diTopOverbiteIntersectCurveActor);
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->RemoveActor(diOverbiteIntersectCurveActor);
		}
		else if(diOverbiteIntersectCurvePointIndex == 1)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			this->ren->RemoveActor(diTopOverbiteIntersectCurveActor);
			if(isLowToothShow == 0)
				switchLowToothShow();
			this->ren->AddActor(diOverbiteIntersectCurveActor);
		}*/

		if(diOverbiteIntersectCurvePointIndex < 2)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			this->ren->AddActor(diTopOverbiteIntersectCurveActor);
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->AddActor(diOverbiteIntersectCurveActor);
			if (isDiOverbiteIntersectCurveShow == 0)
			{
				switchDiOverbiteIntersectCurveShow();
			}
		}
	}
	else if((selectOperationTreeIndex /100 == 141) && operatorType == 2)
	{
		// 选点(DI侧方开合->磨牙1/2/3->点)

		turnoffAllResult();
	}
	else if((selectOperationTreeIndex / 100 == 147) && operatorType == 2)
	{
		// 选点(DI侧方开合->磨牙1/2/3->轮廓曲线->点)

		turnoffAllResult();

		int grindToothIndex = selectOptionTreeIndex - 14700;
		//QMessageBox::information(NULL, tr("Info"), QString::number(grindToothIndex));

		if(diSideOpenCloseIntersectCurvePointIndex[grindToothIndex] == 2)
		{
			removeChildItem(currentOptionTreeWidgetItem);
			diSideOpenCloseIntersectCurvePointIndex[grindToothIndex] = 0;

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(diSideOpenCloseIntersectCurvePointActor[grindToothIndex][i]);
			}
		}

		if(diSideOpenCloseIntersectCurvePointIndex[grindToothIndex] == 0)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->AddActor(diTopSideOpenCloseIntersectCurveActor[grindToothIndex]);
			isDiSideOpenCloseIntersectCurveShow[grindToothIndex] = 1;

			//this->ren->RemoveActor(diSideOpenCloseIntersectCurveActor[grindToothIndex]);

			//this->ren->RemoveActor(diTopSideOpenCloseIntersectCurveActor[grindToothIndex]);

			this->ren->AddActor(diSideOpenCloseIntersectCurveActor[grindToothIndex]);
		}
		else if(diSideOpenCloseIntersectCurvePointIndex[grindToothIndex] == 1)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			if(isLowToothShow == 1)
				switchLowToothShow();
			this->ren->AddActor(diTopSideOpenCloseIntersectCurveActor[grindToothIndex]);
			isDiSideOpenCloseIntersectCurveShow[grindToothIndex] = 2;

			//this->ren->RemoveActor(diSideOpenCloseIntersectCurveActor[grindToothIndex]);

			//this->ren->RemoveActor(diTopSideOpenCloseIntersectCurveActor[grindToothIndex]);

			this->ren->AddActor(diSideOpenCloseIntersectCurveActor[grindToothIndex]);
// 			if(isTopToothShow == 1)
// 				switchTopToothShow();
// 			this->ren->RemoveActor(diTopSideOpenCloseIntersectCurveActor[grindToothIndex]);
// 			if(isLowToothShow == 0)
// 				switchLowToothShow();
// 			this->ren->AddActor(diSideOpenCloseIntersectCurveActor[grindToothIndex]);
// 			isDiSideOpenCloseIntersectCurveShow[grindToothIndex] = 1;
		}
	}
	else if(selectOperationTreeIndex == 1002 && operatorType == 2)
	{
		// 选点(DI其他->中线不调->点)

		turnoffAllResult();

		if(diMiddleUnfitPointIndex == 2)
		{
			diMiddleUnfitPointIndex = 0;

			removeChildItem(currentOptionTreeWidgetItem);

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(measureDistancePointToPointPointActor[i]);
			}
		}

		if(diMiddleUnfitPointIndex == 0)
		{
			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 1)
				switchLowToothShow();
		}
		else if(diMiddleUnfitPointIndex == 1)
		{
			if(isTopToothShow == 1)
				switchTopToothShow();
			if(isLowToothShow == 0)
				switchLowToothShow();
		}
	}
	else if(selectOperationTreeIndex == 1006 && operatorType == 2)
	{
		// 选点(DI其他->中切牙牙间隙->点)

		turnoffAllResult();

		if(diMiddleCutToothGapIndex == 2)
		{
			diMiddleCutToothGapIndex = 0;

			removeChildItem(currentOptionTreeWidgetItem);

			for(int i = 0; i < 2; ++i)
			{
				this->ren->RemoveActor(diMiddleCutToothGapPointActor[i]);
			}
		}

		if(diMiddleCutToothGapIndex == 0)
		{
			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 1)
				switchLowToothShow();
		}
		else if(diMiddleCutToothGapIndex == 1)
		{
			if(isTopToothShow == 0)
				switchTopToothShow();
			if(isLowToothShow == 1)
				switchLowToothShow();
		}
	}
	else if(selectOperationTreeIndex == 401 && operatorType == 2)
	{
		// 选点(DI前牙开合->点)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 901 && operatorType == 2)
	{
		// 选点(DI后牙反合->点)

		turnoffAllResult();

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();
		switchDiBackToothOppositeClosePointShow();
	}
	else if(selectOperationTreeIndex == 901 && operatorType == 5)
	{
		// 删除点(DI后牙反合->点)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 902 && operatorType == 6)
	{
		// 投影(DI后牙反合->点)

		turnoffAllResult();

		if(isFitPlaneShow == 0)
			switchFitPlaneShow();
		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();
	}
	else if(selectOperationTreeIndex == 905 && operatorType == 2)
	{
		// 选点(DI后牙反合->点)

		turnoffAllResult();

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();
		switchDiBackToothLockClosePointShow();
	}
	else if(selectOperationTreeIndex == 905 && operatorType == 5)
	{
		// 删除点(DI后牙正锁合->点)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 906 && operatorType == 6)
	{
		// 投影(DI后牙正锁合->投影)

		turnoffAllResult();

		if(isFitPlaneShow == 0)
			switchFitPlaneShow();
		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();
	}
	else if(selectOperationTreeIndex == 1002 && operatorType == 5)
	{
		// 删除点(DI后牙正锁合->点)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 1006 && operatorType == 5)
	{
		// 删除点(DI其他->中切牙牙间隙->点)

		turnoffAllResult();
	}
	else if((selectOperationTreeIndex > 420 && selectOperationTreeIndex < 820) && operatorType == 5)
	{
		if(((selectOptionTreeIndex - 427) % 20) == 0)
		{
			// 删除点(DI前牙开合->测量1/../20->轮廓曲线->点)
		}
	}
	else if((selectOperationTreeIndex / 100 > 1030 && selectOperationTreeIndex / 100 < 1270) && operatorType == 2)
	{
		if((selectOptionTreeIndex - 1031 * 100) < 100)
		{
			// 选点(DI拥挤度->磨牙->测量1/../12->点)

			turnoffAllResult();
			for(int i = 0; i < 12; ++i)
				switchDiGrindToothScrowdToothShow(i);

			int measureIndex = (selectOptionTreeIndex - 1031 * 100);

			if(diGrindToothScrowdPointIndex[measureIndex] == 2)
			{
				removeChildItem(currentOptionTreeWidgetItem);
				diGrindToothScrowdPointIndex[measureIndex] = 0;

				this->ren->RemoveActor(diGrindToothScrowdPointActor[2 * measureIndex]);
				this->ren->RemoveActor(diGrindToothScrowdPointActor[2 * measureIndex + 1]);
			}
		}
	}
	else if((selectOperationTreeIndex > 1270 && selectOperationTreeIndex < 1350) && operatorType == 2)
	{
		if(((selectOptionTreeIndex - 1271) % 20) == 0)
		{
			// 选点(DI拥挤度->牙弓->测量1/../4->点)

			turnoffAllResult();
			for(int i = 0; i < 4; ++i)
				switchDiToothBowScrowdShow(i);

			int measureIndex = (selectOptionTreeIndex - 1271) / 20;

			if(diToothBowScrowdPointIndex[measureIndex] == 2)
			{
				removeChildItem(currentOptionTreeWidgetItem);
				diToothBowScrowdPointIndex[measureIndex] = 0;

				this->ren->RemoveActor(diToothBowScrowdPointActor[2 * measureIndex]);
				this->ren->RemoveActor(diToothBowScrowdPointActor[2 * measureIndex + 1]);
			}
		}
	}
	else if((selectOperationTreeIndex > 420 && selectOperationTreeIndex < 820) && operatorType == 2)
	{
		if(((selectOptionTreeIndex - 427) % 20) == 0)
		{
			// 选点(DI前牙开合->测量1/.../20->轮廓曲线->点)

			turnoffAllResult();

			int measureIndex = (selectOptionTreeIndex - 427) / 20;

			if(diFrontOpenCloseIntersectCurvePointIndex[measureIndex] == 2)
			{
				removeChildItem(currentOptionTreeWidgetItem);
				diFrontOpenCloseIntersectCurvePointIndex[measureIndex] = 0;

				for(int i = 0; i < 2; ++i)
				{
					this->ren->RemoveActor(diFrontOpenCloseIntersectCurvePointActor[measureIndex][i]);
				}
			}

			if(diFrontOpenCloseIntersectCurvePointIndex[measureIndex] == 0)
			{
				if(isTopToothShow == 0)
					switchTopToothShow();
				this->ren->AddActor(diTopFrontOpenCloseIntersectCurveActor[measureIndex]);
				isDiFrontOpenCloseIntersectCurveShow[measureIndex] = 1;
				if(isLowToothShow == 1)
					switchLowToothShow();
				this->ren->RemoveActor(diFrontOpenCloseIntersectCurveActor[measureIndex]);
			}
			else if(diFrontOpenCloseIntersectCurvePointIndex[measureIndex] == 1)
			{
				if(isTopToothShow == 1)
					switchTopToothShow();
				this->ren->RemoveActor(diTopFrontOpenCloseIntersectCurveActor[measureIndex]);
				if(isLowToothShow == 0)
					switchLowToothShow();
				this->ren->AddActor(diFrontOpenCloseIntersectCurveActor[measureIndex]);
				isDiFrontOpenCloseIntersectCurveShow[measureIndex] = 1;
			}
		}
		else if(((selectOptionTreeIndex - 421) % 20) == 0)
		{
			// 选点(DI前牙开合->测量1/.../20->点)

			turnoffAllResult();
		}
	}
	else if(selectOperationTreeIndex == 5 && operatorType == 5)
	{
		// 删除点(前牙区不齐->合平面->拟合点)

		turnoffAllResult();

		if(isFitPlanePointShow == 0)
		{
			switchFitPlanePointShow();
		}

		if(isCurrentSelectPointShow == 1)
		{
			switchCurrentSelectPointShow();
		}
	}
	else if(selectOperationTreeIndex == 7500 && operatorType == 5)
	{
		// 删除点(覆盖->轮廓曲线->点)
	}
	else if(selectOperationTreeIndex == 75 && operatorType == 5)
	{
		// 删除点(覆盖->轮廓曲线->点)
	}
	else if(selectOperationTreeIndex == 125 && operatorType == 5)
	{
		// 删除点(DI覆盖->轮廓曲线->点)
	}
	else if(selectOperationTreeIndex == 135 && operatorType == 5)
	{
		// 删除点(DI覆合->轮廓曲线->点)
	}
	else if((selectOperationTreeIndex / 100 == 147)
		&& operatorType == 5)
	{
		// 删除点(DI侧方开合->磨牙1/2/3->轮廓曲线->点)
	}
	else if(selectOperationTreeIndex == 4 && operatorType == 3)
	{
		// 生成合平面(前牙区不齐->合平面)

		turnoffAllResult();
	}
	else if((selectOptionTreeIndex / 1000 == 9 && selectOptionTreeIndex % 1000 < 10)
		&& operatorType == 2)
	{
		// 缝隙选点(前牙区不齐->接触点->缝隙1/2/3/4/5)

		turnoffAllResult();

		isGapPointShow = 0;
		switchGapPointShow();
	}
	else if((selectOptionTreeIndex / 1000 == 9 && selectOptionTreeIndex % 1000 < 10)
		&& operatorType == 5)
	{
		// 删除缝隙选点(前牙区不齐->接触点->缝隙1/2/3/4/5)

		turnoffAllResult();

		if(isGapPointShow == 0)
		{
			switchGapPointShow();
		}
	}
	else if(selectOperationTreeIndex == 8 && operatorType == 6)
	{
		// 投影点(前牙区不齐->接触点)

		turnoffAllResult();

		if(isGapPointShow == 0)
		{
			switchGapPointShow();
		}
		if(isFitPlaneShow == 0)
		{
			switchFitPlaneShow();
		}
	}
	else if(selectOperationTreeIndex == 23 && operatorType == 2)
	{
		// 选点(颊侧区咬合->下牙列->正中矢状面), 初始点

		turnoffAllResult();

		//if(isTopToothShow == 1)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();

		if(isPointInVerticalPlaneShow == 0)
			switchPointInVerticalPlaneShow();
	}
	else if(selectOperationTreeIndex == 26 && operatorType == 5)
	{
		// 删除点(颊侧区咬合->下牙列->正中矢状面->初始点)

		turnoffAllResult();

		//if(isTopToothShow == 1)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();
		
		if(isPointInVerticalPlaneShow == 0)
			switchPointInVerticalPlaneShow();
	}
	else if(selectOperationTreeIndex == 24 && operatorType == 2)
	{
		// 下牙列矢状向选点

		turnoffAllResult();

		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		if(isVerticalPlaneShow == 0)
			switchVerticalPlaneShow();

		if(isTopToothProjectPointForVerticalPlane == 0)
		{
			switchTopToothProjectPointForVerticalPlane();
		}

		if(isLowToothPointForVerticalPlaneShow == 0)
		{
			switchLowToothPointForVerticalPlaneShow();
		}
	}
	else if(selectOperationTreeIndex == 32 && operatorType == 6)
	{
		// 投影(颊侧区咬合->下牙列->点(矢状向)->投影结果)

		turnoffAllResult();

		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		if(isVerticalPlaneShow == 0)
			switchVerticalPlaneShow();

		if(isLowToothPointForVerticalPlaneShow == 0)
		{
			switchLowToothPointForVerticalPlaneShow();
		}
	}
	else if(selectOperationTreeIndex == 25 && operatorType == 2)
	{
		// 下牙列水平选点

		turnoffAllResult();

		//if(isTopToothShow == 1)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();

		switchLowToothPointForFitPlaneShow();
	}
	else if(selectOperationTreeIndex == 28 && operatorType == 2)
	{
		// 选点(颊侧区咬合->上牙列->点(矢状向))

		turnoffAllResult();

		if(isTopToothShow == 0)
		{
			switchTopToothShow();
		}
		if(isLowToothShow == 1)
		{
			switchLowToothShow();
		}
		if(isTopToothProjectPointForVerticalPlane == 0)
		{
			switchTopToothProjectPointForVerticalPlane();
		}

		if(isLowToothProjectPointForVerticalPlaneShow == 0)
		{
			switchLowToothProjectPointForVerticalPlaneShow();
		}
	}
	else if(selectOperationTreeIndex == 25 && operatorType == 5)
	{
		// 删除点(颊侧区咬合->下牙列->点(水平向))

		turnoffAllResult();

		//if(isTopToothShow == 1)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();

		switchLowToothPointForFitPlaneShow();
	}
	else if(selectOperationTreeIndex == 33 && operatorType == 6)
	{
		// 投影点(颊侧区咬合->下牙列->点(水平向)->投影结果)

		turnoffAllResult();

		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		if(isFitPlaneShow == 0)
			switchFitPlaneShow();

		if(isLowToothPointForFitPlaneShow == 1)
			switchLowToothPointForFitPlaneShow();
	}
	else if(selectOperationTreeIndex == 31 && operatorType == 6)
	{
		// 投影点(颊侧区咬合->上牙列->点(水平向)->投影结果)

		turnoffAllResult();

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();

		if(isFitPlaneShow == 0)
			switchFitPlaneShow();

		if(isTopToothPointForFitPlaneShow == 1)
			switchTopToothPointForFitPlaneShow();
	}
	else if(selectOperationTreeIndex == 35 && operatorType == 2)
	{
		// 选点(中线->矢状平行面1)

		turnoffAllResult();

		//if(isTopToothShow == 1)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();

		switchVerticalPlaneShow();
		switchVerticalParallelPlaneInitPoint2Show();
		switchVerticalParallelPlane2Show();
	}
	else if(selectOperationTreeIndex == 97 && operatorType == 2)
	{
		// 选点(中线->矢状平行面2)

		turnoffAllResult();

		switchVerticalPlaneShow();
		switchVerticalParallelPlaneInitPointShow();
		switchVerticalParallelPlaneShow();
	}
	else if(selectOperationTreeIndex == 66 && operatorType == 2)
	{
		// 选点(覆盖->拟合曲线->点)

		turnoffAllResult();

		//if(isTopToothShow == 1)
		//	switchTopToothShow();
		//if(isLowToothShow == 0)
		//	switchLowToothShow();

		switchSplinePointShow();
	}
	else if(selectOperationTreeIndex == 67 && operatorType == 5)
	{
		// 删除点(覆盖->拟合曲线->点)

		turnoffAllResult();

		switchSplinePointShow();
	}
	else if(selectOperationTreeIndex == 66 && operatorType == 4)
	{
		// 拟合曲线(覆盖->拟合曲线)

		turnoffAllResult();

		switchFitPlaneShow();
	}
// 	else if(selectOperationTreeIndex == 72 && operatorType == 2)
// 	{
// 		// 选点(覆盖->平面->牙弓放射状相交平面->点)
// 
// 		turnoffAllResult();
// 
// 		if(isLowToothShow == 0)
// 			switchLowToothShow();
// 
// 		switchFitPlaneShow();
// 		switchFittingCurveShow();
// 	}
// 	else if(selectOperationTreeIndex == 76 && operatorType == 2)
// 	{
// 		// 选点(覆盖->平面->交线->点)
// 		turnoffFitPlaneResult();
// 
// 		if(isPlaneOShow == 1)
// 			switchPlaneOShow();
// 	}
	else if((selectOperationTreeIndex == 103 || selectOperationTreeIndex == 104 || 
		selectOperationTreeIndex == 105 || selectOperationTreeIndex ==106) && operatorType == 2)
	{
		// 选点(开合->前牙开合->冠状面->点->上牙列->点对1/2/3/4)
		turnoffAllResult();

		if(isTopToothShow == 0)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();

		if(isTopPointToProjectArticlePlaneShow == 0)
			switchTopPointToProjectArticlePlaneShow();
	}
	else if((selectOperationTreeIndex == 107 || selectOperationTreeIndex == 108 || 
		selectOperationTreeIndex == 109 || selectOperationTreeIndex ==110) && operatorType == 2)
	{
		// 选点(开合->前牙开合->冠状面->点->下牙列->点对1/2/3/4)
		turnoffAllResult();

		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 0)
			switchLowToothShow();

		if(isLowPointToProjectArticlePlaneShow == 0)
			switchLowPointToProjectArticlePlaneShow();
	}
	else if(selectOperationTreeIndex == 88 && operatorType == 6)
	{
		// 投影(开合->前牙开合->冠状面->点)
		turnoffAllResult();

		if(isTopToothShow == 1)
			switchTopToothShow();
		if(isLowToothShow == 1)
			switchLowToothShow();

		if(isArticlePlaneShow == 0)
			switchArticlePlaneShow();
	}
	else if(selectOperationTreeIndex == 92 && operatorType == 2)
	{
		// 选点(开合->前牙开合->牙弓放射状相交平面->点)

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 79 && operatorType == 2)
	{
		// 选点(开合->后牙开合->牙弓放射状相交平面->点)
		turnoffFitPlaneResult();
		turnoffCoverResult();
	}
	else if(selectOperationTreeIndex == 82 && operatorType == 2)
	{
		// 选点(开合->后牙开合->牙弓放射状相交平面->轮廓线->点)
		
		turnoffFitPlaneResult();
		turnoffCoverResult();
	}
	else if(selectOperationTreeIndex == 85 && operatorType == 2)
	{
		// 选点(冠状面)

		//turnoffFitPlaneResult();
		//turnoffCoverResult();

		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 91 && operatorType == 2)
	{
		// 选点(开合->前牙开合->拟合曲线->点)
		turnoffAllResult();
	}
	else if(selectOperationTreeIndex == 89 && operatorType == 4)
	{
		// 拟合曲线(开合->前牙开合->拟合曲线)
		turnoffAllResult();
	}
}

void MainWindow::measureDistancePointToPoint()
{
	turnoffAllResult();

	this->pickStyle->mode = 2;

	measureDistancePointToPointIndex = 0;
	OperateItem It;
	//It.OpePickMode = this->pickStyle->mode;
	It.Opetype = 7;
	It.OpeNum = returnselectOptionTreeIndex();
	// 	It.x = currentSelectPosition[0];
	// 	It.y = currentSelectPosition[1];
	// 	It.z = currentSelectPosition[2];
	It.treeItem = currentOptionTreeWidgetItem;
	saveInput(It);
}

void MainWindow::setPickPointForMeasureDistancePointToPoint(double x, double y, double z)
{
	if(measureDistancePointToPointIndex < 2)
	{
		measureDistancePointToPointPositions[measureDistancePointToPointIndex][0] = x;
		measureDistancePointToPointPositions[measureDistancePointToPointIndex][1] = y;
		measureDistancePointToPointPositions[measureDistancePointToPointIndex][2] = z;

		double color[3] = {1, 1, 0};
		this->drawPoint(measureDistancePointToPointPositions[measureDistancePointToPointIndex], color, 
			measureDistancePointToPointPointActor[measureDistancePointToPointIndex]);

		ismeasureDistancePointToPointShow = 1;
	}

	++measureDistancePointToPointIndex;
	if(measureDistancePointToPointIndex == 2)
	{
		measureDistancePointToPointIndex = 0;
		this->pickStyle->mode = 0;

		double pointColor[3] = {1, 1, 0};
		double lineColor[3] = {0, 0, 1};
		this->drawLine(measureDistancePointToPointPositions[0], pointColor, measureDistancePointToPointPointActor[0],
			measureDistancePointToPointPositions[1], pointColor, measureDistancePointToPointPointActor[1],
			2, lineColor, measureDistancePointToPointLineActor);

		ismeasureDistancePointToPointShow = 1;

		double distance = sqrt(vtkMath::Distance2BetweenPoints(measureDistancePointToPointPositions[0], measureDistancePointToPointPositions[1]));	

		PointToPointDistanceDlg dlg(this);
		dlg.setDistance(distance);
		//dlg.show();
		dlg.exec();
	}
}

void MainWindow::switchMeasureDistanceLineToLine()
{
	if(ismeasureDistancePointToPointShow == 0)
	{
		for(int i = 0; i < 2; ++i)
		{
			this->ren->AddActor(measureDistancePointToPointPointActor[i]);
		}
		this->ren->AddActor(measureDistancePointToPointLineActor);

		ismeasureDistancePointToPointShow = 1;
	}
	else
	{
		for(int i = 0; i < 2; ++i)
		{
			this->ren->RemoveActor(measureDistancePointToPointPointActor[i]);
		}
		this->ren->RemoveActor(measureDistancePointToPointLineActor);

		ismeasureDistancePointToPointShow = 0;
	}

	this->ui.qvtkObjectViewerWidget->update();
}

void MainWindow::initDiFrontOpenClosePointItem(QTreeWidgetItem *item)
{
	removeChildItem(item);

	for(int i = 0; i < 20; ++i)
	{
		if(diFrontOpenClosePointStat[i] == true)
		{
			QString itemText = QString("(").
				append(QString::number(diFrontOpenClosePoint[i][0])).append(", ").
				append(QString::number(diFrontOpenClosePoint[i][1])).append(", ").
				append(QString::number(diFrontOpenClosePoint[i][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(item, QStringList(itemText));
			item->addChild(pointInfoItem);
			if(!item->isExpanded())
				item->setExpanded(true);
		}
	}
}

void MainWindow::initDiFrontOpenCloseMeasure(QTreeWidgetItem *parent, int currentMeasureIndex)//根据标记的点来初始化
{
	int exceptDatas[2] = {401, 402};
	//removeChildItemExcept(parent, exceptDatas, 2);

	for(int i = 0; i < 20; ++i)
	{
		if(diFrontOpenClosePointStat[i] == true && i == currentMeasureIndex)
		{
			QString itemText = QString(tr("Measure")).append(QString::number(i + 1));
			QTreeWidgetItem *measureItem = new QTreeWidgetItem(parent, QStringList(itemText));
			measureItem->setData(0, Qt::UserRole, 420 + i * 20);
			parent->addChild(measureItem);
			if(i == currentMeasureIndex)
			{
				if(!measureItem->isExpanded())
					measureItem->setExpanded(true);
			}

			itemText = tr("Point");
			QTreeWidgetItem *pointItem = new QTreeWidgetItem(measureItem, QStringList(itemText));
			pointItem->setData(0, Qt::UserRole, 421 + i * 20);
			measureItem->addChild(pointItem);
			itemText = tr("Intersect Plane O");
			QTreeWidgetItem *planeOItem = new QTreeWidgetItem(measureItem, QStringList(itemText));
			planeOItem->setData(0, Qt::UserRole, 422 + i * 20);
			measureItem->addChild(planeOItem);
			itemText = tr("Contour Curve");
			QTreeWidgetItem *contourCurveItem = new QTreeWidgetItem(measureItem, QStringList(itemText));
			contourCurveItem->setData(0, Qt::UserRole, 423 + i * 20);
			measureItem->addChild(contourCurveItem);
			itemText = tr("Score");
			QTreeWidgetItem *measureScoreItem = new QTreeWidgetItem(measureItem, QStringList(itemText));
			measureScoreItem->setData(0, Qt::UserRole, 424 + i * 20);
			measureItem->addChild(measureScoreItem);

			itemText = QString("(").
				append(QString::number(diFrontOpenClosePoint[i][0])).append(", ").
				append(QString::number(diFrontOpenClosePoint[i][1])).append(", ").
				append(QString::number(diFrontOpenClosePoint[i][2])).append(")");
			QTreeWidgetItem *pointInfoItem = new QTreeWidgetItem(pointItem, QStringList(itemText));
			pointItem->addChild(pointInfoItem);
			if(i == currentMeasureIndex)
			{
				if(!pointItem->isExpanded())
				{
					pointItem->setExpanded(true);
				}
			}

			if(i == currentMeasureIndex)
			{
				if(!planeOItem->isExpanded())
				{
					planeOItem->setExpanded(true);
				}
			}

			if(i == currentMeasureIndex)
			{
				if(!contourCurveItem->isExpanded())
				{
					contourCurveItem->setExpanded(true);
				}
			}

			itemText = tr("Point");
			QTreeWidgetItem *planeOPointItem = new QTreeWidgetItem(planeOItem, QStringList(itemText));
			planeOPointItem->setData(0, Qt::UserRole, 425 + i * 20);
			planeOItem->addChild(planeOPointItem);
			itemText = tr("Normal");
			QTreeWidgetItem *planeONormalItem = new QTreeWidgetItem(planeOItem, QStringList(itemText));
			planeONormalItem->setData(0, Qt::UserRole, 426 + i * 20);
			planeOItem->addChild(planeONormalItem);

			itemText = QString("(").
				append(QString::number(diFrontOpenClosePlaneOCenter[i][0])).append(", ").
				append(QString::number(diFrontOpenClosePlaneOCenter[i][1])).append(", ").
				append(QString::number(diFrontOpenClosePlaneOCenter[i][2])).append(")");
			QTreeWidgetItem *planePointInfoItem = new QTreeWidgetItem(planeOPointItem, QStringList(itemText));
			planeOPointItem->addChild(planePointInfoItem);
			if(i == currentMeasureIndex)
			{
				if(!planeOPointItem->isExpanded())
				{
					planeOPointItem->setExpanded(true);
				}
			}

			itemText = QString("(").
				append(QString::number(diFrontOpenClosePlaneONormal[i][0])).append(", ").
				append(QString::number(diFrontOpenClosePlaneONormal[i][1])).append(", ").
				append(QString::number(diFrontOpenClosePlaneONormal[i][2])).append(")");
			QTreeWidgetItem *planeNormalInfoItem = new QTreeWidgetItem(planeONormalItem, QStringList(itemText));
			planeONormalItem->addChild(planeNormalInfoItem);
			if(i == currentMeasureIndex)
			{
				if(!planeONormalItem->isExpanded())
				{
					planeONormalItem->setExpanded(true);
				}
			}

			itemText = tr("Point");
			QTreeWidgetItem *curvePointItem = new QTreeWidgetItem(contourCurveItem, QStringList(itemText));
			curvePointItem->setData(0, Qt::UserRole, 427 + i * 20);
			contourCurveItem->addChild(curvePointItem);
			itemText = tr("Distance");
			QTreeWidgetItem *curveDistanceItem = new QTreeWidgetItem(contourCurveItem, QStringList(itemText));
			curveDistanceItem->setData(0, Qt::UserRole, 428 + i * 20);
			contourCurveItem->addChild(curveDistanceItem);			
		}
	}
}

void MainWindow::initCloseRelationMeasureTreeWidget()
{
// 	QTreeWidgetItem *item = currentOptionTreeWidgetItem->parent();
// 	removeChildItemExcept(item, 1371);
	cout<<selectOptionTreeIndex<<endl;
	if (selectOptionTreeIndex == 1371)
	{
		QTreeWidgetItem *item = currentOptionTreeWidgetItem;

		QString itemText = tr("Measure");
		QTreeWidgetItem *diCloseRelationMeasureItem = new QTreeWidgetItem(item, QStringList(itemText));
		diCloseRelationMeasureItem->setData(0, Qt::UserRole, 1372);
		item->addChild(diCloseRelationMeasureItem);
		if(!diCloseRelationMeasureItem->isExpanded())
		{
			diCloseRelationMeasureItem->setExpanded(true);
		}

		itemText = tr("Point A");
		QTreeWidgetItem *diCloseRelationMeasurePointAItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasurePointAItem->setData(0, Qt::UserRole, 1373);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePointAItem);
		itemText = tr("Point B");
		QTreeWidgetItem *diCloseRelationMeasurePointBItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasurePointBItem->setData(0, Qt::UserRole, 1374);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePointBItem);
		itemText = tr("Intersect Plane O");
		QTreeWidgetItem *diCloseRelationMeasurePlaneItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasurePlaneItem->setData(0, Qt::UserRole, 1375);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePlaneItem);
		itemText = tr("Distance");
		QTreeWidgetItem *diCloseRelationMeasureDistanceItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasureDistanceItem->setData(0, Qt::UserRole, 1376);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasureDistanceItem);

		itemText = tr("Point");
		QTreeWidgetItem *diCloseRelationMeasurePlanePointItem = new QTreeWidgetItem(diCloseRelationMeasurePlaneItem, QStringList(itemText));
		diCloseRelationMeasurePlanePointItem->setData(0, Qt::UserRole, 1377);
		diCloseRelationMeasurePlaneItem->addChild(diCloseRelationMeasurePlanePointItem);
		itemText = tr("Normal");
		QTreeWidgetItem *diCloseRelationMeasurePlaneNormalItem = new QTreeWidgetItem(diCloseRelationMeasurePlaneItem, QStringList(itemText));
		diCloseRelationMeasurePlaneNormalItem->setData(0, Qt::UserRole, 1378);
		diCloseRelationMeasurePlaneItem->addChild(diCloseRelationMeasurePlaneNormalItem);
	}else if (selectOptionTreeIndex == 1381)
	{
		QTreeWidgetItem *item = currentOptionTreeWidgetItem;
		QString itemText = tr("Measure");
		QTreeWidgetItem *diCloseRelationMeasureItem = new QTreeWidgetItem(item, QStringList(itemText));
		diCloseRelationMeasureItem->setData(0, Qt::UserRole, 1382);
		item->addChild(diCloseRelationMeasureItem);
		if(!diCloseRelationMeasureItem->isExpanded())
		{
			diCloseRelationMeasureItem->setExpanded(true);
		}

		itemText = tr("Point A");
		QTreeWidgetItem *diCloseRelationMeasurePointAItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasurePointAItem->setData(0, Qt::UserRole, 1383);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePointAItem);
		itemText = tr("Point B");
		QTreeWidgetItem *diCloseRelationMeasurePointBItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasurePointBItem->setData(0, Qt::UserRole, 1384);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePointBItem);
		itemText = tr("Intersect Plane O");
		QTreeWidgetItem *diCloseRelationMeasurePlaneItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasurePlaneItem->setData(0, Qt::UserRole, 1385);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePlaneItem);
		itemText = tr("Distance");
		QTreeWidgetItem *diCloseRelationMeasureDistanceItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
		diCloseRelationMeasureDistanceItem->setData(0, Qt::UserRole, 1386);
		diCloseRelationMeasureItem->addChild(diCloseRelationMeasureDistanceItem);

		itemText = tr("Point");
		QTreeWidgetItem *diCloseRelationMeasurePlanePointItem = new QTreeWidgetItem(diCloseRelationMeasurePlaneItem, QStringList(itemText));
		diCloseRelationMeasurePlanePointItem->setData(0, Qt::UserRole, 1387);
		diCloseRelationMeasurePlaneItem->addChild(diCloseRelationMeasurePlanePointItem);
		itemText = tr("Normal");
		QTreeWidgetItem *diCloseRelationMeasurePlaneNormalItem = new QTreeWidgetItem(diCloseRelationMeasurePlaneItem, QStringList(itemText));
		diCloseRelationMeasurePlaneNormalItem->setData(0, Qt::UserRole, 1388);
		diCloseRelationMeasurePlaneItem->addChild(diCloseRelationMeasurePlaneNormalItem);
	}
	

	
}

void MainWindow::initCloseRelationMeasureTreeWidget2()
{
// 	QTreeWidgetItem *item = currentOptionTreeWidgetItem->parent();
// 	removeChildItemExcept(item, 1381);
	QTreeWidgetItem *item = currentOptionTreeWidgetItem;
	QString itemText = tr("Measure");
	QTreeWidgetItem *diCloseRelationMeasureItem = new QTreeWidgetItem(item, QStringList(itemText));
	diCloseRelationMeasureItem->setData(0, Qt::UserRole, 1382);
	item->addChild(diCloseRelationMeasureItem);
	if(!diCloseRelationMeasureItem->isExpanded())
	{
		diCloseRelationMeasureItem->setExpanded(true);
	}

	itemText = tr("Point A");
	QTreeWidgetItem *diCloseRelationMeasurePointAItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
	diCloseRelationMeasurePointAItem->setData(0, Qt::UserRole, 1383);
	diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePointAItem);
	itemText = tr("Point B");
	QTreeWidgetItem *diCloseRelationMeasurePointBItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
	diCloseRelationMeasurePointBItem->setData(0, Qt::UserRole, 1384);
	diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePointBItem);
	itemText = tr("Intersect Plane O");
	QTreeWidgetItem *diCloseRelationMeasurePlaneItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
	diCloseRelationMeasurePlaneItem->setData(0, Qt::UserRole, 1385);
	diCloseRelationMeasureItem->addChild(diCloseRelationMeasurePlaneItem);
	itemText = tr("Distance");
	QTreeWidgetItem *diCloseRelationMeasureDistanceItem = new QTreeWidgetItem(diCloseRelationMeasureItem, QStringList(itemText));
	diCloseRelationMeasureDistanceItem->setData(0, Qt::UserRole, 1386);
	diCloseRelationMeasureItem->addChild(diCloseRelationMeasureDistanceItem);

	itemText = tr("Point");
	QTreeWidgetItem *diCloseRelationMeasurePlanePointItem = new QTreeWidgetItem(diCloseRelationMeasurePlaneItem, QStringList(itemText));
	diCloseRelationMeasurePlanePointItem->setData(0, Qt::UserRole, 1387);
	diCloseRelationMeasurePlaneItem->addChild(diCloseRelationMeasurePlanePointItem);
	itemText = tr("Normal");
	QTreeWidgetItem *diCloseRelationMeasurePlaneNormalItem = new QTreeWidgetItem(diCloseRelationMeasurePlaneItem, QStringList(itemText));
	diCloseRelationMeasurePlaneNormalItem->setData(0, Qt::UserRole, 1388);
	diCloseRelationMeasurePlaneItem->addChild(diCloseRelationMeasurePlaneNormalItem);


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////2D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////2D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////2D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////2D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////2D
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////2D

void MainWindow::connectDatabase()
{
	cout<<"Connecting DataBase..."<<endl;
	QString driver="QMYSQL";
	QString host="localhost";
	QString dbname="MYDB";//"testdatabase";
	QString user="root";
	QString pw="123456";
	QSqlDatabase db = QSqlDatabase::addDatabase(driver);
	db.setDatabaseName(dbname);
	db.setUserName(user);
	db.setPassword(pw);
	if(!db.open())
	{
		QMessageBox::information(QApplication::activeWindow(),"Database Connect Information","Database Connect Falied !");
	}
	else
	{
		//QMessageBox::information(QApplication::activeWindow(),"Database Connect Information","Database Connected !");
		/*dbConnected = true;
		emit iniDisDia();*/
		cout<<"Connect Database Success!"<<endl;
	}
}

void MainWindow::createActions()
{
	//patientInfoAction = new QAction(tr("病例信息"),this);
	//connect(patientInfoAction,SIGNAL(triggered()),this,SLOT(patientInfoAction_triggered()));

	custemiseAction = new QAction(tr("自定义"),this);
	connect(custemiseAction,SIGNAL(triggered()),this,SLOT(custemiseAction_triggered()));

	samePPIAction = new QAction(tr("统一分辨率"),this);
	connect(samePPIAction,SIGNAL(triggered()),this,SLOT(samePPIAction_triggered()));

	discreteAction = new QAction(tr("定点离散程度"),this);
	connect(discreteAction,SIGNAL(triggered()),this,SLOT(discreteAction_triggered()));

	markPointAction = new QAction(tr("头颅侧位片定点"),this);
	connect(markPointAction,SIGNAL(triggered()),this,SLOT(markPointAction_triggered()));

	coincidentAction = new QAction(tr("重叠"),this);
	connect(coincidentAction,SIGNAL(triggered()),this,SLOT(coincidentAction_triggered()));

}

void MainWindow::deleteActions()
{
	delete custemiseAction;
	delete samePPIAction;
	delete discreteAction;
	delete markPointAction;
	delete coincidentAction;
}

void MainWindow::setupMenus()
{
	QMenu *toolMenu = menuBar()->addMenu(tr("2D Tool"));
	//toolMenu->addAction(patientInfoAction);
	toolMenu->addAction(custemiseAction);
	toolMenu->addAction(samePPIAction);
	toolMenu->addAction(discreteAction);
	toolMenu->addAction(markPointAction);
	toolMenu->addAction(coincidentAction);
}

void MainWindow::loadMesureProg()
{
	QFile file("./doc/type.txt");
	MProgType1Type = new QStringList;
	MProgType1Index = new QStringList;
	MProgType1Name = new QStringList;
	MProgType1Pt = new vector<QStringList>;
	MProgType1Res = new vector<double>;
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line =  in.readLine();
		QStringList t_list = line.split('\t',QString::SkipEmptyParts);
		MProgType1Type->append(t_list.at(0));
		MProgType1Index->append(t_list.at(1));
		MProgType1Name->append(t_list.at(2));
		QStringList t_pt_list;
		for (int i=3;i<t_list.size();i++)
		{
			t_pt_list.append(t_list.at(i));
		}
		MProgType1Pt->push_back(t_pt_list);
	}
}

myVector MainWindow::vectorMinus(myVector pt1, myVector pt2)		// pt2-pt1
{
	myVector res;
	res.x = pt2.x - pt1.x;
	res.y = pt2.y - pt1.y;
	return res;
}

myVector MainWindow::vectorPlus(myVector pt1, myVector pt2)
{
	myVector res;
	res.x = pt2.x + pt1.x;
	res.y = pt2.y + pt1.y;
	return res;
}

double MainWindow::vectorMul(myVector pt1, myVector pt2)
{
	double res = pt1.x * pt2.x + pt1.y*pt2.y;
	return res;
}

double MainWindow::vectorNorm(myVector pt1)
{
	double res = pt1.x*pt1.x + pt1.y*pt1.y;
	res = sqrt(res);
	return res;
}

double MainWindow::computeType1(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	myVector v1 = vectorMinus(pt2 , pt1);
	myVector v2 = vectorMinus(pt4 , pt3);
	double res =vectorMul(v1,v2)/(vectorNorm(v1)*vectorNorm(v2));
	res = acos(res);
	double Pi=3.141592653;
	res=res*180/Pi;
	return res;
}

double MainWindow::computeType21(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	myVector v1 = vectorMinus(pt1,pt4);
	myVector v2 = vectorMinus(pt2,pt3);
	res = fabs( vectorMul(v1,v2) / vectorNorm(v2) );
	return res;
}

double MainWindow::computeType221(myVector pt1,myVector pt2,myVector pt3)
{
	double res;
	
	myVector v1 = vectorMinus(pt3,pt1);
	myVector v2 = vectorMinus(pt3,pt2);

	double len1 = vectorMul(v1,v2) / vectorNorm(v2);
	double len2 = vectorNorm(v1);

	res = sqrt( len2*len2  - len1*len1 );
	
	//define + or -
	myVector midV = vectorPlus(pt2,pt3);
	midV.x = midV.x/2;
	midV.y = midV.y/2;

	myVector dirV = vectorMinus(midV,pt1);
	myVector XZheng;
	XZheng.x = 1;
	XZheng.y = 0;

	double angle = vectorMul(dirV, XZheng);
	
	res = (angle<0)? -res:res;

	/*myVector v3 = vectorMinus( pt2 ,pt1);
	if (vectorNorm(v3) < vectorNorm(v1) )
	{
		res = -1*res;
	}*/
	return res;
}
double MainWindow::computeType222(myVector pt1,myVector pt2,myVector pt3)
{
	double res;

	myVector v1 = vectorMinus(pt3,pt1);
	myVector v2 = vectorMinus(pt3,pt2);

	double len1 = vectorMul(v1,v2) / vectorNorm(v2);
	double len2 = vectorNorm(v1);

	res = sqrt( len2*len2  - len1*len1 );

	return res;
}
double MainWindow::computeType31(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	myVector v1 = vectorMinus(pt2,pt1);
	myVector v2 = vectorMinus(pt4,pt3);
	res = fabs( vectorMul(v1,v2) / vectorNorm(v2) );
	return res;
}
double MainWindow::computeType32(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	myVector v1 = vectorMinus(pt2,pt1);
	myVector v2 = vectorMinus(pt3,pt4);
	res = fabs( vectorMul(v1,v2) / vectorNorm(v2) );
	return res;
}
double MainWindow::computeType33(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	myVector v1 = vectorMinus(pt1,pt2);
	myVector v2 = vectorMinus(pt3,pt4);
	myVector v3;
	v3.x=1;
	v3.y=0;
	double cosq = vectorMul(v2,v3)/(vectorNorm(v2));
	double th = acos(cosq);
	double Pi=3.141592653;
	th+=7/180 *Pi;
	myVector v4;
	v4.x = cos(th);
	v4.y = cos(th);
	
	res = fabs( vectorMul(v1,v4) / vectorNorm(v4) );

	return res;
}
double MainWindow::computeType34(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	
	myVector v1 = vectorMinus(pt1,pt2);
	myVector v2 = vectorMinus(pt3,pt4);
	myVector v3;
	v3.x = 1;
	v3.y = -1* v2.x/v2.y;

	res = fabs( vectorMul(v1,v3) / vectorNorm(v3) );
	return res;
}
double MainWindow::computeType35(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{	
	double res;
	myVector v1 = vectorMinus(pt1,pt2);
	myVector v2 = vectorMinus(pt3,pt4);
	myVector v3;
	v3.x=1;
	v3.y=0;
	double cosq = vectorMul(v2,v3)/(vectorNorm(v2));
	double th = acos(cosq);
	double Pi=3.141592653;
	th+=7/180 *Pi;
	myVector v4;
	v4.x = cos(th);
	v4.y = cos(th);

	double len1 = fabs( vectorMul(v1,v4) / vectorNorm(v4) );
	double len2 = vectorNorm(v1);
	res = sqrt(len2*len2 - len1*len1);

	return res;
}
double MainWindow::computeType36(myVector pt1,myVector pt2,myVector pt3, myVector pt4,myVector pt5,myVector pt6, myVector pt7)
{
	double res;
	
	double k1,k2,b1,b2;
	k1 = (pt3.y-pt2.y) / (pt3.x - pt2.x);
	b1 = pt2.y - pt2.x * k1;

	k2 = (pt4.y-pt5.y) / (pt4.x - pt5.x);
	b2 = pt4.y - pt4.x * k2;

	myVector pp;
	pp.x = (b2-b1)/(k1-k2);
	pp.y = (b2*k1-b1*k2)/(k1-k2);

	myVector v1 = vectorMinus(pp,pt1);
	myVector v2 = vectorMinus(pt5,pt6);

	double len1 = fabs( vectorMul(v1,v2) / vectorNorm(v2) );
	double len2 = vectorNorm(v1);
	res = sqrt(len2*len2 - len1*len1);
	return res;
}
double MainWindow::computeType37(myVector pt1,myVector pt2,myVector pt3, myVector pt4,myVector pt5,myVector pt6, myVector pt7)
{
	double res;
	
	double k1,k2,b1,b2;
	k1 = (pt1.y-pt2.y) / (pt1.x - pt2.x);
	b1 = pt2.y - pt2.x * k1;

	k2 = (pt4.y-pt3.y) / (pt4.x - pt3.x);
	b2 = pt4.y - pt4.x * k2;

	myVector pp;
	pp.x = (b2-b1)/(k1-k2);
	pp.y = (b2*k1-b1*k2)/(k1-k2);

	myVector v1 = vectorMinus(pt5,pp);
	myVector v2 = vectorMinus(pt3,pp);
	double len1 = fabs( vectorMul(v1,v2) / vectorNorm(v2) );
	double len2 = vectorNorm(v1);
	res = sqrt(len2*len2 - len1*len1);
	return res;
}
double MainWindow::computeType371(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	
	myVector v1 = vectorMinus(pt1,pt2);
	myVector v2 = vectorMinus(pt1,pt3);
	double len1 = fabs( vectorMul(v1,v2) / vectorNorm(v2) );
	double len2 = vectorNorm(v1);
	res = sqrt(len2*len2 - len1*len1);
	return res;
}
double MainWindow::computeType41(myVector pt1,myVector pt2)
{
	double res;
	myVector v = vectorMinus(pt2,pt1);
	res = vectorNorm(v);
	return res;
}
double MainWindow::computeType42(myVector pt1,myVector pt2,myVector pt3, myVector pt4, myVector pt5)
{	
	double res;
	double k1,k2,b1,b2;
	k1 = (pt3.y-pt2.y) / (pt3.x - pt2.x);
	b1 = pt2.y - pt2.x * k1;

	k2 = (pt4.y-pt5.y) / (pt4.x - pt5.x);
	b2 = pt4.y - pt4.x * k2;

	myVector pp;
	pp.x = (b2-b1)/(k1-k2);
	pp.y = (b2*k1-b1*k2)/(k1-k2);

	myVector v = vectorMinus(pt1,pp);
	res = vectorNorm(v);

	return res;
}

double MainWindow::computeType43(myVector pt1,myVector pt2,myVector pt3, myVector pt4, myVector pt5,myVector pt6, myVector pt7,myVector pt8)
{
	double res;
	double k1,k2,b1,b2;
	k1 = (pt1.y-pt2.y) / (pt1.x - pt2.x);
	b1 = pt2.y - pt2.x * k1;

	k2 = (pt4.y-pt3.y) / (pt4.x - pt3.x);
	b2 = pt4.y - pt4.x * k2;

	myVector pp1;
	pp1.x = (b2-b1)/(k1-k2);
	pp1.y = (b2*k1-b1*k2)/(k1-k2);

	double k3,k4,b3,b4;
	k3 = (pt5.y-pt6.y) / (pt5.x - pt6.x);
	b3 = pt5.y - pt5.x * k1;

	k4 = (pt7.y-pt8.y) / (pt7.x - pt8.x);
	b4 = pt8.y - pt7.x * k2;

	myVector pp2;
	pp2.x = (b4-b3)/(k3-k4);
	pp2.y = (b4*k3-b3*k4)/(k3-k4);

	myVector v = vectorMinus(pp1,pp2);
	res = vectorNorm(v);
	return res;
}

double MainWindow::computeType51(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	myVector v2 = vectorMinus(pt3,pt4);
	myVector v3;
	v3.x=1;
	v3.y=0;
	double cosq = vectorMul(v2,v3)/(vectorNorm(v2));
	double th = acos(cosq);
	double Pi=3.141592653;
	th+=7/180 *Pi;
	myVector v4;
	v4.x = cos(th);
	v4.y = cos(th);

	myVector v5 = vectorMinus(pt1,pt3);
	myVector v6 = vectorMinus(pt2,pt3);

	double len1 = fabs( vectorMul(v5,v4) / vectorNorm(v4) );
	double len2 = vectorNorm(v5);
	double res1 = sqrt(len2*len2 - len1*len1);

	 len1 = fabs( vectorMul(v6,v4) / vectorNorm(v4) );
	 len2 = vectorNorm(v6);

	 double res2 = sqrt(len2*len2 - len1*len1);

	return res1+res2;

}

double MainWindow::computeType52(myVector pt1,myVector pt2,myVector pt3, myVector pt4)
{
	double res;
	myVector v2 = vectorMinus(pt3,pt4);
	myVector v3;
	v3.x=1;
	v3.y=0;
	double cosq = vectorMul(v2,v3)/(vectorNorm(v2));
	double th = acos(cosq);
	double Pi=3.141592653;
	th+=7/180 *Pi;
	myVector v4;
	v4.x = cos(th);
	v4.y = cos(th);

	myVector v5 = vectorMinus(pt1,pt3);
	myVector v6 = vectorMinus(pt2,pt3);

	double len1 = fabs( vectorMul(v5,v4) / vectorNorm(v4) );
	double len2 = vectorNorm(v5);
	double res1 = sqrt(len2*len2 - len1*len1);

	len1 = fabs( vectorMul(v6,v4) / vectorNorm(v4) );
	len2 = vectorNorm(v6);

	double res2 = sqrt(len2*len2 - len1*len1);
	return fabs(res1-res2);
}


double MainWindow::computeCurrentRes(vector<myVector> *mesPt, int i)
{
	double res = 0;
	if(MProgType1Type->at(i)=="1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType1( pt1, pt2, pt3,  pt4);
		/*resLabel->setText(MProgType1Name->at(i)+" = " + QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="2.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType21( pt1, pt2, pt3,  pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="2.2.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		double t_res = computeType221( pt1, pt2, pt3);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="2.2.2")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		double t_res = computeType222( pt1, pt2, pt3);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType31( pt1, pt2, pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.2")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType32(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.3")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType33(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.4")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType34(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.5")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType35(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.6")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		myVector pt5 = mesPt->at(4);
		myVector pt6 = mesPt->at(5);
		myVector pt7 = mesPt->at(6);
		double t_res = computeType36(pt1,pt2,pt3,pt4,pt5,pt6,pt7);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.7")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		myVector pt5 = mesPt->at(4);
		myVector pt6 = mesPt->at(5);
		myVector pt7 = mesPt->at(6);
		double t_res = computeType37(pt1,pt2,pt3,pt4,pt5,pt6,pt7);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="3.7.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType371(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="4.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		double t_res = computeType41(pt1,pt2);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="4.2")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		myVector pt5 = mesPt->at(4);
		double t_res = computeType42(pt1,pt2,pt3,pt4,pt5);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="4.3")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		myVector pt5 = mesPt->at(4);
		myVector pt6 = mesPt->at(5);
		myVector pt7 = mesPt->at(6);
		myVector pt8 = mesPt->at(7);
		double t_res = computeType43(pt1,pt2,pt3,pt4,pt5,pt6,pt7,pt8);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="5.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType51(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="5.2")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType52(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="6.1")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		myVector pt5 = mesPt->at(4);
		myVector mid4_5;
		mid4_5.x = ( pt4.x+pt5.x)/2;
		mid4_5.y = ( pt4.y+pt5.y)/2;
		double t_res = computeType1(pt1,pt2,pt3,mid4_5);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="6.2")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		myVector pt5 = mesPt->at(4);
		myVector pt6 = mesPt->at(5);
		myVector mid3_4;
		mid3_4.x = ( pt3.x+pt4.x)/2;
		mid3_4.y = ( pt3.y+pt4.y)/2;
		myVector mid5_6;
		mid5_6.x = ( pt5.x+pt6.x)/2;
		mid5_6.y = ( pt5.y+pt6.y)/2;
		double t_res = computeType1(pt1,pt2,mid3_4,mid5_6);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	else if (MProgType1Type->at(i)=="6.3")
	{
		myVector pt1 = mesPt->at(0);
		myVector pt2 = mesPt->at(1);
		myVector pt3 = mesPt->at(2);
		myVector pt4 = mesPt->at(3);
		double t_res = computeType1(pt1,pt2,pt3,pt4);
		/*resLabel->setText(QString::number(t_res));
		resLabel->setVisible(true);*/
		res = t_res;
	}
	return res;
}

void MainWindow::iniDialogs()
{
	customiseDialog = new MyCustoMiseC();

	markPointDialog = new MyMarkPointC();
	connect(this, SIGNAL(ImageFileSelected(QString *,int)),markPointDialog ,SLOT(getImage(QString*)));
	connect(markPointDialog->getSpt(),SIGNAL(stateChanged ( int )),this,SLOT(setSelectPtEnable(int)));
	connect(pwidget[0],SIGNAL(scaleSet(double)),markPointDialog,SLOT(getScale(double)));
	connect(markPointDialog,SIGNAL(showMarkPt(double, double)),pwidget[0],SLOT(showSpt(double,double)));
	connect(markPointDialog,SIGNAL(showAllPtSignal(vector<double> *)),pwidget[0],SLOT(showAllPt2(vector<double> *)));
	connect(markPointDialog,SIGNAL(fileSelected(QString*, int ,double)),pwidget[0],SLOT(setImage2(QString*,int,double)));
	connect(markPointDialog,SIGNAL(imageS(QImage)),pwidget[0],SLOT(setImage3(QImage )));
	connect(markPointDialog,SIGNAL(imageS(QImage)),this,SLOT(setVisGrid()));
	connect(markPointDialog,SIGNAL(showRuler(vector<double> *,bool)),pwidget[0],SLOT(showSampleRuler(vector<double> *, bool)));
	connect(markPointDialog,SIGNAL(rulerSelected(double ,int )),pwidget[0],SLOT(setRuler(double,int)));
	connect(markPointDialog,SIGNAL(getShowPointSignal(vector<double> *, int)),pwidget[0],SLOT(getShowPointSlot(vector<double> *, int)));
	connect(markPointDialog,SIGNAL(setHVRulerSignal(double,double,int)),pwidget[0],SLOT(setHVRuler(double ,double ,int )));
	connect(markPointDialog,SIGNAL(getNewMarkRulerCordSignal(double ,double ,double , double )),pwidget[0],SLOT(getRulerPt(double ,double ,double ,double )));
	connect(markPointDialog,SIGNAL(clearAllPtSignal()),pwidget[0],SLOT(clearAllPt()));

	adjustPPIDialog = new myAdjustPPIDialogC();

	patientDialog = new MyPatientInfoDialog(this->ui.tab);

	discreteDialog = new MyDiscreteC();
	connect(discreteDialog,SIGNAL(setExportMesInfo(QStringList,QStringList,QString)),this,SLOT(getExportMesInfoCompute(QStringList,QStringList,QString)));

	digitImageCoin = new Dialog_DigitImageCoin();
	connect(digitImageCoin,SIGNAL(setImageSignal(QString, int)),this,SLOT(getCoincideImageSlot(QString, int)));
	connect(digitImageCoin,SIGNAL(showCoinRes()),this,SLOT(loadtransDigitImageTriggered()));
	connect(digitImageCoin,SIGNAL(showTracingCoinRes(QTransform )),this,SLOT(loadtransDigitImageTracingTriggered(QTransform )));
	connect(digitImageCoin,SIGNAL(adjustCoinImage()),this,SLOT(adjustDitigImageCoinTriggered()));
	connect(digitImageCoin->rotateCenter,SIGNAL(stateChanged(int )),this,SLOT(selectRotateCenterTriggered(int )));
	connect(digitImageCoin,SIGNAL(setMyLineSignal(double,double,double,double,double,double,double,double)),this,SLOT(getLine(double ,double ,double , double ,double,double,double,double)));
	connect(digitImageCoin,SIGNAL(setPtMoveSignal(double, double, double, double)),this,SLOT(getPtMoveSlot(double, double, double, double)));
	connect(digitImageCoin,SIGNAL(setPtNoTSignal(double, double, double, double)),this,SLOT(getPtMoveSlot2(double, double, double, double)));
	connect(digitImageCoin,SIGNAL(setAngleCord(double,double,double,double,double,double,double,double)),this,SLOT(getAngleSlot(double,double,double,double,double,double,double,double)));
	connect(digitImageCoin,SIGNAL(setTransForm()),this,SLOT(setDigitImageTransSlot()));
	connect(digitImageCoin,SIGNAL(setLoadTransForm()),this,SLOT(setLoadTransSlot()));
	connect(digitImageCoin->clearYuanTuButton,SIGNAL(clicked()),this,SLOT(clearDigitCoinYuantuXianshiTriggered()));
	connect(digitImageCoin->sourceImageTracingCoiCheckBox,SIGNAL(clicked ( bool  ) ),this,SLOT(setSourceTracingCoinSelectPtSlot()));
	connect(digitImageCoin->tracingImageCoinCheckBox,SIGNAL(clicked(bool)),this,SLOT(setTracingCoinSelectPtSlot()));
	connect(digitImageCoin->coinMethodBox2,SIGNAL(currentIndexChanged ( int)),this,SLOT(clearTracingCoinSelectPtSlot()));
	connect(digitImageCoin,SIGNAL(testCoin(QString, QString, QTransform)),this,SLOT(testCoinSLot(QString, QString, QTransform)));

	for (int i=0;i<2;i++)
	{
		connect(pwidget[i],SIGNAL(setSourceTracingPtSignal(vector<double> *,int)),digitImageCoin,SLOT(getSourceTracingPtSLot(vector<double> *,int)));
		connect(pwidget[i],SIGNAL(setTracingCoinPtSignal(vector<double> *,int)),digitImageCoin,SLOT(getTracingPtSLot(vector<double> *,int)));
	}
}

void MainWindow::iniCoinImages()
{
	//imSwidget2 = new ImageShowWidget(this);
	//imSwidget1 = new ImageShowWidget(imSwidget2);
	//tempImageShowWidget2 = new ImageShowWidget(this);
	//tempImageShowWidget1 = new ImageShowWidget(tempImageShowWidget2);
}

void MainWindow::createGridGroupBox()
{
	groupBox2D = new QGroupBox;
	groupBox3D = new QGroupBox;
	setupTabWidgets2D();
	setupTabWidgets3D();
}

void MainWindow::setupTabWidgets2D()
{
	QGridLayout *frameLayout = new QGridLayout;
	for (int i=0;i<2;i++)
	{
		pwidget[i] = new PWidget();
		pwidget[i]->selfIndex = i;
		pwidget[i]->setGeometry (50,40,700,800);
		connect(this, SIGNAL(ImageFileSelected(QString *,int)), pwidget[i],SLOT(setImage(QString*,int)));
		connect(pwidget[i],SIGNAL(PtSelected(double,double)),this,SLOT(getMPtCord2d(double,double)));
		connect(this,SIGNAL(selectPt(bool)),pwidget[i],SLOT(selectPtEn(bool)));
		/*connect(clearAll2D,SIGNAL(clicked()),pwidget[i],SLOT(clearAll()));
		connect(clearLastPt2D,SIGNAL(clicked()),pwidget[i],SLOT(clearLast()));*/
		connect(this,SIGNAL(showSinglePt2d(double,double)),pwidget[i],SLOT(showSpt(double,double)));
		connect(this,SIGNAL(showAllPt2d(vector<double> *, vector<QString> *)),pwidget[i],SLOT(showAllPt(vector<double> *, vector<QString> *)));
		connect(pwidget[i],SIGNAL(setFullScreen(int,bool )),this,SLOT(setFullScreenSlot(int ,bool)));
		/*connect(PointSizeSlider,SIGNAL(valueChanged(int)),pwidget[i],SLOT(changePtSize(int)));
		connect(incbright,SIGNAL(clicked()),pwidget[i],SLOT(addbright()));
		connect(decbright,SIGNAL(clicked()),pwidget[i],SLOT(decBright()));
		connect(restore,SIGNAL(clicked()),pwidget[i],SLOT(restoreImage()));
		connect(histEqualise,SIGNAL(clicked()),pwidget[i],SLOT(imageHistEqualise()));
		connect(sharpen,SIGNAL(clicked()),pwidget[i],SLOT(imageSharpen()));
		connect(smooth,SIGNAL(clicked()),pwidget[i],SLOT(imagesmooth()));
		connect(sobelEdge,SIGNAL(clicked()),pwidget[i],SLOT(imagesobelEdge()));
		connect(inverse,SIGNAL(clicked()),pwidget[i],SLOT(imageinverse()));*/

		frameLayout->addWidget(pwidget[i],0,i);
	}
	pwidget[1]->setVisible(false);
	groupBox2D->setLayout(frameLayout);
}

void MainWindow::setupTabWidgets3D()
{

}

void MainWindow::Mysetlayout()
{
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(groupBox2D);
	mainLayout->addWidget(groupBox3D);

	this->ui.tab_2->setLayout(mainLayout);
}

void MainWindow::custemiseAction_triggered()
{

	if (!customiseDialog->initialised())
	{
		customiseDialog->iniData();
		customiseDialog->setProjItemList(MProgType1Name);
		customiseDialog->setPtItemList();
	}
	customiseDialog->show();
}

void MainWindow::samePPIAction_triggered()
{
	if (!adjustPPIDialog->isInited())
	{
		adjustPPIDialog->iniSampleIndex();
	}
	adjustPPIDialog->show();
}

void MainWindow::discreteAction_triggered()
{
	discreteDialog->show();
}

void MainWindow::patientInfoAction_triggered()
{
	patientDialog->show();
}

void MainWindow::markPointAction_triggered()
{
	//markPointDialog = new MyMarkPointC();
	connect(markPointDialog->getSpt(),SIGNAL(stateChanged ( int )),this,SLOT(setSelectPtEnable(int)));
	connect(pwidget[0],SIGNAL(scaleSet(double)),markPointDialog,SLOT(getScale(double)));
	connect(markPointDialog,SIGNAL(showMarkPt(double, double)),pwidget[0],SLOT(showSpt(double,double)));
	connect(markPointDialog,SIGNAL(showAllPtSignal(vector<double> *)),pwidget[0],SLOT(showAllPt2(vector<double> *)));
	//connect(this,SIGNAL(iniDisDia()),markPointDialog,SLOT(iniFileNameWidget()));
	connect(markPointDialog,SIGNAL(fileSelected(QString*, int ,double)),pwidget[0],SLOT(setImage2(QString*,int,double)));
	connect(markPointDialog,SIGNAL(imageS(QImage)),pwidget[0],SLOT(setImage3(QImage )));
	connect(markPointDialog,SIGNAL(imageS(QImage)),this,SLOT(setVisGrid()));
	connect(markPointDialog,SIGNAL(showRuler(vector<double> *,bool)),pwidget[0],SLOT(showSampleRuler(vector<double> *, bool)));
	connect(markPointDialog,SIGNAL(rulerSelected(double ,int )),pwidget[0],SLOT(setRuler(double,int)));
	connect(markPointDialog,SIGNAL(getShowPointSignal(vector<double> *, int)),pwidget[0],SLOT(getShowPointSlot(vector<double> *, int)));
	connect(markPointDialog,SIGNAL(setHVRulerSignal(double,double,int)),pwidget[0],SLOT(setHVRuler(double ,double ,int )));
	connect(markPointDialog,SIGNAL(getNewMarkRulerCordSignal(double ,double ,double , double )),pwidget[0],SLOT(getRulerPt(double ,double ,double ,double )));
	connect(markPointDialog,SIGNAL(clearAllPtSignal()),pwidget[0],SLOT(clearAllPt()));
	markPointDialog->show();
}

void MainWindow::coincidentAction_triggered()
{
	if (!digitImageCoin->isInit())
	{
		digitImageCoin->iniData();
	}

	digitImageCoin->setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint);
	digitImageCoin->show();
}

void MainWindow::getMPtCord2d(double x,double y)
{
	QString mrid;
	QString ptM;
	QTableWidget *tempTV;
	int t_c;
	if (markPointDialog->getSpt()->isChecked())
	{
		if (!markPointDialog->mesListWidgetSelected())
		{
			return;
		}
		tempTV = markPointDialog->getPointTable();
		t_c=1;
		int t_row = tempTV->currentRow();
		//cout<<t_row<<endl;
		if (t_c==1)
		{
			QTableWidgetItem* item1 = new QTableWidgetItem;
			QTableWidgetItem* item2 = new QTableWidgetItem;
			tempTV->setItem (t_row,t_c,item1);
			QString cordx = QString::number(x);
			item1->setText (cordx);
			tempTV->setItem (t_row,t_c+1,item2);
			QString cordy = QString::number(y);
			item2->setText (cordy);

			if (t_row==tempTV->rowCount()-1)
			{
				QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("标记点已全部选取"));
				tempTV->setCurrentCell(0,0);
			}
			else
				tempTV->setCurrentCell(t_row+1,0);
			QTableWidgetItem* item4 = new QTableWidgetItem;
			item4=tempTV->item(t_row,t_c-1);
			QString PtN=item4->text ();
			QString updateInTime;

			if (!markPointDialog->getRulerShowChecked())
			{
				QString sampleIndex = markPointDialog->getSampleIndexBoxText();
				//cerr<<sampleIndex.toStdString()<<endl;
				QString getId = "select disptvalue.iddisPtValue from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+markPointDialog->currentImage+"' and mname='"+markPointDialog->currentMName+"' and mindex='"+markPointDialog->currentMindex+"' and pointnameeng='"+PtN+"'";
				QSqlQuery query;
				query.exec(getId);
				query.next();
				QString tempId = query.value(0).toString();
				updateInTime = "update disptvalue set X='"+cordx+"', Y='"+cordy+"' where  iddisPtValue="+tempId;
				//updateInTime = "update discrete set X='"+cordx+"', Y='"+cordy+"' where imagename='"+disDialog->currentImage+"' and mname='"+disDialog->currentMName+"' and mindex='"+disDialog->currentMindex+"' and pointnameeng='"+PtN+"'";
				query.exec(updateInTime);

				QString getAver = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+markPointDialog->currentImage+"' and mname!='aver' and pointnameeng='"+PtN+"'";
				query.exec(getAver);
				double tx = 0;
				double ty = 0;
				int isNu = 0;
				query.next();
				for (int ti=0;ti<query.size();ti++)
				{
					double xx = query.value(0).toDouble();
					double yy = query.value(1).toDouble();
					if (xx==0 && yy==0)
					{
						isNu++;
					}
					else
					{
						tx +=xx;
						ty +=yy;
					}
					query.next();
				}
				tx = tx/(query.size()-isNu);
				ty = ty/(query.size()-isNu);
				//cout<<"aver "<<tx<<" "<<ty<<endl;
				QString averX = QString::number(tx);
				QString averY = QString::number(ty);
				getId = "select disptvalue.iddisptvalue from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+markPointDialog->currentImage+"' and mname='aver' and mindex='' and pointnameeng='"+PtN+"'";
				query.exec(getId);
				query.next();
				tempId = query.value(0).toString();
				//cerr<<"idvalye = "<<tempId.toStdString()<<endl;
				QString updateAver = "update disptvalue set X='"+averX+"', Y='"+averY+"' where iddisptvalue="+tempId;
				//cerr<<updateAver.toStdString()<<endl;
				query.exec(updateAver);
			}
			else
			{
				updateInTime = "update ruler set X='"+cordx+"', Y='"+cordy+"' where imagename='"+markPointDialog->currentImage+"' and mname='"+markPointDialog->currentMName+"' and mindex='"+markPointDialog->currentMindex+"' and rulerpoint='"+PtN+"'";
				QSqlQuery query;
				query.exec(updateInTime);
			}	
		}
		return;
	}
	/*else if (dbTab->currentIndex()==2)
	{
		tempTV = dbTableView;
		t_c=tableType;
		mrid = patientBox->currentText();
	}
	else if(dbTab->currentIndex()==3)
	{
		tempTV = MesTableView;
		t_c=1;
		mrid = patientIdBox4->currentText();
		QListWidgetItem *it = pointSBox->currentItem();
		ptM = it->text();
	}*/

	int t_row = tempTV->currentRow();
	if (t_row==-1)
	{
		tempTV->setCurrentCell(0,0);
	}
	t_row = tempTV->currentRow();;
	if (t_c==1)
	{
		QTableWidgetItem* item1 = new QTableWidgetItem;
		QTableWidgetItem* item2 = new QTableWidgetItem;
		tempTV->setItem (t_row,t_c,item1);
		QString cordx = QString::number(x);
		item1->setText (cordx);
		tempTV->setItem (t_row,t_c+1,item2);
		QString cordy = QString::number(y);
		item2->setText (cordy);

		if (t_row==tempTV->rowCount()-1)
		{
			QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("标记点已全部选取"));
			tempTV->setCurrentCell(0,0);
		}
		else
			tempTV->setCurrentCell(t_row+1,0);

		QTableWidgetItem* item4 = new QTableWidgetItem;
		item4=tempTV->item(t_row,t_c-1);
		QString PtN=item4->text ();
		QString updateInTime;
		updateInTime = "update Points set X='"+cordx+"' , Y='"+cordy+"' where mrid ='"+mrid+"' and PointNameEng='"+PtN+"' and mark='"+ptM+"'";
		QSqlQuery query;
		query.exec(updateInTime);


		/*QString temp = "select X, Y from points where mrid ='"+mrid+"' and PointNameEng='"+PtN+"' and mark='"+ptM+"'";
		query.exec(temp);
		query.next();
		cout<<"updated = "<<query.value(0).toDouble()<<" "<<query.value(1).toDouble()<<endl;*/
		QString getAver = "select X, Y from points where mrid ='"+mrid+"' and PointNameEng='"+PtN+"' and mark!='aver'";
		query.exec(getAver);
		query.next();
		double tx = 0;
		double ty = 0;
		int isNu = 0;
		for (int ti=0;ti<query.size();ti++)
		{
			double xx = query.value(0).toDouble();
			double yy = query.value(1).toDouble();
			if (xx==0 && yy==0)
			{
				isNu++;
			}
			else
			{
				tx +=xx;
				ty +=yy;
			}
			query.next();
		}
		tx = tx/(query.size()-isNu);
		ty = ty/(query.size()-isNu);
		QString averX = QString::number(tx);
		QString averY = QString::number(ty);
		QString updateAver = "update Points set X='"+averX+"' , Y='"+averY+"' where mrid ='"+mrid+"' and PointNameEng='"+PtN+"' and mark='aver'";
		query.exec(updateAver);
	}
	pwidget[0]->update();
	pwidget[1]->update();
}

void MainWindow::setFullScreenSlot(int tIndex,bool isF)
{
	int fIndex = 1-tIndex;
	pwidget[fIndex]->setVisible(!isF);
}

void MainWindow::setSelectPtEnable(int t_state)
{
	if (t_state==2)	// QCheckBox checked
	{
		pwidget[0]->setBoolenStatus(0,true);	// 选取标志点 true
		//pwidget[0]->drawLine=false;
		//emit checkBoxC();
	}
	else if (t_state==0)	// QCheckBox unchecked
	{
		pwidget[0]->setBoolenStatus(0,false);	// 选取标志点 true
		//selectPtEnable2->setChecked(false);
		//pwidget[0]->drawLine=false;
	}
}

void MainWindow::setVisGrid()
{
	groupBox2D->setVisible(true);
	groupBox3D->setVisible(false);
}

void MainWindow::getExportMesInfoCompute(QStringList selectList, QStringList mesList,QString saveFileName)
{
	QStringList outList;
	QString header = tr("测量文件库") + "\t" + tr("图片名") + "\t" + tr("测量项目")  + "\t" + tr("测量者")  + "\t" + tr("测量编号")+"\t" + tr("结果") +"\r\n";
	outList.append(header);

	QSqlQuery query;


	for (int j=0;j<mesList.size();j++)
	{
		QString str = mesList.at(j);
		int tind = MProgType1Name->indexOf(str);
		QStringList tPtList;
		tPtList = MProgType1Pt->at(tind);

		QString preImage = "";


		for (int i=0;i<selectList.size();i++)
		{
			QString line = selectList.at(i);
			QStringList sl_list = line.split('\t',QString::SkipEmptyParts);
			QString sampleIndex = sl_list.at(0);
			QString imagename = sl_list.at(1);
			QString mname = sl_list.at(2);
			QString mindex = sl_list.at(3);

			vector<myVector> *mesPt = new vector<myVector>;

			if (preImage!=imagename)
			{
				mesPt->clear();
				for (int m = 0;m<tPtList.size();m++)
				{
					QString ptn = tPtList.at(m);
					if (ptn != "Mid3")
					{
						if (ptn=="O point")
						{
							QString pointname="";
							QString r15="R1.5";
							QString r30="R3";
							QString r45="R4.5";
							QString list[]={"a","b","c","d","e","f","g","h","i","j","k","l"};
							for (int i=0;i<12;i++)
							{
								QString tempPt = r15+list[i];
								if (i==0)
								{
									pointname +="  pointnameeng='"+tempPt+"' ";
								}
								else
								{
									pointname +=" or pointnameeng='"+tempPt+"' ";
								}

								tempPt = r30+list[i];
								pointname +=" or pointnameeng='"+tempPt+"' ";
								tempPt = r45+list[i];
								pointname +=" or pointnameeng='"+tempPt+"' ";
							}

							QString selectStr = "select avg(X), avg(Y) from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"'  and mname!='aver' and (" +pointname+ ")";
							//cerr<<selectStr.toStdString()<<endl;
							query.exec(selectStr);
							query.next();
							double x = query.value(0).toDouble();
							double y = query.value(1).toDouble();
							myVector pt;
							pt.x = x;
							pt.y = y;
							mesPt->push_back(pt);
						}
						else
						{
							QString selectStr = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"' and pointnameeng='"+ptn+"' and mname='aver' ";
							query.exec(selectStr);
							query.next();

							double x = query.value(0).toDouble();
							double y = query.value(1).toDouble();

							myVector pt;
							pt.x = x;
							pt.y = y;
							mesPt->push_back(pt);
						}
					}
					else
					{
						QString selectX = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"' and pointnameeng='UIE' and mname='aver' ";
						query.exec(selectX);
						query.next();
						double x1 = query.value(0).toDouble();
						double y1 = query.value(1).toDouble();
						selectX = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"' and pointnameeng='LIE' and mname='aver' ";
						query.exec(selectX);
						query.next();
						double x2 = query.value(0).toDouble();
						double y2 = query.value(1).toDouble();
						double x = (x1+x2)/2;
						double y = (y1+y2)/2;
						myVector pt;
						pt.x = x;
						pt.y = y;
						mesPt->push_back(pt);
					}
				}
				double res = computeCurrentRes(mesPt,tind);
				QString outline = sampleIndex + "\t" + imagename + "\t" + str  + "\t" + tr("均值")  + "\t" + " "  + "\t" + QString::number(res) +"\r\n";
				outList.append(outline);
			}
			preImage = imagename;

			mesPt->clear();
			for (int m = 0;m<tPtList.size();m++)
			{
				QString ptn = tPtList.at(m);
				if (ptn != "Mid3")
				{
					if (ptn=="O point")
					{
						QString pointname="";
						QString r15="R1.5";
						QString r30="R3";
						QString r45="R4.5";
						QString list[]={"a","b","c","d","e","f","g","h","i","j","k","l"};
						for (int i=0;i<12;i++)
						{
							QString tempPt = r15+list[i];
							if (i==0)
							{
								pointname +="  pointnameeng='"+tempPt+"' ";
							}
							else
							{
								pointname +=" or pointnameeng='"+tempPt+"' ";
							}

							tempPt = r30+list[i];
							pointname +=" or pointnameeng='"+tempPt+"' ";
							tempPt = r45+list[i];
							pointname +=" or pointnameeng='"+tempPt+"' ";
						}

						QString selectStr = "select avg(X), avg(Y) from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"'  and mname!='aver' and (" +pointname+ ")";
						query.exec(selectStr);
						query.next();
						double x = query.value(0).toDouble();
						double y = query.value(1).toDouble();
						myVector pt;
						pt.x = x;
						pt.y = y;
						mesPt->push_back(pt);
					}
					else
					{
						QString selectStr = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"' and pointnameeng='"+ptn+"' and mname='"+mname+"' and mindex='"+mindex+"' ";
						query.exec(selectStr);
						query.next();
						double x = query.value(0).toDouble();
						double y = query.value(1).toDouble();
						myVector pt;
						pt.x = x;
						pt.y = y;
						mesPt->push_back(pt);
					}
				}
				else
				{
					QString selectStr = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"' and pointnameeng='UIE' and mname='"+mname+"' and mindex='"+mindex+"' ";
					query.exec(selectStr);
					query.next();
					double x1 = query.value(0).toDouble();
					double y1 = query.value(1).toDouble();
					selectStr = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+imagename+"' and pointnameeng='LIE' and mname='"+mname+"' and mindex='"+mindex+"' ";
					query.exec(selectStr);
					query.next();
					double x2 = query.value(0).toDouble();
					double y2 = query.value(1).toDouble();
					double x = (x1+x2)/2;
					double y = (y1+y2)/2;
					myVector pt;
					pt.x = x;
					pt.y = y;
					mesPt->push_back(pt);
				}
			}
			double res = computeCurrentRes(mesPt,tind);
			QString outline = sampleIndex + "\t" + imagename + "\t" + str  + "\t" + mname  + "\t" + mindex  + "\t" + QString::number(res) +"\r\n";
			outList.append(outline);
		}
	}
	if (outList.size()>1)
	{
		QFile outFile(saveFileName);
		if (!outFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::information(QApplication::activeWindow(),"information","Open Failed!");
		}
		QTextStream out(&outFile);
		for (int i=0;i<outList.size();i++)
		{
			out<<outList.at(i);
		}
		outFile.close();

		QMessageBox::information(QApplication::activeWindow() ,"information",tr("输出成功!\r\n结果保存在："+saveFileName));
	}
}


void MainWindow::getCoincideImageSlot(QString imageName,int imageType)
{
	cout<<imageName.toStdString()<<" emit"<<endl;
	emit ImageFileSelected(&imageName,imageType);
	if (imageType==1)
	{
		pwidget[1]->setVisible(true);
		groupBox2D->setVisible(true);
		groupBox3D->setVisible(false);
		imageNameTarget = imageName;
	}
	else
	{
		groupBox2D->setVisible(true);
		groupBox3D->setVisible(false);
		imageNameSource = imageName;
	}
	QString yuantuName;
	QTransform t_trans;
	if (digitImageCoin->currentTabIndex==1)
	{
		QFile tInfile("./doc/rulerCoin.txt");
		if (tInfile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream newIn(&tInfile);
			while (!newIn.atEnd())
			{
				QString line = newIn.readLine();
				QStringList t_list = line.split('\t',QString::SkipEmptyParts);
				if (imageName==t_list.at(3) && digitImageCoin->mNameG==t_list.at(4) && digitImageCoin->mIndexG==t_list.at(5))
				{
					yuantuName = t_list.at(1);
					double m11,m12,m21,m22,dx,dy;
					m11 = t_list.at(6).toDouble();
					m12 = t_list.at(7).toDouble();
					m21 = t_list.at(8).toDouble();
					m22 = t_list.at(9).toDouble();
					dx = t_list.at(10).toDouble();
					dy = t_list.at(11).toDouble();
					t_trans.setMatrix(m11,m12,0,m21,m22,0,dx,dy,1);
					break;
				}
			}
		}
	}
	else if (digitImageCoin->currentTabIndex==2)
	{
		int lastPot = imageName.lastIndexOf(".");
		yuantuName = imageName.leftRef(lastPot-1).toString() + imageName.rightRef(imageName.length()-lastPot).toString();
	}

	QImage testImage;
	if (!testImage.load(yuantuName))
	{
		return;
	}

	//QMessageBox::information(this,"T",imageName+"\r\n" + yuantuName);

	connect(tempImageShowWidget1,SIGNAL(sendCurrentGeo(QRect )),tempImageShowWidget2,SLOT(getTargetGeo(QRect )));
	////connect(imSwidget2,SIGNAL(sendCurrentGeo(QRect )),imSwidget1,SLOT(getTargetGeo(QRect )));
	connect(tempImageShowWidget1,SIGNAL(sendCurrentScale(double)),tempImageShowWidget2,SLOT(setScale(double )));
	connect(tempImageShowWidget2,SIGNAL(sendCurrentScale(double)),tempImageShowWidget1,SLOT(setScale(double )));
	connect(tempImageShowWidget1,SIGNAL(sendClose()),tempImageShowWidget2,SLOT(getClose()));
	connect(tempImageShowWidget2,SIGNAL(sendClose()),tempImageShowWidget1,SLOT(getClose()));
	connect(tempImageShowWidget1,SIGNAL(sendStartPt(double ,double )),tempImageShowWidget2,SLOT(setStartPt(double ,double )));
	connect(tempImageShowWidget2,SIGNAL(sendStartPt(double ,double )),tempImageShowWidget1,SLOT(setStartPt(double ,double )));

	tempImageShowWidget2->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	tempImageShowWidget2->setImage(yuantuName);
	tempImageShowWidget1->transForm = t_trans;
	tempImageShowWidget2->type=2;
	tempImageShowWidget2->show();

	tempImageShowWidget1->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	tempImageShowWidget1->setImage(imageName);
	tempImageShowWidget1->setWindowOpacity(0.5);
	//tempImageShowWidget1->bindUpdate();
	tempImageShowWidget1->type=1;
	tempImageShowWidget1->show();
}

void MainWindow::loadtransDigitImageTriggered()
{
	QTransform t_transF;
	QFile inFile("./doc/rulerCoin.txt");
	if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	//int method = digitImageCoin->coinMethodBox2->currentIndex();
	QTextStream in(&inFile);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList t_list = line.split('\t',QString::SkipEmptyParts);
		if (digitImageCoin->sourceSampleIndex==t_list.at(0) && digitImageCoin->sourceImage==t_list.at(1) && digitImageCoin->targetSampleIndex==t_list.at(2) && digitImageCoin->targetImage==t_list.at(3) && digitImageCoin->mNameG==t_list.at(4) && digitImageCoin->mIndexG == t_list.at(5))
		{
			double m11,m12,m21,m22,dx,dy;
			m11 = t_list.at(6).toDouble();
			m12 = t_list.at(7).toDouble();
			m21 = t_list.at(8).toDouble();
			m22 = t_list.at(9).toDouble();
			dx = t_list.at(10).toDouble();
			dy = t_list.at(11).toDouble();
			t_transF.setMatrix(m11,m12,0,m21,m22,0,dx,dy,1);
			break;
		}
	}
	
	inFile.close();
	connect(imSwidget1,SIGNAL(sendCurrentGeo(QRect )),imSwidget2,SLOT(getTargetGeo(QRect )));
	////connect(imSwidget2,SIGNAL(sendCurrentGeo(QRect )),imSwidget1,SLOT(getTargetGeo(QRect )));
	connect(imSwidget1,SIGNAL(sendCurrentScale(double)),imSwidget2,SLOT(setScale(double )));
	connect(imSwidget2,SIGNAL(sendCurrentScale(double)),imSwidget1,SLOT(setScale(double )));
	connect(imSwidget1,SIGNAL(sendClose()),imSwidget2,SLOT(getClose()));
	connect(imSwidget2,SIGNAL(sendClose()),imSwidget1,SLOT(getClose()));
	connect(imSwidget1,SIGNAL(sendStartPt(double ,double )),imSwidget2,SLOT(setStartPt(double ,double )));
	connect(imSwidget2,SIGNAL(sendStartPt(double ,double )),imSwidget1,SLOT(setStartPt(double ,double )));
	//connect(coinImageDialog,SIGNAL(setCord(double , double , double , double, double, double )),imSwidget2,SLOT(setCordPt(double , double , double , double, double, double)));
	//connect(coinImageDialog,SIGNAL(setLine1(double , double , double , double, int)),imSwidget2,SLOT(getLinePt(double ,double ,double , double ,int )));
	imSwidget2->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	imSwidget1->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	imSwidget2->setMTransMatrix(t_transF);
	/*coinImageDialog->sourceTrans = t_transF;*/
	imSwidget1->setImage(imageNameSource);
	imSwidget2->setImage(imageNameTarget);
	imSwidget1->setV(pwidget[0]->points4Compare,2);
	imSwidget2->setV(pwidget[1]->points4Compare,1);
	//imSwidget2->setV(pwidget[1]->ptTst,2);

	imSwidget1->setWindowOpacity(0.5);

	imSwidget1->bindUpdate();

	imSwidget1->type=1;
	imSwidget2->type=2;
	imSwidget2->show();
	imSwidget1->show();
}

void MainWindow::loadtransDigitImageTracingTriggered(QTransform trans)
{
	connect(imSwidget1,SIGNAL(sendCurrentGeo(QRect )),imSwidget2,SLOT(getTargetGeo(QRect )));
	////connect(imSwidget2,SIGNAL(sendCurrentGeo(QRect )),imSwidget1,SLOT(getTargetGeo(QRect )));
	connect(imSwidget1,SIGNAL(sendCurrentScale(double)),imSwidget2,SLOT(setScale(double )));
	connect(imSwidget2,SIGNAL(sendCurrentScale(double)),imSwidget1,SLOT(setScale(double )));
	connect(imSwidget1,SIGNAL(sendClose()),imSwidget2,SLOT(getClose()));
	connect(imSwidget2,SIGNAL(sendClose()),imSwidget1,SLOT(getClose()));

	connect(imSwidget1,SIGNAL(sendStartPt(double ,double )),imSwidget2,SLOT(setStartPt(double ,double )));
	connect(imSwidget2,SIGNAL(sendStartPt(double ,double )),imSwidget1,SLOT(setStartPt(double ,double )));

	connect(digitImageCoin,SIGNAL(setCord(double , double , double , double, double, double )),imSwidget2,SLOT(setCordPt(double , double , double , double, double, double)));
	//connect(coinImageDialog,SIGNAL(setLine1(double , double , double , double, int)),imSwidget2,SLOT(getLinePt(double ,double ,double , double ,int )));

	imSwidget2->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	imSwidget1->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	imSwidget1->setMTransMatrix(trans);
	/*coinImageDialog->sourceTrans = t_transF;*/
	imSwidget1->setImage(imageNameSource);
	imSwidget2->setImage(imageNameTarget);

	imSwidget1->setV(pwidget[0]->points4Compare,2);
	imSwidget2->setV(pwidget[1]->points4Compare,1);
	//imSwidget2->setV(pwidget[1]->ptTst,2);

	imSwidget1->setWindowOpacity(0.5);

	imSwidget1->bindUpdate();

	imSwidget1->type=1;
	imSwidget2->type=2;

	imSwidget2->show();
	imSwidget1->show();
}

void MainWindow::adjustDitigImageCoinTriggered()
{
	connect(imSwidget1,SIGNAL(sendCurrentGeo(QRect )),imSwidget2,SLOT(getTargetGeo(QRect )));
	////connect(imSwidget2,SIGNAL(sendCurrentGeo(QRect )),imSwidget1,SLOT(getTargetGeo(QRect )));
	connect(imSwidget1,SIGNAL(sendCurrentScale(double)),imSwidget2,SLOT(setScale(double )));
	connect(imSwidget2,SIGNAL(sendCurrentScale(double)),imSwidget1,SLOT(setScale(double )));
	connect(imSwidget1,SIGNAL(sendClose()),imSwidget2,SLOT(getClose()));
	connect(imSwidget2,SIGNAL(sendClose()),imSwidget1,SLOT(getClose()));
	connect(imSwidget1,SIGNAL(sendStartPt(double ,double )),imSwidget2,SLOT(setStartPt(double ,double )));
	connect(imSwidget2,SIGNAL(sendStartPt(double ,double )),imSwidget1,SLOT(setStartPt(double ,double )));
	connect(digitImageCoin,SIGNAL(setCord(double , double , double , double, double, double )),imSwidget2,SLOT(setCordPt(double , double , double , double, double, double)));

	connect(digitImageCoin,SIGNAL(xyTranslateValue(double , double , int )),imSwidget1,SLOT(getXYTranslate(double , double , int )));
	connect(digitImageCoin,SIGNAL(xyTranslateValue(double , double , int )),imSwidget2,SLOT(getXYTranslate(double , double , int )));
	connect(digitImageCoin,SIGNAL(rotateValue(double , int )),imSwidget1,SLOT(getRotate(double , int )));
	connect(digitImageCoin,SIGNAL(rotateValue(double , int )),imSwidget2,SLOT(getRotate(double , int )));


	imSwidget2->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	imSwidget2->setImage(imageNameTarget);
	imSwidget2->type=2;
	imSwidget2->show();

	imSwidget1->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	//imSwidget1->setMTransMatrix(t_transF);
	/*coinImageDialog->sourceTrans = t_transF;*/
	imSwidget1->setImage(imageNameSource);
	imSwidget1->setWindowOpacity(0.5);
	imSwidget1->bindUpdate();
	imSwidget1->type=1;
	imSwidget1->show();
}

void MainWindow::selectRotateCenterTriggered(int c_state)
{
	if (c_state==Qt::Checked)
	{
		imSwidget1->selectRotateCenterEnable = true;
	}
	else if (c_state == Qt::Unchecked)
	{
		imSwidget1->selectRotateCenterEnable = false;
	}
}


void MainWindow::getLine(double x1,double y1,double x2, double y2,double x3,double y3,double x4, double y4)
{
	imSwidget1->line[0] = x1;
	imSwidget1->line[1] = y1;
	imSwidget1->line[2] = x2;
	imSwidget1->line[3] = y2;
	imSwidget1->type=1;
	imSwidget1->update();

	imSwidget2->line[0] = x3;
	imSwidget2->line[1] = y3;
	imSwidget2->line[2] = x4;
	imSwidget2->line[3] = y4;
	imSwidget2->type=2;
	imSwidget2->update();
}

void MainWindow::getPtMoveSlot(double x1,double y1,double x2,double y2)
{
	imSwidget2->pt1.setX(x1);
	imSwidget2->pt1.setY(y1);
	imSwidget2->pt2.setX(x2);
	imSwidget2->pt2.setY(y2);
	imSwidget1->update();
	imSwidget2->update();
}

void MainWindow::getPtMoveSlot2(double x1,double y1,double x2,double y2)
{
	imSwidget1->pt3.setX(x1);
	imSwidget1->pt3.setY(y1);
	imSwidget2->pt4.setX(x2);
	imSwidget2->pt4.setY(y2);
}

void MainWindow::getAngleSlot(double x1,double y1,double x2, double y2,double x3,double y3,double x4, double y4)
{
	imSwidget2->aPt1.setX(x1);
	imSwidget2->aPt1.setY(y1);
	imSwidget2->aPt2.setX(x2);
	imSwidget2->aPt2.setY(y2);
	imSwidget2->aPt3.setX(x3);
	imSwidget2->aPt3.setY(y3);
	imSwidget2->aPt4.setX(x4);
	imSwidget2->aPt4.setY(y4);
	imSwidget2->update();
}

void MainWindow::setDigitImageTransSlot()
{
	digitImageCoin->sourceTrans = imSwidget1->transForm;
}

void MainWindow::setLoadTransSlot()
{
	imSwidget1->transForm = digitImageCoin->sourceTrans;
	imSwidget1->update();
}

void MainWindow::clearDigitCoinYuantuXianshiTriggered()
{
	tempImageShowWidget1->close();
	tempImageShowWidget2->close();
}

void MainWindow::setSourceTracingCoinSelectPtSlot()
{
	pwidget[0]->sourceTracingCoinFlag = !pwidget[0]->sourceTracingCoinFlag;
	pwidget[1]->sourceTracingCoinFlag = !pwidget[1]->sourceTracingCoinFlag;
	//cerr<<pwidget[0]->sourceTracingCoinFlag<<" "<<pwidget[1]->sourceTracingCoinFlag<<endl;
}


void MainWindow::setTracingCoinSelectPtSlot()
{
	pwidget[0]->tracingCoinFlag = !pwidget[0]->tracingCoinFlag;
	pwidget[1]->tracingCoinFlag = !pwidget[1]->tracingCoinFlag;

}

void MainWindow::clearTracingCoinSelectPtSlot()
{
	pwidget[0]->tracingPt->clear();
	pwidget[0]->sourceTracingPt->clear();
	pwidget[0]->update();
	pwidget[1]->tracingPt->clear();
	pwidget[1]->sourceTracingPt->clear();
	pwidget[1]->update();
	//cerr<<pwidget[0]->tracingPt->size()<<endl;
}

void MainWindow::testCoinSLot(QString sourceImage, QString targetImage, QTransform trans)
{
	/*QMessageBox::information(this,"T",sourceImage+"\r\n" + targetImage);
	cerr<<"slot = "<<endl;
	cerr<<trans.m11()<<" "<<trans.m12()<<" "<<trans.m13()<<endl;
	cerr<<trans.m21()<<" "<<trans.m22()<<" "<<trans.m23()<<endl;
	cerr<<trans.m31()<<" "<<trans.m32()<<" "<<trans.m33()<<endl;
	cerr<<"--------------------"<<endl;*/

	ImageShowWidget *tempImSwidget2 = new ImageShowWidget(this);
	ImageShowWidget *tempImSwidget1 = new ImageShowWidget(tempImSwidget2);

	connect(tempImSwidget1,SIGNAL(sendCurrentGeo(QRect )),tempImSwidget2,SLOT(getTargetGeo(QRect )));
	////connect(tempImSwidget2,SIGNAL(sendCurrentGeo(QRect )),tempImSwidget1,SLOT(getTargetGeo(QRect )));
	connect(tempImSwidget1,SIGNAL(sendCurrentScale(double)),tempImSwidget2,SLOT(setScale(double )));
	connect(tempImSwidget2,SIGNAL(sendCurrentScale(double)),tempImSwidget1,SLOT(setScale(double )));
	connect(tempImSwidget1,SIGNAL(sendClose()),tempImSwidget2,SLOT(getClose()));
	connect(tempImSwidget2,SIGNAL(sendClose()),tempImSwidget1,SLOT(getClose()));
	connect(tempImSwidget1,SIGNAL(sendStartPt(double ,double )),tempImSwidget2,SLOT(setStartPt(double ,double )));
	connect(tempImSwidget2,SIGNAL(sendStartPt(double ,double )),tempImSwidget1,SLOT(setStartPt(double ,double )));

	tempImSwidget2->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	tempImSwidget2->setImage(targetImage);
	tempImSwidget2->type=2;
	tempImSwidget2->show();

	tempImSwidget1->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	tempImSwidget1->setMTransMatrix(trans);
	/*coinImageDialog->sourceTrans = t_transF;*/
	tempImSwidget1->setImage(sourceImage);
	tempImSwidget1->setWindowOpacity(0.5);
	tempImSwidget1->bindUpdate();
	tempImSwidget1->type=1;
	tempImSwidget1->show();
}

void MainWindow::outPoint(){

    QString img_dir = "E:/fordental-latest/2180/"; 
	QString filename = "./doc/patient_index.txt";
    QFile infile(filename); 
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cerr << "open failed" << endl; 
        return; 
    }

    QString outfilename = "./doc/sn.txt"; 
    QFile outfile(outfilename); 
    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text)){
        return; 
    }
    QTextStream out(&outfile); 

    QSqlQuery query; 
    QStringList point_list; 
    point_list.append("S"); 
    point_list.append("N"); 
    QTextStream in(&infile); 
	int tcase = 0;
    while(!in.atEnd()){
		tcase +=1;
        QString line = in.readLine(); 
        QString file_ind = line; 
        QString pre_img = img_dir + file_ind + "-1.jpg"; 
        QString post_img = img_dir + file_ind + "-2.jpg"; 

        QString mname= "SH"; 

        for (int i=0; i<point_list.size(); i++){
            QString pn = point_list.at(i); 
            QString getPoint = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='2000例' and imagename='"+pre_img+"' and mname='SH' and pointnameeng='"+pn+"'";
            //cerr << getPoint.toStdString() << endl; 
            query.exec(getPoint); 
            query.next();
            double pre_x = query.value(0).toDouble(); 
            double pre_y = query.value(1).toDouble(); 
            //cerr << x << " " << y << endl; 
            
            getPoint = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='2000例' and imagename='"+post_img+"' and mname='SH' and pointnameeng='"+pn+"'";
            //cerr << getPoint.toStdString() << endl; 
            query.exec(getPoint); 
            query.next();
            double post_x = query.value(0).toDouble(); 
            double post_y = query.value(1).toDouble(); 

            QString outstr = file_ind + "\t" + pn + "\t" + QString::number(pre_x) + "\t" + QString::number(pre_y) + "\t" + QString::number(post_x) + "\t" + QString::number(post_y); 
            //cerr << outstr.toStdString() << endl; 
            out<< outstr  << endl; 
        }
    }
	cout<<tcase<<endl;
	//QString getPoint = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+markPointDialog->currentImage+"' and mname!='aver' and pointnameeng='"+PtN+"'";
				
}
