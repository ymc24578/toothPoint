#ifndef MYCUSTOMISE_H
#define MYCUSTOMISE_H

#include <QDialog>
#include <QDir>
#include <QtGui/QtGui>
#include "myDatabase.h"
#include <iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>

class MyCustoMiseC : public QDialog
{
	Q_OBJECT

public:
	MyCustoMiseC(QWidget *parent = 0);
	void createTab();
	bool initialised();
	void iniData();
	void setProjItemList(QStringList *t_list);	// ��ʼ����ǰ������Ŀ
	void setPtItemList();	//��ʼ����ǰ���ݿ������е�

	void saveCusMesType1();	// �������
	void saveCusMesType2();	// �㵽ֱ�߾���
	void saveCusMesType3();	// ֱ�߳ɽ�

public slots:
	void tabWidgetIndexChanged(int t);
	// �����½�������Ŀ���ļ�
	// ����ļ�Ϊ ./CustomedProj/cProj.txt
	void saveProjTriggered();
	void add_Item_Proj_triggered();
	void del_Item_Proj_triggered();

	void savePtTriggered();
	void add_Item_pt_triggered();
	void del_Item_pt_triggered();

	void insertPointsTriggered();
	void confirmInsert();

	void saveCusMesBtTriggered();

private:
	QListWidget *projWatiToChoose, *projChoosedItem;
	QLineEdit *projNameTE;
	QStringList *itemProjList, *selectedProjItem;

	QListWidget *pointWaitToChoose, *pointChoosedItem;
	QLineEdit *ptSetNameTE;
	QStringList *itemListPt, *selectedItemPt;

	QLineEdit *ptNumTE, *ptNameTE, *ptNameChsTE;

	// �Զ��������Ŀ
	QComboBox *lenP2P_combox1,*lenP2P_combox2;
	QComboBox *angleP1_combox1,*angleP1_combox2,*angleP1_combox3,*angleP1_combox4;
	QComboBox *lenP2L_combox1, *lenP2L_combox2, *lenP2L_combox3;
	QComboBox *choseMesTypeCombox;
	QLineEdit *mesSaveNameLE;
	bool inied;

	myDataBaseC mydb;
};

#endif
