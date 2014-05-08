#ifndef GAMECONTROLS_H
#define GAMECONTROLS_H

#include "GameScene.h"

#include "QtGui\qgraphicssceneevent.h"
#include <QtGui\qgraphicsscene.h>
#include "QtGui\qevent.h"

class GameScene;

class GameControls {

private:
	GameScene* scene;

public:
	GameControls(GameScene* scene);
	~GameControls();

	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void keyPressEvent(QKeyEvent* keyEvent);
};

#endif