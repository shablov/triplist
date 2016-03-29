#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editdialog.h"
#include "settingsdialog.h"

#include <QDate>
#include <QSqlError>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQueryModel>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	pStaffModel = new QSqlQueryModel(this);

	ui->staffList->setModel( pStaffModel );

	connect(ui->staffList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_pbEdit_clicked()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::init()
{
	if(!connectDb())
	{
		int btn = QMessageBox::question(this,
								 "Ошибка подключения к БД",
								 "Не удалось подключиться к БД. Убедитесь, что сервер доступен и указаны корректные параметры подключения.",
							  "Проверить настройки",
										"Закрыть");

		if(btn == 0)
		{
			on_settingsAction_triggered();
		}
	}

	if(isConnected())
	{
		refreshQuery();
	}
}

void MainWindow::hideEvent(QHideEvent *e)
{
	QSettings set("triplist");
	set.setValue("geometry", saveGeometry());
	set.setValue("splitter", ui->splitter->saveState());

	QMainWindow::hideEvent(e);
}

void MainWindow::showEvent(QShowEvent *e)
{
	QMainWindow::showEvent(e);

	QSettings set("triplist");

	restoreGeometry( set.value("geometry").toByteArray());
	ui->splitter->restoreState(set.value("splitter").toByteArray());
}

void MainWindow::on_settingsAction_triggered()
{
	if( SettingsDialog().exec())
	{
		ui->textBrowser->clear();
		init();
	}
}

bool MainWindow::connectDb()
{
	if(isConnected())
	{
		QSqlDatabase::database().close();
	}

	QSettings set("triplist");

	QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
	db.setDatabaseName( set.value("database").toString() );
	db.setUserName( set.value("user").toString() );
	db.setPassword( set.value("password").toString() );
	db.setHostName( set.value("host").toString() );
	db.setPort( set.value("port").toInt() );

	bool open = db.open();

	ui->statusBar->showMessage( open ? "Подключено к БД" : "Нет подключения к БД");

	return open;
}

bool MainWindow::isConnected()
{
	return QSqlDatabase::database().isOpen();
}

void MainWindow::selectIndexById(int staffId)
{
	ui->staffList->clearSelection();
	ui->textBrowser->clear();

	for(int i = 0; i < pStaffModel->rowCount(); ++i)
	{
		if(pStaffModel->index(i, 0).data().toInt() == staffId)
		{
			ui->staffList->setCurrentIndex( pStaffModel->index(i, 1));
			on_staffList_clicked(ui->staffList->currentIndex());
			break;
		}
	}
}

void MainWindow::refreshQuery()
{
	pStaffModel->setQuery("SELECT id, familia || ' ' || left(imia, 1) || '.' || left(otchestvo, 1) || '.' FROM staff ORDER BY familia");
	if(pStaffModel->lastError().isValid())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+pStaffModel->lastError().text());
	}
	else
	{
		ui->staffList->setModelColumn(1);
	}
}

void MainWindow::on_pbAdd_clicked()
{
	EditDialog ed;
	if(ed.exec())
	{
		refreshQuery();

		selectIndexById( ed.staffId() );
	}
}

void MainWindow::on_pbRemove_clicked()
{
	QModelIndex idx = ui->staffList->currentIndex();
	if(!idx.isValid())
	{
		return;
	}

	int btn = QMessageBox::question(this,
									"Удаление записи",
									"Сотрудник и все сведения о нем будут удалены из базы. Продолжить?",
									QMessageBox::Yes,
									QMessageBox::No);

	if(btn == QMessageBox::No)
	{
		return;
	}

	int staffId = pStaffModel->index(idx.row(), 0).data().toInt();

	QSqlQuery query;
	query.prepare("DELETE FROM staff WHERE id=:id");
	query.bindValue(":id", staffId);

	if(!query.exec())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+query.lastError().text());
		return;
	}

	refreshQuery();

	ui->textBrowser->clear();
}

void MainWindow::on_pbEdit_clicked()
{
	QModelIndex idx = ui->staffList->currentIndex();
	if(!idx.isValid())
	{
		return;
	}

	int staffId = pStaffModel->index(idx.row(), 0).data().toInt();

	EditDialog ed(staffId);
	if(ed.exec())
	{
		refreshQuery();
		selectIndexById(staffId);
	}
}

void MainWindow::on_staffList_clicked(const QModelIndex &index)
{
	ui->textBrowser->clear();
	if(!index.isValid())
	{
		return;
	}

	int staffId = pStaffModel->index(index.row(), 0).data().toInt();

	QSqlQuery query;
	query.prepare("SELECT * FROM staff WHERE id=:id");
	query.bindValue(":id", staffId);

	if(!query.exec())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+query.lastError().text());
		return;
	}

	if(!query.first())
	{
		QMessageBox::critical(this, "Ошибка данных", "Не удалось получить данные для сотрудника.");
		return;
	}

	QSqlRecord rec = query.record();

	QString html = "<style>";
	html += "h1 {text-align:center; margin-bottom: 30px;}";
	html += "thead td {font-weight: bold; width: 25%; }";
	html += "</style>";
	html += "<h1 align=center>Выездной лист</h1>";
	html += "<br>";
	html += "<table cellspacing=0 cellpadding=5>";
	html += QString("<tr><td width=300><b>Фамилия:</b></td><td>%0</td></tr>").arg(rec.value("familia").toString());
	html += QString("<tr><td><b>Имя:</b></td><td>%0</td></tr>").arg(rec.value("imia").toString());
	html += QString("<tr><td><b>Отчество:</b></td><td>%0</td></tr>").arg(rec.value("otchestvo").toString());
	html += QString("<tr><td><b>Дата рождения:</b></td><td>%0</td></tr>").arg(rec.value("birth").toDate().toString("dd.MM.yyyy"));
	html += QString("<tr><td><b>Пол:</b></td><td>%0</td></tr>").arg(rec.value("pol").toString().toUpper() == "M" ? "Мужской" : "Женский");
	html += "</table>";
	html += "<p>Информация о поездках за границу</p>";
	html += "<table border=1 cellspacing=0 cellpadding=10 width=100%>";
	html += "<thead>";
	html += "<tr>";
	html += "<td>Страна</td><td>Дата начала</td><td>Дата окончания</td><td>Цель</td>";
	html += "</tr>";
	html += "</thead>";

	query.prepare("SELECT * FROM trip WHERE staff_id=:id");
	query.bindValue(":id", staffId);

	if(!query.exec())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+query.lastError().text());
		return;
	}

	while(query.next())
	{
		rec = query.record();

		html += "<tr>";
		html += QString("<td>%0</td>").arg(rec.value("strana").toString());
		html += QString("<td>%0</td>").arg(rec.value("nachalo").toDate().toString("dd.MM.yyyy"));
		html += QString("<td>%0</td>").arg(rec.value("konets").toDate().toString("dd.MM.yyyy"));
		html += QString("<td>%0</td>").arg(rec.value("tsel").toString());
		html += "</tr>";
	}

	html += "</table>";

	ui->textBrowser->setHtml( html );
}
