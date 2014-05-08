#include "GameScene.h"
#include "BaseItem.h"
#include "GameItem.h"
#include "GameWidget.h"
#include "GameBackground.h"

#include <iostream>

static int ixx = 0;

GameScene::GameScene(QWidget* parent) :
	QGraphicsScene(parent) {

	confirmDialog = new ConfirmDialog();
	addWidget(confirmDialog, Qt::FramelessWindowHint);
	confirmDialog->hide();

	menu = new GameMenu();
	addWidget(menu);
	menu->activate(true);
	menu->addConfirmDialog(confirmDialog);

	currentItem = NULL;

	connect(menu, SIGNAL(menuNew()), this, SLOT(onMenuNew()));
	connect(menu, SIGNAL(menuContinue()), this, SLOT(onMenuContinue()));
	connect(menu, SIGNAL(menuExit()), this, SLOT(onMenuExit()));
	connect(menu, SIGNAL(menuRestartLevel()), this, SLOT(onMenuRestartLevel()));
	connect(menu, SIGNAL(menuVideoModeChanged()), this, SLOT(onMenuVideoModeChanged()));
	connect(menu, SIGNAL(menuLevelPack()), this, SLOT(onMenuGamePack()));
	connect(menu, SIGNAL(menuGameExit()), this, SLOT(onMenuGameExit()));
	connect(menu, SIGNAL(menuGameStart()), this, SLOT(onMenuGameStart()));

	lx = ly = 0;

	advanceTimer = new QTimer(this);
	advanceTimer->setInterval(30);
	advanceTimer->start();
	connect(advanceTimer, SIGNAL(timeout()), this, SLOT(nextLoop()));
	
	timeTimer = new QTimer(this);
	timeTimer->setInterval(1000);
	timeTimer->start();
	connect(timeTimer, SIGNAL(timeout()), this, SLOT(countTime()));

	setSceneRect(0, 0, WIDTH, HEIGHT);

	gameBackground = new GameBackground();
	setBackgroundBrush(Qt::black);
	setDefaultGameCursor();

	maxLevel = 10;
	inputDisabled = true;

	initGame();
}

GameScene::~GameScene() {
	delete confirmDialog;
	delete gameBackground;
	delete menu;
}

QString GameScene::getSchemePath() const {
	return GameWidget::getResourcePath() + "/schemes";
}

void GameScene::initGame() {
	concrete = QPixmap(GameWidget::getResourcePath() + "/images/concrete1.png");
	if(concrete.isNull())
		concrete = gameStock->concrete;

	block1 = QPixmap(GameWidget::getResourcePath() + "/images/block1.png");
	if(block1.isNull())
		block1 = gameStock->block1;

	block2 = QPixmap(GameWidget::getResourcePath() + "/images/block2.png");
	if(block2.isNull())
		block2 = gameStock->block2;

	target1 = QPixmap(GameWidget::getResourcePath() + "/images/target1.png");
	if(target1.isNull())
		target1 = gameStock->target1;

	target2 = QPixmap(GameWidget::getResourcePath() + "/images/target2.png");
	if(target1.isNull())
		target1 = gameStock->target1;


	item1 = QPixmap(GameWidget::getResourcePath() + "/images/item1.png");
	if(item1.isNull())
		item1 = gameStock->item1;

	item2 = QPixmap(GameWidget::getResourcePath() + "/images/item2.png");
	if(item1.isNull())
		item1 = gameStock->item2;

	item3 = QPixmap(GameWidget::getResourcePath() + "/images/item3.png");
	if(item3.isNull())
		item3 = gameStock->item3;

	item4 = QPixmap(GameWidget::getResourcePath() + "/images/item4.png");
	if(item4.isNull())
		item4 = gameStock->item4;

	item5 = QPixmap(GameWidget::getResourcePath() + "/images/item5.png");
	if(item5.isNull())
		item5 = gameStock->item5;

	item6 = QPixmap(GameWidget::getResourcePath() + "/images/item6.png");
	if(item6.isNull())
		item1 = gameStock->item6;


	//allItems.clear();

	QStringList extension;
	extension << "item*.png";
	QStringList entries = QDir().entryList(extension);

	for(int i = 0; i < MAX_ITEMS_COUNT; i++) {
		QPixmap pixmap(gameStock->items.at(i));
		allItems.append(pixmap);
	}
	loadMenuBackground();
}

