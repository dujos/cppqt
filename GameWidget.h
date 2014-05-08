#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QtGui\qapplication.h>
#include <QtGui\qgraphicsview.h>
#include <QtCore\qstring.h>

#include "GameScene.h"
#include "GameProfile.h"
#include "GameStock.h"
#include "GameControls.h"

class GameScene;
class GameProfile;
class GameStock;

class GameWidget : public QGraphicsView {

public:
	GameProfile* profile;
	GameScene* scene;
	GameControls* controls;

	static QString resourcePath;

public:
	GameWidget(const QString& qstring, QWidget* parent=NULL);
	~GameWidget();

	static const QString& getResourcePath();

	void setVideoMode();
};

#endif