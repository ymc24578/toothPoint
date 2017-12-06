#include "myPatientInfo.h"

MyPatientInfoDialog::MyPatientInfoDialog(QWidget *parent /* = 0 */)
	:QDialog(parent)
{
	createWidgets(parent);
	initDoctorBox();
	setPatientTable();
	setWindowTitle(tr("病例管理"));
	resize(800,600);
}

MyPatientInfoDialog::~MyPatientInfoDialog()
{}

void MyPatientInfoDialog::createWidgets(QWidget *p)
{
	QGroupBox *patient_group = new QGroupBox(tr("病人基本信息"));
	QGridLayout *patient_group_layout = new QGridLayout;
	QLabel *medical_num_LB = new QLabel(tr("病历号"));
	medical_num_TE = new QTextEdit;
	medical_num_TE->setMaximumSize(100,25);
	QLabel *patnet_name_LB = new QLabel(tr("姓名"));
	patient_name_TE = new QTextEdit;
	patient_name_TE->setMaximumSize(100,25);
	QLabel *gender_LB = new QLabel(tr("性别"));
	gender_TE = new QTextEdit;
	gender_TE->setMaximumSize(100,25);
	QLabel *birth_LB = new QLabel(tr("生日"));
	birth_TE = new QDateEdit;
	birth_TE->setMaximumSize(100,25);
	QLabel *angle_class_LB = new QLabel(tr("安式分类"));
	angle_class_TE = new QTextEdit;
	angle_class_TE->setMaximumSize(100,25);

	int index = 0;
	patient_group_layout->addWidget(medical_num_LB,0,0);
	patient_group_layout->addWidget(medical_num_TE,0,1);
	patient_group_layout->addWidget(patnet_name_LB,1,0);
	patient_group_layout->addWidget(patient_name_TE,1,1);
	patient_group_layout->addWidget(gender_LB,2,0);
	patient_group_layout->addWidget(gender_TE,2,1);
	patient_group_layout->addWidget(birth_LB,3,0);
	patient_group_layout->addWidget(birth_TE,3,1);
	patient_group_layout->addWidget(angle_class_LB,4,0);
	patient_group_layout->addWidget(angle_class_TE,4,1);
	patient_group->setLayout(patient_group_layout);

	QGroupBox *tooth_group = new QGroupBox(tr("拔牙情况"));
	QGridLayout *tooth_group_layout = new QGridLayout;
	QLabel *tooth1_LB = new QLabel(tr("一"));
	tooth1_TE = new QTextEdit;
	tooth1_TE->setMaximumSize(100,25);
	QLabel *tooth2_LB = new QLabel(tr("二"));
	tooth2_TE = new QTextEdit;
	tooth2_TE->setMaximumSize(100,25);
	QLabel *tooth3_LB = new QLabel(tr("三"));
	tooth3_TE = new QTextEdit;
	tooth3_TE->setMaximumSize(100,25);
	QLabel *tooth4_LB = new QLabel(tr("四"));
	tooth4_TE = new QTextEdit;
	tooth4_TE->setMaximumSize(100,25);
	index = 0;
	tooth_group_layout->addWidget(tooth1_LB,index,0);
	tooth_group_layout->addWidget(tooth1_TE,index,1);
	tooth_group_layout->addWidget(tooth2_LB,index,2);
	tooth_group_layout->addWidget(tooth2_TE,index++,3);
	tooth_group_layout->addWidget(tooth3_LB,index,0);
	tooth_group_layout->addWidget(tooth3_TE,index,1);
	tooth_group_layout->addWidget(tooth4_LB,index,2);
	tooth_group_layout->addWidget(tooth4_TE,index++,3);
	tooth_group->setLayout(tooth_group_layout);

	QGroupBox *jiaozheng_group = new QGroupBox(tr("矫正器"));
	QGridLayout *jiaozheng_group_layout = new QGridLayout;
	QLabel *jiaozheng1_LB = new QLabel(tr("一"));
	jiaozheng1_TE = new QTextEdit;
	jiaozheng1_TE->setMaximumSize(100,25);
	QLabel *jiaozheng2_LB = new QLabel(tr("二"));
	jiaozheng2_TE = new QTextEdit;
	jiaozheng2_TE->setMaximumSize(100,25);
	QLabel *jiaozheng3_LB = new QLabel(tr("三"));
	jiaozheng3_TE = new QTextEdit;
	jiaozheng3_TE->setMaximumSize(100,25);
	QLabel *jiaozheng4_LB = new QLabel(tr("四"));
	jiaozheng4_TE = new QTextEdit;
	jiaozheng4_TE->setMaximumSize(100,25);
	index = 0;
	jiaozheng_group_layout->addWidget(jiaozheng1_LB,index,0);
	jiaozheng_group_layout->addWidget(jiaozheng1_TE,index++,1);
	jiaozheng_group_layout->addWidget(jiaozheng2_LB,index,0);
	jiaozheng_group_layout->addWidget(jiaozheng2_TE,index++,1);
	jiaozheng_group_layout->addWidget(jiaozheng3_LB,index,0);
	jiaozheng_group_layout->addWidget(jiaozheng3_TE,index++,1);
	jiaozheng_group_layout->addWidget(jiaozheng4_LB,index,0);
	jiaozheng_group_layout->addWidget(jiaozheng4_TE,index++,1);
	jiaozheng_group->setLayout(jiaozheng_group_layout);

	QGroupBox *treat_result_group = new QGroupBox(tr("治疗效果说明"));
	QGridLayout *treat_result_group_layout = new QGridLayout;
	treat_result_TE = new QTextEdit;
	treat_result_group_layout->addWidget(treat_result_TE,0,0);
	treat_result_group->setLayout(treat_result_group_layout);

	QGroupBox *treat_time_group = new QGroupBox(tr("治疗时间"));
	QGridLayout *treat_time_group_layout = new QGridLayout;
	QLabel *start_time_LB = new QLabel(tr("开始日期"));
	start_time_TE = new QDateEdit;
	start_time_TE->setMaximumSize(100,25);
	QLabel *end_time_LB = new QLabel(tr("结束日期"));
	end_time_TE = new QDateEdit;
	end_time_TE->setMaximumSize(100,25);
	treat_time_group_layout->addWidget(start_time_LB,0,0);
	treat_time_group_layout->addWidget(start_time_TE,0,1);
	treat_time_group_layout->addWidget(end_time_LB,1,0);
	treat_time_group_layout->addWidget(end_time_TE,1,1);
	treat_time_group->setLayout(treat_time_group_layout);

	QGroupBox *doctor_group = new QGroupBox(tr("医师信息"));
	QGridLayout *doctor_group_layout = new QGridLayout;
	QLabel *doctor_name_LB = new QLabel(tr("医师姓名"));
	doctor_name_TE = new QComboBox;
	doctor_name_TE->setEditable(true);
	connect(doctor_name_TE,SIGNAL(currentIndexChanged(int)),this,SLOT(doctor_name_te_changed_slot(int)));
	doctor_group_layout->addWidget(doctor_name_LB,0,0);
	doctor_group_layout->addWidget(doctor_name_TE,0,1);
	doctor_group->setLayout(doctor_group_layout);

	QGroupBox *desc_gropu = new QGroupBox(tr("备注信息"));
	QGridLayout *desc_group_layout = new QGridLayout;
	desc_info_TE = new QTextEdit;
	
	desc_group_layout->addWidget(desc_info_TE,0,0);
	desc_gropu->setLayout(desc_group_layout);

	QGroupBox *bt_group = new QGroupBox(tr(""));
	QGridLayout *bt_group_layout = new QGridLayout;
	QPushButton *new_bt = new QPushButton(tr("新增"));
	new_bt->setMaximumSize(50,30);
	connect(new_bt,SIGNAL(clicked()),this,SLOT(new_bt_triggered()));
	QPushButton *modify_bt = new QPushButton(tr("修改"));
	modify_bt->setMaximumSize(50,30);
	connect(modify_bt,SIGNAL(clicked()),this,SLOT(modify_bt_triggered()));
	QPushButton *delete_bt = new QPushButton(tr("删除"));
	delete_bt->setMaximumSize(50,30);
	connect(delete_bt,SIGNAL(clicked()),this,SLOT(delete_bt_triggered()));
	bt_group_layout->addWidget(new_bt,0,0);
	bt_group_layout->addWidget(modify_bt,0,1);
	bt_group_layout->addWidget(delete_bt,0,2);
	bt_group->setLayout(bt_group_layout);

	QGroupBox *table_group = new QGroupBox(tr(""));
	QGridLayout *table_group_layout = new QGridLayout;
	patient_TB = new QTableWidget();
	patient_TB->setSelectionBehavior(QAbstractItemView::SelectRows);
	patient_TB->setSelectionMode(QAbstractItemView::SingleSelection);
	patient_TB->setContextMenuPolicy(Qt::ActionsContextMenu);
	patient_TB->setEditTriggers(QAbstractItemView::NoEditTriggers);
	connect(patient_TB,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(select_patient_triggered(int,int)));
	table_group_layout->addWidget(patient_TB,0,0);
	table_group->setLayout(table_group_layout);

	QGridLayout *mainlayout = new QGridLayout;
	mainlayout->addWidget(patient_group,0,4,5,1);
	mainlayout->addWidget(treat_time_group,5,4,2,1);
	mainlayout->addWidget(doctor_group,7,4,1,1);
	mainlayout->addWidget(tooth_group,0,6,2,1);
	mainlayout->addWidget(jiaozheng_group,2,6,4,1);
	mainlayout->addWidget(treat_result_group,6,6,2,1);
	mainlayout->addWidget(desc_gropu,8,4,1,3);
	mainlayout->addWidget(bt_group,9,4,1,3);
	mainlayout->addWidget(table_group,0,0,9,3);
	p->setLayout(mainlayout);
}

