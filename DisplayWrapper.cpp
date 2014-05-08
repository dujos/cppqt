#include "DisplayWrapper.h"

DisplayWrapper DisplayWrapper::dwrapper;

DisplayWrapper::DisplayWrapper() :
	baseWidget(NULL) {}

DisplayWrapper::~DisplayWrapper() {}

void DisplayWrapper::init(bool filter, int minWidth, int minHeight) {
	DisplayWrapper::dwrapper.dwrapperInit(filter, minWidth, minHeight);
}

void DisplayWrapper::dwrapperInit(bool filter, int minWidth, int minHeight) {
	this->baseHeight = 0;
	this->baseWidth = 0;
	this->baseBpp = 0;
	
	//data structure for windows display
	DEVMODE devmode;
	DWORD modeenum = 0;
	VideoModeInfo videoModeInfo;

	modeNames.clear();
	modes.clear();

	//current dislplay
	wchar_t* device = NULL;
	DISPLAY_DEVICE displayDevice;
	displayDevice.cb = sizeof(DISPLAY_DEVICE);

	if(baseWidget) {
		QDesktopWidget desktop;
		int screen = desktop.screenNumber(baseWidget);
		if(screen >= 0) {
			if(EnumDisplayDevices(NULL, screen, &displayDevice, 0)) {
				device = displayDevice.DeviceName;
			}
		}
	}

	//enumerate display modes
	while(EnumDisplaySettings(device, modeenum, &devmode)) {
		if(devmode.dmPelsWidth >= minWidth && devmode.dmPelsHeight >= minHeight) {
			this->modeNames.append(QString("%1 x %2")
								   .arg(devmode.dmPelsWidth)
								   .arg(devmode.dmPelsHeight));
			videoModeInfo.width = devmode.dmPelsWidth;
			videoModeInfo.height = devmode.dmPelsHeight;
			videoModeInfo.bpp = devmode.dmBitsPerPel;
			modes.append(videoModeInfo);
		}
		modeenum++;
	}

	if(filter)
		modeNames.removeDuplicates();
}

VideoModeInfo DisplayWrapper::currentVideoMode() {
	return DisplayWrapper::dwrapper.dwrapperCurrentVideoMode();
}

VideoModeInfo DisplayWrapper::dwrapperCurrentVideoMode() {
	DEVMODE devmode;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	VideoModeInfo videoModeInfo;
	videoModeInfo.width = devmode.dmPelsWidth;
	videoModeInfo.height = devmode.dmPelsHeight;
	videoModeInfo.bpp = devmode.dmBitsPerPel;

	return videoModeInfo;
}

const QStringList& DisplayWrapper::listModes() { 
	return DisplayWrapper::dwrapper.modeNames;
}

void DisplayWrapper::restoreVideoModes() {
	DisplayWrapper::dwrapper.dwrapperRestoreMode();
}

void DisplayWrapper::dwrapperRestoreMode() {
	ChangeDisplaySettings(0, 0);
}

bool DisplayWrapper::dwrapperChangeMode(int w, int h, int bpp, bool fs) {
	return false;
}