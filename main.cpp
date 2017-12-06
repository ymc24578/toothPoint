#include "mainwindow.h"

//#include <QtGui/QApplication>
#include <QVTKApplication.h>
#include <qtextcodec.h>
#include <QTranslator>

int main(int argc, char *argv[])
{
	//QApplication a(argc, argv);
	QVTKApplication a(argc, argv);

	QTranslator qtTranslator;
	qtTranslator.load("./qt_zh_CN.qm");
    a.installTranslator(&qtTranslator);


	QTranslator mainTranslator;
    mainTranslator.load("./toothpoint04_zh.qm");
	a.installTranslator(&mainTranslator);

	QApplication::addLibraryPath("./plugins");
	QApplication::addLibraryPath ("./imageformats");
	QTextCodec::setCodecForLocale (QTextCodec::codecForName("gb2312"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gb2312"));
	QTextCodec::setCodecForCStrings (QTextCodec::codecForName("gb2312"));
	//Set Encode GBK
	QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
	MainWindow w;
	w.showMaximized();
	w.show();

	return a.exec();
}