void MyPatientInfoDialog::initDoctorBox(){
	QSqlQuery query;
	QString doc_name_str = "select distinct(doctor_name) from patientinfo";
	query.exec(doc_name_str);
	QStringList doc_name_list;
	while (query.next())
	{
		QString doc_name = query.value(0).toString();
		doc_name_list.append(doc_name);
	}
	doctor_name_TE->clear();
	doctor_name_TE->addItems(doc_name_list);
}

void MyPatientInfoDialog::setPatientTable()
{
	doctor_name = doctor_name_TE->currentText();
	QSqlQuery query;
	QString selectStr = "select patientname, medical_record_num from patientinfo where doctor_name='"+doctor_name+"'";
	query.exec(selectStr);
	QStringList header;
	header<<"患者姓名"<<"病历号";
	int row = query.size();
	int col = query.record().count();
	patient_TB->setRowCount(row);
	patient_TB->setColumnCount(col);
	patient_TB->setHorizontalHeaderLabels(header);
	patient_TB->horizontalHeader()->setStretchLastSection(true);
	query.next();
	for (int i=0;i<row;i++)
	{
		for (int j=0;j<col;j++)
		{
			QString str = query.value(j).toString();
			QTableWidgetItem *item = new QTableWidgetItem;
			patient_TB->setItem(i,j,item);
			item->setText(str);
		}
		query.next();
	}
}

