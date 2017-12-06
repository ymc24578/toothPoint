#include "myMarkPoint.h"

MyMarkPointC::MyMarkPointC(QWidget *parent /* = 0 */)
	:QDialog(parent)
{
	createWidgets();
	
	iniSampleIndexCBox();
	initData();

	setWindowTitle(tr("头颅侧位片定点"));
	resize(680,720);
}

MyMarkPointC::~MyMarkPointC()
{

}

void MyMarkPointC::createWidgets()
{
	//group1
	QGroupBox *group1 = new QGroupBox(tr("测量图片信息"));
	QGridLayout *group1Layout = new QGridLayout;

	sampleIndexCBox = new QComboBox();
	sampleIndexCBox->setEditable(true);

	imageNameListWidget = new QListWidget();
	connect(imageNameListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem * )),this,SLOT(imageSelected(QListWidgetItem *)));
	connect(this,SIGNAL(doubleclickSignal(QListWidgetItem *)),this,SLOT(imageSelected(QListWidgetItem *)));
	connect(this,SIGNAL(getImageNames(QString *)),this,SLOT(getImage(QString *)));

	QAction *deleteSingleImage = new QAction(tr("Delete"),this);
	imageNameListWidget->addAction(deleteSingleImage);
	imageNameListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(deleteSingleImage,SIGNAL(triggered()),this,SLOT(deleteCurrentSingleImage()));

	QPushButton *openImages = new QPushButton(tr("导入样本图片"));
	openImages->setMaximumSize(80,30);
	connect(openImages,SIGNAL(clicked()),this,SLOT(loadImagesTriggered()));

	QPushButton *deleteImages = new QPushButton(tr("清空所选图片"));
	deleteImages->setMaximumSize(80,30);
	connect(deleteImages,SIGNAL(clicked()),this,SLOT(deleteImagesTriggered()));

	QPushButton *refresh = new QPushButton("更新");
	refresh->setMaximumSize(50,30);
	connect(refresh,SIGNAL(clicked()),this,SLOT(iniFileNameWidget()));

	QLabel *filenameLabel = new QLabel(tr("导入文件名称"));
	QLabel *XImageLabel = new QLabel(tr("导入X光片"));
	ptCordLableStr = tr("定点坐标值");
	ptCord = new QLabel(tr(ptCordLableStr));

	filenameLabel->setMaximumHeight(30);
	XImageLabel->setMaximumHeight(30);
	ptCord->setMaximumHeight(30);

	group1Layout->addWidget(filenameLabel,0,0);
	group1Layout->addWidget(sampleIndexCBox,1,0,1,4);
	group1Layout->addWidget(refresh,2,0,1,1);
	group1Layout->addWidget(openImages,2,1,1,1);
	group1Layout->addWidget(deleteImages,2,3,1,1);
	group1Layout->addWidget(XImageLabel,3,0);
	group1Layout->addWidget(imageNameListWidget,4,0,6,4);
	
	group1->setLayout(group1Layout);

	//group2
	QGroupBox *group2 = new QGroupBox(tr("测量者信息"));
	QGridLayout *group2Layout = new QGridLayout;
	
	QLabel *nameL = new QLabel("姓名");
	QLabel *idL = new QLabel("编号");
	QLabel *indexL = new QLabel("第几次测量");
	QLabel *timeL = new QLabel("测量时间");
	nameT = new QLineEdit();
	nameT->setMaximumHeight(20);
	idT = new QLineEdit();
	idT->setMaximumHeight(20);
	indexT = new QLineEdit();
	indexT->setMaximumHeight(20);
	timeT = new QLineEdit();
	timeT->setMaximumHeight(20);
	QPushButton *newMes = new QPushButton(tr("添加"));
	newMes->setMaximumSize(50,30);
	connect(newMes,SIGNAL(clicked()),this,SLOT(addnewMesTriggered()));
	
	QPushButton *editMes = new QPushButton(tr("修改"));
	editMes->setMaximumSize(50,30);
	connect(editMes,SIGNAL(clicked()),this,SLOT(editCurMesTriggered()));

	QPushButton *delMes = new QPushButton(tr("删除"));
	delMes->setMaximumSize(50,30);
	connect(delMes,SIGNAL(clicked()),this,SLOT(deleteCurMesTriggered()));

	QPushButton *newMesAll = new QPushButton(tr("批量添加"));
	newMesAll->setMaximumSize(80,30);
	connect(newMesAll,SIGNAL(clicked()),this,SLOT(addnewMesALLTriggered()));

	QPushButton *newMesPt = new QPushButton(tr("Pt"));
	newMesPt->setMaximumSize(50,30);
	connect(newMesPt,SIGNAL(clicked()),this,SLOT(addMesPtTriggered()));

	addPtStartT = new QLineEdit();
	addPtStartT->setMaximumHeight(30);

	QLabel *mesLabel = new QLabel(tr("已有测量者信息"));
	mesLabel->setMaximumHeight(30);
	mesListWidget = new QListWidget();
	connect(mesListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem * )),this,SLOT(mesSelected(QListWidgetItem *)));
	connect(this,SIGNAL(listAllPt(QListWidgetItem *)),this,SLOT(mesSelected(QListWidgetItem *)));

	int placeInd = 0;
	group2Layout->addWidget(mesLabel,placeInd++,0,1,1);
	group2Layout->addWidget(mesListWidget,placeInd++,0,1,5);
	group2Layout->addWidget(nameL,placeInd,0);
	group2Layout->addWidget(nameT,placeInd++,1,1,4);
	group2Layout->addWidget(idL,placeInd,0);
	group2Layout->addWidget(idT,placeInd++,1,1,4);
	group2Layout->addWidget(indexL,placeInd,0);
	group2Layout->addWidget(indexT,placeInd++,1,1,4);
	group2Layout->addWidget(timeL,placeInd,0);
	group2Layout->addWidget(timeT,placeInd++,1,1,4);
	group2Layout->addWidget(newMes,placeInd,0);
	group2Layout->addWidget(newMesAll,placeInd,1);
	group2Layout->addWidget(editMes,placeInd,2);
	group2Layout->addWidget(delMes,placeInd,3);
	group2Layout->addWidget(newMesPt,placeInd,4);
	group2Layout->addWidget(addPtStartT,placeInd,5);
	group2->setLayout(group2Layout);

	// group3
	QGroupBox *group3 = new QGroupBox(tr("标记点"));
	QGridLayout *group3Layout = new QGridLayout;

	QLabel *mesBox = new QLabel(tr("测量点文件库"));
	mesBox->setMaximumHeight(30);
	cusPtListWidget = new QListWidget;
	cusPtListWidget->setMaximumHeight(70);
	connect(cusPtListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem * )),this,SLOT(ptSetSelected(QListWidgetItem *)));
	iniPtSetWidget();

	pointTable = new QTableWidget();
	pointTable->setSelectionBehavior (QAbstractItemView::SelectRows);
	pointTable->setContextMenuPolicy (Qt::ActionsContextMenu);

	QAction *delAction = new QAction(tr("删除当前点"),this);
	pointTable->addAction(delAction);
	pointTable->setContextMenuPolicy(Qt::ActionsContextMenu);
	connect(delAction,SIGNAL(triggered()),this,SLOT(deleteCurrentRow()));

	QAction *showCurrentPtAction = new QAction(tr("显示当前点"),this);
	pointTable->addAction(showCurrentPtAction);
	connect(showCurrentPtAction,SIGNAL(triggered()),this,SLOT(showCPtActionTriggered()));

	QAction *showAllCurrentPtAction = new QAction(tr("显示不同测量者当前点"),this);
	pointTable->addAction(showAllCurrentPtAction);
	connect(showAllCurrentPtAction,SIGNAL(triggered()),this,SLOT(showCurrentAllPtActionTriggered()));

	QAction *showAllPtInTableAction = new QAction(tr("显示当前同一测量者所有点"),this);
	pointTable->addAction(showAllPtInTableAction);
	connect(showAllPtInTableAction,SIGNAL(triggered()),this,SLOT(showAllPtInTableActionTriggered()));

	QAction *showEclipseAction = new QAction(tr("显示椭圆"),this);
	pointTable->addAction(showEclipseAction);
	connect(showEclipseAction,SIGNAL(triggered()),this,SLOT(showEclipseActionTriggered()));

	QAction *showAllEclipseAction = new QAction(tr("显示所有椭圆"),this);
	pointTable->addAction(showAllEclipseAction);
	connect(showAllEclipseAction,SIGNAL(triggered()),this,SLOT(showAllEllipseBtTriggered()));


	group3Layout->addWidget(mesBox,0,0,1,1);
	group3Layout->addWidget(cusPtListWidget,1,0,1,1);
	group3Layout->addWidget(ptCord,2,0,1,1);
	group3Layout->addWidget(pointTable,3,0,1,1);
	group3->setLayout(group3Layout);

	// group4
	QGroupBox *group4 = new QGroupBox(tr("选点"));
	QGridLayout *group4Layout = new QGridLayout;
	sPt = new QCheckBox(tr("选取标志点"));
	rulerShow = new QCheckBox(tr("选取标尺端点"));
	connect(rulerShow,SIGNAL(stateChanged(int)),this,SLOT(rulerShowStateChanged(int)));
	/*drawRuler = new QCheckBox(tr("显示标尺"));*/
	group4Layout->addWidget(sPt,0,0);
	group4Layout->addWidget(rulerShow,0,1);
	//group4Layout->addWidget(drawRuler,1,1);
	group4->setLayout(group4Layout);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(group1,0,0,5,1);
	mainLayout->addWidget(group2,5,0,5,1);
	mainLayout->addWidget(group3,0,1,9,1);
	mainLayout->addWidget(group4,9,1,1,1);

	setLayout(mainLayout);
}