void GameScene::initProfile() {}

bool GameScene::initLevel(int level) {
	if(maxLevel == 0)
		return false;

	int fieldSize = MAX_COLUMNS * MAX_ROWS;
	
	for(int i = 0; i < fieldSize; i++) {
		PlaceInfo& place = field[i];
		if(place.hasItem())
			delete place.item;
		field[i] = PlaceInfo();
	}
	
	qDeleteAll(tempItems);
	tempItems.clear();

	currentColumn = currentRow = -1;
	targetColumn = currentRow = -1;
	lastClickedPosition = QPoint();
	runningState = MoveState::Standing;

	hintMove = true;
	hintText = "Hint Text";
	paintState = false;
	inputDisabled = true;
	this->level = level;
	targets = 0;
	frameCount = 1;

	if(level > maxLevel)
		return true;
	if(!loadLevel(level))
		return false;

	gameBackground->init();

	profile->setGameStarted(true);
	profile->setGamePaused(false);
	
	loadGameBackground();
	drawHud();
	setDefaultGameCursor();

	createStaticPopup(QRect(320, 320, DX(310), DY(300)), tr("Level %1").arg(level),
		Qt::AlignCenter, gameStock->font12, Qt::white, 1, 0, 50, 50);

	levelStart = true;
	return true;
}

void GameScene::initGraphics() {
	gameStock->initGraphics();
	menu->initGraphics();
}

bool GameScene::loadScene() {
	int itemCount = 6;
	if(itemCount > allItems.count())
		return false;
	
	selectActiveItems(itemCount);
	
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place = getPlace(i, j);

			if(i == (rows-1) || i == (rows-2)) {
				place.place = Target1;
			} else {
				place.place = ItemStart | ItemVisible;
			}

			if(place.canCreateItem()) {
				QSet<int> ids;
				if(j > 0) {
					if(getPlace(i, j).item && getPlace(i, j-1).item) {
						int ix = getPlace(i,j-1).item->getIx();
						if(ix == getPlace(i,j).item->getIx()) {
							ids.insert(ix);
						}
					}
				}
				if(i > 0) {
					if(getPlace(i, j).item && getPlace(i-1, j).item) {
						int ix = getPlace(i-1,j).item->getIx();
						if(ix == getPlace(i,j).item->getIx()) {
							ids.insert(ix);
						}
					}
				}
				place.item = createGameItem(ids);
			}
			
			if(place.hasTarget()) {
				if(place.place & Target1)
					targets++;
				else
					targets+=2;
			}
		}
	}
	return true;
}

const QPoint GameScene::getCursorPosition() const { 
	return cursorPosition.toPoint(); 
}

void GameScene::setProfile(GameProfile* profile) {
	this->profile = profile;
	menu->addProfile(profile);
}

void GameScene::updateCounters(QPainter& painter) {
	painter.setPen(QPen(Qt::white));
	painter.setOpacity(1);
	painter.setFont(gameStock->font12);
}

void GameScene::updateTempItems() {
	for(int i = 0; i < tempItems.count(); i++) {
		BaseItem* item = tempItems.at(i);
		if(!item->next()) {
			delete item;
			tempItems[i] = 0;
		}
	}
	tempItems.removeAll(0);
}

void GameScene::loadGameBackground() {
	QStringList extensions;
	extensions << "bggame.jpg" << "bggame.png";
	QString resources = GameWidget::getResourcePath() + "/images/game/";
	QStringList entries = QDir(resources).entryList(extensions);
	QString gameBackgroundPath = resources + entries.at(0);
	gameBackgroundImage = QPixmap(gameBackgroundPath);
}

void GameScene::loadMenuBackground() {
	QStringList extensions;
	extensions << "bgmenu.jpg" << "bgmenu.png";
	QString resources = GameWidget::getResourcePath() + "/images/menu/";
	QStringList entries = QDir(resources).entryList(extensions);
	QString menuBackgroundPath = resources + entries.at(0);
	menuBackgroundImage = QPixmap(menuBackgroundPath);
}

void GameScene::countTime() {}

