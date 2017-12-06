#include "myAdjustPPI.h"

myAdjustPPIDialogC::myAdjustPPIDialogC(QWidget *parent /* = 0 */)
	:QDialog(parent)
{
	createWidgets();

	inited = false;
	resize(400,300);
	setWindowTitle(tr("统一分辨率"));
}

myAdjustPPIDialogC::~myAdjustPPIDialogC()
{
	delete sampleIndexCBox;
}

void myAdjustPPIDialogC::createWidgets()
{
	QLabel *sampleindexLabel = new QLabel(tr("测量文件库名称"));
	sampleindexLabel->setMaximumHeight(30);
	
	sampleIndexCBox = new QComboBox();
	sampleIndexCBox->setEditable(true);
	
	QPushButton *exportRulerPointBt = new QPushButton(tr("输出标尺端点"));
	exportRulerPointBt->setMaximumSize(100,30);
	connect(exportRulerPointBt,SIGNAL(clicked()),this,SLOT(exportRulerPointBt_triggered()));

	QPushButton *uniPPIBt = new QPushButton(tr("统一分辨率"));
	uniPPIBt->setMaximumSize(100,30);
	connect(uniPPIBt,SIGNAL(clicked()),this,SLOT(uniPPIBt_triggered()));

	QGridLayout *mainlayout = new QGridLayout;
	int placeInd = 0;
	mainlayout->addWidget(sampleindexLabel,placeInd++,0);
	mainlayout->addWidget(sampleIndexCBox,placeInd++,0,1,2);
	mainlayout->addWidget(exportRulerPointBt,placeInd,0,1,1);
	mainlayout->addWidget(uniPPIBt,placeInd++,1,1,1);
	setLayout(mainlayout);
}

void myAdjustPPIDialogC::iniSampleIndex()
{
	QStringList sampleIndexList;
	if (!mydb.GetAllSampleindex(sampleIndexList))
	{
		return;
	}
	
	sampleIndexCBox->clear();
	sampleIndexCBox->addItems(sampleIndexList);

	inited = true;
	/*for (int i=0;i<sampleIndexList.size();i++)
	{
		sampleIndexCBox->addItem(sampleIndexList[i]);
	}*/
}

// 输出标尺端点坐标
// 依次输出每个测量者的数据，最后输出平均值
void myAdjustPPIDialogC::exportRulerPointBt_triggered()
{
	QString sampleindex = sampleIndexCBox->currentText();
	if (sampleindex.length()==0)
	{
		 QMessageBox::information(QApplication::activeWindow(),"Information",tr("请选择测量文件库"));
		return;
	}
	// 从数据库中查找所有测量者信息并输出其所有图片的标尺端点
	QSqlQuery query;
	QString selectMname = "select distinct mname from ruler where sampleindex='"+sampleindex+"'";
	query.exec(selectMname);
	if (query.size()==0)
	{
		return;
	}
	while (query.next())
	{
		QString mname = query.value(0).toString();
		exportSingmeMnameRulerPoint(mname,sampleindex);
	}
	
	// 针对每张图片输出平均标尺坐标
	exportAverageRulerPoint(sampleindex);

	QString outinfor = tr("输出完成\n") + tr("结果保存在 ./doc 路径下\n") + tr("分别以测量者名称作为文件名\n")+tr("平均坐标值：标尺坐标平均值.xls\n");
	 QMessageBox::information(QApplication::activeWindow(),"Information",outinfor);
}

void myAdjustPPIDialogC::exportSingmeMnameRulerPoint(QString mname, QString sampleindex)
{
	QString saveFileName = "./doc/" + mname + ".xls";
	QFile outfile(saveFileName);
	if (!outfile.open(QIODevice::WriteOnly|QIODevice::Text))
	{
		return;
	}
	QTextStream out(&outfile);
	QSqlQuery query;
	QString selectStr = "select imagename,rulerpoint ,x ,y , mindex from ruler where sampleindex='"+sampleindex+"' and mname='"+mname+"' and (rulerpoint='Ruler_1_Down' or rulerpoint='Ruler_1_Up') order by imagename, mindex,rulerpoint";
	query.exec(selectStr);
	if (query.size()==0)
	{
		 QMessageBox::information(QApplication::activeWindow(),"Information",tr(mname+"没有标尺记录"));
		return;
	}
	QString header = "";
	header = tr("图片名称\t")+tr("测量者\t")+tr("测量序号\t")+tr("上端点X\t") + tr("上端点Y\t")+tr("下端点X\t")+tr("下端点Y\t");
	out<<header<<endl;
	while (query.next())
	{
		QString imgname = query.value(0).toString();
		QString rulerpoint1 = query.value(1).toString();
		QString x1 = query.value(2).toString();
		QString y1 = query.value(3).toString();
		QString mindex = query.value(4).toString();
		QString outstr = imgname + "\t" + mname + "\t" + mindex + "\t";
		QString tempV1 = "";
		QString tempV2 = "";
		if (rulerpoint1=="Ruler_1_Down")
		{
			tempV1 += x1 +"\t" + y1+"\t";
			/*poitname = "标尺下端点";*/
		}
		else
		{
			tempV2 += x1 +"\t" + y1+"\t";
		}
		
		query.next();
		QString rulerpoint2 = query.value(1).toString();
		QString x2 = query.value(2).toString();
		QString y2 = query.value(3).toString();
		
		if (rulerpoint2=="Ruler_1_Up")
		{
			tempV2 += x2 +"\t" + y2+"\t";
			/*poitname = "标尺下端点";*/
		}
		else
		{
			tempV1 += x2 +"\t" + y2+"\t";
		}
		outstr += tempV2+tempV1;
		out<<outstr<<endl;
	}
	outfile.close();
}

