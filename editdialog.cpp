#include "editdialog.h"
#include "ui_editdialog.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QItemDelegate>
#include <QSqlTableModel>

EditDialog::EditDialog(int staffId, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditDialog),
	mStaffId(staffId), mRegexp("^[а-яА-Я- ]+$")
{
	ui->setupUi(this);

	pModel = new QSqlTableModel(this);
	pModel->setTable("trip");
	pModel->setFilter(QString("staff_id=%0").arg(mStaffId));
	pModel->setEditStrategy( QSqlTableModel::OnManualSubmit );
	pModel->select();

	if(pModel->lastError().isValid())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+pModel->lastError().text());
	}

	pModel->setHeaderData(pModel->record().indexOf("strana"), Qt::Horizontal, "Страна");
	pModel->setHeaderData(pModel->record().indexOf("nachalo"), Qt::Horizontal, "Дата начала");
	pModel->setHeaderData(pModel->record().indexOf("konets"), Qt::Horizontal, "Дата окончания");
	pModel->setHeaderData(pModel->record().indexOf("tsel"), Qt::Horizontal, "Цель");

	pProxyModel = new ProxyModel(this);
	pProxyModel->setSourceModel( pModel );

	pProxyModel->addColumnToValidate( pModel->record().indexOf("strana") );
	pProxyModel->addColumnToValidate( pModel->record().indexOf("tsel") );
	pProxyModel->setRegexp( mRegexp );

	ui->tableView->setModel(pProxyModel);
	ui->tableView->hideColumn( pModel->record().indexOf("id") );
	ui->tableView->hideColumn( pModel->record().indexOf("staff_id") );
	ui->tableView->setItemDelegate( new QItemDelegate(ui->tableView) );
#ifdef QT5
	ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#else
	ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

	ui->cbPol->addItem("Мужской", "M");
	ui->cbPol->addItem("Женский", "F");

	if(mStaffId >= 0)
	{
		loadPersonData();
	}

	connect(ui->leFamiliia, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
	connect(ui->leImia, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
	connect(ui->leOtchestvo, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
}

EditDialog::~EditDialog()
{
	delete ui;
}

void EditDialog::accept()
{
	if(!validate())
	{
		return;
	}

	QSqlQuery query;
	if(mStaffId < 0)
	{
		query.prepare("INSERT INTO staff (imia, familia, otchestvo, birth, pol) "
					  "VALUES (:imia, :familia, :otchestvo, :birth, :pol) "
					  "RETURNING id");
	}
	else
	{
		query.prepare("UPDATE staff SET imia=:imia, familia=:familia, otchestvo=:otchestvo, birth=:birth, pol=:pol "
					  "WHERE id=:id");
		query.bindValue(":id", mStaffId);
	}

	query.bindValue(":imia", ui->leImia->text());
	query.bindValue(":otchestvo", ui->leOtchestvo->text());
	query.bindValue(":familia", ui->leFamiliia->text());
	query.bindValue(":birth", ui->dtBirth->date());
	query.bindValue(":pol", ui->cbPol->itemData( ui->cbPol->currentIndex()) );

	if(!query.exec())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+query.lastError().text());
		return;
	}

	if(mStaffId < 0)
	{
		if(!query.first())
		{
			QMessageBox::critical(this, "Ошибка SQL", "Не удалось получить идентификатор новой записи.");
			return;
		}

		mStaffId = query.value(0).toInt();

		int staffCol = pModel->record().indexOf("staff_id");

		for(int i = 0; i < pModel->rowCount(); ++i)
		{
			pModel->setData( pModel->index(i, staffCol), mStaffId);
		}
	}

	if(!pModel->submitAll())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+pModel->lastError().text());
		return;
	}

	QDialog::accept();
}

void EditDialog::onTextEdited(const QString &text)
{
	QLineEdit *edit = qobject_cast<QLineEdit*>(sender());

	if(!edit)
	{
		return;
	}

	if(mRegexp.exactMatch(text))
	{
		edit->setStyleSheet( "color:black;" );
	}
	else
	{
		edit->setStyleSheet( "color:red;" );
	}
}

