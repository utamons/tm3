#include <QtCore>
#include <qpoint.h>
#include <qsize.h>
#include "tm3util.h"
#include <QDebug>
#include <QtGui>
#include <QApplication>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStyleFactory>

bool checkWorkDir() {
	bool result = true;

	QString wdPath = getWorkPath();
	QDir wdDir(wdPath);

	if (!wdDir.exists()) {
			if (!wdDir.mkdir(wdPath)) {
					result = false;
					qDebug() << QObject::tr("Cannot make ").append(wdDir.absolutePath());
				}
		}

	return result;
}

quint64 toEpochMins(QDateTime dt) {
    return static_cast<quint64>(dt.toMSecsSinceEpoch() / 60000L);
}

quint64 toEpochMins(QDate dt) {
    QDateTime dtm = dt.startOfDay();
    return static_cast<quint64>(dtm.toMSecsSinceEpoch() / 60000L);
}

QDateTime toDateTime(quint64 epochMins) {
    return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(epochMins * 60000L));
}

QDate toDate(quint64 epochMins) {
    return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(epochMins * 60000L)).date();
}

QString getWorkPath() {
    QString homePath = "/home/oleg/Documents";
	QString wdPath = homePath.append("/db");
	return QDir::toNativeSeparators(wdPath);
}

void saveInsKey(QString key) {
	QSettings settings("TM3","TM3");
	settings.setValue("insKey",key);
}

QString restoreInsKey() {
	QSettings settings("TM3", "TM3");
	return settings.value("insKey","Backspace").toString();
}

void saveWinPos(QString winName, QPoint pos, QSize size) {
	QSettings settings("TM3", "TM3");
	QString sizePrefix = "size";
	QString posPrefix = "pos";
	settings.setValue(posPrefix.append(winName), pos);
	settings.setValue(sizePrefix.append(winName), size);
}

QPoint restoreWinPos(QString winName) {
	QSettings settings("TM3", "TM3");
	QString posPrefix = "pos";
	return settings.value(posPrefix.append(winName), QPoint(200, 200)).toPoint();
}

QSize restoreWinSize(QString winName) {
	QSettings settings("TM3", "TM3");
	QString sizePrefix = "size";
	return settings.value(sizePrefix.append(winName), QPoint(200, 200)).toSize();
}

QString getCopyrightYears() {
	QString str = QString::number(START_DEV_YEAR);
	QDate qDate = QDate::currentDate();
	if (qDate.year() > START_DEV_YEAR) {
			str.append("-").append(QString::number(qDate.year()));
		}
	return str;
}

QString timeFromMins(int mins) {
	QString result = "";
	int m = mins % 60;
	int h = mins / 60;

	if (h > 0) {
			result.append(QString::number(h));
			result.append(QObject::tr(" h. "));
		}

	if (m > 0) {
			result.append(QString::number(m));
			result.append(QObject::tr(" m. "));
		}

	if (m + h == 0) {
			result.append("--");
		}

	return result;
}

QDateTime roundTime(QDateTime dt) {
	int add = dt.time().minute() % 5;
	return add > 2 ? dt.addSecs((5 - add) * 60) : dt.addSecs(-1 * add * 60);
}

int roundMins(int mins) {
	int add = mins % 5;
	return add > 2 ? mins + (5 - add) : mins - add;
}

int askUser(QString txt, QString infTxt) {
	QMessageBox msgBox;
	msgBox.setWindowTitle(" ");
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(txt);
	msgBox.setInformativeText(infTxt);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	return msgBox.exec();
}

bool checkLock(QString lockPath) {
	QFile lockFile(lockPath);
	bool retCode = true;
	if (lockFile.exists()) {
			int answ = askUser(QObject::tr("Probably one copy of TM3 alreary running. Or incorrect exit occured."),
			                   QObject::tr("Launch this copy?"));
			if (answ == QMessageBox::No) {
					retCode = false;
				}
		} else {
			if (!lockFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
					qDebug() << "Cannot open lock file";
					QMessageBox::critical(nullptr, "Error", "Cannot open lock file!");
					retCode = false;
				} else {
					QTextStream out(&lockFile);
					out << "x";
				}
		}
	return retCode;
}

void removeLock(QString lockPath) {
	QFile lockFile(lockPath);
	if (lockFile.exists()) {
			lockFile.remove();
		}
}

bool isNumeric(QString val) {
    QRegularExpression re("\\d*");
    return re.match(val).hasMatch();
}


bool isEmpty(QString val) {
	return val.trimmed().size() == 0;
}


QString tr(const char *str) {
	return QObject::tr(str);
}

void setDark() {
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    QColor darkColor = QColor(45,45,45);
    QColor disabledColor = QColor(127,127,127);
    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18,18,18));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

    qApp->setPalette(darkPalette);

    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}