void MyMarkPointC::placeWidgets()
{
}

void MyMarkPointC::initData()
{
	currentImage = "";
	currentMName = "";
	currentMId = "";
	currentMindex = "";
	ptCordLableStr = "";
}

QCheckBox *MyMarkPointC::getSpt()
{
	return sPt;
}

QTableWidget *MyMarkPointC::getPointTable()
{
	return pointTable;
}

bool MyMarkPointC::getRulerShowChecked()
{
	return rulerShow->isChecked();
}

QString MyMarkPointC::getSampleIndexBoxText()
{
	return sampleIndexCBox->currentText();
}

bool MyMarkPointC::mesListWidgetSelected()
{
	if (mesListWidget->count()==0||mesListWidget->currentRow()==-1)
	{
		return false;
	}
	
	QString temp = mesListWidget->currentItem()->text();
	if (temp.length()==0)
	{
		return false;
	}
	return true;
}

void MyMarkPointC::addnewMesTriggered()
{
	QString mName = nameT->text();
	QString mId = idT->text();
	QString mIndex = indexT->text();
	QString mTime = timeT->text();
	if (imageNameListWidget->currentRow()<0)
	{
		return;
	}
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	currentImage = cImageIndex->text();


	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	QSqlQuery query;
	QString testCurrent = "select * from disinfo where imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"' and sampleindex='"+sampleIndex+"' ";
	query.exec(testCurrent);
	if (query.size()>0)
	{
		return;
	}


	// QMessageBox::information(QApplication::activeWindow(),"test",currentImage);
	if (mTime.length()==0)
	{
		mTime="1-1-1";
	}

	QString selectScale = "select scale from disinfo where imagename='"+currentImage+"' and mname='aver' and sampleIndex='"+sampleIndex+"'";
	query.exec(selectScale);
	query.next();
	QString mScale = query.value(0).toString();

	QString insertDisInfo = "insert into disinfo(imagename,mname,mid,mindex,mtime,scale,sampleindex) values('"+currentImage+"', '"+mName+"' , '"+mId+"','"+mIndex+"' ,'"+mTime+"','"+mScale+"','"+sampleIndex+"')";
	query.exec(insertDisInfo);
	QString getId = "SELECT MAX(iddisinfo) FROM disinfo ";
	query.exec(getId);
	query.next();
	int lastId = query.value(0).toInt();
	//初始化所有点信息
	if (ptIndexList.size()==0)
	{
		QString getAllPt = "select distinct pointindex, pointnameeng,pointnamechs from disptvalue ";
		query.exec(getAllPt);
		while (query.next())
		{
			ptIndexList.append(query.value(0).toString());
			ptNameEngList.append(query.value(1).toString());
			ptNameChsList.append(query.value(2).toString());
		}
	}
	int pointCount = ptIndexList.size();
	QString insertPT = "insert into disptvalue(iddisinfo,pointindex,pointnameeng,pointnamechs,x,y) ";
	for (int i=0;i<pointCount;i++)
	{
		if (i!=0)
		{
			insertPT += " union all ";
		}
		insertPT += "select "+QString::number(lastId)+" , '"+ptIndexList.at(i)+"' ,'"+ptNameEngList.at(i)+"','"+ptNameChsList.at(i)+"','',''";
	}
	query.exec(insertPT);	

	QString rulerPt[] = {"Ruler_1_Up","Ruler_1_Down"};
	insertPT = "insert into ruler ";
	for (int i=0;i<2;i++)
	{
		if (i!=0)
		{
			insertPT += " union all ";
		}
		insertPT += "select '"+currentImage+"','"+mName+"','"+mIndex+"','"+rulerPt[i]+"','','','"+sampleIndex+"'";
	}
	query.exec(insertPT);

	QListWidgetItem *it = imageNameListWidget->currentItem();
	emit doubleclickSignal(it);
}