void GameScene::nextLoop() {
	static bool checkMoves = true;
	updateTempItems();
	--frameCount;
	if(!frameCount) {
		frameCount = 5;
		if(targets <= 0) {
			qDebug() << "level won";
		} else {
			int oldTargets = targets;
			bool isFill = fillEmptyAvailableRows();
			bool isFall = checkMoveDownItems();

			if(runningState == MoveState::Moving || runningState == MoveState::Return)
				checkItemMovement();

			bool isRemove = checkRemoveItems();
			startRemovingItems();

			if(isFill || isFall || isRemove) {
				checkMoves = true;
			} else if(!isFill && !isFall && !isRemove) {
				if(checkMoves) {
					checkMoves = false;
					int depth = 0;
					while(!checkAvailableMoves()) {
						fillItems(depth++);
						if(depth > MAX_ROWS)
							break;
					}
				}
			}

			if(currentItem) {
				if(!currentItem->isStanding() && !currentItem->isSelected()) {
					currentItem = NULL;
					runningState = MoveState::Standing;
				}
			}

			if(runningState == MoveState::Standing || runningState == MoveState::Selected)
				checkMouseActions();
		}
	}
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place = getPlace(i,j);
			place.setUnProcessed();
			if(place.hasItem()) {
				place.item->next();
			}
		}
	}
	update();
}

void GameScene::exitToMainMenu() {
	qDebug() << "show main menu";

	profile->setGameStarted(false);
	profile->setGamePaused(false);

	menu->setCurrentIndex(MENU_MAIN);
	menu->activate(true);
	menu->showNormal();
}

void GameScene::pauseGame() {}

void GameScene::stopGame() {
	//profile->setGameStarted(false);
	//profile->setGamePaused(false);
}

void GameScene::runNextLevel() {
	profile->setGameStarted(true);
	profile->setGamePaused(false);
	int lvl = profile->getCurrentPlayerInfo()->getCurrentLevelInfo()->level;
	if(initLevel(lvl)) {
		exitToMainMenu();
	}
}

bool GameScene::loadLevel(int level) {
	return loadScene();
}

void GameScene::onMenuNew() {
	if(isActive()) {
		if(profile->getCurrentPlayerInfo()->getCurrentLevelInfo()->level > 1) {
			if(!confirmDialog->exec()) {
				menu->showNormal();
				return;
			}
		}
		menu->setMenuPage(MENU_GAME);
		menu->activate(true);
	}
}

void GameScene::onMenuContinue() {
	int maxLevel = 10;
	if(profile->getCurrentPlayerInfo()->getCurrentLevelInfo()->level > maxLevel)
		return;
	
	menu->activate(true);
	profile->setGameStarted(true);
	profile->setGamePaused(false);
	if(!initLevel(profile->getCurrentPlayerInfo()->getCurrentLevelInfo()->level))
		exitToMainMenu();
}

void GameScene::onMenuExit() {
	menu->hide();
	if(confirmDialog->execute(tr("quit"))) {
		stopGame();
		qApp->quit();
	}
	menu->showNormal();
}

void GameScene::onMenuRestartLevel() {}

void GameScene::onMenuVideoModeChanged() {}

void GameScene::onMenuGamePack() {}

void GameScene::onMenuGameExit() {}

void GameScene::onMenuGameStart() {
	menu->activate(false);
	profile->setGameStarted(true);
	profile->setGamePaused(false);
	if(!initLevel(1))
		exitToMainMenu();
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);

	if(mouseEvent->isAccepted())
		return;
	
	mouseEvent->accept();
	if(mouseEvent->button() == Qt::LeftButton) {
		menu->checkMenuMouseActions(mouseEvent->scenePos().toPoint());
		lastClickedPosition = mouseEvent->scenePos();
	}
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) {
	QGraphicsScene::mouseMoveEvent(mouseEvent);
	if(mouseEvent->isAccepted())
		return;

	this->cursorPosition = mouseEvent->scenePos();
	mouseEvent->accept();
	if(mouseEvent->button() & Qt::LeftButton 
		&& MoveState::Selected == runningState) {
		this->lastClickedPosition = mouseEvent->scenePos();
		return;
	}
}

void GameScene::keyPressEvent(QKeyEvent* keyEvent) {
	bool gameWon = false;
	bool active = true;
	if(!menu->isActive()) {
		switch(keyEvent->key()) {
			case Qt::Key_Escape:
				if(active) {
					if(gameWon) {
						this->exitToMainMenu();
						break;
					}
					this->runNextLevel();
					break;
				}
				this->pauseGame();
				break;
		}
	} else {
		QGraphicsScene::keyPressEvent(keyEvent);
	}
}