void myAdjustPPIDialogC::exportAverageRulerPoint(QString sampleindex)
{
	QString saveFileName = "./doc/标尺坐标平均值.xls";
	QFile outfile(saveFileName);
	if (!outfile.open(QIODevice::WriteOnly|QIODevice::Text))
	{
		return;
	}
	QTextStream out(&outfile);

	// 所有图片名
	QSqlQuery query;
	QString selectstr = "select distinct imagename from ruler where sampleindex='"+sampleindex+"'";
	query.exec(selectstr);
	if (query.size()==0)
	{
		return;
	}
	QStringList imgList;
	while (query.next())
	{
		imgList.append(query.value(0).toString());
	}

	QString header = tr("图片名\t") + tr("上端点X\t") + tr("上端点Y\t") + tr("下端点X\t") + tr("下端点Y\t");
	out<<header<<endl;
	// 从数据库中查询每张图片对应点的平均值，并输出
	for (int i=0;i<imgList.size();i++)
	{
		selectstr = "select avg(x),avg(y) from ruler where sampleindex='"+sampleindex+"' and imagename='"+imgList[i]+"' and rulerpoint='Ruler_1_Down'";
		query.exec(selectstr);
		query.next();
		QString downx = query.value(0).toString();
		QString downy = query.value(1).toString();

		selectstr = "select avg(x),avg(y) from ruler where sampleindex='"+sampleindex+"' and imagename='"+imgList[i]+"' and rulerpoint='Ruler_1_Up'";
		query.exec(selectstr);
		query.next();
		QString upx = query.value(0).toString();
		QString upy = query.value(1).toString();

		QString outstr = imgList[i] + "\t" + upx + "\t" + upy + "\t" + downx + "\t" + downy;
		out<<outstr<<endl;
	}
	outfile.close();
}

void myAdjustPPIDialogC::uniPPIBt_triggered()
{
	QString saveFileDir = QFileDialog::getExistingDirectory(this,
		tr("保存文件路径"), QDir::currentPath());
	// QMessageBox::information(QApplication::activeWindow(),tr("test"),saveFileDir);

	QString openfilename = QFileDialog::getOpenFileName(this,
		tr("打开统一分辨率描述文件"),"./doc","*.txt");
	/*cout<<openfilename.toStdString()<<endl;
	return;*/
	//QFile file("./doc/t.txt");
	QFile file(openfilename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		 QMessageBox::information(QApplication::activeWindow(),tr("infomation"),tr("open failed"));
	QTextStream in(&file);
	QString saveFileName=saveFileDir+"/";//"./UnityPPIImage/";
	while(!in.atEnd())
	{
		QString line = in.readLine();
		//line.replace('\t'," ");
		QStringList t_list = line.split('\t',QString::SkipEmptyParts);
		QString fileName = t_list.at(0);
		double realLen = t_list.at(2).toDouble();
		double x1 = t_list.at(3).toDouble();
		double y1 = t_list.at(4).toDouble();
		double x2 = t_list.at(5).toDouble();
		double y2 = t_list.at(6).toDouble();
		//cerr<<realLen<<" "<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
		double len = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
		double mr = realLen/len;
		double ppi = 25.4/mr;
		double myppi = 144;
		double scale = myppi/ppi;
		QImage t_image;
		//cerr<<fileName.toStdString()<<endl;
		t_image.load(fileName);
		//cerr<<"loaded!"<<endl;
		t_image = t_image.scaledToWidth(t_image.width()*scale,Qt::FastTransformation);
		QStringList file_list = fileName.split("/");
		QString tF = file_list.at(file_list.size()-1);
		tF = saveFileName +tF;
		t_image.save(tF);
		//cout<<saveFileName.toStdString()<<endl;
		// QMessageBox::information(QApplication::activeWindow(),tr("test"),tr(line));
	}
	file.close();
	 QMessageBox::information(QApplication::activeWindow(),"Information",tr("输出完毕！"));
}

bool myAdjustPPIDialogC::isInited()
{
	return inited;
}