void MyMarkPointC::addnewMesALLTriggered()
{
	QString mName = nameT->text();
	QString mId = idT->text();
	QString mIndex = indexT->text();
	QString mTime = timeT->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	QSqlQuery query;
	QString getAllImage = "select distinct imagename from disinfo where sampleindex='"+sampleIndex+"'";
	query.exec(getAllImage);
	QStringList imageList;
	while (query.next())
	{
		QString temp = query.value(0).toString();
		imageList.append(temp);
	}

	for (int imgi=0;imgi<imageList.size();imgi++)
	{
		if (imgi%10==0)
		{
			cerr<<".";
		}
		if (imgi%100==0)
		{
			system("cls");
			cerr<<imgi<<"/"<<imageList.size()<<endl;
		}
		currentImage = imageList[imgi];

		QString testCurrent = "select * from disinfo where imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"' and sampleindex='"+sampleIndex+"' ";
		query.exec(testCurrent);
		if (query.size()>0)
		{
			return;
		}


		// QMessageBox::information(QApplication::activeWindow(),"test",currentImage);
		if (mTime.length()==0)
		{
			mTime="1-1-1";
		}

		QString selectScale = "select scale from disinfo where imagename='"+currentImage+"' and mname='aver' and sampleIndex='"+sampleIndex+"'";
		query.exec(selectScale);
		query.next();
		QString mScale = query.value(0).toString();

		QString insertDisInfo = "insert into disinfo(imagename,mname,mid,mindex,mtime,scale,sampleindex) values('"+currentImage+"', '"+mName+"' , '"+mId+"','"+mIndex+"' ,'"+mTime+"','"+mScale+"','"+sampleIndex+"')";
		query.exec(insertDisInfo);
		QString getId = "SELECT MAX(iddisinfo) FROM disinfo ";
		query.exec(getId);
		query.next();
		int lastId = query.value(0).toInt();
		//初始化所有点信息
		if (ptIndexList.size()==0)
		{
			QString getAllPt = "select distinct pointindex, pointnameeng,pointnamechs from disptvalue ";
			query.exec(getAllPt);
			while (query.next())
			{
				ptIndexList.append(query.value(0).toString());
				ptNameEngList.append(query.value(1).toString());
				ptNameChsList.append(query.value(2).toString());
			}
		}
		int pointCount = ptIndexList.size();
		QString insertPT = "insert into disptvalue(iddisinfo,pointindex,pointnameeng,pointnamechs,x,y) ";
		for (int i=0;i<pointCount;i++)
		{
			if (i!=0)
			{
				insertPT += " union all ";
			}
			insertPT += "select "+QString::number(lastId)+" , '"+ptIndexList.at(i)+"' ,'"+ptNameEngList.at(i)+"','"+ptNameChsList.at(i)+"','',''";
		}
		query.exec(insertPT);	

		QString rulerPt[] = {"Ruler_1_Up","Ruler_1_Down"};
		insertPT = "insert into ruler ";
		for (int i=0;i<2;i++)
		{
			if (i!=0)
			{
				insertPT += " union all ";
			}
			insertPT += "select '"+currentImage+"','"+mName+"','"+mIndex+"','"+rulerPt[i]+"','','','"+sampleIndex+"'";
		}
		query.exec(insertPT);

	}
	imageSelected(imageNameListWidget->currentItem());
	 QMessageBox::information(QApplication::activeWindow(),"t",tr("批量添加完成"));
}

void MyMarkPointC::editCurMesTriggered()
{
	QString NmName = nameT->text();
	QString NmId = idT->text();
	QString NmIndex = indexT->text();
	QString NmTime = timeT->text();
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();

	QListWidgetItem *cMesIndex = mesListWidget->currentItem();
	QString cInfo = cMesIndex->text();
	QStringList t_list = cInfo.split(" ",QString::SkipEmptyParts);
	QString mName = t_list.at(0);
	QString mId = t_list.at(1);
	QString mIndex = t_list.at(2);
	QString mTime = t_list.at(3);

	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	QSqlQuery query;
	QString updateStr = "update disinfo set mname='"+NmName+"', mid='"+NmId+"', mindex='"+NmIndex+"',mtime='"+NmTime+"' where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
	query.exec(updateStr);
	updateStr = "update ruler set mname='"+NmName+"', mindex='"+NmIndex+"' where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
	query.exec(updateStr);
	QListWidgetItem *it = imageNameListWidget->currentItem();
	emit doubleclickSignal(it);
}

