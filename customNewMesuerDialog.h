#ifndef CUSTOMNEWMESUERDIALOG_H
#define CUSTOMNEWMESUERDIALOG_H

#include <QDialog>
#include <QDir>
#include <vector>
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QTextEdit;
class QListWidget;
class QCheckBox;
class QListWidgetItem;
class QTabWidget;
using namespace std;
class Dialog_NewMesuer : public QDialog
{
	Q_OBJECT
public:
	Dialog_NewMesuer(QWidget *parent = 0);
	void iniCombox();
	void calAngle();
	void calLength();
	

	QLabel *res1,*res2;
	QComboBox * ptBox[6];
	QLabel *pointCord[6];
	QTabWidget *tabWidget;

	QTextEdit *saveNameType1, *saveNameType2;

	QString sampleindex, imagename, mname, mindex;

	public slots:
		void valueChanged();
		void saveBtTriggered();
		void setIniPara(QString si,QString im,QString mn, QString mi);
signals:
		

private:

};

#endif