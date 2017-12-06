#include "myDatabase.h"

myDataBaseC::myDataBaseC()
{

}

bool myDataBaseC::GetAllPointName(QStringList &allPointList)
{
	QString selectStr = "select distinct pointnameeng from disptvalue";
	/*query.exec(selectStr);*/
	QSqlQuery query;
	query.exec(selectStr);
	if (query.size()==0)
	{
		return false;
	}
	allPointList.clear();
	while (query.next())
	{
		QString pointname = query.value(0).toString();
		allPointList.push_back(pointname);
	}
	return true;
}

bool myDataBaseC::GetCordinate(QString sampleindex, QString imagename, QString mindex, QString mname, QString ptName, vector<double> &ptVec)
{
	ptVec.clear();
	QSqlQuery query;
	QString selectstr = "select X, Y from disinfo,disptvalue where disinfo.iddisinfo=disptvalue.iddisinfo and sampleindex='"+sampleindex+"' and imagename='"+imagename+"' and mindex='"+mindex+"' and mname='"+mname+"' and pointnameeng='"+ptName+"'";
	query.exec(selectstr);
	if (query.size()==0)
	{
		return false;
	}
	query.next();

	ptVec.clear();
	double x = query.value(0).toDouble();
	double y = query.value(1).toDouble();
	ptVec.push_back(x);
	ptVec.push_back(y);
	return true;
}

bool myDataBaseC::GetAllIddisinfo(QStringList &iddisInfoList)
{
	QString selectstr = "select distinct iddisinfo from disptvalue ";
	QSqlQuery query;
	query.exec(selectstr);
	if (query.size()==0)
	{
		return false;
	}
	iddisInfoList.clear();
	while (query.next())
	{
		iddisInfoList.push_back(query.value(0).toString());
	}
	return true;
}

bool myDataBaseC::GetDistinctMName(QString sampleindex, QString imageName,QStringList &mnameList)
{
	QSqlQuery query;
	QString selectstr = "select distinct mname from disinfo where imagename='"+imageName+"' and sampleindex='"+sampleindex+"'";
	query.exec(selectstr);
	if (query.size()==0)
	{
		return false;
	}
	mnameList.clear();
	while (query.next())
	{
		mnameList.push_back(query.value(0).toString());
	}
	return true;
}

bool myDataBaseC::GetDistinctMName(QString sampleindex ,QStringList &mnameList)
{
	QSqlQuery query;
	QString selectstr = "select distinct mname from disinfo where sampleindex='"+sampleindex+"'";
	query.exec(selectstr);
	if (query.size()==0)
	{
		return false;
	}
	mnameList.clear();
	while (query.next())
	{
		mnameList.push_back(query.value(0).toString());
	}
	return true;
}

bool myDataBaseC::GetDistinctMindex(QString sampleindex, QString imageName, QString mname, QStringList &mindexlist)
{
	QSqlQuery query;
	QString str = "select distinct mindex from disinfo where sampleindex='"+sampleindex+"' and imagename='"+imageName+"' and mname='"+mname+"'";
	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	mindexlist.clear();
	while (query.next())
	{
		mindexlist.push_back(query.value(0).toString());
	}
	return true;
}

bool myDataBaseC::GetAllSampleindex(QStringList &sampleIndexList)
{
	QSqlQuery query;
	QString str = "select distinct sampleindex from disinfo";
	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	sampleIndexList.clear();
	while (query.next())
	{
		sampleIndexList.push_back(query.value(0).toString());
	}
	return true;
}

bool myDataBaseC::GetAllImageName(QString sampleindex, QStringList &imageList)
{
	QSqlQuery query;
	QString str = "select distinct imagename from disinfo where sampleindex='"+sampleindex+"'";
	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	imageList.clear();
	while (query.next())
	{
		imageList.push_back(query.value(0).toString());
	}
	return true;
}

bool myDataBaseC::GetDistinctMesInfo(QString sampleindex, QString imagename, QString mname, QString mindex, QStringList &mnamelist, QStringList &midlist, QStringList &mindexlist)
{
	QSqlQuery query;
	QString str = "select distinct mname, mid ,mindex from disinfo where imagename='"+imagename+"' and sampleindex='"+sampleindex+"' and mname='"+mname+"' and mindex='"+mindex+"'";

	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	mnamelist.clear();
	midlist.clear();
	mindexlist.clear();
	while (query.next())
	{
		mnamelist.push_back(query.value(0).toString());
		midlist.push_back(query.value(1).toString());
		mindexlist.push_back(query.value(2).toString());
	}
	return true;
}

bool myDataBaseC::GetAllPointNameInfo(QStringList &ptIndexList, QStringList &ptNameEngList, QStringList &ptNameChsList)
{
	QSqlQuery query;
	QString str = "select distinct pointindex, pointnameeng,pointnamechs from disptvalue ";

	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	ptIndexList.clear();
	ptNameChsList.clear();
	ptNameEngList.clear();
	while (query.next())
	{
		ptIndexList.push_back(query.value(0).toString());
		ptNameEngList.push_back(query.value(1).toString());
		ptNameChsList.push_back(query.value(2).toString());
	}
	return true;
}

bool myDataBaseC::GetPointInfo(QString sampleindex, QStringList &imageList, QStringList &pointnameList, vector<vector<double>> &ptlist)
{
	QSqlQuery query;
	QString str = "select imagename, pointnameeng, X,Y from disinfo,disptvalue where mname='aver' and sampleIndex='"+sampleindex+"' and disinfo.iddisinfo=disptvalue.iddisinfo";
	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	imageList.clear();
	pointnameList.clear();
	ptlist.clear();
	while (query.next())
	{
		imageList.push_back(query.value(0).toString());
		pointnameList.push_back(query.value(1).toString());
		double x = query.value(2).toDouble();
		double y = query.value(3).toDouble();
		vector<double> temp;
		temp.push_back(x);
		temp.push_back(y);
		ptlist.push_back(temp);
	}
	return true;
}

bool myDataBaseC::GetMaxIdDifinfo(QString &maxId)
{
	QSqlQuery query;
	QString str = "select MAX(iddisinfo) FROM disinfo ";
	query.exec(str);
	if (query.size()==0)
	{
		return false;
	}
	query.next();
	maxId = query.value(0).toString();
	return true;
}