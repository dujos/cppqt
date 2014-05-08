#ifndef BASEITEM_H
#define BASEITEM_H

#include <QtGui\qpainter.h>
#include <QtGui\qstringlistmodel.h>

class BaseItem {

protected:
	qreal opacity, opacityStep;
	int step;

public:
	BaseItem(qreal opacity, int step);
	virtual ~BaseItem();

	virtual bool next() = 0;
	virtual void paint(QPainter& painter) = 0;
};

#endif