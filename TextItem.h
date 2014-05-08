#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "BaseItem.h"

#include <QtGui\qcolor.h>
#include <QtGui\qfont.h>

class TextItem : public BaseItem {

protected:
	QRect rectangle;
	QColor qcolor;
	QFont qfont;
	QString qtxt;

	int dx, dy;
	int staySteps;
	int qflag;

public:
	TextItem(QRect qrect, const QString& qtxt, int qflag, const QFont& qfont,
		const QColor& qcolor, qreal opacity, int staySteps, int step, 
		int dx = 0, int dy = 0);
	virtual ~TextItem();

	virtual bool next();
	virtual void paint(QPainter& painter);
};

#endif