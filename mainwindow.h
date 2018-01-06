#pragma once

#include <QMainWindow>
#include <QLabel>
#include "ui_MainWindow.h"
#include "activitytablemodel.h"

class MainWindow: public QMainWindow, private Ui_MainWindow {
Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	void showEvent(QShowEvent *) {
		resizeColumns();
		showStatus();
		selectLastRow();
	}

protected:
	void closeEvent(QCloseEvent *event);

public slots:
	void about();
	void addAct();
	void addAct2();   // Additional insert for my notebook keyboard
	void updateAct(QModelIndex idx);
	void updateAction();
	void delAct();
	void actUnitsDlg();
	void actTagDlg();
	void actRates();
	void actCatDlg();
	void dateChanged();
	void dayBack();
	void dayFwd();
	void actSimpleReport();
	void today();
	void toggleInsKey();

private:
	QString getLoadStr(double load);
	void fillLoadLabel();
	void readSettings();
	void resizeColumns();
	void showStatus();
	void selectLastRow();
    QLabel lblStatus;
    ActivityTableModel model;
    std::pair<Activity,Activity> splitByDay(Activity act);
};
