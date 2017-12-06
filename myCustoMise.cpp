#include "myCustoMise.h"

MyCustoMiseC::MyCustoMiseC(QWidget *parent /* = 0 */)
	:QDialog(parent)
{
	createTab();
	selectedProjItem = new QStringList;
	itemProjList = new QStringList;

	itemListPt = new QStringList;
	selectedItemPt = new QStringList;
	inied = false;
	resize(500,300);
	setWindowTitle(tr("自定义"));
}

void MyCustoMiseC::createTab()
{
	QGridLayout *mainLayout = new QGridLayout();
	QTabWidget *tabWidget = new QTabWidget();
	connect(tabWidget,SIGNAL(currentChanged (int)),this,SLOT(tabWidgetIndexChanged(int)));

	QWidget *tab0 = new QWidget();
	QGridLayout *layout0 = new QGridLayout();

	projWatiToChoose = new QListWidget();
	projChoosedItem = new QListWidget();
	QPushButton *add_item_proj = new QPushButton("->");
	QPushButton *del_item_proj = new QPushButton("<-");
	add_item_proj->setMaximumSize(50,50);
	del_item_proj->setMaximumSize(50,50);
	connect(add_item_proj,SIGNAL(clicked()),this,SLOT(add_Item_Proj_triggered()));
	connect(del_item_proj,SIGNAL(clicked()),this,SLOT(del_Item_Proj_triggered()));
	connect(projWatiToChoose,SIGNAL(itemDoubleClicked ( QListWidgetItem *)),this,SLOT(add_Item_Proj_triggered()));
	connect(projChoosedItem,SIGNAL(itemDoubleClicked ( QListWidgetItem *)),this,SLOT(del_Item_Proj_triggered()));

	projNameTE = new QLineEdit();
	projNameTE->setMaximumHeight(25);
	QPushButton *saveProj = new QPushButton("保存");
	saveProj->setMaximumSize(120,50);
	connect(saveProj,SIGNAL(clicked()),this,SLOT(saveProjTriggered()));
	QLabel *cusProjLabel = new QLabel(tr("自定义项目名"));
	layout0->addWidget(projWatiToChoose,0,0,4,1);
	layout0->addWidget(add_item_proj,1,1,1,1);
	layout0->addWidget(del_item_proj,2,1,1,1);
	layout0->addWidget(projChoosedItem,0,2,4,1);
	layout0->addWidget(cusProjLabel,4,0);
	layout0->addWidget(projNameTE,4,2);
	layout0->addWidget(saveProj,5,0);
	tab0->setLayout(layout0);
	tabWidget->addTab(tab0,tr("添加新测量项目"));

	QWidget *tab3 = new QWidget();
	QGridLayout *layout3 = new QGridLayout();

	QGroupBox *l3group1 = new QGroupBox(tr("两点距离"));
	QGridLayout *l3g1Layout = new QGridLayout();
	QLabel *lenP2P_inforLabel = new QLabel(tr("P1 与 P2 距离"));
	QLabel *lenP2P_label1 = new QLabel(tr("P1"));
	QLabel *lenP2P_label2 = new QLabel(tr("P1"));
	lenP2P_combox1 = new QComboBox();
	lenP2P_combox2 = new QComboBox();
	l3g1Layout->addWidget(lenP2P_inforLabel);
	l3g1Layout->addWidget(lenP2P_label1);
	l3g1Layout->addWidget(lenP2P_combox1);
	l3g1Layout->addWidget(lenP2P_label2);
	l3g1Layout->addWidget(lenP2P_combox2);
	l3group1->setLayout(l3g1Layout);

	QGroupBox *l3group2 = new QGroupBox(tr("点到直线距离"));
	QGridLayout *l3g2Layout = new QGridLayout();
	QLabel *lenP2L_inforLabel = new QLabel(tr("P 到 直线L1-L2 的距离"));
	QLabel *lenP2L_PLabel = new QLabel("P");
	QLabel *lenP2L_L1Label = new QLabel("L1");
	QLabel *lenP2L_L2Label = new QLabel("L2");
	lenP2L_combox1 = new QComboBox();
	lenP2L_combox2 = new QComboBox();
	lenP2L_combox3 = new QComboBox();
	l3g2Layout->addWidget(lenP2L_inforLabel);
	l3g2Layout->addWidget(lenP2L_PLabel);
	l3g2Layout->addWidget(lenP2L_combox1);
	l3g2Layout->addWidget(lenP2L_L1Label);
	l3g2Layout->addWidget(lenP2L_combox2);
	l3g2Layout->addWidget(lenP2L_L2Label);
	l3g2Layout->addWidget(lenP2L_combox3);
	l3group2->setLayout(l3g2Layout);

	QGroupBox *l3group3 = new QGroupBox(tr("直线成角"));
	QGridLayout *l3g3Layout = new QGridLayout();
	QLabel *angleP1_inforLabel = new QLabel(tr("直线L1-L2与L3-L4所成角度"));
	QLabel *angleP1_label1 = new QLabel("L1");
	QLabel *angleP1_label2 = new QLabel("L2");
	QLabel *angleP1_label3 = new QLabel("L3");
	QLabel *angleP1_label4 = new QLabel("L4");
	angleP1_combox1 = new QComboBox();
	angleP1_combox2 = new QComboBox();
	angleP1_combox3 = new QComboBox();
	angleP1_combox4 = new QComboBox();
	l3g3Layout->addWidget(lenP2L_inforLabel);
	l3g3Layout->addWidget(angleP1_label1);
	l3g3Layout->addWidget(angleP1_combox1);
	l3g3Layout->addWidget(angleP1_label2);
	l3g3Layout->addWidget(angleP1_combox2);
	l3g3Layout->addWidget(angleP1_label3);
	l3g3Layout->addWidget(angleP1_combox3);
	l3g3Layout->addWidget(angleP1_label4);
	l3g3Layout->addWidget(angleP1_combox4);
	l3group3->setLayout(l3g3Layout);

	QGroupBox *l3group4 = new QGroupBox(tr("保存自定义项目"));
	QGridLayout *l3g4Layout = new QGridLayout();
	QLabel *mesTypeLabel = new QLabel(tr("选择保存测量类型"));
	choseMesTypeCombox = new QComboBox();
	choseMesTypeCombox->addItem(tr("两点距离"));
	choseMesTypeCombox->addItem(tr("点到直线距离"));
	choseMesTypeCombox->addItem(tr("直线成角"));
	choseMesTypeCombox->setEditable(false);
	QLabel *mesSaveNameLabel = new QLabel(tr("自定义项目名称"));
	mesSaveNameLE = new QLineEdit();
	QPushButton *saveMesBt = new QPushButton(tr("保存自定义项目"));
	connect(saveMesBt,SIGNAL(clicked()),this,SLOT(saveCusMesBtTriggered()));
	l3g4Layout->addWidget(mesTypeLabel,0,0,1,1);
	l3g4Layout->addWidget(choseMesTypeCombox,0,1,1,1);
	l3g4Layout->addWidget(mesSaveNameLabel,1,0,1,1);
	l3g4Layout->addWidget(mesSaveNameLE,1,1,1,1);
	l3g4Layout->addWidget(saveMesBt,1,2,1,1);
	l3group4->setLayout(l3g4Layout);

	layout3->addWidget(l3group1,0,0,1,1);
	layout3->addWidget(l3group2,0,1,1,1);
	layout3->addWidget(l3group3,0,2,1,1);
	layout3->addWidget(l3group4,1,0,1,3);
	tab3->setLayout(layout3);
	tabWidget->addTab(tab3,tr("自定义测量项目"));

	QWidget *tab1 = new QWidget();
	QGridLayout *layout1 = new QGridLayout();

	pointWaitToChoose = new QListWidget();
	pointChoosedItem = new QListWidget();
	QPushButton *add_item_pt = new QPushButton("->");
	QPushButton *del_item_pt = new QPushButton("<-");
	add_item_pt->setMaximumSize(50,50);
	del_item_pt->setMaximumSize(50,50);
	ptSetNameTE = new QLineEdit();
	ptSetNameTE->setMaximumHeight(25);
	QPushButton *savePt = new QPushButton("保存");
	savePt->setMaximumSize(120,50);
	connect(add_item_pt,SIGNAL(clicked()),this,SLOT(add_Item_pt_triggered()));
	connect(del_item_pt,SIGNAL(clicked()),this,SLOT(del_Item_pt_triggered()));
	connect(pointWaitToChoose,SIGNAL(itemDoubleClicked ( QListWidgetItem *)),this,SLOT(add_Item_pt_triggered()));
	connect(pointChoosedItem,SIGNAL(itemDoubleClicked ( QListWidgetItem *)),this,SLOT(del_Item_pt_triggered()));

	connect(savePt,SIGNAL(clicked()),this,SLOT(savePtTriggered()));
	QLabel *cusPtLabel = new QLabel(tr("自定义项目名"));
	
	layout1->addWidget(pointWaitToChoose,0,0,4,1);
	layout1->addWidget(add_item_pt,1,1,1,1);
	layout1->addWidget(del_item_pt,2,1,1,1);
	layout1->addWidget(pointChoosedItem,0,2,4,1);
	layout1->addWidget(cusPtLabel,4,0);
	layout1->addWidget(ptSetNameTE,4,2);
	layout1->addWidget(savePt,5,0);

	tab1->setLayout(layout1);
	tabWidget->addTab(tab1,tr("定制测量点集合"));

	QWidget *tab2 = new QWidget();
	QGridLayout *layout2 = new QGridLayout();

	QLabel *ptNumLb = new QLabel(tr("标定点编号"));
	QLabel *ptNameLb = new QLabel(tr("标定点名称"));
	QLabel *ptNameChsLb = new QLabel(tr("标定点中文名称"));
	ptNumTE = new QLineEdit;
	ptNameTE = new QLineEdit;
	ptNameChsTE = new QLineEdit;
	ptNumTE->setMaximumHeight(25);
	ptNameTE->setMaximumHeight(25);
	ptNameChsTE->setMaximumHeight(25);
	QPushButton *insertPoints = new QPushButton(tr("批量添加文件"));
	connect(insertPoints,SIGNAL(clicked()),this,SLOT(insertPointsTriggered()));

	QPushButton *comfirmBt = new QPushButton(tr("确定添加"));
	connect(comfirmBt,SIGNAL(clicked()),this,SLOT(confirmInsert()));

	layout2->addWidget(ptNumLb,0,0);
	layout2->addWidget(ptNumTE,0,1);
	layout2->addWidget(ptNameLb,1,0);
	layout2->addWidget(ptNameTE,1,1);
	layout2->addWidget(ptNameChsLb,2,0);
	layout2->addWidget(ptNameChsTE,2,1);
	layout2->addWidget(insertPoints,3,0);
	layout2->addWidget(comfirmBt,3,1);

	tab2->setLayout(layout2);
	tabWidget->addTab(tab2,tr("添加新测量点"));


	mainLayout->addWidget(tabWidget,0,0);
	setLayout(mainLayout);
}

