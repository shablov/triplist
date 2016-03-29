#include "mainwindow.h"
#include <QTextCodec>
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

#ifndef QT5
	QTextCodec::setCodecForCStrings( QTextCodec::codecForName("utf-8") );
#endif


	QTextCodec::setCodecForLocale( QTextCodec::codecForName("utf-8") );

	MainWindow w;
	w.show();
	w.init();

	return a.exec();
}
