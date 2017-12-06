#ifndef MYPATIENTINFO_H
#define MYPATIENTINFO_H

#include <QDialog>
#include <QDir>
#include <QtGui/QtGui>
#include <iostream>
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>

using namespace std;

class MyPatientInfoDialog : public QDialog
{
	Q_OBJECT

public:
	MyPatientInfoDialog(QWidget *parent = 0);
	~MyPatientInfoDialog();
	void createWidgets(QWidget *p);
	void initDoctorBox();
	void setPatientTable();
	//void iniDialogs();
	//void initData();

	QString medical_num, patient_name, gender, birth, angle_class;
	QString tooth1, tooth2, tooth3, tooth4;
	QString jiaozheng1, jiaozheng2, jiaozheng3, jiaozheng4;
	QString start_time, end_time;
	QString treat_result, desc_info;
	QString doctor_name;

public slots:
	void new_bt_triggered();
	void modify_bt_triggered();
	void delete_bt_triggered();
	void select_patient_triggered(int row,int col);
	void doctor_name_te_changed_slot(int);

private:
	QTextEdit *medical_num_TE, *patient_name_TE, *gender_TE;
	QTextEdit *angle_class_TE, *tooth1_TE,*tooth2_TE,*tooth3_TE,*tooth4_TE;
	QTextEdit *jiaozheng1_TE,*jiaozheng2_TE,*jiaozheng3_TE,*jiaozheng4_TE;
	QDateEdit *start_time_TE, *end_time_TE,*birth_TE;
	QTextEdit *treat_result_TE;
	QComboBox *doctor_name_TE;
	QTextEdit *desc_info_TE;
	QTableWidget *patient_TB;
};

#endif