bool EditDialog::validate()
{
	if(!mRegexp.exactMatch( ui->leImia->text().simplified()))
	{
		QMessageBox::information(this, "Ошибка данных", "Некорректное значение поля <b>Имя</b>");
		return false;
	}

	if(!mRegexp.exactMatch( ui->leOtchestvo->text().simplified()))
	{
		QMessageBox::information(this, "Ошибка данных", "Некорректное значение поля <b>Отчество</b>");
		return false;
	}

	if(!mRegexp.exactMatch( ui->leFamiliia->text().simplified()))
	{
		QMessageBox::information(this, "Ошибка данных", "Некорректное значение поля <b>Фамилия</b>");
		return false;
	}

	if(!pProxyModel->isValid())
	{
		QMessageBox::information(this, "Ошибка данных", "Некорректное значение в таблице");
		return false;
	}

	return true;
}

void EditDialog::on_pbAddRecord_clicked()
{
	int row = pModel->rowCount();
	pModel->insertRow( row );

	if(mStaffId >=0 )
	{
		int staffCol = pModel->record().indexOf("staff_id");
		pModel->setData( pModel->index(row, staffCol), mStaffId );
	}
}

void EditDialog::on_pbRemoveRecord_clicked()
{
	QModelIndex idx = ui->tableView->currentIndex();

	if(!idx.isValid())
	{
		return;
	}

	int btn = QMessageBox::question(this,
															"Удаление записи",
															"Запись будет удалена из базы. Продолжить?",
															QMessageBox::Yes,
															QMessageBox::No);

	if(btn == QMessageBox::No)
	{
		return;
	}

	ui->tableView->hideRow(idx.row());
	pModel->removeRow(idx.row());
}

void EditDialog::loadPersonData()
{
	QSqlQuery query;
	query.prepare("SELECT imia, familia, otchestvo, pol, birth FROM staff WHERE id=:id");
	query.bindValue(":id", mStaffId);

	if(!query.exec())
	{
		QMessageBox::critical(this, "Ошибка SQL", "Не удалось выполнить SQL-запрос: "+pModel->lastError().text());
		return;
	}

	if(!query.first())
	{
		QMessageBox::critical(this, "Ошибка данных", "Не удалось получить данные для сотрудника.");
		return;
	}

	QSqlRecord rec = query.record();

	ui->leImia->setText( rec.value("imia").toString());
	ui->leFamiliia->setText( rec.value("familia").toString());
	ui->leOtchestvo->setText( rec.value("otchestvo").toString());
	ui->dtBirth->setDate( rec.value("birth").toDate());

	ui->cbPol->setCurrentIndex( ui->cbPol->findData( rec.value("pol").toString().toUpper()) );
}


/// ProxyModel class
ProxyModel::ProxyModel(QObject *parent) : QIdentityProxyModel(parent)
{

}

void ProxyModel::addColumnToValidate(int col)
{
	mColumnsToValidate << col;
}

void ProxyModel::clearColumnsToValidate()
{
	mColumnsToValidate.clear();
}

bool ProxyModel::isValid()
{
	for(int i = 0; i < rowCount(); ++i)
	{
		foreach(int col, mColumnsToValidate)
		{
			const QString &s = data( index(i, col)).toString();
			if(!mRegexp.exactMatch(s))
			{
				return false;
			}
		}
	}

	return true;
}

QVariant ProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
	if(!proxyIndex.isValid() || proxyIndex.row() >= rowCount() || proxyIndex.column() >= columnCount())
	{
		return QVariant();
	}

	if(role == Qt::ForegroundRole && mColumnsToValidate.contains( proxyIndex.column()))
	{
		QString s = proxyIndex.data().toString();

		if(mRegexp.exactMatch(s))
		{
			return QColor(Qt::black);
		}
		else
		{
			return QColor(Qt::red);
		}
	}

	return QIdentityProxyModel::data(proxyIndex, role);
}

Qt::ItemFlags ProxyModel::flags(const QModelIndex &/*index*/) const
{
	return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
