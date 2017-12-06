#include "myDiscrete.h"

MyDiscreteC::MyDiscreteC(QWidget *parent /* = 0 */)
	:QDialog(parent)
{
	createWidgets();
	iniSampleIndexCBox();
	initData();
	iniDialogs();
	setWindowTitle(tr("定点离散程度"));
	resize(300,600);
}

MyDiscreteC::~MyDiscreteC()
{
	delete sampleIndexCBox;
	delete imageNameListWidget;
	delete mesListWidget;
	delete ptCord;
	delete newMesuerDialog;
	delete exportMesDialog;
	delete exportTreeDialog;
}

void MyDiscreteC::iniDialogs()
{
	newMesuerDialog = new Dialog_NewMesuer();
	exportMesDialog = new Dialog_ExportMesTree();
	connect(exportMesDialog,SIGNAL(gotSaveInformation(QStringList, QStringList, QString)),this,SLOT(getexportMesSlot(QStringList , QStringList , QString )));

	exportTreeDialog = new Dialog_ExportTree();
	connect(exportTreeDialog,SIGNAL(gotSaveInformation(QStringList, QStringList, QString)),this,SLOT(judgePoints(QStringList, QStringList, QString)));
}

void MyDiscreteC::createWidgets()
{
	// group1 测量信息：图片名，测量者信息
	QGroupBox *group1 = new QGroupBox(tr("测量图片信息"));
	QGridLayout *group1Layout = new QGridLayout;

	sampleIndexCBox = new QComboBox();
	sampleIndexCBox->setEditable(false);
	
	imageNameListWidget = new QListWidget();
	connect(imageNameListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem * )),this,SLOT(imageSelected(QListWidgetItem *)));
	connect(this,SIGNAL(doubleclickSignal(QListWidgetItem *)),this,SLOT(imageSelected(QListWidgetItem *)));
	
	mesListWidget = new QListWidget();
	connect(mesListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem * )),this,SLOT(mesSelected(QListWidgetItem *)));

	QLabel *filenameLabel = new QLabel(tr("导入文件名称"));
	QLabel *XImageLabel = new QLabel(tr("导入X光片"));
	ptCordLableStr = tr("定点坐标值");
	ptCord = new QLabel(tr(ptCordLableStr));
	QLabel *mesLabel = new QLabel(tr("已有测量者信息"));
	
	filenameLabel->setMaximumHeight(30);
	XImageLabel->setMaximumHeight(30);
	ptCord->setMaximumHeight(30);
	mesLabel->setMaximumHeight(30);
	
	QPushButton *refresh = new QPushButton("更新");
	refresh->setMaximumSize(50,30);
	connect(refresh,SIGNAL(clicked()),this,SLOT(iniFileNameWidget()));

	group1Layout->addWidget(filenameLabel,0,0);
	group1Layout->addWidget(sampleIndexCBox,1,0,1,3);
	group1Layout->addWidget(refresh,1,3,1,1);
	group1Layout->addWidget(XImageLabel,2,0);
	group1Layout->addWidget(imageNameListWidget,3,0,6,4);
	group1Layout->addWidget(mesLabel,9,0);
	group1Layout->addWidget(mesListWidget,10,0,6,4);

	group1->setLayout(group1Layout);

	// group2 输出结果部分
	QGroupBox *group2 = new QGroupBox(tr("测量"));
	QGridLayout *group2Layout = new QGridLayout;

	QPushButton *exportDiscreteBt = new QPushButton(tr("输出离散结果"));
	exportDiscreteBt->setMaximumSize(80,30);
	connect(exportDiscreteBt,SIGNAL(clicked()),this,SLOT(exportDiscreteBt_triggered()));

	QPushButton *exportMesResBt = new QPushButton(tr("输出测量结果"));
	exportMesResBt->setMaximumSize(80,30);
	connect(exportMesResBt,SIGNAL(clicked()),this,SLOT(exportMesResBt_triggered()));

	QPushButton *custMesBt = new QPushButton(tr("定制测量项目"));
	custMesBt->setMaximumSize(80,30);
	connect(custMesBt,SIGNAL(clicked()),this,SLOT(custMesBt_triggered()));

	group2Layout->addWidget(exportDiscreteBt,0,0);
	group2Layout->addWidget(exportMesResBt,1,0);
	group2Layout->addWidget(custMesBt,1,1);

	group2->setLayout(group2Layout);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(group1,0,0,5,1);
	mainLayout->addWidget(group2,5,0,2,1);
	setLayout(mainLayout);
}

void MyDiscreteC::initData()
{
	currentImage = "";
	currentMName = "";
	currentMId = "";
	currentMindex = "";
	ptCordLableStr = "";
}

void MyDiscreteC::iniSampleIndexCBox()
{
	QStringList sampleList;
	if (!mydb.GetAllSampleindex(sampleList))
	{
		return;
	}
	sampleIndexCBox->clear();
	sampleIndexCBox->addItems(sampleList);
}