void MyPatientInfoDialog::new_bt_triggered()
{
	medical_num = medical_num_TE->text();
	patient_name = patient_name_TE->text();
	gender = gender_TE->text();
	birth = birth_TE->text();
	angle_class = angle_class_TE->text();
	doctor_name = doctor_name_TE->currentText();
	start_time = start_time_TE->text();
	end_time = end_time_TE->text();
	tooth1 = tooth1_TE->text();
	tooth2 = tooth2_TE->text();
	tooth3 = tooth3_TE->text();
	tooth4 = tooth4_TE->text();
	jiaozheng1 = jiaozheng1_TE->text();
	jiaozheng2 = jiaozheng2_TE->text();
	jiaozheng3 = jiaozheng3_TE->text();
	jiaozheng4 = jiaozheng4_TE->text();
	treat_result = treat_result_TE->text();
	desc_info = desc_info_TE->text();
	if (doctor_name.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Information"),tr("请输入医师姓名"));
		return;
	}
	if (medical_num.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Information"),tr("请输入病例编号"));
		return;
	}
	if (patient_name.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Information"),tr("请输入患者姓名"));
		return;
	}
	QSqlQuery query;
	QString max_id_str = "select max(id) from patientinfo";
	query.exec(max_id_str);
	query.next();
	int max_id = query.value(0).toInt();
	max_id += 1;
	QString insertStr = "insert into patientinfo(id,doctor_name,medical_record_num,"
						"PatientName,Gender,Birthday,Age,TreatStartTime,TreatEndTime,"
						"AngleClass,Tooth1,Tooth2,Tooth3,Tooth4,Jiaozheng1,Jiaozheng2,"
						"Jiaozheng3,Jiaozheng4,TreatResult,DescInfo) values('"+QString::number(max_id)+"','"
						+doctor_name+"','"+medical_num+"','"+patient_name+"','"+gender+"','"
						+birth+"','','"+start_time+"','"+end_time+"','"+angle_class+"','"
						+tooth1+"','"+tooth2+"','"+tooth3+"','"+tooth4+"','"+jiaozheng1+"','"
						+jiaozheng2+"','"+jiaozheng3+"','"+jiaozheng4+"','"+treat_result+"','"
						+desc_info+"')";
	query.exec(insertStr);
	QMessageBox::information(QApplication::activeWindow(),
		tr("Information"),tr("新增患者信息成功"));
	initDoctorBox();
	setPatientTable();
}

