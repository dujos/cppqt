#ifndef SCENE_H
#define SCENE_H

#include <QtGui\qapplication.h>
#include <QtCore\qlist.h>
#include <QtGui\qpainter.h>
#include <QtGui\qcursor.h>
#include <QtGui\qpixmap.h>

#include "BaseItem.h"
#include "GameItem.h"
#include "GameStock.h"

#define MAX_COLUMNS 8
#define MAX_ROWS 8

class BaseItem;
class Scaler;
class GameStock;

struct PlaceInfo {
	GameItem* item;
	quint32 place;

	PlaceInfo() : item(0), place(NoPlace) {}

	bool hasItem() const;
	bool empty() const;
	bool canPlaceItem() const;
	bool canCreateItem() const;
	bool canMoveItem() const;
	bool canHighlightItem() const;
	bool canVisibleItem() const;

	bool hasTarget() const;
	bool hasBlock() const;

	bool isProcessed() const;
	bool setProcessed();
	bool setUnProcessed();
};

struct PlaceAvailableInfo {
	int ix;
	bool hasItem;
	bool movable;
};

class Scene {
	
protected:
	QCursor lastCursor;
	GameStock* gameStock;
	
	QPixmap concrete;
	QPixmap block1;
	QPixmap block2;
	QPixmap target1;
	QPixmap target2;

	QPixmap item1;
	QPixmap item2;
	QPixmap item3;
	QPixmap item4;
	QPixmap item5;
	QPixmap item6;

	PlaceInfo field[MAX_COLUMNS*MAX_ROWS];
	PlaceAvailableInfo fieldAvailable[MAX_COLUMNS*MAX_ROWS];

	int rows;
	int columns;

	int xcols;
	int xrows;

	int xoffset;
	int yoffset;
	int targets;
	int level;
	int maxLevel;
	int frameCount;

	QList<BaseItem* > tempItems;

public:
	Scene();
	virtual ~Scene();

	void createPixmapPopup(int x, int y, int dx, int dy, const QPixmap& pixmap, int steps);
	void createStaticPopup(QRect rect, const QString &text, int textFlags,
		const QFont &font, QColor color, qreal opacity, 
		int straySteps = 0, int steps = 10,
		int dx = 0, int dy = 0);

	void setDefaultGameCursor();
	void setRestoreCursor();

	void drawTransRect(QPainter& painter, const QRect& rectangle, QColor borderColor, 
		QColor backgroundColor, qreal opacity);
	void drawTextHint(QPainter& painter);

	inline PlaceInfo& getPlace(int row, int column) { return field[row*MAX_COLUMNS + column]; }
	inline PlaceAvailableInfo& getAvailablePlace(int row, int column) { 
		return fieldAvailable[row*MAX_COLUMNS + column]; }

	inline int getNumRows() const { return this->rows; }
	inline int getNumColumns() const { return this->columns; }
};

#endif