void MyDiscreteC::iniFileNameWidget()
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

void MyDiscreteC::imageSelected(QListWidgetItem *it)
{
	ptCord->setText(ptCordLableStr);
	QString fileName = it->text();
	currentImage = fileName;
	cout<<currentImage.toStdString()<<endl;
	QString sampleIndex = sampleIndexCBox->currentText();
	if (sampleIndex.length()==0)
	{
		QMessageBox::warning(this,"Warning","Please Input Sample Index!");
		return;
	}
	QSqlQuery query;
	
	mesListWidget->clear();
	QString selectF  = "select distinct mname, mid, mindex,mtime from disinfo where imagename='"+currentImage+"' and mname!='aver' and sampleIndex='"+sampleIndex+"' order by mname, mindex";
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
	if (mesListWidget->count()>0)
	{
		mesListWidget->setCurrentRow(0);
		mesSelected(mesListWidget->currentItem());
	}
}

void MyDiscreteC::mesSelected(QListWidgetItem *it)
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
	
	emit setMesureInfoSignal(sampleIndex,currentImage,mname,mIndex);
}

void MyDiscreteC::exportDiscreteBt_triggered()
{
	QString sampleindex = sampleIndexCBox->currentText();
	if (sampleindex.length()==0)
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("请选择要输出的测量文件库名称"));
		return;
	}
	exportTreeDialog->iniTreeWidget(sampleindex);
	exportTreeDialog->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	exportTreeDialog->show();
}

void MyDiscreteC::exportMesResBt_triggered()
{
	QString sampleindex = sampleIndexCBox->currentText();
	if (sampleindex.length()==0)
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("请选择要输出的测量文件库名称"));
		return;
	}
	exportMesDialog->iniTreeWidget(sampleindex);
	exportMesDialog->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	exportMesDialog->show();
}

