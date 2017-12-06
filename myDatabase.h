#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <iostream>
using namespace std;

class myDataBaseC {
public:
	myDataBaseC();

	bool GetAllPointName(QStringList &allPointList);
	bool GetCordinate(	QString sampleindex,
						QString imagename,
						QString mindex,
						QString mname,
						QString ptName,
						vector<double> &ptVec);

	bool GetAllIddisinfo(QStringList &iddisInfoList);	//获取所有ID dis info 信息

	bool GetDistinctMName(	QString sampleindex,
							QString imageName,
							QStringList &mnameList);	//

	bool GetDistinctMName(	QString sampleindex,
							QStringList &mnameList);

	bool GetDistinctMindex(	QString sampleindex,
							QString imageName,
							QString mname,
							QStringList &mindexlist);

	bool GetAllSampleindex(QStringList &sampleIndexList);	//

	bool GetAllImageName(	QString sampleindex,
							QStringList &imageList);

	bool GetDistinctMesInfo(	QString sampleindex,
								QString imagename,
								QString mname,
								QString mindex,
								QStringList &mnamelist,
								QStringList &midlist,
								QStringList &mindexlist);

	bool GetAllPointNameInfo(	QStringList &ptIndexList,
								QStringList &ptNameEngList,
								QStringList &ptNameChsList);

	bool GetPointInfo(	QString sampleindex,
						QStringList &imageList,
						QStringList &pointnameList,
						vector<vector<double>> &ptlist);

	bool GetMaxIdDifinfo(QString &maxId);
};

#endif

