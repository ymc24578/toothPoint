#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QSqlDatabase>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>

#include <engine.h>

#include "glm.h"
//#include "ArcBall.h"
//#include "glwidget.h"
#include <QtGui>
class GLWidget;
//class QStringList;
#include <QHash>
#include <QList>
#include <QTime>
#include <QGroupBox>
#include <QFile>
#include <QString>
#include "window.h"
#include "myDatabase.h"
#include "myCustoMise.h"
#include "myMarkPoint.h"
#include "pwidget.h"
#include "myAdjustPPI.h"
#include "myDiscrete.h"
#include "myPatientInfo.h"
#include "digitImageCoin.h"
#include "imageShowWidget.h"

#include "ui_mainwindow.h"
#include <set>
class PickInteractorStyle;
class OperateItem;
//*2D
class QLabel;
class QPlainTextEdit;
class QLineEdit;

struct OperateItem{
	int OpeNum;
	int Opetype;
	QTreeWidgetItem *treeItem;
	/*
	0.selectpoint
	1.pickPoint
	2.deletePoint
	3.projectPoint
	4.distancePoint
	5.calFittingPlane
	6.calFittingCurve
	7.additem
	8.measureDistancePointToPoint
	
	
	record score
	*/
	int OpePickMode;
	double x,y,z;

};
struct myVector
{
	double x;
	double y;
};

class MyPlane
{
public:
	double normal[3];
	double centre[3];
	double points[3][3];
	vtkSmartPointer<vtkActor> PlaneActor;
	vtkSmartPointer<vtkActor> PlaneFrameActor;
protected:
private:
};

class MyCurve
{
public:
	double normal[3];
	double centre[3];
	vtkSmartPointer<vtkActor> topCurveActor;
	vtkSmartPointer<vtkActor> lowCurveActor;
protected:
private:
};

class MyVector
{
public:
	double point1[3];
	double point2[3];
protected:
private:
};