void GameScene::checkMouseActions() {
	if(lastClickedPosition.isNull()) {
		return;
	}

	QPoint clickedPosition(lastClickedPosition.toPoint());
	lastClickedPosition = QPointF();

	int c = clickedPosition.x() >= xoffset ? (clickedPosition.x()-xoffset)/X56 : -1;
	int r = clickedPosition.y() >= yoffset ? (clickedPosition.y()-yoffset)/Y56 : -1;
	bool itemClicked = (c >= 0 && c < columns && r >= 0 && r < rows);

	if(itemClicked) {
		PlaceInfo& place = getPlace(r,c);
		if(!place.hasItem()) {
			deselectCurrentItem();
			return;
		}
		if(place.item == currentItem) {
			return;
		}
		if(!place.canMoveItem())
			return;

		if(currentItem) {
			currentItem->isStanding();
			int rowNeighbour = std::abs(r-currentRow);
			int columnNeighbour = std::abs(c-currentColumn);
			if((rowNeighbour == 0 && columnNeighbour == 1) || 
			   (rowNeighbour == 1 && columnNeighbour == 0)) {
				if(place.canMoveItem()) {
					targetRow = r;
					targetColumn = c;
					runningState = MoveState::Moving;
					executeMovement();
					return;
				}
			}
		}

		currentItem = place.item;
		currentRow = r;
		currentColumn = c;
		currentItem->select();
		runningState = MoveState::Selected;
		return;
	}
	deselectCurrentItem();
}

void GameScene::selectActiveItems(int itemCount) {
	if(activeItems.count() == itemCount)
		return;

	activeItems.clear();
	for(int i = 0; i < itemCount; i++)
		activeItems.append(i);
}

GameItem* GameScene::createGameItem(const QSet<int>& ids) {
	int ix = activeItems.at(qrand() % activeItems.count());
	if(ids.contains(ix)) {
		for(int i = 0; i < activeItems.count(); i++) {
			if(!ids.contains(activeItems.at(i)))
				break;
		}
	}
	GameItem* item = new GameItem(ix);
	return item;
}

bool GameScene::fillEmptyAvailableRows() {
	bool result = false;
	
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place = getPlace(i,j);
			if(place.canVisibleItem()) {
				place.item = createGameItem();
				place.item->standing();
				result = true;
			}
		}
	}
	return result;
}

