#ifndef EXPORTMESTREEDIALOG_H
#define EXPORTMESTREEDIALOG_H

#include <QDialog>
#include <QDir>
#include <vector>
#include <ActiveQt/qaxobject.h>

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
class QTreeWidgetItem;
using namespace std;

class Dialog_ExportMesTree : public QDialog
{
	Q_OBJECT
public:
	Dialog_ExportMesTree(QWidget *parent = 0);
	~Dialog_ExportMesTree();

	public slots:
		void browserBtTriggered();
		void ExportBtTriggered();
		void iniTreeWidget();
		void iniTreeWidget(QString sampleindex);
		void treeItemChanged(QTreeWidgetItem* item, int column);
		void updateParentItem(QTreeWidgetItem* item);
		void getAllSelected(QTreeWidgetItem *item,QString line);
		void getAllPt(QTreeWidgetItem *item);

signals:
		void gotSaveInformation(QStringList, QStringList, QString);

private:
	QTreeWidget *tree;
	QString saveFileName;
	QStringList selectList,mesList;
	QString currentSampleIndex;
};

#endif