void MyDiscreteC::custMesBt_triggered()
{
	QListWidgetItem *cImageIndex = imageNameListWidget->currentItem();

	QListWidgetItem *cMesIndex = mesListWidget->currentItem();

	if (cMesIndex==NULL)
	{
		QMessageBox::warning(this,"Warning",tr("请选择测量信息"));
		return;
	}
	QString cInfo = cMesIndex->text();
	if (cInfo.length()==0)
	{
		QMessageBox::warning(this,"Warning",tr("请选择测量信息"));
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

	Dialog_NewMesuer *newMesuerDialog = new Dialog_NewMesuer;
	newMesuerDialog->setIniPara(sampleIndex,currentImage,mName,mIndex);
	newMesuerDialog->iniCombox();
	connect(this,SIGNAL(setMesureInfoSignal(QString,QString,QString,QString)),newMesuerDialog,SLOT(setIniPara(QString,QString,QString,QString)));
	newMesuerDialog->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	newMesuerDialog->show();
}

//输出测量项目
// sl:selecte mes id; ml: mesure method list; sf: savefilename
void MyDiscreteC::getexportMesSlot(QStringList sl, QStringList ml, QString sf)
{
	emit setExportMesInfo(sl,ml,sf);
}

// 输出离散项目
// sList:selecte mes id; ptList: point list; saveFileName: savefilename
void MyDiscreteC::judgePoints(QStringList sList,QStringList ptList,QString saveFileName)
{
	QStringList outListNull;

	QStringList outList;
	QString header = tr("图片名") + "\t" + tr("标定点")  + "\t" + tr("测量者")  + "\t" + tr("测量编号")+"\r\n";
	outList.append(header);

	outListNull.append(header);

	QFile file("./doc/EclipsePara.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line = in.readLine();
		line.replace('\t'," ");
		QStringList t_list = line.split(" ",QString::SkipEmptyParts);
		QString ptName_t = t_list.at(0);
		if (!ptList.contains(ptName_t))
		{
			continue;
		}
		else
		{
			double p1 = t_list.at(1).toDouble();
			double p2 = t_list.at(2).toDouble();
			double p3 = t_list.at(3).toDouble();

			if (ptName_t=="S")
			{
				for (int i=0;i<sList.size();i++)
				{
					QString str = sList.at(i);
					QStringList tempList = str.split('\t',QString::SkipEmptyParts);
					QString sampleindex = tempList.at(0);
					QString imagename = tempList.at(1);
					QString mName = tempList.at(2);
					QString mIndex = tempList.at(3);
					QSqlQuery query;

					QString getPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and pointnameeng='"+ptName_t+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
					query.exec(getPt);
					query.next();
					double X = query.value(0).toDouble();
					double Y = query.value(1).toDouble();
					if (X==0 && Y==0)
					{
						QString line = imagename +"\t" +ptName_t +"\t" +mName +"\t" +mIndex +"\r\n";
						//QMessageBox::information(QApplication::activeWindow(),"NULL",mName+" "+mIndex+" " +imagename + ptName_t);
						outListNull.append(line);
						continue;
					}

					QString getAver = "select X,Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and pointnameeng='"+ptName_t+"' and mname='aver'";
					query.exec(getAver);
					query.next();
					double averX = query.value(0).toDouble();
					double averY = query.value(1).toDouble();

					double dis = (X - averX)*(X - averX) + (Y - averY)*(Y - averY);
					dis = sqrt(dis);
					double judgeDis = 144/25.4;
					if (dis>judgeDis)
					{
						QString line = imagename + "\t" + ptName_t + "\t" + mName + "\t" + mIndex +"\r\n";
						outList.append(line);
					}
				}
			}
			else if (ptName_t=="N")
			{
				for (int i=0;i<sList.size();i++)
				{
					QString str = sList.at(i);
					QStringList tempList = str.split('\t',QString::SkipEmptyParts);
					QString sampleindex = tempList.at(0);
					QString imagename = tempList.at(1);
					QString mName = tempList.at(2);
					QString mIndex = tempList.at(3);
					QSqlQuery query;

					QString getPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and pointnameeng='"+ptName_t+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
					query.exec(getPt);
					query.next();
					double X = query.value(0).toDouble();
					double Y = query.value(1).toDouble();
					if (X==0 && Y==0)
					{
						//QMessageBox::information(QApplication::activeWindow(),"NULL",mName+" "+mIndex+" " +imagename + ptName_t);
						QString line = imagename +"\t" +ptName_t +"\t" +mName +"\t" +mIndex +"\r\n";
						outListNull.append(line);
						continue;
					}

					QString getAver = "select X,Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and pointnameeng='"+ptName_t+"' and mname='aver'";
					query.exec(getAver);
					query.next();
					double averX = query.value(0).toDouble();
					double averY = query.value(1).toDouble();

					double dis = (X - averX)*(X - averX) + (Y - averY)*(Y - averY);
					dis = sqrt(dis);
					double judgeDis = 1.5*144/25.4;
					if (dis>judgeDis)
					{
						QString line = imagename + "\t" + ptName_t + "\t" + mName + "\t" + mIndex +"\r\n";
						outList.append(line);
					}
				}
			}
			else
			{
				for (int i=0;i<sList.size();i++)
				{
					QString str = sList.at(i);
					QStringList tempList = str.split('\t',QString::SkipEmptyParts);
					QString sampleindex = tempList.at(0);
					QString imagename = tempList.at(1);
					QString mName = tempList.at(2);
					QString mIndex = tempList.at(3);
					QSqlQuery query;

					QString getPt = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and pointnameeng='"+ptName_t+"' and mname='"+mName+"' and mindex='"+mIndex+"'";
					query.exec(getPt);
					query.next();
					double X = query.value(0).toDouble();
					double Y = query.value(1).toDouble();
					if (X==0 && Y==0)
					{
						//QMessageBox::information(QApplication::activeWindow(),"NULL",mName+" "+mIndex+" " +imagename + ptName_t);
						QString line = imagename +"\t" +ptName_t +"\t" +mName +"\t" +mIndex +"\r\n";
						outListNull.append(line);
						continue;
					}

					QString getAver = "select X,Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and pointnameeng='"+ptName_t+"' and mname='aver'";
					query.exec(getAver);
					query.next();
					double averX = query.value(0).toDouble();
					double averY = query.value(1).toDouble();

					double kafang = 5.99;
					X = X-averX;
					Y = Y-averY;
					double dis = p1*X*X + 2*p2*X*Y + p3*Y*Y -kafang;
					if (dis>0)
					{
						QString line = imagename + "\t" + ptName_t + "\t" + mName + "\t" + mIndex +"\r\n";
						outList.append(line);
					}
				}
			}
		}
	}
	file.close();

	if (outList.size()>1)
	{
		QFile file2(saveFileName);
		if (!file2.open(QIODevice::WriteOnly ))
			QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
		QTextStream out(&file2);
		for (int i=0;i<outList.size();i++)
		{
			out<<outList.at(i);
		}
		file2.close();

		QMessageBox::information(QApplication::activeWindow(),"Information",tr("范围外部点输出在")+"\r\n"+saveFileName);
	}
	else
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("无范围外部点"));
	}

	if (outListNull.size()>1)
	{
		QStringList t_list = saveFileName.split('/',QString::SkipEmptyParts);
		QString cDir = "";
		for (int j=0;j<t_list.size()-1;j++)
		{
			cDir += t_list.at(j)+'/';
		}

		QString nullFileName = cDir+tr("缺省记录.xls");
		QFile nullFile(nullFileName);
		if (!nullFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::information(QApplication::activeWindow(),tr("Information"),tr("Open failed"));
		}
		QTextStream outNull(&nullFile);

		for (int i=0;i<outListNull.size();i++)
		{
			outNull<<outListNull.at(i);
		}
		nullFile.close();
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("有缺省记录，保存在")+nullFileName);
	}
}