bool GameScene::checkAvailableMoves() {
	bool result = false;
	start = QPoint(-1,-1);
	finish = QPoint(-1,-1);

	PlaceAvailableInfo* fieldAvailable = new PlaceAvailableInfo[MAX_COLUMNS*MAX_ROWS];

	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			PlaceAvailableInfo& placeAvailable1 = getAvailablePlace(i,j);
			PlaceInfo& place1 = getPlace(i,j);
			
			placeAvailable1.hasItem = place1.hasItem();
			if(placeAvailable1.hasItem) {
				placeAvailable1.movable = place1.canMoveItem();
				placeAvailable1.ix = place1.item->getIx();
			} else {
				placeAvailable1.movable = false;
				placeAvailable1.ix = -1;
			}
		}
	}

	for(int i = rows-1; i >= 0; i--) {
		for(int j = 0; j < columns-1; j++) {
			PlaceAvailableInfo placeSource = getAvailablePlace(i,j);
			if(!placeSource.movable)
				continue;

			//right
			PlaceAvailableInfo placeTarget = getAvailablePlace(i,j+1);
			if(placeTarget.movable) {
				PlaceAvailableInfo temp1 = placeSource;
				PlaceAvailableInfo temp2 = placeTarget;
				getAvailablePlace(i,j) = temp2;
				getAvailablePlace(i,j+1) = temp1;
				
				currentRow = i;
				currentColumn = j;
				targetRow = i;
				targetColumn = j+1;

				bool b1 = checkAvailableItemsInRow((PlaceAvailableInfo*)fieldAvailable, targetRow, 
					qMax(0,targetColumn-2), qMin(columns-1,targetColumn+2));
				
				bool b2 = checkAvailableItemsInColumn((PlaceAvailableInfo*)fieldAvailable, targetColumn, 
					qMax(0,targetRow-2), qMin(rows-1,targetRow+2));
				
				bool b3 = checkAvailableItemsInRow((PlaceAvailableInfo*)fieldAvailable, currentRow, 
					qMax(0,currentColumn-2), qMin(columns-1,currentColumn+2));
				
				bool b4 = checkAvailableItemsInColumn((PlaceAvailableInfo*)fieldAvailable, currentColumn, 
					qMax(0,currentRow-2), qMin(rows-1,currentRow+2));

				getAvailablePlace(i,j) = temp1;
				getAvailablePlace(i,j+1) = temp2;

				if(b1 || b2 || b3|| b4) {
					this->start = QPoint(currentColumn, currentRow);
					this->finish = QPoint(targetColumn, targetRow);
					result = true;
					delete [] fieldAvailable;
					return result;
				}
			}

			//top
			if(i == 0)
				continue;
			placeSource = getAvailablePlace(i-1, j);
			if(placeSource.movable) {
				PlaceAvailableInfo temp1 = placeTarget;
				PlaceAvailableInfo temp2 = placeSource;
				getAvailablePlace(i,j) = temp2;
				getAvailablePlace(i-1,j) = temp1;

				currentRow = i;
				currentColumn = j;
				targetRow = i-1;
				targetColumn = j;

				bool b1 = checkAvailableItemsInRow((PlaceAvailableInfo*)fieldAvailable, 
					targetRow, qMax(0,targetColumn-2), qMin(columns-1,targetColumn+2));
				
				bool b2 = checkAvailableItemsInColumn((PlaceAvailableInfo*)fieldAvailable, 
					targetColumn, qMax(0,targetRow-2), qMin(rows-1,targetRow+2));
				
				bool b3 = checkAvailableItemsInRow((PlaceAvailableInfo*)fieldAvailable,
					currentRow, qMax(0,currentColumn-2), qMin(columns-1,currentColumn+2));
				
				bool b4 = checkAvailableItemsInColumn((PlaceAvailableInfo*)fieldAvailable,
					currentColumn, qMax(0,currentRow-2), qMin(rows-1,currentRow+2));

				getAvailablePlace(i,j) = temp1;
				getAvailablePlace(i-1,j) = temp2;

				if(b1 || b2 || b3 || b4) {
					this->start = QPoint(currentColumn, currentRow);
					this->finish = QPoint(targetColumn, targetRow);
					result = true;
					delete [] fieldAvailable;
					return result;
				}
			}
		}
	}
	return result;
}

bool GameScene::checkItemMovement() {
	bool result = false;
	if(runningState == MoveState::Moving) {
		bool b1 = checkRemoveItemsInRow(targetRow, qMax(0, targetColumn-2), qMin(columns-1, targetColumn+2));
		bool b2 = checkRemoveItemsInColumn(targetColumn, qMax(0, targetRow-2), qMin(rows-1, targetRow+2));
		bool b3 = checkRemoveItemsInRow(currentRow, qMax(0, currentColumn-2), qMin(columns-1, currentColumn+2));
		bool b4 = checkRemoveItemsInColumn(currentColumn, qMax(0, currentRow-2), qMin(rows-1, currentRow+2));
		if(b1 || b2 || b3 || b4) {
			result = true;
		} else {
			runningState = MoveState::Return;
		}
	}

	if(runningState == MoveState::Return) {
		PlaceInfo& place1 = getPlace(targetRow, targetColumn);
		PlaceInfo& place2 = getPlace(currentRow, currentColumn);

		place1.item = place2.item;
		place1.item->falling(targetColumn-currentColumn, targetRow-currentRow);

		place2.item = currentItem;
		place2.item->falling(currentColumn-targetColumn, currentRow-targetRow);
	}

	runningState = MoveState::Standing;
	currentItem = 0;
	currentRow = currentColumn = -1;
	targetRow = targetColumn = -1;
	return result;
}

bool GameScene::checkAvailableItemsInRow(struct PlaceAvailableInfo* placeAvailable, 
	int row, int source, int target) {
	
	PlaceAvailableInfo& p1 = getAvailablePlace(row, source);
	PlaceAvailableInfo& p2 = getAvailablePlace(row, source+1);
	PlaceAvailableInfo& p3 = getAvailablePlace(row, source+2);
	PlaceAvailableInfo& p4 = getAvailablePlace(row, source+3);
	PlaceAvailableInfo& p5 = getAvailablePlace(row, source+4);

	if(source+2 > target)
		return false;
	checkPlaces(p1, p2, p3);
	if(source+3 > target)
		return false;
	checkPlaces(p2, p3, p4);
	if(source+4 > target)
		return false;
	checkPlaces(p3, p4, p5);
	return false;
}

