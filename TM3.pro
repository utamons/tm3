QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += sdk_no_version_check

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

macx:ICON = $${PWD}/icons.icns

INCLUDEPATH += $$PWD\
    $$PWD/dlg\
    $$PWD/entity\
    $$PWD/helper\
    $$PWD/model


SOURCES += main.cpp\
        mainwindow.cpp \
    tm3util.cpp \
    catabbrevvalidator.cpp \
    dlg/actupdatedlg.cpp \
    dlg/cattreedlg.cpp \
    dlg/catupdatedlg.cpp \
    dlg/ratelistdlg.cpp \
    dlg/rateupdatedlg.cpp \
    dlg/ratevalupdatedlg.cpp \
    dlg/simplereportdlg.cpp \
    helper/catreporthelper.cpp \
    helper/dbhelper.cpp \
    helper/periodhelper.cpp \
    helper/ratereporthelper.cpp \
    helper/reporthelper.cpp \
    helper/tagreporthelper.cpp \
    model/activitytablemodel.cpp \
    model/cattreemodel.cpp \
    model/ratelistmodel.cpp \
    model/ratevaltablemodel.cpp \
    model/tagcombomodel.cpp \
    model/unitlistmodel.cpp \
    dlg/tagcombodlg.cpp \
    model/acttagslistmodel.cpp\
    model/catlistmodel.cpp \
    model/cattagslistmodel.cpp \
    model/taglistmodel.cpp \
    dlg/taglistdlg.cpp \
    dlg/unitlistdlg.cpp

HEADERS  += mainwindow.h \
    tm3util.h \
    catabbrevvalidator.h \
    model/activitytablemodel.h \
    dlg/actupdatedlg.h \
    dlg/cattreedlg.h \
    dlg/catupdatedlg.h \
    dlg/ratelistdlg.h \
    dlg/rateupdatedlg.h \
    dlg/ratevalupdatedlg.h \
    dlg/simplereportdlg.h \
    entity/activity.h \
    entity/category.h \
    entity/rate.h \
    entity/rateval.h \
    entity/unit.h \
    helper/catreporthelper.h \
    helper/dbhelper.h \
    helper/periodhelper.h \
    helper/ratereporthelper.h \
    helper/reporthelper.h \
    helper/tagreporthelper.h \
    model/cattreemodel.h \
    model/ratelistmodel.h \
    model/ratevaltablemodel.h \
    model/tagcombomodel.h \
    model/unitlistmodel.h \
    dlg/tagcombodlg.h \
    model/acttagslistmodel.h \
    model/treenode.h \
    model/catlistmodel.h \
    model/cattagslistmodel.h \
    model/baselistmodel.h \
    helper/insertfunctor.h \
    helper/updatefunctor.h \
    helper/deletefunctor.h \
    model/taglistmodel.h \
    dlg/taglistdlg.h \
    dlg/unitupdatedlg.h \
    dlg/unitlistdlg.h \
    model/basetablemodel.h \
    entity/entity.h

QT += sql

FORMS    += \
    ui/ActUpdateDlg.ui \
    ui/CatTreeDlg.ui \
    ui/CatUpdateDlg.ui \
    ui/MainWindow.ui \
    ui/RateListDlg.ui \
    ui/RateUpdateDlg.ui \
    ui/RateValUpdateDlg.ui \
    ui/SimpleReport.ui \
    ui/UnitDlg.ui \
    ui/UnitNameDlg.ui \
    ui/TagComboDlg.ui

RESOURCES += \
    TM3res.qrc


TRANSLATIONS += \
    tm3_ru_RU.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
