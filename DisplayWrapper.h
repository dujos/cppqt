#ifndef DISPLAYWRAPPER_H
#define DISPLAYWRAPPER_H

#include <QtCore\qstringlist.h>
#include <QtGui\qwidget.h>
#include <QtGui\qdesktopwidget.h>
#include <QtCore\qtextstream.h>

#include <Windows.h>

struct VideoModeInfo {
	int width;
	int height;
	int bpp;
};

class DisplayWrapper {

private:
	static DisplayWrapper dwrapper;
	
	QWidget* baseWidget;
	int baseWidth;
	int baseHeight;
	int baseBpp;

	QStringList modeNames;
	QList<VideoModeInfo> modes;

private:
	void dwrapperInit(bool filter=true, int minWidth=800, int minHeight=800);
	VideoModeInfo dwrapperCurrentVideoMode();
	void dwrapperRestoreMode();
	bool dwrapperChangeMode(int baseWidth, int baseHeight, int baseBpp, bool flags);

public:
	DisplayWrapper();
	~DisplayWrapper();

	static void init(bool filter=true, int minWidth=800, int minHeight=800);

	static void restoreVideoModes();
	static const QStringList& listModes();
	static VideoModeInfo currentVideoMode();
};

#endif