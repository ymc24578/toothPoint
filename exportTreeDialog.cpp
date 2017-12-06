#include "exportTreeDialog.h"
#include <QtGui>
#include <QMessageBox>
#include <QFile>
#include<iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <cmath>

using namespace std;

Dialog_ExportTree::Dialog_ExportTree(QWidget *parent)
	: QDialog(parent)
{
	QGridLayout *mainLayout = new QGridLayout;

	QPushButton *browserBt = new QPushButton(tr("选择输出文件"));
	browserBt->setMaximumHeight(30);
	connect(browserBt,SIGNAL(clicked()),this,SLOT(browserBtTriggered()));

	QPushButton *ExportBt = new QPushButton(tr("Export"));
	ExportBt->setMaximumHeight(30);
	connect(ExportBt,SIGNAL(clicked()),this,SLOT(ExportBtTriggered()));

	tree = new QTreeWidget;
	connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(treeItemChanged(QTreeWidgetItem*, int)));

	mainLayout->addWidget(browserBt);
	mainLayout->addWidget(ExportBt);
	mainLayout->addWidget(tree);

	setLayout(mainLayout);

	//iniTreeWidget();

	currentSampleIndex = "";

	//setGeometry(100,100,600,400);
	setWindowTitle(tr("输出检验结果"));
	resize(600, 400);
}

Dialog_ExportTree::~Dialog_ExportTree()
{
	delete tree;
}

void Dialog_ExportTree::browserBtTriggered()
{
	saveFileName = QFileDialog::getSaveFileName(this, tr("Save File"),"",tr("Excel (*.xls )"));
}

void Dialog_ExportTree::ExportBtTriggered()
{
	selectList.clear();
	ptList.clear();
	int topCount = tree->topLevelItemCount();
	for (int i=0;i<topCount-1;i++)
	{
		QTreeWidgetItem *topItem = tree->topLevelItem(i);
		getAllSelected(topItem,"");		
	}
	QTreeWidgetItem *topItem = tree->topLevelItem(topCount-1);
	getAllPt(topItem);

	if (saveFileName.length()==0)
	{
		QMessageBox::warning(this,"Warning","Save File Not Selected!");
		return;
	}
	else
	{
		emit gotSaveInformation(selectList,ptList,saveFileName);
		this->close();
	}
}

void Dialog_ExportTree::getAllPt(QTreeWidgetItem *item)
{
	if (item->checkState(0)!=Qt::Unchecked)
	{
		int childCount = item->childCount();
		if (childCount==0)
		{
			QString ptN = item->text(0);
			if (!ptList.contains(ptN))
			{
				ptList.append(ptN);
			}
		}
		else
		{
			for (int i=0;i<childCount;i++)
			{
				QTreeWidgetItem *childItem = item->child(i);
				getAllPt(childItem);
			}
		}
	}
}

void Dialog_ExportTree::getAllSelected(QTreeWidgetItem *item,QString line)
{
	if (item->checkState(0)!=Qt::Unchecked)
	{
		line += item->text(0) +"\t";
		int childC = item->childCount();
		if (childC==0)
		{
			//line.replace('\t'," ");
			QStringList t_list = line.split('\t',QString::SkipEmptyParts);
			if (t_list.size()==4)
			{
				selectList.append(line);
			}
		}
		else
		{
			for (int i=0;i<childC;i++)
			{
				QTreeWidgetItem *childItem = item->child(i);
				getAllSelected(childItem,line);
			}
		}
	}
}