bool GameScene::checkAvailableItemsInColumn(struct PlaceAvailableInfo* placeAvailable, 
	int column, int source, int target) {

	PlaceAvailableInfo& p1 = getAvailablePlace(source,   column);
	PlaceAvailableInfo& p2 = getAvailablePlace(source+1, column);
	PlaceAvailableInfo& p3 = getAvailablePlace(source+2, column);
	PlaceAvailableInfo& p4 = getAvailablePlace(source+3, column);
	PlaceAvailableInfo& p5 = getAvailablePlace(source+4, column);

	if(source+2 > target)
		return false;
	checkPlaces(p1, p2, p3);
	if(source+3 > target)
		return false;
	checkPlaces(p2, p3, p4);
	if(source+4 > target)
		return false;
	checkPlaces(p3, p4, p5);
	return false;
}

bool GameScene::checkRemoveItemsInRow(int row, int source, int target, bool remove) {
	bool result = false;
	for(int j = target; j >= source && j >= 2; j--) {
		PlaceInfo& place1 = getPlace(row, j);
		
		if(!place1.hasItem())
			continue;
		if(!(place1.item->isStanding() || place1.item->isDead()))
			continue;

		int ix = place1.item->getIx();
		int count = 1;
		while(j--) {
			PlaceInfo &place2 = getPlace(row,j);
			if(!place2.hasItem())
				break;
			if(place2.item->getIx() != ix || !(place2.item->isStanding() || place2.item->isDead()))
				break;
			count++;
		}
		j++;
		if(count >= 3) {
			if(remove) {
				removeItemsRow(row,j,count);
			}
			result = true;
		}
	}
	return result;
}

bool GameScene::checkRemoveItemsInColumn(int column, int source, int target, bool remove) {
	bool result = false;
	for(int i = target; i >= source && i >= 2; i--) {
		PlaceInfo& place1 = getPlace(i, column);
		if(!place1.hasItem())
			continue;
		if(!(place1.item->isStanding() || place1.item->isDead())) {
			continue;
		}
		int ix = place1.item->getIx();
		int count = 1;
		while(i--) {
			PlaceInfo& place2 = getPlace(i, column);
			if(!place2.hasItem())
				break;
			if(place2.item->getIx() != ix || !(place2.item->isStanding() || place2.item->isDead())) {
				break;
			}
			count++;
		}
		i++;
		if(count >= 3) {
			if(remove)
				removeItemsColumn(i,column,count);
			result = true;
		}
	}
	return result;
}

bool GameScene::checkRemoveItems() {
	bool result = false;
	for(int i = 0; i < rows; i++) {
		if(checkRemoveItemsInRow(i,0,columns-1))
			result = true;
	}

	for(int j = 0; j < columns; j++) {
		if(checkRemoveItemsInColumn(j,0,rows-1))
			result = true;
	}
	return result;
}

bool GameScene::checkMoveDownItems() {
	bool result = false;
	for(int i = rows-1; i > 0; i--) {
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place1 = getPlace(i,j);
			if(!place1.canPlaceItem()) {
				continue;
			}

			//check top
			PlaceInfo& place2 = getPlace(i-1,j);
			if(place2.canMoveItem()) {
				startItemMove(i-1,j,i,j);
				result = true;
				continue;
			}
			//check top-left
			if(j > 0) {
				PlaceInfo& place3 = getPlace(i-1, j-1);
				if(place3.canMoveItem()) {
					startItemMove(i-1,j-1,i,j);
					result = true;
					continue;
				}
			}
			//check top-right
			if(j < columns-1) {
				PlaceInfo& place4 = getPlace(i-1, j+1);
				if(place4.canMoveItem()) {
					if(getPlace(i,j+1).canPlaceItem() == false) {
						startItemMove(i-1,j+1,i,j);
						result = true;
						continue;
					}
				}
			}
		}
	}
	return result;
}

void GameScene::removeItemsColumn(int row, int column, int count) {
	for(int j = row; j < row+count; j++)
		removeAndCountItem(j, column);
}

void GameScene::removeItemsRow(int row, int column, int count) {
	for(int j = column; j < column+count; j++)
		removeAndCountItem(row, j);
}