void MyCustoMiseC::tabWidgetIndexChanged(int t)
{

}

// 保存新建测量项目到文件
// 输出文件为 ./CustomedProj/cProj.txt
void MyCustoMiseC::saveProjTriggered()
{
	QFile file2("./CustomedProj/projName.txt");
	if (!file2.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in2(&file2);
	QStringList proNamelist;
	while(!in2.atEnd())
	{
		QString line =  in2.readLine();
		proNamelist.append(line);
	}
	file2.close();

	QFile file("./CustomedProj/cProj.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
		return;
	QTextStream out(&file);

	QString name = projNameTE->text();
	int mbRes=-1;
	if (proNamelist.contains(name))
	{
		mbRes = QMessageBox::warning(this,tr("Warning"),tr("此项目名称已存在，是否重新命名？"),QMessageBox::Yes|QMessageBox::No);
		//YES 16384 No 65536
	}
	if (mbRes==16384)
	{
		projNameTE->clear();
	}
	else if (mbRes==65536)
	{
		projNameTE->clear();
	}
	else
	{
		out<<name<<"\r\n";
		for (int i=0;i<selectedProjItem->count();i++)
		{
			out<<selectedProjItem->at(i)<<" ";
		}
		out<<"\r\n";
		file.close();

		QFile file3("./CustomedProj/projName.txt");
		if(!file3.open(QIODevice::WriteOnly | QIODevice::Append))
			return;
		QTextStream out3(&file3);
		out3<<name<<"\r\n";
		file3.close();
		this->close();
	}
}

void MyCustoMiseC::add_Item_Proj_triggered()
{
	QListWidgetItem *it = projWatiToChoose->currentItem();
	if (it==NULL)
	{
		return;
	}
	if (!selectedProjItem->contains(it->text()))
	{
		selectedProjItem->append(it->text());
		projChoosedItem->clear();
		projChoosedItem->addItems(*selectedProjItem);
		projChoosedItem->scrollToBottom();
	}
}

void MyCustoMiseC::del_Item_Proj_triggered()
{
	QListWidgetItem *it = projChoosedItem->currentItem();
	if (it==NULL)
	{
		return;
	}
	selectedProjItem->removeAt(selectedProjItem->findIndex(it->text()));
	projChoosedItem->clear();
	projChoosedItem->addItems(*selectedProjItem);
}

void MyCustoMiseC::savePtTriggered()
{
	QFile file2("./CustomedProj/projName.txt");
	if (!file2.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in2(&file2);
	QStringList proNamelist;
	while(!in2.atEnd())
	{
		QString line =  in2.readLine();
		proNamelist.append(line);
	}
	file2.close();

	QFile file("./CustomedProj/cPtCus.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
		return;
	QTextStream out(&file);

	QString name = ptSetNameTE->text();
	int mbRes=-1;
	if (proNamelist.contains(name))
	{
		mbRes = QMessageBox::warning(this,tr("Warning"),tr("此项目名称已存在，是否重新命名？"),QMessageBox::Yes|QMessageBox::No);
		//YES 16384 No 65536
	}
	if (mbRes==16384)
	{
		ptSetNameTE->clear();
	}
	else if (mbRes==65536)
	{
		ptSetNameTE->clear();
	}
	else
	{
		out<<name<<"\r\n";
		for (int i=0;i<selectedItemPt->count();i++)
		{
			out<<selectedItemPt->at(i)<<"\t";
		}
		out<<"\r\n";
		file.close();

		QFile file3("./CustomedProj/ptName.txt");
		if(!file3.open(QIODevice::WriteOnly | QIODevice::Append))
			return;
		QTextStream out3(&file3);
		out3<<name<<"\r\n";
		file3.close();
		this->close();
	}
}

void MyCustoMiseC::add_Item_pt_triggered()
{
	QListWidgetItem *it = pointWaitToChoose->currentItem();
	if (it==NULL)
	{
		return;
	}
	if (!selectedItemPt->contains(it->text()))
	{
		selectedItemPt->append(it->text());
		pointChoosedItem->clear();
		pointChoosedItem->addItems(*selectedItemPt);
		pointChoosedItem->scrollToBottom();
	}
}

void MyCustoMiseC::del_Item_pt_triggered()
{
	QListWidgetItem *it = pointChoosedItem->currentItem();
	if (it==NULL)
	{
		return;
	}
	selectedItemPt->removeAt(selectedItemPt->findIndex(it->text()));
	pointChoosedItem->clear();
	pointChoosedItem->addItems(*selectedItemPt);
}

// 从文件批量添加测量点
void MyCustoMiseC::insertPointsTriggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("选择批量添加点描述文件"), tr("./"));
	if (fileName.isEmpty())
	{
		return;
	}
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		 QMessageBox::information(QApplication::activeWindow(),"Information","File Open Failed!");
	}

	QTextStream in(&file);

	bool hasPre = false;
	QString prePti,prePtn,prePtnc;

	/*QSqlQuery query;
	QString getAllId = "select distinct iddisinfo from disptvalue ";
	query.exec(getAllId);
	QStringList allId;
	while (query.next())
	{
	allId.append(query.value(0).toString());
	}*/

	QStringList allId;
	if (!mydb.GetAllIddisinfo(allId))
	{
		cout<<"No Record"<<endl;
		return;
	}

	QStringList ptNameList,ptIndexList,ptNameCList;
	while (!in.atEnd())
	{
		int count=0;

		QString line = in.readLine();
		line.replace('\t'," ");
		QStringList t_list = line.split(" ",QString::SkipEmptyParts);
		QString ptIndex = t_list.at(0);
		QString ptName = t_list.at(1);
		QString ptNameChs = t_list.at(2);

		ptNameList.append(ptName);
		ptIndexList.append(ptIndex);
		ptNameCList.append(ptNameChs);
	}	
	file.close();
	QSqlQuery query;
	for (int i=0;i<allId.size();i++)
	{
		for (int j=0;j<ptNameList.size();j++)
		{
			QString insertPT = "insert into disptvalue(iddisinfo,pointindex,pointnameeng,pointnamechs,x,y) ";
			insertPT += "select "+allId.at(i)+", '"+ptIndexList.at(j)+"','"+ptNameList.at(j)+"','"+ptNameCList.at(j)+"','',''";
			query.exec(insertPT);
		}
	}

	 QMessageBox::information(QApplication::activeWindow(),"Information",tr("添加成功"));
}

void MyCustoMiseC::confirmInsert()
{
	QString ptIndex = ptNumTE->text();
	QString ptName = ptNameTE->text();
	QString ptNameChs = ptNameChsTE->text();

	if (ptIndex.length()==0 || ptName.length()==0 || ptNameChs.length()==0)
	{
		 QMessageBox::information(QApplication::activeWindow(),"Information",tr("请完整填写添加点信息"));
		return;
	}
	QSqlQuery query;
	/*QSqlQuery query;
	QString getAllId = "select distinct iddisinfo from disptvalue ";
	query.exec(getAllId);
	QStringList allId;
	while (query.next())
	{
	allId.append(query.value(0).toString());
	}*/

	QStringList allId;
	if (!mydb.GetAllIddisinfo(allId))
	{
		cout<<"No Record"<<endl;
		return;
	}
	QString insertPT = "insert into disptvalue(iddisinfo,pointindex,pointnameeng,pointnamechs,x,y) ";
	int count=allId.size();
	for (int i=0;i<count;i++)
	{
		if (i!=0)
		{
			insertPT += " union all ";
		}
		insertPT += "select "+allId.at(i)+", '"+ptIndex+"','"+ptName+"','"+ptNameChs+"','',''";
	}
	query.exec(insertPT);
	int t =  QMessageBox::information(QApplication::activeWindow(),"Information",tr("添加成功"));
}

void MyCustoMiseC::saveCusMesBtTriggered()
{
	QString mesSaveName = mesSaveNameLE->text();
	if (mesSaveName.length()==0)
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("请输入保存项目名称"));
		return;
	}
	int mesType = choseMesTypeCombox->currentIndex();
	switch (mesType)
	{
	case 0:
		saveCusMesType1();
		break;
	case 1:
		saveCusMesType2();
		break;
	case 2:
		saveCusMesType3();
		break;
	}
}