void MyPatientInfoDialog::modify_bt_triggered()
{
	medical_num = medical_num_TE->text();
	patient_name = patient_name_TE->text();
	gender = gender_TE->text();
	birth = birth_TE->text();
	angle_class = angle_class_TE->text();
	doctor_name = doctor_name_TE->currentText();
	start_time = start_time_TE->text();
	end_time = end_time_TE->text();
	tooth1 = tooth1_TE->text();
	tooth2 = tooth2_TE->text();
	tooth3 = tooth3_TE->text();
	tooth4 = tooth4_TE->text();
	jiaozheng1 = jiaozheng1_TE->text();
	jiaozheng2 = jiaozheng2_TE->text();
	jiaozheng3 = jiaozheng3_TE->text();
	jiaozheng4 = jiaozheng4_TE->text();
	treat_result = treat_result_TE->text();
	desc_info = desc_info_TE->text();
	if (doctor_name.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Informatino"),tr("请输入医师姓名"));
		return;
	}
	if (medical_num.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Informatino"),tr("请输入病例编号"));
		return;
	}
	if (patient_name.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Informatino"),tr("请输入患者姓名"));
		return;
	}
	QSqlQuery query;
	QString max_id_str = "select max(id) from patientinfo";
	query.exec(max_id_str);
	query.next();
	int max_id = query.value(0).toInt();
	max_id += 1;
	/*QString insertStr = "insert into patientinfo(id,doctor_name,medical_record_num,"
		"PatientName,Gender,Birthday,Age,TreatStartTime,TreatEndTime,"
		"AngleClass,Tooth1,Tooth2,Tooth3,Tooth4,Jiaozheng1,Jiaozheng2,"
		"Jiaozheng3,Jiaozheng4,TreatResult,DescInfo) values('"+QString::number(max_id)+"','"
		+doctor_name+"','"+medical_num+"','"+patient_name+"','"+gender+"','"
		+birth+"','','"+start_time+"','"+end_time+"','"+angle_class+"','"
		+tooth1+"','"+tooth2+"','"+tooth3+"','"+tooth4+"','"+jiaozheng1+"','"
		+jiaozheng2+"','"+jiaozheng3+"','"+jiaozheng4+"','"+treat_result+"','"
		+desc_info+"')";*/
	QString updateStr = "update patientinfo set patientname='"+patient_name+"',"
		+"gender='"+gender+"',"+"birthday='"+birth+"',"+"TreatStartTime='"+start_time+"',"
		+"TreatEndTime='"+end_time+"',"+"AngleClass='"+angle_class+"',"+"tooth1='"+tooth1+"',"
		+"tooth2='"+tooth2+"',"+"tooth3='"+tooth3+"',"+"tooth4='"+tooth4+"',"
		+"Jiaozheng1='"+jiaozheng1+"',"+"Jiaozheng2='"+jiaozheng2+"',"+"Jiaozheng3='"+jiaozheng3+"',"
		+"Jiaozheng4='"+jiaozheng4+"',"+"TreatResult='"+treat_result+"',"+"DescInfo='"+desc_info+"' "
		+"where doctor_name='"+doctor_name+"' and medical_record_num='"+medical_num+"'";
	//cerr<<updateStr.toStdString()<<endl;
	query.exec(updateStr);
	QMessageBox::information(QApplication::activeWindow(),
		tr("Informatino"),tr("更新成功"));
	initDoctorBox();
	setPatientTable();
}