void GameScene::removeAndCountItem(int row, int column) {
	PlaceInfo& place = getPlace(row, column);
	if(place.isProcessed())
		return;
	
	place.setProcessed();
	bool remove = true;


	if(place.hasBlock()) {
		remove = false;
		removeBlock(place, row, column);
	} else if(place.hasTarget()) {
		targets--;
		removeTarget(place, row, column);
	} else if(!place.item->isDead()) {
		//pass
	}
	if(remove && !place.item->isDead()) {
		place.item->die();
	}
}

void GameScene::removeAndCountItemOnly(int row, int column) {}

void GameScene::removeBlock(PlaceInfo& place, int row, int column) {
	bool block = true;
	if(place.place & Block2) {
		block = false;
		place.place = (place.place & ~Block2) | Block1;
	} else {
		place.place = (place.place & ~Block1);
	}
	createPixmapPopup(row, column, (qrand() % 3) - 1, (qrand() % 3) - 1, block ? block1 : block2, 50);
}

void GameScene::removeTarget(PlaceInfo& place, int row, int column) {
	bool target = true;
	if(place.place & Target2) {
		target = false;
		place.place = (place.place & ~Target2) | Target1;
	} else {
		place.place = (place.place & ~Target1);
	}
	createPixmapPopup(row, column, (qrand() % 3) - 1, (qrand() % 3) - 1, target ? target1 : target2, 50);
}

void GameScene::startRemovingItems() {
	bool removed = false;
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place = getPlace(i,j);
			if(!place.hasItem())
				continue;
			if(place.item->isDead()) {
				delete place.item;
				place.item = 0;
				removed = true;
			}
		}
	}
}

void GameScene::executeMovement() {
	PlaceInfo &place1 = getPlace(targetRow, targetColumn);
	PlaceInfo &place2 = getPlace(currentRow, currentColumn);

	place2.item = place1.item;
	qDebug() << currentColumn-targetColumn << currentRow-targetRow << "place2";
	place2.item->falling(currentColumn-targetColumn, currentRow-targetRow);

	place1.item = currentItem;
	qDebug() << targetColumn-currentColumn << targetRow-currentRow << "place1";
	place1.item->falling(targetColumn-currentColumn, targetRow-currentRow);
}

void GameScene::startItemMove(int cRow, int cColumn, int tRow, int tColumn) {
	PlaceInfo& place1 = getPlace(cRow, cColumn);
	PlaceInfo& place2 = getPlace(tRow, tColumn);

	place2.item = place1.item;
	place2.item->falling(tColumn-cColumn, tRow-cRow);
	place1.item = 0;
}

bool GameScene::checkPlaces(PlaceAvailableInfo& p1, PlaceAvailableInfo& p2, PlaceAvailableInfo& p3){
	if(p1.ix == p2.ix && p2.ix == p3.ix && p3.ix != -1)
		return true;
}

void GameScene::fillItems(int deep) {
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place = getPlace(i,j);

			if(place.item == 0) {
			}

			if(place.item && (place.place & ItemVisible)) {
				place.item->setIx(activeItems.at(qrand() % activeItems.count()));
			}
		}
	}

	for(int d = 0; d < deep; d++) {
		for(int i = 0; i < rows; i++) {
			for(int j = 0; j < columns; j++) {
				PlaceInfo& pi1 = getPlace(i,j);
				if(pi1.item && (pi1.place & ItemVisible)) {
					int ii = i + d;
					if(ii >= rows) {
						ii = rows-1;
					}
					PlaceInfo& pi2 = getPlace(ii, j);
					if(pi2.item)
						pi2.item->setIx(activeItems.at(qrand() % activeItems.count()));
				}
			}
		}
	}
}

void GameScene::drawBackground(QPainter* qPainter, const QRectF& rectF) {
	QPainter& painter(*qPainter);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setOpacity(1);
	paintState = true;

	if(menu->isActive()) {
		painter.drawPixmap(0, 0, menuBackgroundImage);
		painter.setFont(gameStock->font12);
		painter.setPen(QPen(Qt::white));

		QString gameVersion = "1";
		painter.drawText(DX(10), DY(10), QString("Version %1").arg(gameVersion));
	} else {
		painter.drawPixmap(0, 0, gameBackgroundImage);
		pixmapitems.push_back(item1);
		pixmapitems.push_back(item2);
		pixmapitems.push_back(item3);
		pixmapitems.push_back(item4);
		pixmapitems.push_back(item5);
		pixmapitems.push_back(item6);

		drawLevelStartup(painter);
	}
}

