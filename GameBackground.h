#ifndef GAMEBACKGROUND_H
#define GAMEBACKGROUND_H

#include <QtGui\qpainter.h>
#include <QtCore\qrect.h>
#include <QtCore\qdir.h>

class GameBackground {

public:
	QList<QString> backgroundList;
	QString backgroundPath;
	
	QPixmap currentBackgroundImage;
	QPixmap transBackgroundImage;
	
	int frame;
	int currentBackgroundIx;
	int transBackgroundIx;

public:
	GameBackground();

	void init();

	void loadTransBackground();
	void drawBackground(QPainter& painter, const QRect& rectangle);

private:
	void draw(QPainter& painter, const QRect& rectangle);
};

#endif