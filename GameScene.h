#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QtGui\qcursor.h>
#include <QtGui\qdesktopwidget.h>
#include <QtCore\qdir.h>
#include <QtGui\qgraphicsscene.h>
#include <QtCore\qpoint.h>
#include <QtCore\qset.h>
#include <QtCore\qtimer.h>
#include <Qt\qtimeline.h>
#include "QtGui\qgraphicssceneevent.h"
#include "QtGui\qevent.h"
#include <QtGui\qpushbutton.h>
#include <QtCore\qdebug.h>

#include "Scene.h"
#include "Defines.h"
#include "GameMenu.h"
#include "GameBackground.h"
#include "GameProfile.h"
#include "GameStock.h"
#include "GameItem.h"

#include <iostream>

class ConfirmDialog;
class Gamemenu;
class GameProfile;
class GameBackground;
class GameStock;
class GameItem;

class GameScene : public QGraphicsScene, public Scene {

Q_OBJECT

friend class GameWidget;

protected:
	enum MoveState {Standing, Selected, Moving, Return};

private:
	QPointF lastClickedPosition;
	QPointF cursorPosition;

	QPoint start;
	QPoint finish;

	QTimer* timeTimer;
	QTimer* advanceTimer;

	ConfirmDialog* confirmDialog;
	GameProfile* profile;
	GameBackground* gameBackground;
	GameMenu* menu;
	GameItem* currentItem;

	int xoffset, yoffset;
	int currentRow, currentColumn;
	int targetRow, targetColumn;
	int lx, ly;

	bool paintState;
	bool inputDisabled;
	bool levelStart;

	QPixmap menuBackgroundImage;
	QPixmap gameBackgroundImage;
	QList<QPixmap> allItems;
	QList<QPixmap> pixmapitems;

	QList<int> activeItems;

	MoveState runningState;

	bool hintMove;
	QString hintText;

	QString signature;
	QString levelName;
	QString packname, author, comment;
	int levelCount, position, offset, size, items;
	QString sig;
	qreal time;

public:
	GameScene(QWidget* parent);
	virtual ~GameScene();

	QString getSchemePath() const;

	void initGame();
	bool initLevel(int level);
	void initProfile();
	void initGraphics();

	void exitToMainMenu();

	const QPoint getCursorPosition() const;
	
	void drawBackground(QPainter* qPainter, const QRectF& rectF);
	void drawHint(QPainter& painter);
	void drawLevelStartup(QPainter& painter);
	void drawLevel(QPainter& painter);
	void drawTransRect(QPainter& p, const QRect& rectangle, QColor borderColor, QColor backgroundColor, qreal opacity);
	void drawHud();
	
	void updateCounters(QPainter& painter);
	void updateTempItems();

	void loadGameBackground();
	void loadMenuBackground();

	void setProfile(GameProfile* profile);

	void pauseGame();
	void stopGame();

	void runNextLevel();
	bool loadLevel(int level);
	bool loadScene();

	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
	void keyPressEvent(QKeyEvent* keyEvent);

	void selectActiveItems(int itemCount);
	GameItem* createGameItem(const QSet<int>& ids = QSet<int>());

	bool fillEmptyAvailableRows();
	void fillItems(int deep);

	bool checkAvailableMoves();
	bool checkItemMovement();
	void checkMouseActions();

	bool checkAvailableItemsInRow(struct PlaceAvailableInfo* placeAvailable, 
		int row, int source, int target);
	bool checkAvailableItemsInColumn(struct PlaceAvailableInfo* placeAvailable, 
		int column, int source, int target);

	bool checkRemoveItemsInRow(int row, int source, int target, bool remove=true);
	bool checkRemoveItemsInColumn(int column, int source, int target, bool remove=true);
	bool checkRemoveItems();
	bool checkMoveDownItems();

	bool checkPlaces(PlaceAvailableInfo& p1, PlaceAvailableInfo& p2, PlaceAvailableInfo& p3);

	void removeItemsColumn(int row, int column, int count);
	void removeItemsRow(int row, int column, int count);
	void removeAndCountItem(int row, int column);
	void removeAndCountItemOnly(int row, int column);
	void removeBlock(PlaceInfo& place, int row, int column);
	void removeTarget(PlaceInfo& place, int row, int column);
	void startRemovingItems();

	void executeMovement();
	void startItemMove(int sourceRow, int sourceColumn, int targetRow, int targetColumn);

	void closeEvent(QCloseEvent& evt);
	bool confirmExit();

	void deselectCurrentItem();

protected slots:
	void onMenuNew();
	void onMenuContinue();
	void onMenuExit();
	void onMenuRestartLevel();
	void onMenuVideoModeChanged();
	void onMenuGamePack();
	void onMenuGameExit();
	void onMenuGameStart();
	void countTime();
	void nextLoop();
};

#endif