void MyMarkPointC::deleteCurMesTriggered()
{
	QString mName = nameT->text();
	QString mId = idT->text();
	QString mIndex = indexT->text();
	QString mTime = timeT->text();
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();

	QListWidgetItem *cMesIndex = mesListWidget->currentItem();
	QString cInfo = cMesIndex->text();
	QStringList t_list = cInfo.split(" ",QString::SkipEmptyParts);
	mName = t_list.at(0);
	mId = t_list.at(1);
	mIndex = t_list.at(2);
	mTime = t_list.at(3);

	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	QSqlQuery query;
	QString getId = "select iddisinfo from disinfo where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
	query.exec(getId);
	query.next();
	int currentId = query.value(0).toInt();
	//删除disinfo表记录
	getId = "delete from disinfo where iddisinfo="+QString::number(currentId);
	query.exec(getId);
	//删除 disptvalue 表记录
	getId = "delete from disptvalue where iddisinfo="+QString::number(currentId);
	query.exec(getId);

	//删除 ruler 表记录
	QString	deleteStr = "delete from ruler where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
	query.exec(deleteStr);
	QListWidgetItem *it = imageNameListWidget->currentItem();
	emit doubleclickSignal(it);
}


void MyMarkPointC::addMesPtTriggered()
{
	QString startAddStr = addPtStartT->text();
	int startAdd = 0;
	if (startAddStr.length()==0)
	{
		startAdd = 0;
	}
	else
	{
		startAdd = startAddStr.toInt();
	}
	system("cls");
	// get all point name
	QStringList ptNameList;
	QFile infile("./PtDoc/ptNameList.txt");
	if (!infile.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		return;
	}
	QTextStream in(&infile);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		ptNameList.append(line);
	}
	infile.close();

	// get all image in database
	QSqlQuery query;
	QString mName = nameT->text();
	QString mId = idT->text();
	QString mIndex = indexT->text();
	QString mTime = timeT->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	QString getAllImage = "select distinct imagename from disinfo where sampleindex='"+sampleIndex+"'";
	query.exec(getAllImage);
	QStringList imageList, imageListShort;
	while (query.next())
	{
		QString temp = query.value(0).toString();
		imageList.append(temp);
		temp = temp.leftRef(temp.lastIndexOf(".")).toString();
		temp = temp.rightRef(temp.length() - temp.lastIndexOf("/")-1).toString();
		//cerr<<temp.toStdString()<<endl;
		imageListShort.append(temp);
	}

	// get all add image
	QString ptDir = "./PtDoc/all/";
	QDir dir(ptDir);
	QStringList filters;
	filters<<"*.txt";
	QStringList addImageList = dir.entryList(filters,QDir::Files,QDir::Name);
	for (int i=startAdd;i<addImageList.size();i++)
	{
		if (i%10==0)
		{
			cerr<<".";
		}
		if (i%100==0)
		{
			system("cls");
			cerr<<i<<"/"<<addImageList.size()<<endl;
		}
		QString temp = addImageList[i];
		temp = temp.leftRef(temp.lastIndexOf(".")).toString();
		if (imageListShort.contains(temp))
		{
			//cout<<temp.toStdString()<<endl;
			//QMessageBox::information(this,"t",imageList[imageListShort.indexOf(temp)]);
			//cerr<<imageList[imageListShort.indexOf(temp)].toStdString()<<endl;
			currentImage = imageList[imageListShort.indexOf(temp)];
			QString getAllIddisinfo = "select distinct iddisinfo from disinfo where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' ";/*and mname!='aver'*/
			query.exec(getAllIddisinfo);
			QStringList disInfoList;
			while (query.next())
			{
				QString iddisinfo = query.value(0).toString();
				//cerr<<iddisinfo.toStdString()<<endl;
				disInfoList.append(iddisinfo);
			}

			QFile ptInfile(ptDir + addImageList[i]);
			if (!ptInfile.open(QIODevice::ReadOnly|QIODevice::Text))
			{
				return;
			}
			QTextStream ptIn(&ptInfile);
			QString line = ptIn.readLine();
			ptInfile.close();
			line.replace('\t'," ");
			QStringList t_list = line.split(" ",QString::SkipEmptyParts);
			int count = 0;
			for (int jj=1;jj<t_list.size()/2+1;jj++)
			{
				double x = t_list[jj*2-1].toDouble();
				double y = t_list[jj*2].toDouble();

				QString ptNameEng = ptNameList[jj-1];

				for (int disId = 0;disId<disInfoList.size();disId++)
				{
					QString tempDisinfo = disInfoList[disId];
					QString updatestr = "update disptvalue set x='"+QString::number(x)+"', y='"+QString::number(y)+"' where iddisinfo='"+tempDisinfo+"' and pointnameeng='"+ptNameEng+"'";
					query.exec(updatestr);
				}
			}
			//QString updateStr = "update disinfo set mname='"+NmName+"', mid='"+NmId+"', mindex='"+NmIndex+"',mtime='"+NmTime+"' where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
			//QString updateStr = "update "
			//query.exec(updateStr);

		}
	}
	QMessageBox::information(this,"Information","Add Point Done");
}

