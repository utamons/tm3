#pragma once

#include <QDialog>
#include <QLineEdit>

#include "activity.h"
#include "ratevaltablemodel.h"
#include "catlistmodel.h"
#include "activitytablemodel.h"
#include "acttagslistmodel.h"
#include "ui_ActUpdateDlg.h"

class ActUpdateDlg : public QDialog,private Ui_ActUpdateDlg
{
    Q_OBJECT
public:
    explicit ActUpdateDlg(Activity act = Activity(),  QDateTime beginTime = ActivityTableModel::getLastTime(), QWidget *parent = 0);
    void accept();

    void showEvent(QShowEvent *) {
       resizeColumns();
       comboCats->lineEdit()->setSelection(0,act.cat.abbrev.length());
    }

    Activity getAct() {return act;}

public slots:
    void catFieldEdited();
    void addRate();
    void delRate();
    void updateRate(QModelIndex idx);
    void showCatTree();
    void timeChanged (const QTime &time);
    void addTag();
    void delTag();
    void minsEditFinished ();
    void enterPressed();
    void rateModelChanged();
    void tagModelChanged();

private:
	Activity act;
    CatListModel catModel;
    RateValTableModel rateModel;
    ActTagsListModel tagModel;
    void resizeColumns();
};
