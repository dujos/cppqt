#include "Scaler.h"
#include <QtGui\qdesktopwidget.h>

Scaler* scaler = NULL;

Scaler::Scaler() {
	this->rescale(1024.0, 768.0);
}

void Scaler::init() {
	scaler = new Scaler();
}

void Scaler::rescale(int width, int height) {
	QDesktopWidget* desktopWidget;

	if(width == 0 || height == 0) {
		width  = desktopWidget->width();
		height = desktopWidget->height();
	}

	windowWidth = width;
	windowHeight = height;

	cWidth = (float)windowWidth/1024.0;
	cHeight = (float)windowHeight/768.0;

	x56 = (int) (cWidth*56);
	y56 = (int) (cHeight*56);
}