void MyMarkPointC::mesSelected(QListWidgetItem *it)
{
	ptCord->setText(ptCordLableStr);
	QString line = it->text();
	QStringList t_list = line.split(" ",QString::SkipEmptyParts);
	QString mname = t_list.at(0);
	QString mId = t_list.at(1);
	QString mIndex = t_list.at(2);
	QString mDate = t_list.at(3);

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	
	currentMId = mId;
	currentMindex = mIndex;
	currentMName = mname;
	QSqlQuery query;
	QString selectPt;
	if (cusPtListWidget->currentRow()==-1)
	{
		cusPtListWidget->setCurrentRow(1);
	}
	if (rulerShow->isChecked())
	{
		cusPtListWidget->setCurrentRow(0);
		selectPt = "select rulerpoint, x, y from ruler where imagename='"+currentImage+"' and mname='"+currentMName+"' and mindex='"+currentMindex+"'  and sampleIndex='"+sampleIndex+"' and (rulerpoint='Ruler_1_Down' or rulerpoint='Ruler_1_Up') ";
	
	}
	else
	{
		selectPt = "select pointnameeng, x, y from disptvalue,disinfo where disptvalue.iddisinfo=disinfo.iddisinfo and disinfo.imagename='"+currentImage+"' and disinfo.mname='"+currentMName+"' and disinfo.mindex='"+currentMindex+"' and disinfo.sampleIndex='"+sampleIndex+"'";
	}
	query.exec(selectPt);
	int row = query.size();
	int col = query.record().count();

	QStringList header;
	header<<"标定点"<<"X坐标"<<"Y坐标";
	pointTable->setRowCount(row);
	pointTable->setColumnCount(col);
	pointTable->setHorizontalHeaderLabels (header);
	pointTable->horizontalHeader ()->setStretchLastSection(true);
	query.next();
	for (int i=0;i<row;i++)
	{
		for (int j=0;j<col;j++)
		{
			QString str = query.value(j).toString();
			QTableWidgetItem* item = new QTableWidgetItem;
			pointTable->setItem (i,j,item);
			item->setText (str);
		}
		query.next();
	}
	pointTable->setCurrentCell(0,0);

	nameT->setText(mname);
	idT->setText(mId);
	indexT->setText(mIndex);
	timeT->setText(mDate);

	if (cusPtListWidget->currentRow()!=0)
	{
		ptSetSelected(cusPtListWidget->currentItem());
	}
	
	emit clearAllPtSignal();
}

void MyMarkPointC::imageSelected(QListWidgetItem *it)
{
	ptCord->setText(ptCordLableStr);
	QString fileName = it->text();
	currentImage = fileName;
	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	// QMessageBox::information(QApplication::activeWindow(),"test",str);
	QSqlQuery query;

	QString getScale = "select scale from disinfo where imagename='"+fileName+"' and mname='aver' and sampleIndex='"+sampleIndex+"'";

	QString selectF = "select imagename , scale from disinfo where imagename='"+fileName+"' and mname='aver' and sampleIndex='"+sampleIndex+"'";
	query.exec(selectF);
	query.next();
	QString scale = query.value(1).toString();
	/*if (scale.length()==0)
	{
		QMessageBox::warning(this,"Warning",tr("标尺未选择"));
		QImage image;
		image.load(fileName);
		emit imageS(image);
	}
	else
	{*/

		QImage image;
		image.load(fileName);
		double dScale = scale.toDouble();
		if (dScale!=0)
		{
			 QMessageBox::information(QApplication::activeWindow(),"Information",tr("标尺已存在"));
			image = image.scaledToWidth(image.width()*dScale,Qt::FastTransformation);
		}
		
		//emit fileSelected(&currentImage,0,dScale);
		emit imageS(image);

		
		mesListWidget->clear();
		selectF  = "select distinct mname, mid, mindex,mtime from disinfo where imagename='"+currentImage+"' and mname!='aver' and sampleIndex='"+sampleIndex+"' order by mname, mindex";
		query.exec(selectF);
		if (query.size()==0)
		{
			return;
		}
		int col = query.record().count();
		query.next();
		for (int j=0;j<query.size();j++)
		{
			QString line="";
			for (int k=0;k<col;k++)
			{
				line += query.value(k).toString()+"    ";
			}
			mesListWidget->addItem(line);
			query.next();
		}
		mesListWidget->setCurrentRow(0);
		mesSelected(mesListWidget->currentItem());
		sPt->setChecked(false);
	/*}*/
}

void MyMarkPointC::ptSetSelected(QListWidgetItem *it)
{
	ptCord->setText(ptCordLableStr);
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();

	QListWidgetItem *cMesIndex = mesListWidget->currentItem();

	if (cMesIndex==NULL)
	{
		return;
	}
	QString cInfo = cMesIndex->text();
	if (cInfo.length()==0)
	{
		return;
	}
	QStringList t_list = cInfo.split(" ",QString::SkipEmptyParts);
	QString mName = t_list.at(0);
	QString mId = t_list.at(1);
	QString mIndex = t_list.at(2);
	QString mTime = t_list.at(3);

	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	QString ptSetName = it->text();
	if (ptSetName.length()==0)
	{
		QListWidgetItem *it1 = mesListWidget->currentItem();
		emit listAllPt(it1);
	}
	else
	{
		QFile file("./CustomedProj/cPtCus.txt");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
		QTextStream in(&file);
		while(!in.atEnd())
		{
			QString line =  in.readLine();
			if (line==it->text())
			{
				break;
			}
		}
		QString newLine = in.readLine();
		file.close();
		QStringList t_list = newLine.split('\t',QString::SkipEmptyParts);
		QStringList header;
		header<<"标定点"<<"X坐标"<<"Y坐标";
		//cout<<t_list.size()<<" size"<<endl;
		pointTable->setRowCount(t_list.size());
		pointTable->setColumnCount(3);
		pointTable->setHorizontalHeaderLabels (header);
		pointTable->horizontalHeader ()->setStretchLastSection(true);
		QSqlQuery query;
		//cout<<"test  "<<item->text().toStdString()<<endl;
		for (int i=0;i<t_list.size();i++)
		{
			QString selectStr = "select PointNameEng, X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and disinfo.sampleindex='"+sampleIndex+"' and disinfo.imagename='"+currentImage+"' and disinfo.mname='"+mName+"' and disinfo.mindex='"+mIndex+"' and PointNameEng='"+t_list.at(i)+"'";
			query.exec(selectStr);
			query.next();
			for (int j=0;j<query.record().count();j++)
			{
				QString str = query.value(j).toString();
				QTableWidgetItem* item = new QTableWidgetItem;
				pointTable->setItem (i,j,item);
				item->setText (str);
			}
		}
	}
}