void GameScene::drawLevelStartup(QPainter& painter) {
	for(int i = 0; i < rows; i++) {
		yoffset = (HEIGHT-rows*X56)/2;
		int rowy = i * X56 + yoffset;
		for(int j = 0; j < columns; j++) {
			PlaceInfo& place = getPlace(i,j);
			if(place.empty()) {
				xoffset = (WIDTH-columns*Y56)/2;
				int colx = (j * Y56) + xoffset;
				painter.setOpacity(0.8);
				painter.drawPixmap(colx, rowy, concrete);

				if(place.hasTarget()) {
					painter.setOpacity(1);
					if(j % 2 == 0) {
						painter.drawPixmap(colx, rowy, target1);
					} else {
						painter.drawPixmap(colx, rowy, target2);
					}
				}

				if(place.hasItem()) {
					painter.setOpacity(place.item->getOpacity());

					//painter.drawPixmap(colx + place.item->getdx(),
					//	rowy + place.item->getdy(),
					//	allItems.at(place.item->getIx()));
					//painter.drawPixmap(50, 50, pixmapitems.at(0));

					//std::cout << place.item->getIx() << std::endl;

					painter.drawPixmap(colx + place.item->getdx(),
						rowy + place.item->getdy(), pixmapitems.at(place.item->getIx()));
				}
			}
		}
	}
	levelStart = false;
}

void GameScene::drawHint(QPainter& painter) {}

void GameScene::drawLevel(QPainter& painter) {
	/*
	painter.setOpacity(1);
	int r = 4;
	int c = 4;
	for(int i = 0; i < r; i++) {
		yoffset = (HEIGHT-rows*X56)/2;
		int rowy = i * X56 + yoffset;
		for(int j = 0; j < c; j++) {
			PlaceInfo& place1 = getPlace(i,j);
			if(place1.empty()) {
				xoffset = (WIDTH-columns*Y56)/2;
				int colx = (j * Y56) + xoffset;

				if(place1.hasTarget()) {
					//painter.drawPixmap(colx, rowy, place1.place & Target1 ? target1 : target2);
					painter.drawPixmap(colx, rowy, target1);
				}
			}
		}
	}

	for(int k = 0; k < r; k++) {
		for(int h = 0; h < c; h++) {
			PlaceInfo& place2 = getPlace(k,h);
			if(place2.hasItem()) {
				if(place2.item->isSelected()) {
					painter.setOpacity(0.3);
					painter.setBrush(Qt::yellow);
				}
				//painter.setOpacity(place2.item->setOpacity(0.4));
				//painter.drawPixmap();
			}

			if(place2.hasBlock()) {}
		}
	}
	*/
}

void GameScene::drawHud() {
	QPainter painter(&gameBackgroundImage);
	painter.setOpacity(0.3);
	painter.setPen(QPen(Qt::white, 2));
	painter.setBrush(Qt::black);

	painter.drawRect(900-1, 1, 1024-900, 210);
    painter.drawRect(900-1, 240-1, 1024-900, 768-240);
    painter.drawRect(1, 710-1, 860, 58);

	painter.setPen(QPen(Qt::white));
	painter.setOpacity(1);
	painter.setFont(gameStock->font12);

	painter.drawText(910, 10, 100, 20, Qt::AlignLeft, "Level 1");
}

void GameScene::drawTransRect(QPainter& painter, const QRect& rectangle, QColor borderColor, 
	QColor backgroundColor, qreal opacity) {
	
	painter.setOpacity(opacity);
	painter.setPen(QPen(borderColor, 4));
	painter.setBrush(backgroundColor);
	painter.drawRect(rectangle);
}

void GameScene::closeEvent(QCloseEvent& evt) {
	if(!confirmExit())
		evt.ignore();
}

bool GameScene::confirmExit() {
	if(menu->isActive())
		menu->hide();
	if(confirmDialog->execute(tr("aaa")));
		return true;
	if(menu->isActive())
		menu->showNormal();
	return false;
}

void GameScene::deselectCurrentItem() {
	if(currentItem) {
		currentItem->standing();
		currentItem = NULL;
		runningState = MoveState::Standing;
	}
}