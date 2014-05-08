#include "PixmapItem.h"

PixmapItem::PixmapItem(int xx, int yy, int dxx, int dyy, const QPixmap& pp, int step) :
	BaseItem(1, step),
	x(xx), y(yy), dx(dxx), dy(dyy),
	pixmap(pp) {
}

PixmapItem::~PixmapItem() {

}

bool PixmapItem::next() {
	--step;
	if(step <= 0)
		return false;
	x += dx;
	y += dy;
	opacity += opacityStep;
	return true;
}

void PixmapItem::paint(QPainter& painter) {
	painter.setOpacity(opacity);
	painter.drawPixmap(x, y, pixmap);
}