void Dialog_ExportTree::iniTreeWidget()
{
	QSqlQuery query1;
	QString selectSampleindex = "select distinct sampleindex from disinfo";
	query1.exec(selectSampleindex);
	while (query1.next())
	{
		QString t_S_index = query1.value(0).toString();
		QTreeWidgetItem *item = new QTreeWidgetItem;
		item->setText(0,t_S_index);
		item->setCheckState(0,Qt::Unchecked);
		QSqlQuery query2;
		QString selectIN = "select distinct imagename from disinfo where sampleindex='"+t_S_index+"'";
		query2.exec(selectIN);
		while (query2.next())
		{
			QString imname = query2.value(0).toString();
			QTreeWidgetItem *inItem = new QTreeWidgetItem;
			inItem->setText(0,tr(imname));
			inItem->setCheckState(0,Qt::Unchecked);

			QSqlQuery query3;
			QString getMname = "select distinct mname from disinfo where sampleindex='"+t_S_index+"' and imagename='"+imname+"' and mname!='aver'";
			query3.exec(getMname);
			while (query3.next())
			{
				QString mName = query3.value(0).toString();
				QTreeWidgetItem *mNameItem = new QTreeWidgetItem;
				mNameItem->setText(0,tr(mName));
				mNameItem->setCheckState(0,Qt::Unchecked);

				QSqlQuery query4;
				QString getmIndex = "select distinct mindex from disinfo where sampleindex='"+t_S_index+"' and imagename='"+imname+"' and mname='"+mName+"'";
				query4.exec(getmIndex);
				while (query4.next())
				{
					QString mindex = query4.value(0).toString();
					QTreeWidgetItem *indexItem = new QTreeWidgetItem;
					indexItem->setText(0,tr(mindex));
					indexItem->setCheckState(0,Qt::Unchecked);

					mNameItem->addChild(indexItem);
				}

				inItem->addChild(mNameItem);
			}

			item->addChild(inItem);
		}
		tree->addTopLevelItem(item);
	}

	QTreeWidgetItem *topItem = new QTreeWidgetItem;
	topItem->setText(0,tr("定制测量点"));
	topItem->setCheckState(0,Qt::Unchecked);

	QFile file("./CustomedProj/cPtCus.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line =  in.readLine();
		if (line.length()==0)
		{
			continue;
		}
		QTreeWidgetItem *cusPtItem = new QTreeWidgetItem;
		cusPtItem->setText(0,line);
		cusPtItem->setCheckState(0,Qt::Unchecked);

		line = in.readLine();
		//line.replace('\t'," ");
		QStringList t_list = line.split('\t',QString::SkipEmptyParts);
		for (int i=0;i<t_list.size();i++)
		{
			QTreeWidgetItem *ptItem = new QTreeWidgetItem;
			ptItem->setText(0,t_list.at(i));
			ptItem->setCheckState(0,Qt::Unchecked);
			cusPtItem->addChild(ptItem);
		}
		topItem->addChild(cusPtItem);
	}
	file.close();

	QTreeWidgetItem *allPtItem = new QTreeWidgetItem;
	allPtItem->setText(0,tr("全部测量点"));
	allPtItem->setCheckState(0,Qt::Unchecked);

	QSqlQuery allptQuery;
	QString allPt = "select distinct pointnameeng from disptvalue";
	allptQuery.exec(allPt);
	while (allptQuery.next())
	{
		QString ptName = allptQuery.value(0).toString();
		QTreeWidgetItem *ptItem = new QTreeWidgetItem;
		ptItem->setText(0,ptName);
		ptItem->setCheckState(0,Qt::Unchecked);
		allPtItem->addChild(ptItem);
	}

	topItem->addChild(allPtItem);

	tree->addTopLevelItem(topItem);
}

