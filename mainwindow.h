#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class QSqlQueryModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void init();

protected:
	virtual void hideEvent(QHideEvent *e);
	virtual void showEvent(QShowEvent *e);

private slots:
	void on_settingsAction_triggered();

	void on_pbAdd_clicked();

	void on_pbRemove_clicked();

	void on_pbEdit_clicked();

	void on_staffList_clicked(const QModelIndex &index);

private:
	bool connectDb();
	bool isConnected();
	void selectIndexById(int staffId);

	Ui::MainWindow *ui;

	QSqlQueryModel *pStaffModel;
	void refreshQuery();
};

#endif // MAINWINDOW_H