void MyPatientInfoDialog::delete_bt_triggered()
{
	medical_num = medical_num_TE->text();
	patient_name = patient_name_TE->text();
	gender = gender_TE->text();
	birth = birth_TE->text();
	angle_class = angle_class_TE->text();
	doctor_name = doctor_name_TE->currentText();
	start_time = start_time_TE->text();
	end_time = end_time_TE->text();
	tooth1 = tooth1_TE->text();
	tooth2 = tooth2_TE->text();
	tooth3 = tooth3_TE->text();
	tooth4 = tooth4_TE->text();
	jiaozheng1 = jiaozheng1_TE->text();
	jiaozheng2 = jiaozheng2_TE->text();
	jiaozheng3 = jiaozheng3_TE->text();
	jiaozheng4 = jiaozheng4_TE->text();
	treat_result = treat_result_TE->text();
	desc_info = desc_info_TE->text();
	if (doctor_name.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Informatino"),tr("请输入医师姓名"));
		return;
	}
	if (medical_num.length() == 0){
		QMessageBox::information(QApplication::activeWindow(),
			tr("Informatino"),tr("请输入病例编号"));
		return;
	}
	QSqlQuery query;
	/*QString max_id_str = "select max(id) from patientinfo";
	query.exec(max_id_str);
	query.next();*/
	QString deleteStr = "delete from patientinfo where doctor_name='"+doctor_name+"' "
		+"and medical_record_num='"+medical_num+"'";
	query.exec(deleteStr);
	QMessageBox::information(QApplication::activeWindow(),
		tr("Informatino"),tr("删除患者信息成功"));
	initDoctorBox();
	setPatientTable();
}

void MyPatientInfoDialog::select_patient_triggered(int row,int col)
{
	QTableWidgetItem* item1 = new QTableWidgetItem;
	item1 = patient_TB->item(row,0);
	doctor_name = doctor_name_TE->currentText();
	patient_name = item1->text();
	item1 = patient_TB->item(row,1);
	medical_num = item1->text();
	//cerr<<patient_name.toStdString()<<" "<<medical_num.toStdString()<<endl;
	QSqlQuery query;
	QString selectStr = "select birthday, gender, treatstarttime,"
		"treatendtime,angleclass,tooth1,tooth2,tooth3,tooth4,jiaozheng1, "
		"jiaozheng2,jiaozheng3,jiaozheng4,treatresult,descinfo "
		"from patientinfo where doctor_name='"+doctor_name+"' "
		" and patientname='"+patient_name+"' and medical_record_num='"+medical_num+"'";
	//cerr<<selectStr.toStdString()<<endl;
	query.exec(selectStr);
	query.next();
	/*for (int i=0;i<query.record().count();i++)
	{
		QString val = query.value(i).toString();
		cerr<<val.toStdString()<<endl;
	}*/
	birth = query.value(0).toString();
	gender = query.value(1).toString();
	start_time = query.value(2).toString();
	end_time = query.value(3).toString();
	angle_class = query.value(4).toString();
	tooth1 = query.value(5).toString();
	tooth2 = query.value(6).toString();
	tooth3 = query.value(7).toString();
	tooth4 = query.value(8).toString();
	jiaozheng1 = query.value(9).toString();
	jiaozheng2 = query.value(10).toString();
	jiaozheng3 = query.value(11).toString();
	jiaozheng4 = query.value(12).toString();
	treat_result = query.value(13).toString();
	desc_info = query.value(14).toString();

	medical_num_TE->setText(medical_num);
	patient_name_TE->setText(patient_name);
	gender_TE->setText(gender);
	angle_class_TE->setText(angle_class);
	tooth1_TE->setText(tooth1);
	tooth2_TE->setText(tooth2);
	tooth3_TE->setText(tooth3);
	tooth4_TE->setText(tooth4);
	jiaozheng1_TE->setText(jiaozheng1);
	jiaozheng2_TE->setText(jiaozheng2);
	jiaozheng3_TE->setText(jiaozheng3);
	jiaozheng4_TE->setText(jiaozheng4);
	start_time_TE->setDate(QDate::fromString(start_time,"yyyy-MM-dd"));
	end_time_TE->setDate(QDate::fromString(end_time,"yyyy-MM-dd"));
	birth_TE->setDate(QDate::fromString(birth,"yyyy-MM-dd"));
	treat_result_TE->setText(treat_result);
	desc_info_TE->setText(desc_info);
}

void MyPatientInfoDialog::doctor_name_te_changed_slot(int index)
{
	doctor_name = doctor_name_TE->currentText();
	setPatientTable();
}