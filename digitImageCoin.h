#ifndef DIGITIMAGECOIN_H
#define DIGITIMAGECOIN_H

#include <QDialog>
#include <QDir>
#include <vector>
#include <ActiveQt/qaxobject.h>
#include "myDatabase.h"

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QTextEdit;
class QListWidget;
class QCheckBox;
class QListWidgetItem;
class QTreeWidget;
class QRadioButton;
class QScrollBar;
class QLineEdit;
class QGroupBox;
class QGridLayout;
class QProgressBar;
using namespace std;

class Dialog_DigitImageCoin : public QDialog
{
	Q_OBJECT

public:
	Dialog_DigitImageCoin(QWidget *parent = 0);
	void initialDialog();
	bool isInit();
	void iniTree(QTreeWidget *t);
	void iniInfoTree();
	void iniCombox();
	void iniNameCombox();
	void inibangdingPicTree(QTreeWidget *t);
	void iniMesBox();
	void iniData();	// initial data of widgets;

	void exportType1();
	void exportType2();
	void exportType3();
	void exportType4();
	void exportType5();

	void copyVector(vector<double> *dst, const vector<double> *src);

	QTreeWidget *tree,*tree2, *infoTree, *bangdingPicTree;
	QRadioButton *sourceImageButton,*targetImageButton;
	QRadioButton *sourceTracingImageButton,*targetTracingImageButton;
	QRadioButton *sourceCoinImageButton,*targetCoinImageButton;
	QScrollBar *xTranslateBar, *yTranslateBar, *rotateBar;
	QLineEdit *transStepBox,*rotateStepBox;
	QCheckBox *rotateCenter;
	QComboBox * ptBox[6];
	QComboBox * ptMoveBox[2],*ptAngleBox[2];
	QLabel *disLabel[2];
	QComboBox *coinMethodBox, *coinMethodBox2, *cordBox;
	QRadioButton * pointTypeButton[3];
	QPushButton *clearYuanTuButton;
	QGroupBox *groupbox6;
	QRadioButton *sourceImageTracingCoiCheckBox, *tracingImageCoinCheckBox;
	QComboBox *picTypeComboBox;

	QRadioButton *export1RBX,*export2RBX,*export3RBX,*export4RBX;
	QRadioButton *export5RBX;
	QComboBox *mesName1CBX,*mesName2CBX;
	QComboBox *mesSourceSampleIndexCBX,*mesTargetSampleIndexCBX;

	QGridLayout *layout1, *layout2;
	QGridLayout *hlay, *hlay2;

	QComboBox *patientNameTE;
	QComboBox *preAndPostCBox;
	QProgressBar *presserBar;

	// ÖØµþ suhong
	QComboBox *coinPlaneBox, *referPlaneBox1,*referPlaneBox2, *mesurePtBox;
	QComboBox *sampleIndexCBox;
	QString referPlanePt1,referPlanePt2,mesurePt, coinPlanePt1, coinPlanePt2;
	
	double cord[6];
	QString sourceImage,targetImage, sourceSampleIndex, targetSampleIndex;	// sourceimage: Ô­Í¼; targetimage:Êý×ÖÃèÍ¼
    QString sourceStage, targetStage; 
	QString mNameG, mIndexG;
	int preDx,preDy,preAngle;
	int currentTabIndex;
	double translateStep,rotateStep;
	QTransform sourceTrans;
	vector<double> *sourcePt, *tracingPt;
	vector<double> *tracingCoinPtS, *tracingCoinPtT;

	void iniSampleIndexCBox();
	void setCoinPlaneContext();
	void setAllPoints();

	void myInverse4(double m1[][4],double m2[][4],int n);
	void convertData(QTransform trans);
	QStringList calculateExportMove(QTransform trans, QString mname, QString mindex);
	QStringList calculateExportAngle(QTransform trans, QString mname, QString mindex);
	QTransform getCoinTrans(QString patientNum, QString mesName, QString coinType,QString imageType, QString &preImage, QString &postImage, QString &yuantu,int yuantuType);
	QTransform getCoinTrans(QString preImage, QString postImage, QString mesName, QString coinType);
	QTransform getCoinTrans2(QString yuantu, QString tracingImage, QString mesName);
	QString getYuantu(QString imageName);
	void convertAllData();
	QTransform clacuTransform(double sx1,double sy1,double sx2,double sy2, double tx1, double ty1, double tx2, double ty2);
	QString calcuMoveDis(double refx,double refy, double sx, double sy, double tx, double ty);

	// tianjia bangding tupian piliang
	void addBangdingTupian();

public slots:
	void treeDoubleClickedTriggered();
	void tree2DoubleClickedTriggered();
	void convertDataButtonTriggered();
	void XYTranslateBarValueChanged(int v);
	void rotateBarValueChanged(int v);
	void setMesActionTriggered();
	void cordPtValueChanged();
	void cordPtValueChanged2(int index);
	void ptMoveValueChanged();
	void ptAngleValueChanged();
	void exportBtTriggered();
	void testBtTriggered();
	void confirmTransButtonTriggered();
	void loadTransButtonTriggered();
	void tabWidgetIndexChanged(int ind);
	void getSourceTracingPtSLot(vector<double>* pt, int selfindex);
	void getTracingPtSLot(vector<double>* pt, int selfindex);
	void coinPrePostTracingButtonTriggered();
	void loadCoinPrePostTracingButtonTriggered();
	void selectBangdingPicBTTriggered();

	void coinPlaneCurrentIndexTriggered(const QString &text);
	void referPlaneBox1CurrentChangedTriggered(const QString &text);
	void referPlaneBox2CurrentChangedTriggered(const QString &text);
	void mesurePtBoxCurrentChangedTriggered(const QString &text);
	void exportSHBtTriggered();

signals:
	void setImageSignal(QString, int);
	void showCoinRes();
	void showTracingCoinRes(QTransform );
	void adjustCoinImage();
	void xyTranslateValue(double , double ,int);
	void rotateValue(double ,int);

	void setCord(double , double , double , double, double, double );
	void setMyLineSignal(double,double,double,double,double,double,double,double);
	void setPtMoveSignal(double, double, double, double);
	void setPtNoTSignal(double, double, double, double);
	void setAngleCord(double, double, double ,double,double, double, double ,double);

	void setTransForm();
	void setLoadTransForm();

	void testCoin(QString, QString, QTransform);

private:
	bool initialised;
};

#endif
