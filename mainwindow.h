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
	void connectDatabase();	// 在初始化阶段连接数据库
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

	// 重叠测量部分
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
	// 菜单
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
	//基本测量部分
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
	// 操作栏
	//

	void clickOptionTreeItem(QTreeWidgetItem*, int);
	
	//
	// 右键菜单
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

	QGroupBox *groupBox3D, *groupBox2D;//三维 二维groupbox

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
	// 111: 牙列
	// 1: 上牙列
	// 2: 下牙列
	// 1500: ABO-OGS指数测量
	// 1501: ABO-OGS排列
	// 1502: ABO-OGS排列->点
	// 1503: ABO-OGS排列->得分
	// 1510--: ABO-OGS排列->点->点对1/2/.../5
	// 112: DI指数测量
	// 113: 平面
	// 114: DI覆盖
	// 118: DI覆盖->点
	// 115: DI覆盖->平面
	// 119: DI覆盖->平面->牙弓放射状相交平面
	// 123: DI覆盖->平面->牙弓放射状相交平面->点
	// 124: DI覆盖->平面->牙弓放射状相交平面->法向量
	// 120: DI覆盖->平面->合平面平行平面
	// 125: DI覆盖->平面->合平面平行平面->点
	// 126: DI覆盖->平面->合平面平行平面->法向量
	// 116: DI覆盖->交线
	// 121: DI覆盖->交线->交点A
	// 122: DI覆盖->交线->交点B
	// 127: DI覆盖->交线->距离
	// 117: DI覆盖->得分

	// 118: DI覆盖
	// 119: DI覆盖->点
	// 120: DI覆盖->牙弓放射状相交平面
	// 123: DI覆盖->牙弓放射状相交平面->点
	// 124: DI覆盖->牙弓放射状相交平面->法向量
	// 121: DI覆盖->轮廓曲线
	// 125: DI覆盖->轮廓曲线->点
	// 126: DI覆盖->轮廓曲线->距离
	// 122: DI覆盖->得分

	// 128: DI覆合
	// 129: DI覆合->点
	// 130: DI覆合->牙弓放射状相交平面
	// 133: DI覆合->牙弓放射状相交平面->点
	// 134: DI覆合->牙弓放射状相交平面->法向量
	// 131: DI覆合->轮廓曲线
	// 135: DI覆合->轮廓曲线->点
	// 136: DI覆合->轮廓曲线->距离
	// 132: DI覆合->得分

	// 137: DI侧方开合
	// 140: DI侧方开合->磨牙1
	// 160: DI侧方开合->磨牙2
	// 180: DI侧方开合->磨牙3
	// 141: DI侧方开合->磨牙1->点
	// 161: DI侧方开合->磨牙2->点
	// 181: DI侧方开合->磨牙3->点
	// 142: DI侧方开合->磨牙1->牙弓放射状相交平面
	// 145: DI侧方开合->磨牙1->牙弓放射状相交平面->点
	// 146: DI侧方开合->磨牙1->牙弓放射状相交平面->法向量
	// 162: DI侧方开合->磨牙2->牙弓放射状相交平面
	// 165: DI侧方开合->磨牙2->牙弓放射状相交平面->点
	// 166: DI侧方开合->磨牙2->牙弓放射状相交平面->法向量
	// 182: DI侧方开合->磨牙3->牙弓放射状相交平面
	// 185: DI侧方开合->磨牙3->牙弓放射状相交平面->点
	// 186: DI侧方开合->磨牙3->牙弓放射状相交平面->法向量
	// 143: DI侧方开合->磨牙1->轮廓曲线
	// 147: DI侧方开合->磨牙1->轮廓曲线->点
	// 148: DI侧方开合->磨牙1->轮廓曲线->距离
	// 163: DI侧方开合->磨牙2->轮廓曲线
	// 167: DI侧方开合->磨牙2->轮廓曲线->点
	// 168: DI侧方开合->磨牙2->轮廓曲线->距离
	// 183: DI侧方开合->磨牙3->轮廓曲线
	// 187: DI侧方开合->磨牙3->轮廓曲线->点
	// 188: DI侧方开合->磨牙3->轮廓曲线->距离
	// 144: DI侧方开合->磨牙1->得分
	// 164: DI侧方开合->磨牙2->得分
	// 184: DI侧方开合->磨牙3->得分
	// 200: DI侧方开合->得分
	// 400: DI前牙开合
	// 401: DI前牙开合->点
	// 402: DI前牙开合->得分
	// 420--820: DI前牙开合->测量1/2/.../19/20
	// 420: DI前牙开合->测量1
	// 421: DI前牙开合->测量1->点
	// 422: DI前牙开合->测量1->牙弓放射状相交平面
	// 423: DI前牙开合->测量1->轮廓曲线
	// 424: DI前牙开合->测量1->得分
	// 425: DI前牙开合->测量1->牙弓放射状相交平面->点
	// 426: DI前牙开合->测量1->牙弓放射状相交平面->法向量
	// 427: DI前牙开合->测量1->轮廓曲线->点
	// 428: DI前牙开合->测量1->轮廓曲线->距离
	// 900: DI后牙反合
	// 901: DI后牙反合->点
	// 902: DI后牙反合->投影
	// 903: DI后牙反合->得分
	// 904: DI后牙正锁合
	// 905: DI后牙正锁合->点
	// 906: DI后牙正锁合->投影
	// 907: DI后牙正锁合->得分
	// 1000: DI其他
	// 1001: DI其他->中线不调
	// 1002: DI其他->中线不调->点
	// 1003: DI其他->中线不调->距离
	// 1004: DI其他->中线不调->得分
	// 1005: DI其他->中切牙牙间隙
	// 1006: DI其他->中切牙牙间隙->点
	// 1007: DI其他->中切牙牙间隙->距离
	// 1008: DI其他->中切牙牙间隙->得分
	// 1009: DI其他->得分
	// 1020: DI拥挤度
	// 1021: DI拥挤度->磨牙
	// 1022: DI拥挤度->牙弓
	// 1023: DI拥挤度->得分
	// 1024: DI拥挤度->距离
	// 1030--1270: DI拥挤度->磨牙->测量1/../测量12
	// 1030: DI拥挤度->磨牙->测量1
	// 1031: DI拥挤度->磨牙->测量1->点
	// 1032: DI拥挤度->磨牙->测量1->距离
	// 1028: DI拥挤度->磨牙->距离
	// 1270--1350: DI拥挤度->牙弓->测量1/../测量4
	// 1270: DI拥挤度->牙弓->测量1
	// 1271: DI拥挤度->牙弓->测量1->点
	// 1272: DI拥挤度->牙弓->测量1->距离	
	// 1029: DI拥挤度->牙弓->距离
	// 1370: DI合关系
	// 1371: DI合关系->得分
	// 1372: DI合关系->测量
	// 1373: DI合关系->测量->点A
	// 1374: DI合关系->测量->点B
	// 1375: DI合关系->测量->牙弓放射状相交平面
	// 1376: DI合关系->测量->距离
	// 1377: DI合关系->测量->牙弓放射状相交平面->点
	// 1378: DI合关系->测量->牙弓放射状相交平面->法向量
	// 3: 前牙区不齐
	// 4: 前牙区不齐->合平面
	// 5: 前牙区不齐->合平面->拟合点
	// 6: 前牙区不齐->合平面->法向量
	// 7: 前牙区不齐->合平面->中心
	// 8: 前牙区不齐->接触点
	// 9: 前牙区不齐->接触点->缝隙1
	// 10: 前牙区不齐->接触点->缝隙2
	// 11: 前牙区不齐->接触点->缝隙3
	// 12: 前牙区不齐->接触点->缝隙4
	// 13: 前牙区不齐->接触点->缝隙5
	// 14: 前牙区不齐->接触点->投影结果
	// 15: 前牙区不齐->得分
	// 16: 前牙区不齐->得分->前牙区不齐距离
	// 17: 前牙区不齐->得分->分值
	// 18: 颊侧区咬合
	// 19: 颊侧区咬合->矢装向得分
	// 20: 颊侧区咬合->上牙列
	// 21: 颊侧区咬合->下牙列
	// 22: 颊侧区咬合->水平向得分
	// 23: 颊侧区咬合->下牙列->正中矢状面
	// 24: 颊侧区咬合->下牙列->点(垂直向)
	// 25: 颊侧区咬合->下牙列->点(水平向)
	// 26: 颊侧区咬合->下牙列->正中矢状面->初始点
	// 27: 颊侧区咬合->下牙列->正中矢状面->法向量
	// 28: 颊侧区咬合->上牙列->点(垂直向)
	// 29: 颊侧区咬合->上牙列->点(水平向)
	// 30: 颊侧区咬合->上牙列->点(垂直向)->投影结果
	// 31: 颊侧区咬合->上牙列->点(水平向)->投影结果
	// 32: 颊侧区咬合->下牙列->点(垂直向)->投影结果
	// 33: 颊侧区咬合->下牙列->点(水平向)->投影结果
	// 34: 中线
	// 35: 中线->矢状平行面1
	// 97: 中线->矢状平行面2
	// 98: 中线->距离
	// 36: 中线->矢状平行面1->初始点
	// 37: 中线->矢状平行面1->法向量
	// 38: 中线->矢状平行面->距离
	// 99: 中线->矢状平行面2->初始点
	// 100: 中线->矢状平行面2->法向量
	// 39: 中线->点
	// 40: 中线->点->投影结果
	// 41: 中线->点->投影结果->距离
	// 42: 中线->得分
	// 43: 覆合
	// 44: 覆合->轮廓曲线
	// 45: 覆合->轮廓曲线->相交平面
	// 46: 覆合->轮廓曲线->相交平面->初始点
	// 47: 覆合->轮廓曲线->相交平面->法向量
	// 48: 覆合->得分
	// 49: 反合覆盖
	// 50: 反合覆盖->上牙列
	// 51: 反合覆盖->上牙列->点对1
	// 52: 反合覆盖->上牙列->点对2
	// 53: 反合覆盖->上牙列->点对3
	// 54: 反合覆盖->上牙列->点对4
	// 55: 反合覆盖->得分
	// 60: 反合覆盖->下牙列
	// 56: 反合覆盖->下牙列->点对1
	// 57: 反合覆盖->下牙列->点对2
	// 58: 反合覆盖->下牙列->点对3
	// 59: 反合覆盖->下牙列->点对4
	// 61: 开合
	// 62: 开合->后牙开合
	// 63: 开合->后牙开合->拟合曲线
	// 64: 开合->后牙开合->拟合曲线->点


	// 68: 覆盖
	// 69: 覆盖->点
	// 70: 覆盖->牙弓放射状相交平面
	// 73: 覆盖->牙弓放射状相交平面->点
	// 74: 覆盖->牙弓放射状相交平面->法向量
	// 71: 覆盖->轮廓曲线
	// 75: 覆盖->轮廓曲线->点
	// 76: 覆盖->轮廓曲线->距离
	// 72: 覆盖->得分


	// 78: 开合->后牙开合->牙弓放射状相交平面
	// 79: 开合->后牙开合->牙弓放射状相交平面->点
	// 80: 开合->后牙开合->牙弓放射状相交平面->法向量
	// 81: 开合->后牙开合->牙弓放射状相交平面->轮廓线
	// 82: 开合->后牙开合->牙弓放射状相交平面->轮廓线->点
	// 83: 开合->后牙开合->牙弓放射状相交平面->轮廓线->距离
	// 84: 开合->前牙开合
	// 85: 开合->前牙开合->冠状面
	// 86: 开合->前牙开合->冠状面->初始点
	// 87: 开合->前牙开合->冠状面->法向量
	// 88: 开合->前牙开合->冠状面->点
	// 101: 开合->前牙开合->冠状面->点->上牙列
	// 103: 开合->前牙开合->冠状面->点->上牙列->点对1
	// 104: 开合->前牙开合->冠状面->点->上牙列->点对2
	// 105: 开合->前牙开合->冠状面->点->上牙列->点对3
	// 106: 开合->前牙开合->冠状面->点->上牙列->点对4
	// 102: 开合->前牙开合->冠状面->点->下牙列
	// 107: 开合->前牙开合->冠状面->点->下牙列->点对1
	// 108: 开合->前牙开合->冠状面->点->下牙列->点对2
	// 109: 开合->前牙开合->冠状面->点->下牙列->点对3
	// 110: 开合->前牙开合->冠状面->点->下牙列->点对4
	// 89: 开合->前牙开合->拟合曲线
	// 90: 开合->前牙开合->牙弓放射状相交平面
	// 91: 开合->前牙开合->拟合曲线->点
	// 92: 开合->前牙开合->牙弓放射状相交平面->点
	// 93: 开合->前牙开合->牙弓放射状相交平面->法向量
	// 94: 开合->前牙开合->牙弓放射状相交平面->轮廓线
	// 95: 开合->前牙开合->牙弓放射状相交平面->轮廓线->点
	// 96: 开合->前牙开合->牙弓放射状相交平面->轮廓线->距离
	//
	// 支持隐藏/显示操作的包括: 1, 2, 4, 5, 8, 14, 23, 26, 28, 30, 29, 31, 24, 32, 25, 33, 35, 36, 39, 
	//							44, 45, 46, 49, 63, 64, 66, 67, 69, 70, 72, 71, 76, 78, 79, 81, 82, 86, 
	//							85, 91, 89, 90, 94, 95					
	// 支持选点操作: 4, 9, 10, 11, 12, 13, 23, 28, 29, 24, 25, 35, 39, 45, 51, 52, 53, 54, 56, 57, 58, 
	//				 59, 64, 67, 72, 76, 79, 82, 86, 91, 95
	// 支持拟合平面操作: 4
	// 支持点删除操作: 5的子节点
	// 支持投影操作: 8(将接触点投影到合平面), 31, 32, 33, 40, 50, 60
	// 支持得分操作: 15, 19, 22, 42, 48, 55
	// 支持拟合曲线操作: 63, 66, 89
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
	void findLowerCuttingPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB,double *ProCenterB);//切端点
	void findUpperConvexPoint(vtkSmartPointer<vtkActor> CurveActor,double *pA,double *pB,double *ProCenterB);//上最凹点
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
	// 覆盖
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
	///////颊侧区咬合 测量部分

	void switchJCoverPointShow();
	void switchJCoverPlaneOShow();
	void switchJCoverIntersectCurveShow();
	void switchJCoverIntersectCurvePointShow();

	//
	// 反合覆盖
	//
	void switchInverseFitCoverShow();

	//
	// 开合
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
	// DI覆盖
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
	// DI覆合
	//

	void switchDiOverbitePointShow();
	void switchDiOverbitePlaneOShow();
	void switchDiOverbiteIntersectCurveShow();
	void switchDiOverbiteIntersectCurvePointShow();

	//
	// DI侧方开合
	//

	void switchDiSideOpenClosePointShow(int index);
	void switchDiSideOpenClosePlaneOShow(int index);
	void switchDiSideOpenCloseIntersectCurveShow(int index);
	void switchDiSideOpenCloseIntersectCurvePointShow(int index);

	//
	// DI前牙开合
	//

	void switchDiFrontOpenClosePointShow(int index);
	void switchDiFrontOpenClosePlaneOShow(int index);
	void switchDiFrontOpenCloseIntersectCurveShow(int index);
	void switchDiFrontOpenCloseIntersectCurvePointShow(int index);

	//
	// DI合关系
	//

	void switchDiCloseRelationPointAShow();
	void switchDiCloseRelationPointBShow();
	void switchDiCloseRelationPlaneOShow();

	//
	// DI后牙反合
	//

	void switchDiBackToothOppositeClosePointShow();
	void switchDiBackToothOppositeCloseProjectPointShow();

	//
	// DI后牙正锁合
	//

	void switchDiBackToothLockClosePointShow();
	void switchDiBackToothLockCloseProjectPointShow();

	//
	// DI其他->中线不调
	//

	void switchDiMiddleUnfitPointShow();

	//
	// DI其他->中切牙牙间隙
	//

	void switchDiMiddleCutToothGapShow();
	
	//
	// DI拥挤度
	//

	void switchDiGrindToothScrowdToothShow(int index);
	void switchDiToothBowScrowdShow(int index);

	//
	// ABO-OGS排列
	//

	void switchAboArrangePointShow(int index);
	void switchAboArrangeProjectPointShow(int index);
	void switchAboArrangePlaneOShow(int index);

	//
	// ABO-OGS边缘嵴高度
	//

	void switchAboEdgeVertebralHeighPointShow(int index);
	void switchAboEdgeVertebralHeighProjectPointShow(int index);

	//
	// ABO-OGS颊舌向倾斜度
	//

	void switchChapFitPlaneShow();
	void switchChapPointShow(int index);
	void switchChapProjectPointShow(int index);

	//
	// ABO-OGS牙合关系
	//

	void switchAboToothColoseRelationLowPointShow(int index);
	void switchAboToothCloseRelationPlaneOShow(int index);
	void switchAboToothCloseRelationTopPointShow(int index);

	//
	// ABO-OGS牙合接触
	//

	void switchAboToothCloseTouchPointAShow(int index);
	void switchAboToothCloseTouchPointBShow(int index);
	void switchAboToothCloseTouchPlaneOShow(int index);
	void switchAboToothCloseTouchIntersectCurveShow(int index);

	//
	// ABO-OGS覆盖
	//

	void switchAboCoverPointAShow(int index);
	void switchAboCoverPointBShow(int index);
	void switchAboCoverPlaneOShow(int index);
	void switchisCoverIntersectCurveShow(int index);

	//
	// ABO-OGS邻牙接触关系
	//

	void switchboTouchRelationPointShow(int index);
	void switchAboTouchRelationProjectPointShow(int index);

	//
	// 测量
	//

	void switchMeasureDistanceLineToLine();

	//
	// 隐藏操作结果
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
	// 操作前置条件
	//

	// operatorType: 
	//		0: 显示
	//		1: 隐藏
	//		2: 选点
	//		3: 拟合平面
	//		4: 拟合曲线
	//		5: 删除点
	//		6: 投影点
	//		7: 得分
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

	QString path; // 运行目录

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
	// 上/下牙模型
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
	//基本测量
	//
	//平面
	vector<pair<int,MyPlane>> PlaneMapSet;//平面
	int PlaneState[300];
	int PlaneCount;
	int PlaneNum;

	//投影点
	int projCount;
	double projPointPosition[300][3];
	vtkSmartPointer<vtkActor> projPointActor[300];
	double projPointState[300];
	vector<pair<int,double*>> projPointMapSet;

	//切割曲线
	int cutCurveCount;
	vector<pair<int,MyCurve>> cutCurveMapSet;

	//向量
	int VectorCount;
	vector<pair<int,MyVector>> VecMapSet;

	//
	//自动分割
	//
	int pickerCount;
	double PickPointPosition[300][3];
	vtkSmartPointer<vtkActor> SegPointActor[300];
	double PickPointState[300];
	vector<pair<int,double*>> SegPointMap;


	//
	// 合平面
	//

	bool fitPlanePointStat[20];
	double fitPlanePointPosition[20][3];
	vtkSmartPointer<vtkActor> fitPlanePointActor[20];
	int isFitPlanePointShow;
	double fitPlaneNormal[3], fitPlaneCenter[3];
	double fitPlanePoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> fitPlaneActor, fitPlaneFrameActor;
	int isFitPlaneShow;

	//前牙区不齐
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
	// 颊侧区咬合
	//
	double faceSideVerticalScore, faceSideHorizonalScore,faceSideSagittalScore;
	double JVerticalFlag[8];


	// 正中矢状面
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

	// 上牙列矢状向
	int topToothPointForVerticalPlaneIndex;
	double topToothPointForVerticalPlanePosition[10][3], topToothProjectPointForVerticalPlanePosition[10][3];
	vtkSmartPointer<vtkActor> topToothPointForVerticalPlaneActor[16], topToothProjectPointForVerticalPlaneActor[16];
	int isTopToothPointForVerticalPlaneShow, isTopToothProjectPointForVerticalPlane;

	// 上牙列水平向
	bool topToothPointForFitPlaneStat[16];
	double topToothPointForFitPlanePosition[16][3], topToothProjectPointForFitPlanePosition[16][3];
	vtkSmartPointer<vtkActor> topToothPointForFitPlaneActor[16], topToothProjectPointForFitPlaneActor[16];
	int isTopToothPointForFitPlaneShow, isTopToothProjectPointForFitPlaneShow;

	// 下牙列矢状向
	bool lowToothPointForVerticalPlaneStat[10];
	int lowToothPointForVerticalPlaneIndex;
	double lowToothPointForVerticalPlanePosition[10][3], lowToothProjectPointForVerticalPlanePosition[10][3];
	vtkSmartPointer<vtkActor> lowToothPointForVerticalPlaneActor[10], lowToothProjectPointForVerticalPlaneActor[10];
	int isLowToothPointForVerticalPlaneShow, isLowToothProjectPointForVerticalPlaneShow;

	// 下牙列水平向
	bool lowToothPointForFitPlaneStat[10];
	double lowToothPointForFitPlanePosition[10][3], lowToothProjectPointForFitPlanePosition[10][3];
	vtkSmartPointer<vtkActor> lowToothPointForFitPlaneActor[10], lowToothProjectPointForFitPlaneActor[10];
	int isLowToothPointForFitPlaneShow, isLowToothProjectPointForFitPlaneShow;

	//
	// 覆盖
	//

	// 拟合曲线
	int		isSplinePointShow, isSplineProjectPointShow;
	int		indexOfSplinePoint;
	bool	splinePointState[20];
	double	splintPointPosition[20][3], splineProjectPointPosition[20][3];
	vtkSmartPointer<vtkActor> splinePointActor[20], splineProjectPointActor[20];
	vtkSmartPointer<vtkActor> fittingCurveActor[40];
	int isFittingCurveShow;

	// 牙弓放射状相交平面
	double pointPositionA0[3], projectPointPositionA0[3];
	vtkSmartPointer<vtkActor> pointA0Actor, projectPointA0Actor;
	int isPointA0Show;
	//double pointPositionA[3];
	vtkSmartPointer<vtkActor> pointAActor; // 用于绘制平面中心
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
	vtkSmartPointer<vtkActor> CoverCenterPointActor; // 用于绘制平面中心
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
	//颊侧区咬合 测量部分
	//
	double JCoverPoint[3], JCoverProjectPoint[3];
	vtkSmartPointer<vtkActor> JCoverPointActor, JCoverProjectPointActor;
	int isJCoverPointShow;
	vtkSmartPointer<vtkActor> JCoverCenterPointActor; // 用于绘制平面中心
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
	// 反合覆盖
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
	// 覆合
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
	// 开合
	//

	// 前牙开合

	// 冠状面
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

	// 拟合曲线
	bool frontToothOpenCloseFittingPointStat[20];
	double frontToothOpenCloseFittingPointPosition[20][3], frontToothOpenCloseFittingProjectPointPosition[20][3];
	vtkSmartPointer<vtkActor> frontToothOpenCloseFittingPointActor[20], frontToothOpenCloseFittingProjectPointActor[20], frontToothOpenCloseFittingCurveActor[40];
	int isFrontToothOpenCloseFittingPointShow, isFrontToothOpenCloseFittingCurveShow;

	// 牙弓放射状相交平面(前牙开合)
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

	// 后牙开合
	bool backToothOpenCloseFittingPointStat[20];
	double backToothOpenCloseFittingPointPosition[20][3], backToothOpenCloseFittingProjectPointPosition[20][3];
	vtkSmartPointer<vtkActor> backToothOpenCloseFittingPointActor[20], backToothOpenCloseFittingProjectPointActor[20], backToothOpenCloseFittingCurveActor[40];
	int isBackToothOpenCloseFittingPointShow, isBackToothOpenCloseFittingCurveShow;

	// 牙弓放射状相交平面(后牙开合)
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
	// 中线
	//
	 
	// 矢状平行面
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
	// DI覆盖
	//
	/*
	double diPointPositionA0[3], diProjectPointPositionA0[3];
	vtkSmartPointer<vtkActor> diPointA0Actor, diProjectPointA0Actor;
	int isDiPointA0Show;
	vtkSmartPointer<vtkActor> diPointAActor; // 用于绘制平面中心
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
	vtkSmartPointer<vtkActor> diCoverCenterPointActor; // 用于绘制平面中心
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
	// DI覆合
	//

	double diOverbitePoint[3], diOverbiteProjectPoint[3];
	vtkSmartPointer<vtkActor> diOverbitePointActor, diOverbiteProjectPointActor;
	int isDiOverbitePointShow;
	vtkSmartPointer<vtkActor> diOverbiteCenterPointActor; // 用于绘制平面中心
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
	// DI前牙开合
	//

	bool diFrontOpenClosePointStat[20];
	double diFrontOpenClosePoint[20][3], diFrontOpenCloseProjectPoint[20][3];
	vtkSmartPointer<vtkActor> diFrontOpenClosePointActor[20], diFrontOpenCloseProjectPointActor[20];
	int isDiFrontOpenClosePointShow[20];

	vtkSmartPointer<vtkActor> diFrontOpenCloseCenterPointActor[20]; // 用于绘制平面中心
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
	// DI侧方开合
	//

	double diSideOpenClosePoint[10][3], diSideOpenCloseProjectPoint[10][3];
	vtkSmartPointer<vtkActor> diSideOpenClosePointActor[10], diSideOpenCloseProjectPointActor[10];
	int isDiSideOpenClosePointShow[10];
	int diSideOpenCloseToothNum;
	bool diSideOpenClosFlag[8];

	vtkSmartPointer<vtkActor> diSideOpenCloseCenterPointActor[10]; // 用于绘制平面中心
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
	// DI拥挤度
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
	// DI合关系
	//

	double diCloseRelationScore;

	double diCloseRelationPointA[3], diCloseRelationPointB[3];
	int isDiCloseRelationPointAShow, isDiCloseRelationPointBShow;
	vtkSmartPointer<vtkActor> diCloseRelationPointAActor, diCloseRelationPointBActor;

	double diCloseRelationProjectPointB[3];
	vtkSmartPointer<vtkActor> diCloseRelationProjectPointBActor;
	vtkSmartPointer<vtkActor> diCloseRelationPlanePointAActor; // 用于绘制平面中心
	double diCloseRelationPlaneONormal[3];
	double diCloseRelationPlaneOCenter[3];
	double diCloseRelationPlaneOPoint[3][3]; // 0: origin
	vtkSmartPointer<vtkActor> diCloseRelationPlaneOActor, diCloseRelationPlaneOWireframeActor;
	int isDiCloseRelationPlaneOShow;
	int DiCloseRelationType;
	double diCloseRelationDistance;

	//
	// DI后牙反合
	//

	double diBackToothOppositeClosePoint[16][3], diBackToothOppositeCloseProjectPoint[16][3];
	bool diBackToothOppositeClosePointState[16];
	vtkSmartPointer<vtkActor> diBackToothOppositeClosePointActor[16], diBackToothOppositeCloseProjectPointActor[16];
	int isDiBackToothOppositeClosePointShow, isDiBackToothOppositeCloseProjectPointShow;
	double diBackToothOppositeCloseScore;

	//
	// DI后牙正锁合
	//

	double diBackToothLockClosePoint[10][3], diBackToothLockCloseProjectPoint[10][3];
	bool diBackToothLockClosePointState[10];
	vtkSmartPointer<vtkActor> diBackToothLockClosePointActor[10], diBackToothLockCloseProjectPointActor[10];
	int isDiBackToothLockClosePointShow, isDiBackToothLockCloseProjectPointShow;
	double diBakcToothLockCloseScore;

	//
	// DI其他->中线不调
	//

	double diMiddleUnfitPoint[2][3];
	int diMiddleUnfitPointIndex;
	int diMiddleUnfitPointShow;
	double diMiddleUnfitDistance;
	double diMiddleUnfitScore;
	vtkSmartPointer<vtkActor> diMiddleUnfitPointActor[2], diMiddleUnfitPlaneActor[2], diMiddleUnfitPlaneWireframeActor[2];

	//
	// DI其他->中切牙牙间隙
	//

	double diMiddleCutToothGapPoint[2][3];
	int diMiddleCutToothGapIndex;
	int isDiMiddleCutToothGapShow;
	double diMiddleCutToothGapDistance;
	double diMiddleCutToothGapScore;
	vtkSmartPointer<vtkActor> diMiddleCutToothGapPointActor[2];

	//
	// DI其他->得分
	//

	double diOtherScore;

	//
	// ABO-OGS排列
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
	// ABO-OGS边缘嵴高度
	//

	double aboEdgeVertebralHeighPoints[20][2][3], aboEdgeVertebralHeighPointProjectToMiddlePlane[20][2][3], aboEdgeVertebralHeighPointProjectToArticlePlane[20][2][3];
	vtkSmartPointer<vtkActor> aboEdgeVertebralHeighPointAcotor[20][2], aboEdgeVertebralHeighPointProjectToMiddlePlaneAcotr[20][2], aboEdgeVertebralHeighPointProjectToArticlePlaneActor[20][2];
	int isAboEdgeVertebralHeighPointShow[20][2], isAboEdgeVertebralHeighProjectPointShow[20][2];
	int aboEdgeVertebralHeighPointPickNum[20];
	double aboEdgeVertebralHeighPointPareDistance[20], aboEdgeVertebralHeighPointPareScore[20], aboEdgeVertebralHeighScore;
	double aboEdgeVertebralScoreT[28];
	//
	// ABO-OGS颊舌向倾斜度
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
	// ABO-OGS牙合关系
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
	// ABO-OGS牙合接触
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
	// ABO-OGS覆盖
	//

	double aboCoverPoints[18][2][3]; // 0: 上牙		1: 下牙
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
	// ABO-OGS邻牙接触关系
	//

	double aboTouchRelationPoints[26][2][3], aboTouchRelationProjectPoints[26][2][3];
	vtkSmartPointer<vtkActor> aboTouchRelationPointAcotor[26][2], aboTouchRelationProjectPointAcotor[26][2];
	int isAboTouchRelationPointShow[26][2], isAboTouchRelationProjectPointShow[26][2];
	int aboTouchRelationPointNum[26];
	double aboTouchRelationMeasureDistance[26], aboTouchRelationMeasureScore[26], aboTouchRelationScore;
	double aboTouchRelationScoreT[28];

	//
	// ABO-OGS得分
	//

	double aboScore;

	//
	// 测量
	//

	// 点-点
	int measureDistancePointToPointIndex;
	double measureDistancePointToPointPositions[2][3];
	vtkSmartPointer<vtkActor> measureDistancePointToPointPointActor[2], measureDistancePointToPointLineActor;
	int ismeasureDistancePointToPointShow;

	//
	// 设置
	//

	// 颜色
	double pickPointColor[3]; // 牙列上选点的颜色
	double projectToFittingPlanePointColot[3]; // 下牙列选点在合平面上投影的颜色
	double topProjectToFittingPlanePointColot[3]; // 上牙列选点在合平面上投影的颜色
	double planeColor[3]; // 平面的颜色
	double frameColor[3]; // 平面上线框的颜色
	double intersectLlineColor[3]; // 平面相交直线的颜色
	double intersectCurveColor[3]; // 下牙列轮廓线颜色
	double topIntersectCurveColor[3]; // 上牙列轮廓线颜色
};

#endif // MAINWINDOW_H