void MyCustoMiseC::saveCusMesType1()
{
	QString p1Name = lenP2P_combox1->currentText();
	QString p2Name = lenP2P_combox2->currentText();
	QString saveMesName = mesSaveNameLE->text();
	if (p1Name.length()==0 || p2Name.length()==0)
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("请选择测量点名称"));
		return;
	}
	QFile file("./CustomedProj/newCusProg.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
		return;
	QTextStream out(&file);
	QString outstr = "l1\t" + saveMesName + "\t" + p1Name + "\t" + p2Name;
	out<<outstr<<endl;
	file.close();
}

void MyCustoMiseC::saveCusMesType2()
{
	QString p1Name = lenP2L_combox1->currentText();
	QString p2Name = lenP2L_combox2->currentText();
	QString p3Name = lenP2L_combox3->currentText();
	QString saveMesName = mesSaveNameLE->text();
	if (p1Name.length()==0 || p2Name.length()==0 || p3Name.length()==0)
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("请选择测量点名称"));
		return;
	}
	QFile file("./CustomedProj/newCusProg.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
		return;
	QTextStream out(&file);
	QString outstr = "l3\t" + saveMesName + "\t" + p1Name + "\t" + p2Name + "\t" + p3Name;
	out<<outstr<<endl;
	file.close();
}

