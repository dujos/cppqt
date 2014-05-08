#include "GameStock.h"
#include "Scaler.h"
#include "GameHelper.h"

#include <iostream>

GameStock::GameStock() {
	initGraphics();
}

void GameStock::init() {}

void GameStock::initGraphics() {
	font12 = QFont("helvetics", qMax(DY(12), 12));
	QFontMetrics fontMetrics(font12);
	if(fontMetrics.boundingRect("000").width() > DY(120))
		font12.setPointSize(DY(16));

	//gameCursor = QCursor(QPixmap(":/Application/images/cursor_wand.png").
	//	scaled(DX(64), DY(64), Qt::IgnoreAspectRatio, Qt::SmoothTransformation), 0, 0);
	gameCursor = QCursor(QPixmap(":/Application/images/cursor_wand.png"), 0, 0);

	menuBackground = QPixmap(":/Application/images/bgmenu.jpg");

	concrete = QPixmap(":/Application/images/concrete1.png");
	target1 = QPixmap(":/Application/images/target1.png");
	target2 = QPixmap(":/Application/images/target2.png");
	block1 = QPixmap(":/Application/images/block1.png");
	block2 = QPixmap(":/Application/images/block2.png");


	item1 = QPixmap(":/Application/images/item1.png");
	item2 = QPixmap(":/Application/images/item2.png");
	item3 = QPixmap(":/Application/images/item3.png");
	item4 = QPixmap(":/Application/images/item4.png");
	item5 = QPixmap(":/Application/images/item5.png");
	item6 = QPixmap(":/Application/images/item6.png");

	items.clear();
	items.push_back(item1);
	items.push_back(item2);
	items.push_back(item3);
	items.push_back(item4);
	items.push_back(item5);
	items.push_back(item6);

	//for(int i = 0; i < MAX_ITEMS_COUNT; i++) {
		//items.push_back(QPixmap(QString(":/Application/images/item%1.png").arg(i+1)));
		//QString str = QString(":/Application/images/item%1.png").arg(i+1);
	//}
}