void MyMarkPointC::getImage(QString *imageName)
{
	currentImage = *imageName;
	// QMessageBox::information(QApplication::activeWindow(),"test",currentImage);
	QSqlQuery query;
	QString ptIndex[] = {"201","202","203","204","206","207","腭侧点","208","209","210","212","214","216",
		"218","219","220","221","222","223","225","228","301","302","303","304","305","306","307","308",
		"309","310","311","312","315","316","317","319","320","321","324","325","327","329","330","331",
		"332","333","334","335","338","339","340","342","344","101","102","103","104","105","401","403",
		"404","405","406","407","408","409","410","412","413","414","415","416","417","418","419","420",
		"503","508","515","601","602","603"};
	QString ptNameEng[] = {"Or","PNS","ANS","A","Pr","Pr(p)","UIE","UIA","UIR","UI","UM","UMR","U61","U62",
		"U71","U72","Ptm","Ptn","A2","Ms","LIE","Id","Id(l)","LIA","LIR","LI","B","Pg","Gn","Me","D","RGn",
		"Tgo","Go","SupGo","Ar","Co","Co’","J","LM","LMR","L61","L62","L71","L72","Mid1","Mid2","Mid3","L1",
		"PPMP","XI","Pm","Mi","S","N","Bo","Po","Ba","G","Ns","Prn","Cm","Sn","As","UL","Stoms","Stomi","LL",
		"Bs","Pos","Gns","Mes","C","Tr","GI","UL2","TB","U","TPPW","U6&L6","U4&L4","E"};
	QString ptNameChs[] = {"眶点：眶下缘之最低点","后鼻棘点","前鼻棘点","上齿槽座点","上齿槽缘点","上齿槽缘点",
		"上中切牙切缘点","上中切牙根尖点","右上颌尖牙牙尖点","上中切牙点","上颌磨牙颊尖点","上颌磨牙根尖点",
		"上颌第一磨牙近中颊尖点","右颌第一磨牙近中颊尖点","上颌第二磨牙近中颊尖点","右颌第二磨牙近中颊尖点",
		"翼上颌裂点","Ptn点","A2点","上颌第一磨牙近中颊尖点在正中矢状面的投影","下中切牙切缘点","下齿槽缘点",
		"下齿槽缘点舌侧点","下中切牙根尖点","右下颌尖牙牙尖点","下中切牙点","下齿槽座点","颏前点","颏顶点",
		"颏下点","下颌体骨性联合部之中心点","下颌联合后下点","下颌角下缘之最下点",
		"下颌角点","下颌角上点","关节点","髁顶点","髁突后点","J点","下颌磨牙颊尖点","下颌磨牙根尖点",
		"左下颌第一磨牙近中颊尖点","右下颌第一磨牙近中颊尖点","左下颌第二磨牙近中颊尖点","右下颌第二磨牙近中颊尖点",
		"Mid1点","Mid2点","Mid3点","L1点","PPMP点","下颌升支的几何中点","Mi点","蝶鞍点","鼻根点","Bolton点",
		"耳点","颅底点","额点","软组织鼻根点","鼻顶点","鼻小柱点","鼻下点","上唇凹点","上唇突点","上口点",
		"下口点","下唇突点","下唇凹点","软组织颏前点","软组织颏顶点","软组织颏底点","颈点","发际点",
		"眉间点","UL2点","舌后缘与下颌后缘的交点","悬雍垂点","咽后壁","上下颌磨牙近中牙尖之中点","上下颌第一前磨牙（乳磨牙）牙尖之中点","E点"};

	if (ptIndexList.size()==0)
	{
		//QString getAllPt = "select distinct pointindex, pointnameeng,pointnamechs from discrete ";
		QString getAllPt = "select distinct pointindex, pointnameeng,pointnamechs from disptvalue ";
		query.exec(getAllPt);
		while (query.next())
		{
			ptIndexList.append(query.value(0).toString());
			ptNameEngList.append(query.value(1).toString());
			ptNameChsList.append(query.value(2).toString());
		}
	}
	if (ptIndexList.size()==0)
	{
		for (int i=0;i<82;i++)
		{
			ptIndexList.append(ptIndex[i]);
			ptNameEngList.append(ptNameEng[i]);
			ptNameChsList.append(ptNameChs[i]);
		}
	}
	
	QString sampleIndex = sampleIndexCBox->currentText();
	QString mName="aver";

	if (sampleIndex.length()==0)
	{
		//QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	QString insertDisInfo = "insert into disinfo(imagename,mname,mid,mindex,mtime,scale,sampleindex) values('"+*imageName+"', '"+mName+"' , '','' ,'1-1-1','','"+sampleIndex+"')";
	query.exec(insertDisInfo);
	QString getId = "SELECT MAX(iddisinfo) FROM disinfo ";
	query.exec(getId);
	query.next();
	int lastId = query.value(0).toInt();

	//初始化所有点信息
	int pointCount = ptIndexList.size();
	QString insertPT = "insert into disptvalue(iddisinfo,pointindex,pointnameeng,pointnamechs,x,y) ";
	for (int i=0;i<pointCount;i++)
	{
		if (i!=0)
		{
			insertPT += " union all ";
		}
		insertPT += "select "+QString::number(lastId)+" , '"+ptIndexList.at(i)+"' ,'"+ptNameEngList.at(i)+"','"+ptNameChsList.at(i)+"','',''";
	}
	query.exec(insertPT);

	//更新文件列表
	QStringList *filenameList = new QStringList;
	
	QString selectFileName = "select distinct imagename from disinfo where sampleIndex='"+sampleIndex+"'";
	query.exec(selectFileName);
	query.next();
	for (int i=0;i<query.size();i++)
	{
		QString str = query.value(0).toString();
		filenameList->append(str);
		query.next();
	}
	imageNameListWidget->clear();
	imageNameListWidget->addItems(*filenameList);
}

void MyMarkPointC::deleteCurrentSingleImage()
{
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	currentImage = cImageIndex->text();
	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	QSqlQuery query;
	QString getid = "select iddisinfo from disinfo where imagename='"+currentImage+"' and sampleindex='"+sampleIndex+"'";
	query.exec(getid);
	QStringList allId;
	while (query.next())
	{
		allId.append(query.value(0).toString());
	}
	for (int i=0;i<allId.size();i++)
	{
		QString dStr = "delete from disptvalue where iddisinfo="+allId.at(i);
		query.exec(dStr);
	}
	QString deleteStr = "delete from disinfo where imagename='"+currentImage+"' and sampleindex='"+sampleIndex+"'";
	query.exec(deleteStr);

	deleteStr = " delete from ruler where  imagename='"+currentImage+"' and sampleindex='"+sampleIndex+"'";
	query.exec(deleteStr);

	iniFileNameWidget();
}

void MyMarkPointC::iniPtSetWidget()
{
	QFile file("./CustomedProj/ptName.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	cusPtListWidget->clear();
	cusPtListWidget->addItem("");
	while(!in.atEnd())
	{
		QString line =  in.readLine();
		cusPtListWidget->addItem(line);
	}
	file.close();
}

void MyMarkPointC::deleteCurrentRow()
{
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	QListWidgetItem *cMesIndex = mesListWidget->currentItem();
	QString cInfo = cMesIndex->text();
	QStringList t_list = cInfo.split(" ",QString::SkipEmptyParts);
	QString mName = t_list.at(0);
	QString mId = t_list.at(1);
	QString mIndex = t_list.at(2);
	QString mTime = t_list.at(3);

	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	int tableIndex = pointTable->currentRow();
	QTableWidgetItem* item1 = new QTableWidgetItem;
	item1 = pointTable->item(tableIndex,0);
	QString ptName = item1->text();
	QSqlQuery query;
	QString deleteStr;
	if (rulerShow->isChecked())
	{
		deleteStr = " update ruler set X='', Y='' where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"' and rulerpoint='"+ptName+"'";
	}
	else
	{
		QString getid = "select iddisinfo from disinfo where sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
		query.exec(getid);
		query.next();
		getid = query.value(0).toString();
		deleteStr = " update disptvalue  set X='', Y='' where iddisinfo="+getid + "  and pointnameeng='"+ptName+"' ";
	}
	query.exec(deleteStr);
	ptSetSelected(cusPtListWidget->currentItem());
}

void MyMarkPointC::showCPtActionTriggered()
{
	vector<double> *pt = new vector<double>;
	int t_row = pointTable->currentRow();
	if (t_row==-1)
	{
		pointTable->setCurrentCell(0,0);
	}
	t_row = pointTable->currentRow();
	QTableWidgetItem* item1 = new QTableWidgetItem;
	QTableWidgetItem* item2 = new QTableWidgetItem;
	item1 = pointTable->item(t_row,1);
	item2 = pointTable->item(t_row,2);
	double tx = item1->text().toDouble();
	double ty = item2->text().toDouble();
	pt->push_back(tx);
	pt->push_back(ty);
	int showType = 3;
	emit getShowPointSignal(pt,showType);
}

void MyMarkPointC::showCurrentAllPtActionTriggered()
{
	vector<double> *pt = new vector<double>;

	QString mName = nameT->text();
	QString mIndex = indexT->text();
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	int t_row = pointTable->currentRow();
	if (t_row==-1)
	{
		pointTable->setCurrentCell(0,0);
	}
	t_row = pointTable->currentRow();
	QTableWidgetItem* item1 = new QTableWidgetItem;
	item1 = pointTable->item(t_row,0);
	QString ptName = item1->text();
	// QMessageBox::information(QApplication::activeWindow(),"T",mName +"\t"+mIndex+"\t"+currentImage+"\t"+sampleIndex+"\t"+ptName);
	QSqlQuery query;
	QString getAllPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and pointnameeng='"+ptName+"' and mname!='aver'";
	query.exec(getAllPt);
	while (query.next())
	{
		int tx = query.value(0).toDouble();
		int ty = query.value(1).toDouble();
		if (tx!=0 && ty!=0)
		{
			pt->push_back(tx);
			pt->push_back(ty);
		}
	}
	int showType = 2;
	emit getShowPointSignal(pt,showType);
}

void MyMarkPointC::showAllPtInTableActionTriggered()
{
	vector<double> *pt = new vector<double>;

	QString mName = nameT->text();
	QString mIndex = indexT->text();
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	int t_row = pointTable->currentRow();
	if (t_row==-1)
	{
		pointTable->setCurrentCell(0,0);
	}
	t_row = pointTable->currentRow();
	QTableWidgetItem* item1 = new QTableWidgetItem;
	item1 = pointTable->item(t_row,0);
	QString ptName = item1->text();
	// QMessageBox::information(QApplication::activeWindow(),"T",mName +"\t"+mIndex+"\t"+currentImage+"\t"+sampleIndex+"\t"+ptName);
	QSqlQuery query;
	QString getAllPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"'  and mname='"+mName+"' and mindex='"+mIndex+"'";
	query.exec(getAllPt);
	while (query.next())
	{
		int tx = query.value(0).toDouble();
		int ty = query.value(1).toDouble();
		if (tx!=0 && ty!=0)
		{
			pt->push_back(tx);
			pt->push_back(ty);
		}
	}
	int showType = 2;
	cerr<<"type = "<<showType<<" size = "<<pt->size()<<endl;
	emit getShowPointSignal(pt,showType);
}

void MyMarkPointC::showEclipseActionTriggered()
{
	vector<double> *pt = new vector<double>;

	QString mName = nameT->text();
	QString mIndex = indexT->text();
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	int t_row = pointTable->currentRow();
	if (t_row==-1)
	{
		pointTable->setCurrentCell(0,0);
	}
	t_row = pointTable->currentRow();
	QTableWidgetItem* item1 = new QTableWidgetItem;
	item1 = pointTable->item(t_row,0);
	QString ptName = item1->text();
	// QMessageBox::information(QApplication::activeWindow(),"T",mName +"\t"+mIndex+"\t"+currentImage+"\t"+sampleIndex+"\t"+ptName);
	QSqlQuery query;
	QString getAllPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and pointnameeng='"+ptName+"' and mname='aver'";
	query.exec(getAllPt);
	while (query.next())
	{
		int tx = query.value(0).toDouble();
		int ty = query.value(1).toDouble();
		if (tx!=0 && ty!=0)
		{
			pt->push_back(tx);
			pt->push_back(ty);
		}
	}

	if (ptName=="S")
	{
		double a = 144/25.4;
		a=a*2;
		pt->push_back(a);
		pt->push_back(a);
		pt->push_back(0);
	}
	else if (ptName=="N")
	{
		double a = 1.5*144/25.4;
		a=a*2;
		pt->push_back(a);
		pt->push_back(a);
		pt->push_back(0);
	}
	else
	{
		QFile file("./doc/EclipseParaT.txt");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
		QTextStream in(&file);
		while(!in.atEnd())
		{
			QString line = in.readLine();
			line.replace('\t'," ");
			QStringList t_list = line.split(" ",QString::SkipEmptyParts);
			QString ptName_t = t_list.at(0);
			if (ptName_t!=ptName)
			{
				continue;
			}
			else
			{
				double a = t_list.at(1).toDouble();
				double b = t_list.at(2).toDouble();
				double th = t_list.at(3).toDouble();
				pt->push_back(a);
				pt->push_back(b);
				pt->push_back(th);
				break;
			}
		}
		file.close();
	}
	int showType = 1;
	emit getShowPointSignal(pt,showType);
}

void MyMarkPointC::showAllEllipseBtTriggered()
{
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();
	QListWidgetItem *cMesIndex = mesListWidget->currentItem();
	QString cInfo = cMesIndex->text();
	QStringList t_list = cInfo.split(" ",QString::SkipEmptyParts);
	QString mName = t_list.at(0);
	QString mIndex = t_list.at(2);

	currentImage = cImageIndex->text();

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	vector<double> *pt = new vector<double>;

	QFile file("./doc/EclipseParaT.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line = in.readLine();
		line.replace('\t'," ");
		QStringList t_list = line.split(" ",QString::SkipEmptyParts);
		QString ptName = t_list.at(0);

		QSqlQuery query;
		QString getAllPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleIndex+"' and imagename='"+currentImage+"' and pointnameeng='"+ptName+"' and mname='aver'";
		query.exec(getAllPt);
		while (query.next())
		{
			int tx = query.value(0).toDouble();
			int ty = query.value(1).toDouble();
			pt->push_back(tx);
			pt->push_back(ty);
		}

		if (ptName=="S")
		{
			double a = 144/25.4;
			a=a*2;
			pt->push_back(a);
			pt->push_back(a);
			pt->push_back(0);
		}
		else if (ptName=="N")
		{
			double a = 1.5*144/25.4;
			a=a*2;
			pt->push_back(a);
			pt->push_back(a);
			pt->push_back(0);
		}
		else
		{
			double a = t_list.at(1).toDouble();
			double b = t_list.at(2).toDouble();
			double th = t_list.at(3).toDouble();
			pt->push_back(a);
			pt->push_back(b);
			pt->push_back(th);
		}
	}

	int showType = 1;
	emit getShowPointSignal(pt,showType);
}

void MyMarkPointC::loadImagesTriggered()
{
	QStringList fileList = QFileDialog::getOpenFileNames(this,tr("Open Image files"));
	if (fileList.count()==0)
		return;
	for (int i=0;i<fileList.count();i++)
	{
		if (i%10==0)
		{
			cerr<<".";
		}
		if (i%100==0)
		{
			system("cls");
			cerr<<i<<"/"<<fileList.size()<<endl;
		}
		QString fileN = fileList.at(i);
		fileN.replace("\\","/");
		emit getImageNames(&fileN);
	}
	iniSampleIndexCBox();
	sampleIndexCBox->setCurrentIndex(sampleIndexCBox->count()-1);
}

void MyMarkPointC::deleteImagesTriggered()
{
	QSqlQuery query;
	QStringList *filenameList = new QStringList;

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	QString getIds = "select iddisinfo from disinfo where sampleindex='"+sampleIndex+"'";
	query.exec(getIds);
	QStringList allIds;
	while (query.next())
	{
		allIds.append(query.value(0).toString());
	}
	QString deleteStr = "delete from disinfo where sampleindex='"+sampleIndex+"'";
	query.exec(deleteStr);

	for (int i=0;i<allIds.size();i++)
	{
		deleteStr = "delete from disptvalue where iddisinfo="+allIds.at(i);
		query.exec(deleteStr);
	}
	cout<<"delete all images done"<<endl;
	iniSampleIndexCBox();
	iniFileNameWidget();
}

void MyMarkPointC::iniSampleIndexCBox()
{
	QStringList sampleList;
	if (!mydb.GetAllSampleindex(sampleList))
	{
		return;
	}
	sampleIndexCBox->clear();
	sampleIndexCBox->addItems(sampleList);
}

void MyMarkPointC::iniFileNameWidget()
{
	QSqlQuery query;
	QStringList *filenameList = new QStringList;

	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}

	QString selectFileName = "select distinct imageName from disinfo where sampleIndex='"+sampleIndex+"'";
	//cout<<selectFileName.toStdString()<<endl;

	query.exec(selectFileName);
	if (query.size()==0)
	{
		return;
	}
	while (query.next())
	{
		filenameList->append(query.value(0).toString());
	}
	imageNameListWidget->clear();
	imageNameListWidget->addItems(*filenameList);
	
	if (imageNameListWidget->count()>0)
	{
		imageNameListWidget->setCurrentRow(0);
		imageSelected(imageNameListWidget->currentItem());
	}
}

void MyMarkPointC::getScale(double s)
{
	//cout<<"scale = "<<s<<endl;
	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	QSqlQuery query;
	QString update = "update disinfo set scale='"+QString::number(s)+"' where imagename='"+currentImage+"' and sampleIndex='"+sampleIndex+"' order by X DESC";
	query.exec(update);
}

void MyMarkPointC::rulerShowStateChanged(int sta)
{
	if (imageNameListWidget->currentRow()<0 || mesListWidget->currentRow()<0)
	{
		rulerShow->setChecked(false);
		return;
	}
	mesSelected(mesListWidget->currentItem());
}