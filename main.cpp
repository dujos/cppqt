#include "GameMenu.h"
#include <QtGui\qapplication.h>

#include <QtGui\qapplication.h>
#include <QtGui\qmessagebox.h>
#include <QtGui\qdirmodel.h>
#include <QtCore\qthread.h>
#include <QtCore\qsettings.h>

#include "DisplayWrapper.h"
#include "Defines.h"
#include "GameHelper.h"
#include "Scaler.h"
#include "GameWidget.h"
#include "GameHelper.h"

//insert into ui_menu if disapears
//QObject::connect(bNew, SIGNAL(clicked()), GameMenu, SLOT(onButtonNewClicked()));
//QObject::connect(bGameStart, SIGNAL(clicked()), GameMenu, SLOT(onGameStartClicked()));
int main(int argc, char *argv[]) {
	QApplication::setGraphicsSystem("raster");
	QApplication application(argc, argv);

	QString resourcePath = application.applicationDirPath();
	//QString resourcePath = "C:/Users/dujo/Documents/Visual Studio 2010/Projects/Qt_api/Qt_api/data";
	//resourcePath.chop(3);

	QThread::currentThread()->setPriority(QThread::HighPriority);
	DisplayWrapper::init();
	Scaler::init();
	
	GameWidget gameWidget(resourcePath);
	DisplayWrapper::restoreVideoModes();

	return application.exec();
}
