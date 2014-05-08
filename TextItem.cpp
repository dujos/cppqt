#include "TextItem.h"


TextItem::TextItem(QRect qrect, const QString& qtxt, int qflag, const QFont& qfont,
	const QColor& qcolor, qreal opacity, int staySteps, int step, int dx, int dy) : 
  BaseItem(opacity, step),
  rectangle(qrect),
  qcolor(qcolor),
  qfont(qfont),
  dx(dx),
  dy(dy),
  staySteps(staySteps),
  qflag(qflag),
  qtxt(qtxt) {
	opacityStep = opacity/(double)step;
}
	
TextItem::~TextItem() {}

bool TextItem::next() {
	if(staySteps) {
		staySteps--;
		return true;
	}

	if(step-- <= 0)
		return false;

	rectangle.moveLeft(rectangle.left() + dx);
	rectangle.moveTop(rectangle.top() + dy);
	opacity -= opacityStep;
	return true;
}

void TextItem::paint(QPainter& painter) {
	painter.setOpacity(opacity);
	painter.setFont(qfont);
	painter.setPen(QPen(qcolor));
	painter.drawText(rectangle, qflag, qtxt);
}