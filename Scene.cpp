#include "Scene.h"
#include "Defines.h"
#include "Scaler.h"
#include "TextItem.h"
#include "PixmapItem.h"

bool PlaceInfo::hasItem() const {
	return item;
}

bool PlaceInfo::empty() const {
	return !(place & PlaceConcrete);
}

bool PlaceInfo::canPlaceItem() const {
	return (!item && empty());
}

bool PlaceInfo::canVisibleItem() const {
	return (canPlaceItem() && (place & ItemVisible));
}

bool PlaceInfo::canCreateItem() const {
	return (canPlaceItem() && (place & ItemStart));
}

bool PlaceInfo::canMoveItem() const {
	return (item && !(place & Blocks) && (item->isStanding() || item->isSelected()));
}

bool PlaceInfo::canHighlightItem() const {
	return (item && (item->isStanding() || item->isSelected()));
}

bool PlaceInfo::hasTarget() const { 
	return place & Targets; 
}

bool PlaceInfo::hasBlock() const { 
	return place & Blocks; 
}

bool PlaceInfo::isProcessed() const { 
	return place & Processed; 
}

bool PlaceInfo::setProcessed() { 
	return place |= Processed;
}

bool PlaceInfo::setUnProcessed() { 
	return place &= ~Processed; 
}

Scene::Scene() {
	gameStock = new GameStock();
	
	columns = MAX_COLUMNS;
	rows = MAX_ROWS;
}
	
Scene::~Scene() {
	delete gameStock;
}

void Scene::createPixmapPopup(int x, int y, int dx, int dy, const QPixmap& pixmap, int steps) {
	PixmapItem* pixmapItem = new PixmapItem(x, y, dx, dy, pixmap, steps);
	tempItems.append(pixmapItem);
}

void Scene::createStaticPopup(QRect rect, const QString &text, int textFlags,
	const QFont &font, QColor color, qreal opacity, 
	int straySteps, int steps, int dx, int dy) {

	TextItem* textItem = new TextItem(rect, text, textFlags, font, color, 
		opacity, straySteps, steps, dx, dy);
	tempItems.append(textItem);
}

void Scene::setDefaultGameCursor() {
	//QApplication::setOverrideCursor(gameStock->gameCursor);
}

void Scene::setRestoreCursor() {
	//lastCursor = QApplication::overrideCursor() ? (*QApplication::overrideCursor()) : QCursor();
	//while(QApplication::overrideCursor())
	//	QApplication::restoreOverrideCursor();
	//setDefaultGameCursor();

}

void Scene::drawTransRect(QPainter& painter, const QRect& rectangle, QColor borderColor, 
	QColor backgroundColor, qreal opacity) {

	painter.setOpacity(opacity);
	painter.setPen(QPen(borderColor, 2));
	painter.setBrush(backgroundColor);
	int radius = 5;
	painter.drawRoundedRect(rectangle, radius, radius);
}

void Scene::drawTextHint(QPainter& painter) {
	painter.setOpacity(1);
	painter.setPen(QPen(Qt::white));
	painter.setFont(gameStock->font12);
	QString hintText = "Hint";
	painter.drawText(0, DY(690), DX(860), DY(30), Qt::AlignRight, hintText);
}