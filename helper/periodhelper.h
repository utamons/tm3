#pragma once

#include <QDateEdit>

class PeriodHelper : private QObject {
Q_OBJECT
public:
    explicit PeriodHelper(QDateEdit *dtFirst, QDateEdit *dtSecond);

	int periodMins() const;
	int dt1toMinsSinceEpoch() const;
	int dt2toMinsSinceEpoch() const;
	QDate firstDate() const {
		checkValid();
		return dtFirst->date();
	}
	QDate lastDate() const {
		checkValid();
		return dtSecond->date();
	}

	const QString& getName() const {
		checkValid();
		return name;
	}

	bool isValid() const { return valid; }

public slots:

	void parentDead();

signals:

	void makeReport();

private:

	void dt1Change();
	void dt2Change();
	void fwd();
	void back();
	void day();
	void week();
	void month();
	void year();

	void checkValid() const ;

	void setPeriod(QDate baseDate, int day, int days, int period);

	void setNameByDates();
	QString getMonthName(QDate dt);

	const int MONTH_PERIOD, YEAR_PERIOD;
    bool dtChangeProcess;
	QDateEdit *dtFirst, *dtSecond;
    int period;
	QString name;

	bool valid;

	friend class SimpleReportDlg;
};
