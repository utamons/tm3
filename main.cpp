#include <QApplication>
#include "mainwindow.h"
#include "tm3util.h"
#include "dbhelper.h"

int main(int argc, char *argv[]) {
	int retcode = 1;
	const QString lockFile = "/tmp/TM3.lock";

	QApplication app(argc, argv);
	Q_INIT_RESOURCE(TM3res);

	app.setOrganizationName("TM3");
	app.setApplicationName("TM3");

	if (checkWorkDir()) {
		if (checkLock(lockFile)) {
			DBHelper *db = DBHelper::getInstance();
			MainWindow mainWin;
			// This is for Mac!
			app.setWindowIcon(QIcon(":/icons.icns"));
			mainWin.show();

			retcode = app.exec();

			db->close();
			removeLock(lockFile);
		}
	} else {
		QMessageBox::critical(nullptr, "Error", "Cannot create work directory!");
	}

	return retcode;
}