void Dialog_ExportTree::iniTreeWidget(QString sampleindex)
{
	if (currentSampleIndex==sampleindex)
	{
		return;
	}
	currentSampleIndex = sampleindex;
	tree->clear();
	QString t_S_index = sampleindex;
	QTreeWidgetItem *item = new QTreeWidgetItem;
	item->setText(0,t_S_index);
	item->setCheckState(0,Qt::Unchecked);
	QSqlQuery query2;
	QString selectIN = "select distinct imagename from disinfo where sampleindex='"+t_S_index+"'";
	query2.exec(selectIN);
	while (query2.next())
	{
		QString imname = query2.value(0).toString();
		QTreeWidgetItem *inItem = new QTreeWidgetItem;
		inItem->setText(0,tr(imname));
		inItem->setCheckState(0,Qt::Unchecked);

		QSqlQuery query3;
		QString getMname = "select distinct mname from disinfo where sampleindex='"+t_S_index+"' and imagename='"+imname+"' and mname!='aver'";
		query3.exec(getMname);
		while (query3.next())
		{
			QString mName = query3.value(0).toString();
			QTreeWidgetItem *mNameItem = new QTreeWidgetItem;
			mNameItem->setText(0,tr(mName));
			mNameItem->setCheckState(0,Qt::Unchecked);

			QSqlQuery query4;
			QString getmIndex = "select distinct mindex from disinfo where sampleindex='"+t_S_index+"' and imagename='"+imname+"' and mname='"+mName+"'";
			query4.exec(getmIndex);
			while (query4.next())
			{
				QString mindex = query4.value(0).toString();
				QTreeWidgetItem *indexItem = new QTreeWidgetItem;
				indexItem->setText(0,tr(mindex));
				indexItem->setCheckState(0,Qt::Unchecked);

				mNameItem->addChild(indexItem);
			}

			inItem->addChild(mNameItem);
		}

		item->addChild(inItem);
	}
	tree->addTopLevelItem(item);

	QTreeWidgetItem *topItem = new QTreeWidgetItem;
	topItem->setText(0,tr("定制测量点"));
	topItem->setCheckState(0,Qt::Unchecked);

	QFile file("./CustomedProj/cPtCus.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	while(!in.atEnd())
	{
		QString line =  in.readLine();
		if (line.length()==0)
		{
			continue;
		}
		QTreeWidgetItem *cusPtItem = new QTreeWidgetItem;
		cusPtItem->setText(0,line);
		cusPtItem->setCheckState(0,Qt::Unchecked);

		line = in.readLine();
		//line.replace('\t'," ");
		QStringList t_list = line.split('\t',QString::SkipEmptyParts);
		for (int i=0;i<t_list.size();i++)
		{
			QTreeWidgetItem *ptItem = new QTreeWidgetItem;
			ptItem->setText(0,t_list.at(i));
			ptItem->setCheckState(0,Qt::Unchecked);
			cusPtItem->addChild(ptItem);
		}
		topItem->addChild(cusPtItem);
	}
	file.close();

	QTreeWidgetItem *allPtItem = new QTreeWidgetItem;
	allPtItem->setText(0,tr("全部测量点"));
	allPtItem->setCheckState(0,Qt::Unchecked);

	QSqlQuery allptQuery;
	QString allPt = "select distinct pointnameeng from disptvalue";
	allptQuery.exec(allPt);
	while (allptQuery.next())
	{
		QString ptName = allptQuery.value(0).toString();
		QTreeWidgetItem *ptItem = new QTreeWidgetItem;
		ptItem->setText(0,ptName);
		ptItem->setCheckState(0,Qt::Unchecked);
		allPtItem->addChild(ptItem);
	}

	topItem->addChild(allPtItem);

	tree->addTopLevelItem(topItem);
}

void Dialog_ExportTree::treeItemChanged(QTreeWidgetItem* item, int column)
{
	QString itemText = item->text(0);
	//选中时
	if (Qt::Checked == item->checkState(0))
	{
		QTreeWidgetItem* parent = item->parent();
		int count = item->childCount();
		if (count > 0)
		{
			for (int i = 0; i < count; i++)
			{
				//子节点也选中
				item->child(i)->setCheckState(0, Qt::Checked);
			}   

		}
	}
	else if (Qt::Unchecked == item->checkState(0))
	{
		int count = item->childCount();
		if (count > 0)
		{
			for (int i = 0; i < count; i++)
			{
				item->child(i)->setCheckState(0, Qt::Unchecked);
			}
		}
	}
	updateParentItem(item);
}

void Dialog_ExportTree::updateParentItem(QTreeWidgetItem* item)
{
	QTreeWidgetItem *parent = item->parent();
	if (parent == NULL)
	{
		return;
	}

	//选中的子节点个数
	double selectedCount = 0;
	int childCount = parent->childCount();
	for (int i = 0; i < childCount; i++)
	{
		QTreeWidgetItem *childItem = parent->child(i);
		if (childItem->checkState(0) == Qt::Checked)
		{
			selectedCount++;
		}
		else if (childItem->checkState(0) == Qt::PartiallyChecked)
		{
			selectedCount = selectedCount + 0.5;
		}
	}

	if (selectedCount <= 0)
	{
		//选中状态
		parent->setCheckState(0, Qt::Unchecked);
	}
	else if (selectedCount > 0 && selectedCount < childCount)
	{
		//部分选中状态
		parent->setCheckState(0, Qt::PartiallyChecked);
	}
	else if (selectedCount == childCount)
	{
		//未选中状态
		parent->setCheckState(0, Qt::Checked);
	}
}