void MyCustoMiseC::saveCusMesType3()
{
	QString p1Name = angleP1_combox1->currentText();
	QString p2Name = angleP1_combox2->currentText();
	QString p3Name = angleP1_combox3->currentText();
	QString p4Name = angleP1_combox4->currentText();
	QString saveMesName = mesSaveNameLE->text();
	if (p1Name.length()==0 || p2Name.length()==0 || p3Name.length()==0 || p4Name.length()==0)
	{
		QMessageBox::information(QApplication::activeWindow(),"Information",tr("请选择测量点名称"));
		return;
	}
	QFile file("./CustomedProj/newCusProg.txt");
	if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
		return;
	QTextStream out(&file);
	QString outstr = "l3\t" + saveMesName + "\t" + p1Name + "\t" + p2Name + "\t" + p3Name + "\t" + p4Name;
	out<<outstr<<endl;
	file.close();
}

bool MyCustoMiseC::initialised()
{
	return inied;
}

void MyCustoMiseC::iniData()
{
	inied = true;
}

void MyCustoMiseC::setProjItemList(QStringList *t_list)
{
	//cerr<<"ini"<<" "<<t_list->size()<<endl;
	QStringList *itemList = new QStringList;
	for (int i=0;i<t_list->count();i++)
	{
		itemList->append(t_list->at(i));
	}
	QFile file("./CustomedProj/newCusProg.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		if (line.length()==0)
		{
			continue;
		}
		line.replace('\t'," ");
		QStringList t_list = line.split(" ",QString::SkipEmptyParts);
		QString proName = t_list.at(1);
		itemList->append(proName);
	}

	projWatiToChoose->addItems(*itemList);
}

void MyCustoMiseC::setPtItemList()
{
	QStringList allPointList;
	if (!mydb.GetAllPointName(allPointList))
	{
		cout<<"No Points In Database"<<endl;
		return;
	}
	pointWaitToChoose->addItems(allPointList);

	lenP2P_combox1->addItems(allPointList);
	lenP2P_combox2->addItems(allPointList);
	angleP1_combox1->addItems(allPointList);
	angleP1_combox2->addItems(allPointList);
	angleP1_combox3->addItems(allPointList);
	angleP1_combox4->addItems(allPointList);
	lenP2L_combox1->addItems(allPointList);
	lenP2L_combox2->addItems(allPointList);
	lenP2L_combox3->addItems(allPointList);
}