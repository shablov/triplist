#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	QSettings set("triplist");

	ui->leHost->setText( set.value("host", "localhost").toString() );
	ui->sbPort->setValue( set.value("port", 5432).toInt() );
	ui->leDb->setText( set.value("database", "triplist").toString() );
	ui->leUser->setText( set.value("user", "postgres").toString() );
	ui->lePassword->setText( set.value("password", "postgres").toString() );
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::accept()
{
	QSettings set("triplist");
	set.setValue( "host", ui->leHost->text() );
	set.setValue( "port", ui->sbPort->value() );
	set.setValue( "database", ui->leDb->text() );
	set.setValue( "user", ui->leUser->text() );
	set.setValue( "password", ui->lePassword->text() );

	QDialog::accept();
}
