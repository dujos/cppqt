#include "GameBackground.h"
#include "GameWidget.h"

GameBackground::GameBackground() {}

void GameBackground::init() {
	this->frame = 0;
	currentBackgroundIx = transBackgroundIx = -1;
	this->backgroundPath = GameWidget::getResourcePath() + "/schemes/default";
	QStringList ext;
	ext << "bg*.png" << "bg*.jpg";
	backgroundList = QDir(backgroundPath).entryList(ext);

	if(backgroundList.isEmpty())
		return;

	currentBackgroundIx = qrand() % backgroundList.count();
	currentBackgroundImage = QPixmap(backgroundPath + "/" + backgroundList.at(currentBackgroundIx));
	//loadTransBackground();
}

void GameBackground::loadTransBackground() {
	qDebug() << "loadTransBackground";
	if(backgroundList.isEmpty())
		return;

	if(backgroundList.count() == 1)
		transBackgroundIx = 0;
	else
		do {
			transBackgroundIx = qrand() % backgroundList.count();
		} while(currentBackgroundIx == transBackgroundIx);
	transBackgroundImage = QPixmap(backgroundPath + "/" + backgroundList.at(transBackgroundIx));
}

void GameBackground::draw(QPainter& painter, const QRect& rectangle) {
	qDebug() << "draw";
	painter.setOpacity(1);
	painter.drawPixmap(rectangle, currentBackgroundImage);
	++frame;
	
	if(frame < 100)
		return;
	
	if(frame >= 480 && frame < 500) {
		painter.setOpacity((double)(frame-480)/20.0);
		painter.drawPixmap(0, 0, transBackgroundImage);
		return;
	}

	painter.drawPixmap(0, 0, transBackgroundImage);
	frame = 0;
	currentBackgroundImage = transBackgroundImage;
	currentBackgroundIx = transBackgroundIx;
	loadTransBackground();
}

void GameBackground::drawBackground(QPainter& painter, const QRect& rectangle) {
	qDebug() << "drawBackground";
	draw(painter, rectangle);
}