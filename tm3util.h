#pragma once

#include <vector>
#include <QDebug>
#include <QRegExp>

/*
 маленькие полезные штучки.
*/

const int START_DEV_YEAR = 2013; // год начала разработки
const QString PROGRAM_VERSION = "0.5.3"; // версия программы

bool checkWorkDir();
QString getWorkPath();
void saveWinPos(QString winName, QPoint pos, QSize size);
QPoint restoreWinPos(QString winName);
QSize restoreWinSize(QString winName);
QString getCopyrightYears();

QString timeFromMins(int mins);

quint64 toEpochMins(QDateTime dt);

quint64 toEpochMins(QDate dt);

QDateTime toDateTime (quint64 epochMins);

QDate toDate (quint64 epochMins);

QDateTime roundTime(QDateTime dt);
int roundMins(int mins);

template <typename T>
void removeFromVector(std::vector<T> &v, T obj);

template <typename T>
void removeFromVector(std::vector<T> &v, T obj) {
    v.erase(std::remove( v.begin(),v.end(), obj ), v.end());
}

int askUser(QString txt, QString infTxt);

bool checkLock(QString lockPath);

void removeLock(QString lockPath);

bool isEmpty(QString val);

bool isNumeric(QString val);

QString tr(const char *str);
