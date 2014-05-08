#include "GameControls.h"

GameControls::GameControls(GameScene* gameScene) :
	scene(gameScene) {
}

GameControls::~GameControls() {

}

void GameControls::keyPressEvent(QKeyEvent* keyEvent) {
//	scene->keyPressEvt(keyEvent);
}

void GameControls::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
//	scene->mousePressEvt(mouseEvent);
}

void GameControls::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) {
//	scene->mouseMoveEvt(mouseEvent);
}
