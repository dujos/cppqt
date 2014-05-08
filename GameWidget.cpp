#include "DisplayWrapper.h"
#include "Scaler.h"
//#include "GameMenu.h"
#include "GameWidget.h"

QString GameWidget::resourcePath;

const QString& GameWidget::getResourcePath() {
	return (GameWidget::resourcePath = QDir::currentPath() + "/Application");
}

GameWidget::GameWidget(const QString& qstring, QWidget* parent) :
	QGraphicsView(parent) {

	resourcePath = qstring;
	setWindowTitle(GameName);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

	profile = new GameProfile(this);
	//profile->loadProfile();

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	scene = new GameScene(this);
	scene->setProfile(profile);
	setScene(scene);

	//controls = new GameControls(scene);
	setVideoMode();
}

GameWidget::~GameWidget() {
	delete profile;
	delete scene;
}

void GameWidget::setVideoMode() {
	DisplayWrapper::restoreVideoModes();
	this->showNormal();
	QApplication::processEvents();

	this->setFixedSize(WIDTH, HEIGHT);
	setSceneRect(1, 1, WIDTH-1, HEIGHT-1);
	scene->initGraphics();
}