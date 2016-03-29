#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QSet>
#include <QDialog>

#include <QIdentityProxyModel>

namespace Ui {
class EditDialog;
}

class ProxyModel;
class QSqlTableModel;

class EditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EditDialog(int staffId = -1, QWidget *parent = 0);
	~EditDialog();

	int staffId() const {return mStaffId; }

public slots:
	void accept();

private slots:
	void onTextEdited(const QString &text);

	void on_pbAddRecord_clicked();

	void on_pbRemoveRecord_clicked();

private:
	void loadPersonData();

	bool validate();

	Ui::EditDialog *ui;

	int mStaffId;

	QRegExp mRegexp;

	QSqlTableModel *pModel;
	ProxyModel *pProxyModel;
};

class ProxyModel : public QIdentityProxyModel
{
	Q_OBJECT
public:
	ProxyModel(QObject *parent = 0);

	void addColumnToValidate(int col);
	void clearColumnsToValidate();

	void setRegexp(const QRegExp &rx) {mRegexp = rx;}

	bool isValid();

	virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private:
	QSet<int> mColumnsToValidate;
	QRegExp mRegexp;
};

#endif // EDITDIALOG_H
