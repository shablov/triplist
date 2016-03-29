QT       += core gui sql

DESTDIR = bin

OBJECTS_DIR = obj
UI_DIR = obj
MOC_DIR = obj
RCC_DIR = obj

greaterThan(QT_MAJOR_VERSION, 4): {
	QT += widgets
	DEFINES += QT5
}

TARGET = triplist
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
	editdialog.cpp \
	settingsdialog.cpp

HEADERS  += mainwindow.h \
	editdialog.h \
	settingsdialog.h

FORMS    += mainwindow.ui \
	editdialog.ui \
	settingsdialog.ui

RESOURCES += \
	res.qrc
