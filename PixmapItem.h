#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include "BaseItem.h"

class PixmapItem : public BaseItem {

private:
	int x, y, dx, dy;
	QPixmap pixmap;

public:
	PixmapItem(int x, int y, int dx, int dy, const QPixmap& pixmap, int step);
	virtual ~PixmapItem();

	bool next();
	void paint(QPainter& painter);
};

#endif