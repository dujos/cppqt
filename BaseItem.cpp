#include "BaseItem.h"

BaseItem::BaseItem(qreal opacity, int step) :
    opacity(opacity),
	step(step) {
	opacityStep = opacity/(double)step; 
}

BaseItem::~BaseItem() {}