//&2D
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();
	//*2D
	void connectDatabase();	// �ڳ�ʼ���׶��������ݿ�
	void createActions();
	void deleteActions();
	void setupMenus();
	void iniDialogs();
	void iniCoinImages();
	void createGridGroupBox();
	void setupTabWidgets2D();
	void setupTabWidgets3D();
	void Mysetlayout();
	void loadMesureProg();
	void ScanTree();

	double computeType1(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType21(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType221(myVector pt1,myVector pt2,myVector pt3);
	double computeType222(myVector pt1,myVector pt2,myVector pt3);
	double computeType31(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType32(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType33(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType34(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType35(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType36(myVector pt1,myVector pt2,myVector pt3, myVector pt4,myVector pt5,myVector pt6, myVector pt7);
	double computeType37(myVector pt1,myVector pt2,myVector pt3, myVector pt4,myVector pt5,myVector pt6, myVector pt7);
	double computeType371(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType41(myVector pt1,myVector pt2);
	double computeType42(myVector pt1,myVector pt2,myVector pt3, myVector pt4, myVector pt5);
	double computeType43(myVector pt1,myVector pt2,myVector pt3, myVector pt4, myVector pt5,myVector pt6, myVector pt7,myVector pt8);
	double computeType51(myVector pt1,myVector pt2,myVector pt3, myVector pt4);
	double computeType52(myVector pt1,myVector pt2,myVector pt3, myVector pt4);

	double computeCurrentRes(vector<myVector> *mesPt, int i);

	myVector vectorMinus(myVector pt1, myVector pt2);
	myVector vectorPlus(myVector pt1, myVector pt2);
	double vectorMul(myVector pt1, myVector pt2);
	double vectorNorm(myVector pt1);

	QString imageNameTarget, imageNameSource;
	//&2D

public slots:
	//*2D
	void patientInfoAction_triggered();
	void custemiseAction_triggered();
	void samePPIAction_triggered();
	void discreteAction_triggered();
	void markPointAction_triggered();
	void coincidentAction_triggered();

	void getMPtCord2d(double x,double y);

	void setFullScreenSlot(int tIndex,bool isF);
	void setSelectPtEnable(int t_state);
	void setVisGrid();
	void getExportMesInfoCompute(QStringList selectList, QStringList mesList,QString saveFileName);

	// �ص���������
	void getCoincideImageSlot(QString imageName,int imageType);
	void loadtransDigitImageTriggered();
	void loadtransDigitImageTracingTriggered(QTransform trans);
	void adjustDitigImageCoinTriggered();
	void selectRotateCenterTriggered(int c_state);

	void getLine(double x1,double y1,double x2, double y2,double x3,double y3,double x4, double y4);
	void getPtMoveSlot(double x1,double y1,double x2,double y2);
	void getPtMoveSlot2(double x1,double y1,double x2,double y2);
	void getAngleSlot(double x1,double y1,double x2, double y2,double x3,double y3,double x4, double y4);

	void setDigitImageTransSlot();
	void setLoadTransSlot();

	void clearDigitCoinYuantuXianshiTriggered();
	void setSourceTracingCoinSelectPtSlot();
	void setTracingCoinSelectPtSlot();
	void clearTracingCoinSelectPtSlot();
	void testCoinSLot(QString sourceImage, QString targetImage, QTransform trans);

	void outPoint();
	//&2D


	//
	// �˵�
	//

	void loadModel();

	void hideSelect();
	void showSelect();

	void pickPoint();
	void deletePoint();
	void projectPoint();
	void distancePoint();

	void calFittingPlane();
	void calFittingCurve();
	void saveRes();

	void score();

	void measureDistancePointToPoint();

	void saveModelInfo();

	void loadModelInfo();

	void segment();

	void startsegment();

	void additem();

	void StartMeasure();
	//������������
	void iPickPoint();
	void iFittingPlane();
	void icalcFitPlane(double ipoints[100][3],int pointLenth, double *center,double *normal,double *planepoint1);

	void iProjectPoint();
	void iCutCurve();
	void iVector();
	void iPlaneVector();
	void iPointToPointDistance();
	void iPointToVectorDistance();
	void iPointToPlaneDistance();
	void iPlaneToPlaneDistance();
	void iVectorAngle();
	void iPointsAngle();
	void itoothAngle1();
	void itoothAngle2();
	//
	// ������
	//

	void clickOptionTreeItem(QTreeWidgetItem*, int);
	
	//
	// �Ҽ��˵�
	//

	void showContextMenu(const QPoint& pos);
	void showTop();
	void showLow();
	void hideTop();
	void hideLow();

	void pause();
	void continueRotate();
	//*2D
signals:
	void ImageFileSelected(QString *, int index);
	void selectPt(bool);
	void showSinglePt2d(double,double);
	void showAllPt2d(vector<double> *,vector<QString> *);
	//&2D
public:
	void setPickPointPosition(double x, double y, double z);
	void setPickPointForMeasureDistancePointToPoint(double x, double y, double z);
	void setSegPickPointPoisition(double x, double y, double z);
	void saveData(QString no);
	void loadData(QString no);
	double getFrontToothDistance();

	void setScore(double score);

	void initCloseRelationMeasureTreeWidget();
	void initCloseRelationMeasureTreeWidget2();
	int returnselectOptionTreeIndex();


	void saveInput(OperateItem it);


	void loadInput();

	void addCellId(int selectMode, int cellId,int celltype);

	void startBigPick();
	void startMiddlePick();
	void startSmallPick();

	void savePick();

	bool isCellIdInSet(int cellId);

	void drawTriCell(int cellId,int celltype);

	void setDiCloseRelationType(int T);
private:
	//*2D
	myDataBaseC mydb;
	QAction *patientInfoAction,*custemiseAction, *samePPIAction, *discreteAction, *markPointAction, *coincidentAction;

	MyCustoMiseC *customiseDialog;
	MyMarkPointC *markPointDialog;
	myAdjustPPIDialogC *adjustPPIDialog;
	MyDiscreteC *discreteDialog;
	Dialog_DigitImageCoin *digitImageCoin;
	MyPatientInfoDialog *patientDialog;

	ImageShowWidget *imSwidget2,*imSwidget1;
	ImageShowWidget *tempImageShowWidget1, *tempImageShowWidget2;

	QGroupBox *groupBox3D, *groupBox2D;//��ά ��άgroupbox

	QStringList *MProgType1Type,*MProgType1Index,*MProgType1Name;
	vector<QStringList> *MProgType1Pt;
	vector<double> *MProgType1Res;

	/////////////////////////////////////
	PickInteractorStyle *pickStyleT;

	QString currentModelPath;

	std::set<int> cellIdSet;
	std::set<int> pointSet;

	vtkPolyData *data;




	////////////////////////////////////
	PWidget *pwidget[2];
	bool DrawOnWhich;
	bool isFull;
	bool isconnected;
	int MinWidgetNum;
	int selectedNum;
	//&2D
	//

	// 0: select nothing
	// 111: ����
	// 1: ������
	// 2: ������
	// 1500: ABO-OGSָ������
	// 1501: ABO-OGS����
	// 1502: ABO-OGS����->��
	// 1503: ABO-OGS����->�÷�
	// 1510--: ABO-OGS����->��->���1/2/.../5
	// 112: DIָ������
	// 113: ƽ��
	// 114: DI����
	// 118: DI����->��
	// 115: DI����->ƽ��
	// 119: DI����->ƽ��->��������״�ཻƽ��
	// 123: DI����->ƽ��->��������״�ཻƽ��->��
	// 124: DI����->ƽ��->��������״�ཻƽ��->������
	// 120: DI����->ƽ��->��ƽ��ƽ��ƽ��
	// 125: DI����->ƽ��->��ƽ��ƽ��ƽ��->��
	// 126: DI����->ƽ��->��ƽ��ƽ��ƽ��->������
	// 116: DI����->����
	// 121: DI����->����->����A
	// 122: DI����->����->����B
	// 127: DI����->����->����
	// 117: DI����->�÷�

	// 118: DI����
	// 119: DI����->��
	// 120: DI����->��������״�ཻƽ��
	// 123: DI����->��������״�ཻƽ��->��
	// 124: DI����->��������״�ཻƽ��->������
	// 121: DI����->��������
	// 125: DI����->��������->��
	// 126: DI����->��������->����
	// 122: DI����->�÷�

	// 128: DI����
	// 129: DI����->��
	// 130: DI����->��������״�ཻƽ��
	// 133: DI����->��������״�ཻƽ��->��
	// 134: DI����->��������״�ཻƽ��->������
	// 131: DI����->��������
	// 135: DI����->��������->��
	// 136: DI����->��������->����
	// 132: DI����->�÷�

	// 137: DI�෽����
	// 140: DI�෽����->ĥ��1
	// 160: DI�෽����->ĥ��2
	// 180: DI�෽����->ĥ��3
	// 141: DI�෽����->ĥ��1->��
	// 161: DI�෽����->ĥ��2->��
	// 181: DI�෽����->ĥ��3->��
	// 142: DI�෽����->ĥ��1->��������״�ཻƽ��
	// 145: DI�෽����->ĥ��1->��������״�ཻƽ��->��
	// 146: DI�෽����->ĥ��1->��������״�ཻƽ��->������
	// 162: DI�෽����->ĥ��2->��������״�ཻƽ��
	// 165: DI�෽����->ĥ��2->��������״�ཻƽ��->��
	// 166: DI�෽����->ĥ��2->��������״�ཻƽ��->������
	// 182: DI�෽����->ĥ��3->��������״�ཻƽ��
	// 185: DI�෽����->ĥ��3->��������״�ཻƽ��->��
	// 186: DI�෽����->ĥ��3->��������״�ཻƽ��->������
	// 143: DI�෽����->ĥ��1->��������
	// 147: DI�෽����->ĥ��1->��������->��
	// 148: DI�෽����->ĥ��1->��������->����
	// 163: DI�෽����->ĥ��2->��������
	// 167: DI�෽����->ĥ��2->��������->��
	// 168: DI�෽����->ĥ��2->��������->����
	// 183: DI�෽����->ĥ��3->��������
	// 187: DI�෽����->ĥ��3->��������->��
	// 188: DI�෽����->ĥ��3->��������->����
	// 144: DI�෽����->ĥ��1->�÷�
	// 164: DI�෽����->ĥ��2->�÷�
	// 184: DI�෽����->ĥ��3->�÷�
	// 200: DI�෽����->�÷�
	// 400: DIǰ������
	// 401: DIǰ������->��
	// 402: DIǰ������->�÷�
	// 420--820: DIǰ������->����1/2/.../19/20
	// 420: DIǰ������->����1
	// 421: DIǰ������->����1->��
	// 422: DIǰ������->����1->��������״�ཻƽ��
	// 423: DIǰ������->����1->��������
	// 424: DIǰ������->����1->�÷�
	// 425: DIǰ������->����1->��������״�ཻƽ��->��
	// 426: DIǰ������->����1->��������״�ཻƽ��->������
	// 427: DIǰ������->����1->��������->��
	// 428: DIǰ������->����1->��������->����
	// 900: DI��������
	// 901: DI��������->��
	// 902: DI��������->ͶӰ
	// 903: DI��������->�÷�
	// 904: DI����������
	// 905: DI����������->��
	// 906: DI����������->ͶӰ
	// 907: DI����������->�÷�
	// 1000: DI����
	// 1001: DI����->���߲���
	// 1002: DI����->���߲���->��
	// 1003: DI����->���߲���->����
	// 1004: DI����->���߲���->�÷�
	// 1005: DI����->����������϶
	// 1006: DI����->����������϶->��
	// 1007: DI����->����������϶->����
	// 1008: DI����->����������϶->�÷�
	// 1009: DI����->�÷�
	// 1020: DIӵ����
	// 1021: DIӵ����->ĥ��
	// 1022: DIӵ����->����
	// 1023: DIӵ����->�÷�
	// 1024: DIӵ����->����
	// 1030--1270: DIӵ����->ĥ��->����1/../����12
	// 1030: DIӵ����->ĥ��->����1
	// 1031: DIӵ����->ĥ��->����1->��
	// 1032: DIӵ����->ĥ��->����1->����
	// 1028: DIӵ����->ĥ��->����
	// 1270--1350: DIӵ����->����->����1/../����4
	// 1270: DIӵ����->����->����1
	// 1271: DIӵ����->����->����1->��
	// 1272: DIӵ����->����->����1->����	
	// 1029: DIӵ����->����->����
	// 1370: DI�Ϲ�ϵ
	// 1371: DI�Ϲ�ϵ->�÷�
	// 1372: DI�Ϲ�ϵ->����
	// 1373: DI�Ϲ�ϵ->����->��A
	// 1374: DI�Ϲ�ϵ->����->��B
	// 1375: DI�Ϲ�ϵ->����->��������״�ཻƽ��
	// 1376: DI�Ϲ�ϵ->����->����
	// 1377: DI�Ϲ�ϵ->����->��������״�ཻƽ��->��
	// 1378: DI�Ϲ�ϵ->����->��������״�ཻƽ��->������
	// 3: ǰ��������
	// 4: ǰ��������->��ƽ��
	// 5: ǰ��������->��ƽ��->��ϵ�
	// 6: ǰ��������->��ƽ��->������
	// 7: ǰ��������->��ƽ��->����
	// 8: ǰ��������->�Ӵ���
	// 9: ǰ��������->�Ӵ���->��϶1
	// 10: ǰ��������->�Ӵ���->��϶2
	// 11: ǰ��������->�Ӵ���->��϶3
	// 12: ǰ��������->�Ӵ���->��϶4
	// 13: ǰ��������->�Ӵ���->��϶5
	// 14: ǰ��������->�Ӵ���->ͶӰ���
	// 15: ǰ��������->�÷�
	// 16: ǰ��������->�÷�->ǰ�����������
	// 17: ǰ��������->�÷�->��ֵ
	// 18: �ղ���ҧ��
	// 19: �ղ���ҧ��->ʸװ��÷�
	// 20: �ղ���ҧ��->������
	// 21: �ղ���ҧ��->������
	// 22: �ղ���ҧ��->ˮƽ��÷�
	// 23: �ղ���ҧ��->������->����ʸ״��
	// 24: �ղ���ҧ��->������->��(��ֱ��)
	// 25: �ղ���ҧ��->������->��(ˮƽ��)
	// 26: �ղ���ҧ��->������->����ʸ״��->��ʼ��
	// 27: �ղ���ҧ��->������->����ʸ״��->������
	// 28: �ղ���ҧ��->������->��(��ֱ��)
	// 29: �ղ���ҧ��->������->��(ˮƽ��)
	// 30: �ղ���ҧ��->������->��(��ֱ��)->ͶӰ���
	// 31: �ղ���ҧ��->������->��(ˮƽ��)->ͶӰ���
	// 32: �ղ���ҧ��->������->��(��ֱ��)->ͶӰ���
	// 33: �ղ���ҧ��->������->��(ˮƽ��)->ͶӰ���
	// 34: ����
	// 35: ����->ʸ״ƽ����1
	// 97: ����->ʸ״ƽ����2
	// 98: ����->����
	// 36: ����->ʸ״ƽ����1->��ʼ��
	// 37: ����->ʸ״ƽ����1->������
	// 38: ����->ʸ״ƽ����->����
	// 99: ����->ʸ״ƽ����2->��ʼ��
	// 100: ����->ʸ״ƽ����2->������
	// 39: ����->��
	// 40: ����->��->ͶӰ���
	// 41: ����->��->ͶӰ���->����
	// 42: ����->�÷�
	// 43: ����
	// 44: ����->��������
	// 45: ����->��������->�ཻƽ��
	// 46: ����->��������->�ཻƽ��->��ʼ��
	// 47: ����->��������->�ཻƽ��->������
	// 48: ����->�÷�
	// 49: ���ϸ���
	// 50: ���ϸ���->������
	// 51: ���ϸ���->������->���1
	// 52: ���ϸ���->������->���2
	// 53: ���ϸ���->������->���3
	// 54: ���ϸ���->������->���4
	// 55: ���ϸ���->�÷�
	// 60: ���ϸ���->������
	// 56: ���ϸ���->������->���1
	// 57: ���ϸ���->������->���2
	// 58: ���ϸ���->������->���3
	// 59: ���ϸ���->������->���4
	// 61: ����
	// 62: ����->��������
	// 63: ����->��������->�������
	// 64: ����->��������->�������->��


	// 68: ����
	// 69: ����->��
	// 70: ����->��������״�ཻƽ��
	// 73: ����->��������״�ཻƽ��->��
	// 74: ����->��������״�ཻƽ��->������
	// 71: ����->��������
	// 75: ����->��������->��
	// 76: ����->��������->����
	// 72: ����->�÷�


	// 78: ����->��������->��������״�ཻƽ��
	// 79: ����->��������->��������״�ཻƽ��->��
	// 80: ����->��������->��������״�ཻƽ��->������
	// 81: ����->��������->��������״�ཻƽ��->������
	// 82: ����->��������->��������״�ཻƽ��->������->��
	// 83: ����->��������->��������״�ཻƽ��->������->����
	// 84: ����->ǰ������
	// 85: ����->ǰ������->��״��
	// 86: ����->ǰ������->��״��->��ʼ��
	// 87: ����->ǰ������->��״��->������
	// 88: ����->ǰ������->��״��->��
	// 101: ����->ǰ������->��״��->��->������
	// 103: ����->ǰ������->��״��->��->������->���1
	// 104: ����->ǰ������->��״��->��->������->���2
	// 105: ����->ǰ������->��״��->��->������->���3
	// 106: ����->ǰ������->��״��->��->������->���4
	// 102: ����->ǰ������->��״��->��->������
	// 107: ����->ǰ������->��״��->��->������->���1
	// 108: ����->ǰ������->��״��->��->������->���2
	// 109: ����->ǰ������->��״��->��->������->���3
	// 110: ����->ǰ������->��״��->��->������->���4
	// 89: ����->ǰ������->�������
	// 90: ����->ǰ������->��������״�ཻƽ��
	// 91: ����->ǰ������->�������->��
	// 92: ����->ǰ������->��������״�ཻƽ��->��
	// 93: ����->ǰ������->��������״�ཻƽ��->������
	// 94: ����->ǰ������->��������״�ཻƽ��->������
	// 95: ����->ǰ������->��������״�ཻƽ��->������->��
	// 96: ����->ǰ������->��������״�ཻƽ��->������->����
	//
	// ֧������/��ʾ�����İ���: 1, 2, 4, 5, 8, 14, 23, 26, 28, 30, 29, 31, 24, 32, 25, 33, 35, 36, 39, 
	//							44, 45, 46, 49, 63, 64, 66, 67, 69, 70, 72, 71, 76, 78, 79, 81, 82, 86, 
	//							85, 91, 89, 90, 94, 95					
	// ֧��ѡ�����: 4, 9, 10, 11, 12, 13, 23, 28, 29, 24, 25, 35, 39, 45, 51, 52, 53, 54, 56, 57, 58, 
	//				 59, 64, 67, 72, 76, 79, 82, 86, 91, 95
	// ֧�����ƽ�����: 4
	// ֧�ֵ�ɾ������: 5���ӽڵ�
	// ֧��ͶӰ����: 8(���Ӵ���ͶӰ����ƽ��), 31, 32, 33, 40, 50, 60
	// ֧�ֵ÷ֲ���: 15, 19, 22, 42, 48, 55
	// ֧��������߲���: 63, 66, 89
	//
	int getSelectOptionTreeIndex(QVariant selectNodeData);
	QTreeWidgetItem* getOptionTreeByData(int targetData);
	QTreeWidgetItem* getChildItem(QTreeWidgetItem *parent, int itemData);
	void removeChildItem(QTreeWidgetItem *parent);
	void removeChildItemExcept(QTreeWidgetItem *parent, int exceptData);
	void removeChildItemExcept(QTreeWidgetItem *parent, int *exceptDatas, int exceptNum);
	
	void getPointPositionFromTreeItem(QTreeWidgetItem *item, double *position);

	void drawPoint(double *pointPosition, double *pointColor, vtkSmartPointer<vtkActor> pointActor);
	void drawPlane(double *origin, double *point1, double *point2, double *color, double opacity, vtkSmartPointer<vtkActor> planeActor,
		double *frameColor, int xResolution, int yResolution, vtkSmartPointer<vtkActor> planeFrameActor);
	void drawLine(double *point1, double *point1Color, vtkSmartPointer<vtkActor> point1Actor,
		double *point2, double *point2Color, vtkSmartPointer<vtkActor> point2Actor, 
		float lineWidth, double *lineColor, vtkSmartPointer<vtkActor> lineActor);
	void drawIntersectCurve(vtkPolyData *data, double *planeCenter, double *planeNormal, double *color, double width, vtkSmartPointer<vtkActor> actor);
	void findLowerCuttingPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB,double *ProCenterB);//�ж˵�
	void findUpperConvexPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB,double *ProCenterB);//�����
	void findLowerClosestPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB);
	void init();

	void initDataWidget();
	void initFitPlane();
	void initPlaneByFittingCurve(double curvePlaneCenter[3], double curvePlaneNormal[3], 
		double (*fittingPoint)[3], bool *fittingPointStat, int fittingPointNum,
		double planePoint[3],
		int planeSize,
		double planeCenter[3], double planeNrmal[3], double planePoints[3][3]);

	void updatePointInfoItem(QTreeWidgetItem *item, double (*points)[3], bool *pointsState, int pointNum);

	void updateDiGrindToothScrowdSumDistance(QTreeWidgetItem *item);
	void updateDiToothBowScrowdSumDistance(QTreeWidgetItem *item);

	void updateAboSumScoreInfo(QTreeWidgetItem *item);

	void switchTopToothShow();
	void switchLowToothShow();

	void switchFitPlanePointShow();
	void switchFitPlaneShow();
	void switchGapPointShow();
	void switchGapProjectPointShow();
	
	void switchPointInVerticalPlaneShow();
	void switchVerticalPlaneShow();
	void switchTopToothPointForVerticalPlaneShow();
	void switchTopToothProjectPointForVerticalPlane();
	void switchTopToothPointForFitPlaneShow();
	void switchTopToothProjectPointForFitPlaneShow();
	void switchLowToothPointForVerticalPlaneShow();
	void switchLowToothProjectPointForVerticalPlaneShow();
	void switchLowToothPointForFitPlaneShow();
	void switchLowToothProjectPointForFitPlaneShow();

	//
	// ����
	//
	/*
	void switchSplinePointShow();
	void switchFittingCurveShow();
	void switchPointA0Show();
	void switchPlaneOShow();
	void switchPlanePShow();
	void switchCoverPOIntersectLineShow();
	void switchIntersectionPointsShow();
	*/
	void switchSplinePointShow();
	void switchFittingCurveShow();

	void switchCoverPointShow();
	void switchCoverPlaneOShow();
	void switchCoverIntersectCurveShow();
	void switchCoverIntersectCurvePointShow();

	//////////////
	///////�ղ���ҧ�� ��������

	void switchJCoverPointShow();
	void switchJCoverPlaneOShow();
	void switchJCoverIntersectCurveShow();
	void switchJCoverIntersectCurvePointShow();

	//
	// ���ϸ���
	//
	void switchInverseFitCoverShow();

	//
	// ����
	//

	void switchPointForArticlePlaneShow();
	void switchArticlePlaneShow();
	void switchFrontToothOpenCloseFittingPointShow();
	void switchFrontToothOpenCloseFittingCurveShow();
	void switchFrontToothPlaneOShow();
	void switchFrontToothContourCurveShaow();
	void switchFrontToothContiourCurvePointShow();
	void switchTopPointToProjectArticlePlaneShow();
	void switchLowPointToProjectArticlePlaneShow();
	void switchProjectArticlePlane();
	void switchFrontToothToProjectPointPositionA0Show();

	void switchBackToothOpenCloseFittingPointShow();
	void switchBackToothOpenCloseFittingCurveShow();
	void switchBackToothPointA0Show();
	void switchBackToothPlaneOShow();
	void switchBackToothContourCurveShaow();
	void switchBackToothContiourCurvePointShow();

	void switchPointInOverbiteParallelPlaneShow();
	void switchOverbiteParrelPlaneShow();
	void switchOverbiteIntersectCurveShow();

	void switchVerticalParallelPlaneInitPointShow();
	void switchVerticalParallelPlaneShow();
	void switchVerticalParallelPlaneInitPoint2Show();
	void switchVerticalParallelPlane2Show();
	void switchTwoPointToProjectFitPlaneShow();

	void switchCurrentSelectPointShow();

	//
	// DI����
	//
	/*
	void switchDiPointA0Show();
	void switchDiPlaneOShow();
	void switchDiPlanePShow();
	void switchDiCoverIntersectLineShow();
	*/
	
	void switchDiCoverPointShow();
	void switchDiCoverPlaneOShow();
	void switchDiCoverIntersectCurveShow();
	void switchDiCoverIntersectCurvePointShow();
	
	//
	// DI����
	//

	void switchDiOverbitePointShow();
	void switchDiOverbitePlaneOShow();
	void switchDiOverbiteIntersectCurveShow();
	void switchDiOverbiteIntersectCurvePointShow();

	//
	// DI�෽����
	//

	void switchDiSideOpenClosePointShow(int index);
	void switchDiSideOpenClosePlaneOShow(int index);
	void switchDiSideOpenCloseIntersectCurveShow(int index);
	void switchDiSideOpenCloseIntersectCurvePointShow(int index);

	//
	// DIǰ������
	//

	void switchDiFrontOpenClosePointShow(int index);
	void switchDiFrontOpenClosePlaneOShow(int index);
	void switchDiFrontOpenCloseIntersectCurveShow(int index);
	void switchDiFrontOpenCloseIntersectCurvePointShow(int index);

	//
	// DI�Ϲ�ϵ
	//

	void switchDiCloseRelationPointAShow();
	void switchDiCloseRelationPointBShow();
	void switchDiCloseRelationPlaneOShow();

	//
	// DI��������
	//

	void switchDiBackToothOppositeClosePointShow();
	void switchDiBackToothOppositeCloseProjectPointShow();

	//
	// DI����������
	//

	void switchDiBackToothLockClosePointShow();
	void switchDiBackToothLockCloseProjectPointShow();

	//
	// DI����->���߲���
	//

	void switchDiMiddleUnfitPointShow();

	//
	// DI����->����������϶
	//

	void switchDiMiddleCutToothGapShow();
	
	//
	// DIӵ����
	//

	void switchDiGrindToothScrowdToothShow(int index);
	void switchDiToothBowScrowdShow(int index);

	//
	// ABO-OGS����
	//

	void switchAboArrangePointShow(int index);
	void switchAboArrangeProjectPointShow(int index);
	void switchAboArrangePlaneOShow(int index);

	//
	// ABO-OGS��Ե�ո߶�
	//

	void switchAboEdgeVertebralHeighPointShow(int index);
	void switchAboEdgeVertebralHeighProjectPointShow(int index);

	//
	// ABO-OGS��������б��
	//

	void switchChapFitPlaneShow();
	void switchChapPointShow(int index);
	void switchChapProjectPointShow(int index);

	//
	// ABO-OGS���Ϲ�ϵ
	//

	void switchAboToothColoseRelationLowPointShow(int index);
	void switchAboToothCloseRelationPlaneOShow(int index);
	void switchAboToothCloseRelationTopPointShow(int index);

	//
	// ABO-OGS���ϽӴ�
	//

	void switchAboToothCloseTouchPointAShow(int index);
	void switchAboToothCloseTouchPointBShow(int index);
	void switchAboToothCloseTouchPlaneOShow(int index);
	void switchAboToothCloseTouchIntersectCurveShow(int index);

	//
	// ABO-OGS����
	//

	void switchAboCoverPointAShow(int index);
	void switchAboCoverPointBShow(int index);
	void switchAboCoverPlaneOShow(int index);
	void switchisCoverIntersectCurveShow(int index);

	//
	// ABO-OGS�����Ӵ���ϵ
	//

	void switchboTouchRelationPointShow(int index);
	void switchAboTouchRelationProjectPointShow(int index);

	//
	// ����
	//

	void switchMeasureDistanceLineToLine();

	//
	// ���ز������
	//

	void turnoffFitPlaneResult();
	void turnoffFaceSideResult();
	void turnoffCoverResult();
	void turnoffJCoverResult();
	void turnoffInverseFitCoverResult();
	void turnoffOverBiteResult();
	void turnoffOpenCloseResult();
	void turnoffMiddleLineResult();
	void turnoffMeasureResult();

	void turnoffDiCoverResult();
	void turnoffDiOverbiteResult();
	void turnoffDiSideOpenCloseResult();
	void turnoffDiFrontOpenCloseResult();
	void turnoffDiBackToothOppositeCloseResult();
	void turnoffDiBackToothLockCloseResult();
	void turnoffDiMiddleUnfitResult();
	void turnoffDiMiddleCutToothGapResult();
	void turnoffDiScrowdResult();
	void turnoffDiCloseRelationResult();


	void turnoffAboArrangeResult();
	void turnoffAboEdgeVertebralHeighResult();
	void turnoffChapResult();
	void turnoffToothCloseRelationResult();
	void turnoffToothCloseTouchResult();
	void turnoffAboCoverResult();
	void turnoffAboToothTouchRelationResult();

	void turnoffAllResult();

	//
	// ����ǰ������
	//

	// operatorType: 
	//		0: ��ʾ
	//		1: ����
	//		2: ѡ��
	//		3: ���ƽ��
	//		4: �������
	//		5: ɾ����
	//		6: ͶӰ��
	//		7: �÷�
	void setOperatePreCondition(int selectOperationTreeIndex, int operatorType);

	int getFitPointIndex(double x, double y, double z);
	void getGapPointIndex(double x, double y, double z, int *index);
	int getPointInVerticalPlaneIndex(double x, double y, double z);
	int getLowToothPointForVerticalPlaneIndex(double x, double y, double z);
	int getLowToothPointForFitPlaneIndex(double x, double y, double z);
	int getSplinePointIndex(double x, double y, double z);

	void initDiFrontOpenClosePointItem(QTreeWidgetItem *item);
	void initDiFrontOpenCloseMeasure(QTreeWidgetItem *parent, int currentMeasureIndex);
private:
	Ui::MainWindowClass ui;

	Engine *ep;

	QSqlDatabase db;

	QString path; // ����Ŀ¼

	QMenu *contexMenu;

	vtkRenderer* ren;

	PickInteractorStyle *pickStyle;

	int selectOptionTreeIndex;
	QTreeWidgetItem *currentOptionTreeWidgetItem;

	int isCurrentSelectPointShow;
	vtkSmartPointer<vtkActor> currentSelectPointActor;

	vector<OperateItem> OperateList;
	vector<OperateItem> InputOperateList;
	//
	// ��/����ģ��
	//
	QString lowerModelName, topModelName;
	vtkPolyData *lowerToothData, *topToothData, *toothData;
	double modelBound[6];
	vtkSmartPointer<vtkActor> lowerToothActor, topToothActor;
	int isLowToothShow, isTopToothShow;

	double pointSize;

	double pointlist[194][3];
	double ScoreSet[3][8];
	//
	//��������
	//
	//ƽ��
	vector<pair<int,MyPlane>> PlaneMapSet;//ƽ��
	int PlaneState[300];
	int PlaneCount;
	int PlaneNum;

	//ͶӰ��
	int projCount;
	double projPointPosition[300][3];
	vtkSmartPointer<vtkActor> projPointActor[300];
	double projPointState[300];
	vector<pair<int,double*>> projPointMapSet;

	//�и�����
	int cutCurveCount;
	vector<pair<int,MyCurve>> cutCurveMapSet;

	//����
	int VectorCount;
	vector<pair<int,MyVector>> VecMapSet;

	//
	//�Զ��ָ�
	//
	int pickerCount;
	double PickPointPosition[300][3];
	vtkSmartPointer<vtkActor> SegPointActor[300];
	double PickPointState[300];
	vector<pair<int,double*>> SegPointMap;


	//
	// ��ƽ��
	//

	bool fitPlanePointStat[20];
	double fitPlanePointPosition[20][3];
	vtkSmartPointer<vtkActor> fitPlanePointActor[20];
	int isFitPlanePointShow;
	double fitPlaneNormal[3], fitPlaneCenter[3];
	double fitPlanePoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> fitPlaneActor, fitPlaneFrameActor;
	int isFitPlaneShow;

	//ǰ��������
	bool gapPointStat[10][2];
	double gapPointPosition[10][2][3];
	vtkSmartPointer<vtkActor> gapPointActor[10][2];
	int isGapPointShow;
	double gapProjecPointPosition[10][2][3];
	vtkSmartPointer<vtkActor> gapProjectPointActor[10][2];
	int isGapProjectPointShow;
	double gapPointScoreT[10];
	double gapPointScore;

	double frontToothDistance, frontToothScore;

	//
	// �ղ���ҧ��
	//
	double faceSideVerticalScore, faceSideHorizonalScore,faceSideSagittalScore;
	double JVerticalFlag[8];


	// ����ʸ״��
	int pointInVerticalPlaneIndex;
	bool pointInVerticalPlaneStat[2];
	double pointInVerticalPlanePosition[2][3];
	double verticalPlaneCenter[3];
	double verticalPlanePoint[3][3]; // 0: origin
	double verticalPlaneNormal[3];
	vtkSmartPointer<vtkActor> pointInVerticalPlaneActor[2];
	int isPointInVerticalPlaneShow;
	vtkSmartPointer<vtkActor> verticalPlaneActor;
	vtkSmartPointer<vtkActor> verticalWireFramePlaneActor;
	int isVerticalPlaneShow;

	// ������ʸ״��
	int topToothPointForVerticalPlaneIndex;
	double topToothPointForVerticalPlanePosition[10][3], topToothProjectPointForVerticalPlanePosition[10][3];
	vtkSmartPointer<vtkActor> topToothPointForVerticalPlaneActor[16], topToothProjectPointForVerticalPlaneActor[16];
	int isTopToothPointForVerticalPlaneShow, isTopToothProjectPointForVerticalPlane;

	// ������ˮƽ��
	bool topToothPointForFitPlaneStat[16];
	double topToothPointForFitPlanePosition[16][3], topToothProjectPointForFitPlanePosition[16][3];
	vtkSmartPointer<vtkActor> topToothPointForFitPlaneActor[16], topToothProjectPointForFitPlaneActor[16];
	int isTopToothPointForFitPlaneShow, isTopToothProjectPointForFitPlaneShow;

	// ������ʸ״��
	bool lowToothPointForVerticalPlaneStat[10];
	int lowToothPointForVerticalPlaneIndex;
	double lowToothPointForVerticalPlanePosition[10][3], lowToothProjectPointForVerticalPlanePosition[10][3];
	vtkSmartPointer<vtkActor> lowToothPointForVerticalPlaneActor[10], lowToothProjectPointForVerticalPlaneActor[10];
	int isLowToothPointForVerticalPlaneShow, isLowToothProjectPointForVerticalPlaneShow;

	// ������ˮƽ��
	bool lowToothPointForFitPlaneStat[10];
	double lowToothPointForFitPlanePosition[10][3], lowToothProjectPointForFitPlanePosition[10][3];
	vtkSmartPointer<vtkActor> lowToothPointForFitPlaneActor[10], lowToothProjectPointForFitPlaneActor[10];
	int isLowToothPointForFitPlaneShow, isLowToothProjectPointForFitPlaneShow;

	//
	// ����
	//

	// �������
	int		isSplinePointShow, isSplineProjectPointShow;
	int		indexOfSplinePoint;
	bool	splinePointState[20];
	double	splintPointPosition[20][3], splineProjectPointPosition[20][3];
	vtkSmartPointer<vtkActor> splinePointActor[20], splineProjectPointActor[20];
	vtkSmartPointer<vtkActor> fittingCurveActor[40];
	int isFittingCurveShow;

	// ��������״�ཻƽ��
	double pointPositionA0[3], projectPointPositionA0[3];
	vtkSmartPointer<vtkActor> pointA0Actor, projectPointA0Actor;
	int isPointA0Show;
	//double pointPositionA[3];
	vtkSmartPointer<vtkActor> pointAActor; // ���ڻ���ƽ������
	double planeONormal[3];
	double planeOCenter[3];
	double planeOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> planeOActor, planeOWireframeActor;
	int isPlaneOShow;

	int isPlanePShow;
	vtkSmartPointer<vtkActor> planePActor, planePWireframeActor;
	double planePPoint[3][3]; // 0: origin

	vtkSmartPointer<vtkActor> coverPOIntersectLineActor;
	int isCoverPOIntersectLineShow;

	bool intersectionPointsStat[20];
	double intersectionPointsPosition[20][3];
	double selectedIntersectionPointPositon[3];
	int coverSelectIntersectPointIndex;
	vtkSmartPointer<vtkActor> coverSelectIntersectPointActor;
	vtkSmartPointer<vtkActor> intersectionPointsAction[20];
	int isIntersectionPointsShow;
	///////////////////(DI)
	double CoverPoint[3], CoverProjectPoint[3];
	vtkSmartPointer<vtkActor> CoverPointActor, CoverProjectPointActor;
	int isCoverPointShow;
	vtkSmartPointer<vtkActor> CoverCenterPointActor; // ���ڻ���ƽ������
	double CoverPlaneONormal[3];
	double CoverPlaneOCenter[3];
	double CoverPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> CoverPlaneOActor, CoverPlaneOWireframeActor;
	int isCoverPlaneOShow;

	vtkSmartPointer<vtkActor> CoverIntersectCurveActor, TopCoverIntersectCurveActor;
	int isParCoverIntersectCurveShow;

	double CoverIntersectCurvePoint[2][3],  CoverIntersectCurveProjectArticlePlanePoint[2][3], CoverIntersectCurveProjectFitPlanePoint[2][3];
	int CoverIntersectCurvePointIndex;
	vtkSmartPointer<vtkActor> CoverIntersectCurvePointActor[2];
	int isCoverIntersectCurvePointShow;
	double CoverDistance;

	double CoverScore;
	//
	//�ղ���ҧ�� ��������
	//
	double JCoverPoint[3], JCoverProjectPoint[3];
	vtkSmartPointer<vtkActor> JCoverPointActor, JCoverProjectPointActor;
	int isJCoverPointShow;
	vtkSmartPointer<vtkActor> JCoverCenterPointActor; // ���ڻ���ƽ������
	double JCoverPlaneONormal[3];
	double JCoverPlaneOCenter[3];
	double JCoverPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> JCoverPlaneOActor, JCoverPlaneOWireframeActor;
	int isJCoverPlaneOShow;

	vtkSmartPointer<vtkActor> JCoverIntersectCurveActor, TopJCoverIntersectCurveActor;
	int isParJCoverIntersectCurveShow;

	double JCoverIntersectCurvePoint[2][3],  JCoverIntersectCurveProjectArticlePlanePoint[2][3], JCoverIntersectCurveProjectFitPlanePoint[2][3];
	int JCoverIntersectCurvePointIndex;
	vtkSmartPointer<vtkActor> JCoverIntersectCurvePointActor[2];
	int isJCoverIntersectCurvePointShow;
	double JCoverDistance;

	double JCoverScore;
	//
	// ���ϸ���
	//

	int topLowIndex, topLowProjectIndex; // 0: top 1: low
	int pairIndex;

	bool inverseFitCoverPointStat[16];
	double inverseFitCoverPointPosition[16][3];
	vtkSmartPointer<vtkActor> inverseFitCoverPointActor[16];
	int isInverseFitCoverPointShow;

	double inverseFitConvertProjectPointPosition[16][3];
	vtkSmartPointer<vtkActor> inverseFitCoverProjectPointActor[16], inverseFitCoverProjectLineActor[8];
	int isInverseFitCoverProjectShow;

	double inverseFitCoverScore;

	//
	// ����
	//
	double pointInOverbiteParallelPlanePosition[3];
	vtkSmartPointer<vtkActor> pointForOverbitParallelPlaneActor;
	vtkSmartPointer<vtkActor> overbiteParallelPlaneActor, overbiteParallelPlaneWireFrameActor;
	vtkSmartPointer<vtkActor> overbiteIntersectCurveActor, topOverbiteIntersectCurveActor;
	int isPointInOverbiteParallelPlaneShow, isOverbiteParrelPlaneShow, isOverbiteIntersectCurveShow;
	double overbitIntersectCurveScore;
	double overbitOpenCloseScore;
	double overbitIntersectSumScore;
	//
	// ����
	//

	// ǰ������

	// ��״��
	double pointPositionForArticlePlane[3];
	double articlePlaneNormal[3],articlePlanePoint[3][3];;
	vtkSmartPointer<vtkActor> pointForArticleActor, articlePlaneActor, articalPlaneWireframeActor;
	int isPointForArticlePlaneShow, isArticlePlaneShow;

	bool topPointToProjectArticlePlaneStat[4][2];
	double topPointToProjectArticlePlanePosition[4][2][3];
	vtkSmartPointer<vtkActor> topPointToProjectArticlePlaneActor[4][2];
	int isTopPointToProjectArticlePlaneShow;

	bool lowPointToProjectArticlePlaneStat[4][2];
	double lowPointToProjectArticlePlanePosition[4][2][3];
	vtkSmartPointer<vtkActor> lowPointToProjectArticlePlaneActor[4][2];
	int isLowPointToProjectArticlePlaneShow;

	vtkSmartPointer<vtkActor> topPointProjectArticlePlaneActor[4][2], topLineProjectArticlePlaneActor[4];
	vtkSmartPointer<vtkActor> lowPointProjectArticlePlaneActor[4][2], lowLineProjectArticlePlaneActor[4];
	int isProjectArticlePlane;

	// �������
	bool frontToothOpenCloseFittingPointStat[20];
	double frontToothOpenCloseFittingPointPosition[20][3], frontToothOpenCloseFittingProjectPointPosition[20][3];
	vtkSmartPointer<vtkActor> frontToothOpenCloseFittingPointActor[20], frontToothOpenCloseFittingProjectPointActor[20], frontToothOpenCloseFittingCurveActor[40];
	int isFrontToothOpenCloseFittingPointShow, isFrontToothOpenCloseFittingCurveShow;

	// ��������״�ཻƽ��(ǰ������)
	double frontToothToProjectPointPositionA0[3], frontToothProjectPointPositionA0[3];
	vtkSmartPointer<vtkActor> frontToothToProjectPointA0Actor, frontToothProjectPointA0Actor;
	int idFrontToothToProjectPointPositionA0Show;
	double frontToothPlaneONormal[3];
	double frontToothPlaneOCenter[3];
	double frontToothPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> frontToothPlaneCenterActor, frontToothPlaneOActor, frontToothPlaneOWireframeActor;
	int isFrontToothPlaneOShow;

	vtkSmartPointer<vtkActor> frontToothContourCurveActor, frontToothTopContourCurveActor;
	int isFrontToothContourCurveShaow;

	int frontToothContourPointIndex;
	double frontToothContourCurvePointPosition[2][3];
	vtkSmartPointer<vtkActor> frontToothContourCurvePointActor[2];
	double distanceFrontToothContourCurvePoint;
	double FrontToothContourCurveScore;
	int isFrontToothContiourCurvePointShow;

	// ��������
	bool backToothOpenCloseFittingPointStat[20];
	double backToothOpenCloseFittingPointPosition[20][3], backToothOpenCloseFittingProjectPointPosition[20][3];
	vtkSmartPointer<vtkActor> backToothOpenCloseFittingPointActor[20], backToothOpenCloseFittingProjectPointActor[20], backToothOpenCloseFittingCurveActor[40];
	int isBackToothOpenCloseFittingPointShow, isBackToothOpenCloseFittingCurveShow;

	// ��������״�ཻƽ��(��������)
	double backToothPointPositionA0[3], packToothProjectPointPositionA0[3];
	vtkSmartPointer<vtkActor> backToothPointA0Actor, backToothProjectPointA0Actor;
	int isBackToothPointA0Show;
	double backToothPlaneONormal[3];
	double backToothPlaneOCenter[3];
	double backToothPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> backToothPlaneCenterActor, backToothPlaneOActor, backToothPlaneOWireframeActor;
	int isBackToothPlaneOShow;

	vtkSmartPointer<vtkActor> backToothContourCurveActor, backToothTopContourCurveActor;
	int isBackToothContourCurveShaow;

	int backToothContourPointIndex;
	double backToothContourCurvePointPosition[2][3];
	vtkSmartPointer<vtkActor> backToothContourCurvePointActor[2];
	double distanceBackToothContourCurvePoint;
	int isBackToothContiourCurvePointShow;

	//
	// ����
	//
	 
	// ʸ״ƽ����
	bool verticalParallelPlaneStat[2];

	double verticalParallelPlaneInitPointPosition[3];
	vtkSmartPointer<vtkActor> verticalParallelInitPointActor;
	int isVerticalParallelPlaneInitPointShow;
	vtkSmartPointer<vtkActor> verticalparallelPlaneActor, verticalParallelWireFramePlaneActor;
	int isVerticalParallelPlaneShow;
	double distanceToVerticalPlane;

	double verticalParallelPlaneInitPoint2Position[3];
	vtkSmartPointer<vtkActor> verticalParallelInitPoint2Actor;
	int isVerticalParallelPlaneInitPoint2Show;
	vtkSmartPointer<vtkActor> verticalparallelPlane2Actor, verticalParallelWireFramePlane2Actor;
	int isVerticalParallelPlane2Show;

	double distanceBetweenTwoVerticalPlane;

	int indexOfTwoSelectPointOnLowerModel;
	double towPointToProjectFitPlanePositon[2][3], towPointProjectFitPlanePositon[2][3];
	double distanceBetweenTwoProjectOnFitPlanePoint;
	vtkSmartPointer<vtkActor> towPointToProjectFitPlaneActor[2], towPointProjectFitPlaneActor[2];
	int isTwoPointToProjectFitPlaneShow, isTwoPointProjectFitPlaneShow;

	double middleLineScore;

	//
	// DI����
	//
	/*
	double diPointPositionA0[3], diProjectPointPositionA0[3];
	vtkSmartPointer<vtkActor> diPointA0Actor, diProjectPointA0Actor;
	int isDiPointA0Show;
	vtkSmartPointer<vtkActor> diPointAActor; // ���ڻ���ƽ������
	double diPlaneONormal[3];
	double diPlaneOCenter[3];
	double diPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diPlaneOActor, diPlaneOWireframeActor;
	int isDiPlaneOShow;

	int isDiPlanePShow;
	vtkSmartPointer<vtkActor> diPlanePActor, diPlanePWireframeActor;
	double diPlanePPoint[3][3]; // 0: origin

	vtkSmartPointer<vtkActor> diCoverIntersectLineActor;
	int isDiCoverIntersectLineShow;
	double diCoverDistance, diCoverScore;
	double diCoverIntersectPointPosition[3];
	vtkSmartPointer<vtkActor> diCoverIntersectPointActor;
	*/
	double diCoverPoint[3], diCoverProjectPoint[3];
	vtkSmartPointer<vtkActor> diCoverPointActor, diCoverProjectPointActor;
	int isDiCoverPointShow;
	vtkSmartPointer<vtkActor> diCoverCenterPointActor; // ���ڻ���ƽ������
	double diCoverPlaneONormal[3];
	double diCoverPlaneOCenter[3];
	double diCoverPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diCoverPlaneOActor, diCoverPlaneOWireframeActor;
	int isDiCoverPlaneOShow;

	vtkSmartPointer<vtkActor> diCoverIntersectCurveActor, diTopCoverIntersectCurveActor;
	int isDiCoverIntersectCurveShow;

	double diCoverIntersectCurvePoint[2][3],  diCoverIntersectCurveProjectArticlePlanePoint[2][3], diCoverIntersectCurveProjectFitPlanePoint[2][3];
	int diCoverIntersectCurvePointIndex;
	vtkSmartPointer<vtkActor> diCoverIntersectCurvePointActor[2];
	int isDiCoverIntersectCurvePointShow;
	double diCoverDistance;

	double diCoverScore;

	//
	// DI����
	//

	double diOverbitePoint[3], diOverbiteProjectPoint[3];
	vtkSmartPointer<vtkActor> diOverbitePointActor, diOverbiteProjectPointActor;
	int isDiOverbitePointShow;
	vtkSmartPointer<vtkActor> diOverbiteCenterPointActor; // ���ڻ���ƽ������
	double diOverbitePlaneONormal[3];
	double diOverbitePlaneOCenter[3];
	double diOverbitePlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diOverbitePlaneOActor, diOverbitePlaneOWireframeActor;
	int isDiOverbitePlaneOShow;

	vtkSmartPointer<vtkActor> diOverbiteIntersectCurveActor, diTopOverbiteIntersectCurveActor;
	int isDiOverbiteIntersectCurveShow;

	double diOverbiteIntersectCurvePoint[2][3],  diOverbiteIntersectCurveProjectArticlePlanePoint[2][3], diOverbiteIntersectCurveProjectFitPlanePoint[2][3];
	int diOverbiteIntersectCurvePointIndex;
	vtkSmartPointer<vtkActor> diOverbiteIntersectCurvePointActor[2];
	int isDiOverbiteIntersectCurvePointShow;
	double diOverbiteDistance;

	double diOverbiteScore;
	double diOverbiteFlag;

	//
	// DIǰ������
	//

	bool diFrontOpenClosePointStat[20];
	double diFrontOpenClosePoint[20][3], diFrontOpenCloseProjectPoint[20][3];
	vtkSmartPointer<vtkActor> diFrontOpenClosePointActor[20], diFrontOpenCloseProjectPointActor[20];
	int isDiFrontOpenClosePointShow[20];

	vtkSmartPointer<vtkActor> diFrontOpenCloseCenterPointActor[20]; // ���ڻ���ƽ������
	double diFrontOpenClosePlaneONormal[20][3];
	double diFrontOpenClosePlaneOCenter[20][3];
	double diFrontOpenClosePlaneOPoint[20][3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diFrontOpenClosePlaneOActor[20], diFrontOpenClosePlaneOWireframeActor[20];
	int isDiFrontOpenClosePlaneOShow[20];

	vtkSmartPointer<vtkActor> diFrontOpenCloseIntersectCurveActor[20], diTopFrontOpenCloseIntersectCurveActor[20];
	int isDiFrontOpenCloseIntersectCurveShow[20];

	double diFrontOpenCloseIntersectCurvePoint[20][2][3],  diFrontOpenCloseIntersectCurveProjectArticlePlanePoint[20][2][3], diFrontOpenCloseIntersectCurveProjectFitPlanePoint[20][2][3];
	int diFrontOpenCloseIntersectCurvePointIndex[20];
	vtkSmartPointer<vtkActor> diFrontOpenCloseIntersectCurvePointActor[20][2];
	int isDiFrontOpenCloseIntersectCurvePointShow[20];
	double diFrontOpenCloseDistance[20];
	double diFrontOpenCloseMeasureScore[20];
	bool diFrontOpenCloseFlag[6];
	double diFrontOpenCloseScore;

	//
	// DI�෽����
	//

	double diSideOpenClosePoint[10][3], diSideOpenCloseProjectPoint[10][3];
	vtkSmartPointer<vtkActor> diSideOpenClosePointActor[10], diSideOpenCloseProjectPointActor[10];
	int isDiSideOpenClosePointShow[10];
	int diSideOpenCloseToothNum;
	bool diSideOpenClosFlag[8];

	vtkSmartPointer<vtkActor> diSideOpenCloseCenterPointActor[10]; // ���ڻ���ƽ������
	double diSideOpenClosePlaneONormal[10][3];
	double diSideOpenClosePlaneOCenter[10][3];
	double diSideOpenClosePlaneOPoint[10][3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diSideOpenClosePlaneOActor[10], diSideOpenClosePlaneOWireframeActor[10];
	int isDiSideOpenClosePlaneOShow[10];

	vtkSmartPointer<vtkActor> diSideOpenCloseIntersectCurveActor[10], diTopSideOpenCloseIntersectCurveActor[10];
	int isDiSideOpenCloseIntersectCurveShow[10];

	double diSideOpenCloseIntersectCurvePoint[10][2][3],  diSideOpenCloseIntersectCurveProjectArticlePlanePoint[10][2][3], diSideOpenCloseIntersectCurveProjectFitPlanePoint[10][2][3];
	int diSideOpenCloseIntersectCurvePointIndex[10];
	vtkSmartPointer<vtkActor> diSideOpenCloseIntersectCurvePointActor[10][2];
	int isDiSideOpenCloseIntersectCurvePointShow[10];
	double diSideOpenCloseDistance[10];
	double diSideOpenCloseGrindToothScore[10];

	double diSideOpenCloseScore;

	//
	// DIӵ����
	//

	double diGrindToothScrowdPoint[24][3];
	int diGrindToothScrowdPointIndex[12];
	bool diGrindToothScrowdPointState[12];
	vtkSmartPointer<vtkActor> diGrindToothScrowdPointActor[24];
	int isDiGrindToothScrowdToothShow[12];
	double diGrindToothScrowdDistance[12], diGrindToothScrowdSumDistance;

	double diToothBowScrowdPoint[8][3];
	int diToothBowScrowdPointIndex[4];
	bool diToothBowScrowdPointState[4];
	vtkSmartPointer<vtkActor> diToothBowScrowdPointActor[8];
	int isDiToothBowScrowdShow[4];
	double diToothBowScrowdDistance[4], diToothBowScrowdSumDistance;

	double diScrowdDistance;
	double diScrowdScore;
	//
	// DI�Ϲ�ϵ
	//

	double diCloseRelationScore;

	double diCloseRelationPointA[3], diCloseRelationPointB[3];
	int isDiCloseRelationPointAShow, isDiCloseRelationPointBShow;
	vtkSmartPointer<vtkActor> diCloseRelationPointAActor, diCloseRelationPointBActor;

	double diCloseRelationProjectPointB[3];
	vtkSmartPointer<vtkActor> diCloseRelationProjectPointBActor;
	vtkSmartPointer<vtkActor> diCloseRelationPlanePointAActor; // ���ڻ���ƽ������
	double diCloseRelationPlaneONormal[3];
	double diCloseRelationPlaneOCenter[3];
	double diCloseRelationPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diCloseRelationPlaneOActor, diCloseRelationPlaneOWireframeActor;
	int isDiCloseRelationPlaneOShow;
	int DiCloseRelationType;
	double diCloseRelationDistance;

	//
	// DI��������
	//

	double diBackToothOppositeClosePoint[16][3], diBackToothOppositeCloseProjectPoint[16][3];
	bool diBackToothOppositeClosePointState[16];
	vtkSmartPointer<vtkActor> diBackToothOppositeClosePointActor[16], diBackToothOppositeCloseProjectPointActor[16];
	int isDiBackToothOppositeClosePointShow, isDiBackToothOppositeCloseProjectPointShow;
	double diBackToothOppositeCloseScore;

	//
	// DI����������
	//

	double diBackToothLockClosePoint[10][3], diBackToothLockCloseProjectPoint[10][3];
	bool diBackToothLockClosePointState[10];
	vtkSmartPointer<vtkActor> diBackToothLockClosePointActor[10], diBackToothLockCloseProjectPointActor[10];
	int isDiBackToothLockClosePointShow, isDiBackToothLockCloseProjectPointShow;
	double diBakcToothLockCloseScore;

	//
	// DI����->���߲���
	//

	double diMiddleUnfitPoint[2][3];
	int diMiddleUnfitPointIndex;
	int diMiddleUnfitPointShow;
	double diMiddleUnfitDistance;
	double diMiddleUnfitScore;
	vtkSmartPointer<vtkActor> diMiddleUnfitPointActor[2], diMiddleUnfitPlaneActor[2], diMiddleUnfitPlaneWireframeActor[2];

	//
	// DI����->����������϶
	//

	double diMiddleCutToothGapPoint[2][3];
	int diMiddleCutToothGapIndex;
	int isDiMiddleCutToothGapShow;
	double diMiddleCutToothGapDistance;
	double diMiddleCutToothGapScore;
	vtkSmartPointer<vtkActor> diMiddleCutToothGapPointActor[2];

	//
	// DI����->�÷�
	//

	double diOtherScore;

	//
	// ABO-OGS����
	//

	double aboPoints[26][2][3], aboArrangePointProjectToPlaneO[26][2][3], aboArrangePointProjectToFitPlanePoint[26][2][3];
	vtkSmartPointer<vtkActor> aboPointAcotor[26][2], aboArrangePointProjectToPlaneOAcotr[26][2], aboArrangePointProjectToFitPlane[26][2];
	int isAboArrangePointShow[26][2], isAboArrangeProjectPointShow[26][2];
	int aboArrangePointPickNum[26];
	double aboArrangePointPareDistance[26], aboArrangePointPareScore[26], aboArrangeScore;

	double aboArrangePlaneONormal[26][3];
	double aboArrangePlaneOCenter[26][3];
	double aboArrangePlaneOPoint[26][3][3]; // 0: origin
	vtkSmartPointer<vtkActor> aboArrangePlaneOActor[26], aboArrangePlaneOWireframeActor[26];
	int isAboArrangePlaneOShow[26];
	double aboArrangeScoreT[28];
	//
	// ABO-OGS��Ե�ո߶�
	//

	double aboEdgeVertebralHeighPoints[20][2][3], aboEdgeVertebralHeighPointProjectToMiddlePlane[20][2][3], aboEdgeVertebralHeighPointProjectToArticlePlane[20][2][3];
	vtkSmartPointer<vtkActor> aboEdgeVertebralHeighPointAcotor[20][2], aboEdgeVertebralHeighPointProjectToMiddlePlaneAcotr[20][2], aboEdgeVertebralHeighPointProjectToArticlePlaneActor[20][2];
	int isAboEdgeVertebralHeighPointShow[20][2], isAboEdgeVertebralHeighProjectPointShow[20][2];
	int aboEdgeVertebralHeighPointPickNum[20];
	double aboEdgeVertebralHeighPointPareDistance[20], aboEdgeVertebralHeighPointPareScore[20], aboEdgeVertebralHeighScore;
	double aboEdgeVertebralScoreT[28];
	//
	// ABO-OGS��������б��
	//
	double aboChapFitPlaneNormal[3];
	double aboChapFitPlaneCenter[3];
	double aboChapFitPlanePoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> aboChapFitPlaneActor, aboChapFitPlaneWireframeActor;
	int isChapFitPlaneShow;

	double aboChapPoints[14][2][3], aboChapProjectPoint[14][2][3];
	vtkSmartPointer<vtkActor> aboChapPointAcotor[5][2], aboChapProjectPointAcotr[14][2];
	int isChapPointShow[14][2], isChapProjectPointShow[14][2];
	int aboChapPointPickNum[14];
	double aboChapPointPareDistance[14], aboChapPointPareScore[14], aboChapScore;
	double aboChapScoreT[28];
	//
	// ABO-OGS���Ϲ�ϵ
	//

	double aboToothCloseRelationPoints[10][2][3];
	vtkSmartPointer<vtkActor> aboToothCloseRelationPointAcotor[10][2];
	int isAboToothCloseRelationPointShow[10][2];
	int aboToothCloseRelationPointPickNum[10];
	double aboToothCloseRelationPointPlaneDistance[10], aboToothCloseRelationMeasureScore[10], aboToothCloseRelationScore;

	double aboToothCloseRelationPlaneONormal[10][3];
	double aboToothCloseRelationPlaneOCenter[10][3];
	double aboToothCloseRelationPlaneOPoint[10][3][3]; // 0: origin
	vtkSmartPointer<vtkActor> aboToothCloseRelationPlaneOActor[10], aboToothCloseRelationPlaneOWireframeActor[10];
	int isAboToothCloseRelationPlaneOShow[10];

	//
	// ABO-OGS���ϽӴ�
	//

	double aboToothCloseTouchPoints[20][2][3];
	vtkSmartPointer<vtkActor> aboToothCloseTouchPointAcotor[20][2];
	int isAboToothCloseTouchPointShow[20][2];
	double aboToothCloseTouchMeasureDistance[20], aboToothCloseTouchMeasureScore[20], aboToothCloseTouchScore;

	double aboToothCloseTouchPlaneONormal[20][3];
	double aboToothCloseTouchPlaneOCenter[20][3];
	double aboToothCloseTouchPlaneOPoint[20][3][3]; // 0: origin
	vtkSmartPointer<vtkActor> aboToothCloseTouchPlaneOActor[20], aboToothCloseTouchPlaneOWireframeActor[20];
	int isAboToothCloseTouchPlaneOShow[20];

	vtkSmartPointer<vtkActor> aboToothCloseTouchTopIntersectCurveActor[20], aboToothCloseTouchLowIntersectCurveActor[20];
	int isAboToothCloseTouchIntersectCurveShow[20];

	//
	// ABO-OGS����
	//

	double aboCoverPoints[18][2][3]; // 0: ����		1: ����
	vtkSmartPointer<vtkActor> aboCoverAcotor[18][2];
	int isAboCoverPointShow[18][2];
	double aboTCoverMeasureDistance[18], aboCoverMeasureScore[18], aboCoverScore;

	double aboCoverPlaneONormal[18][3];
	double aboCoverPlaneOCenter[18][3];
	double aboCoverPlaneOPoint[18][3][3]; // 0: origin
	vtkSmartPointer<vtkActor> aboCoverPlaneOActor[18], aboCoverPlaneOWireframeActor[18];
	int isAboCoverPlaneOShow[18];

	vtkSmartPointer<vtkActor> aboCoverIntersectCurveActor[18], aboCoverLowIntersectCurveActor[18];
	int isCoverIntersectCurveShow[18][2];

	//
	// ABO-OGS�����Ӵ���ϵ
	//

	double aboTouchRelationPoints[26][2][3], aboTouchRelationProjectPoints[26][2][3];
	vtkSmartPointer<vtkActor> aboTouchRelationPointAcotor[26][2], aboTouchRelationProjectPointAcotor[26][2];
	int isAboTouchRelationPointShow[26][2], isAboTouchRelationProjectPointShow[26][2];
	int aboTouchRelationPointNum[26];
	double aboTouchRelationMeasureDistance[26], aboTouchRelationMeasureScore[26], aboTouchRelationScore;
	double aboTouchRelationScoreT[28];

	//
	// ABO-OGS�÷�
	//

	double aboScore;

	//
	// ����
	//

	// ��-��
	int measureDistancePointToPointIndex;
	double measureDistancePointToPointPositions[2][3];
	vtkSmartPointer<vtkActor> measureDistancePointToPointPointActor[2], measureDistancePointToPointLineActor;
	int ismeasureDistancePointToPointShow;

	//
	// ����
	//

	// ��ɫ
	double pickPointColor[3]; // ������ѡ�����ɫ
	double projectToFittingPlanePointColot[3]; // ������ѡ���ں�ƽ����ͶӰ����ɫ
	double topProjectToFittingPlanePointColot[3]; // ������ѡ���ں�ƽ����ͶӰ����ɫ
	double planeColor[3]; // ƽ�����ɫ
	double frameColor[3]; // ƽ�����߿����ɫ
	double intersectLlineColor[3]; // ƽ���ֱཻ�ߵ���ɫ
	double intersectCurveColor[3]; // ��������������ɫ
	double topIntersectCurveColor[3]; // ��������������ɫ
};

#endif // MAINWINDOW_H
