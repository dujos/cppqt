#ifndef GAMEMENU_H
#define GAMEMENU_H

#include <QtGui\qdialog.h>
#include <QtGui\qinputdialog.h>
#include <QtGui\qstackedwidget.h>
#include <QtGui\qfiledialog.h>
#include <QtCore\qtextstream.h>
#include <QtGui\qtablewidget.h>
#include <QtGui\qmessagebox.h>
#include <QtCore\qdebug.h>

#include "ui_menu.h"
#include "GameProfile.h"
#include "GameStock.h"
#include "Scaler.h"

class GameStock;
class GameProfile;

const int MENU_MAIN     = 0;
const int MENU_OPTIONS  = 1;
const int MENU_PAUSE    = 2;
const int MENU_GRAPHICS = 3;
const int MENU_AUDIO    = 4;
const int MENU_GAME     = 5;
const int MENU_PROFILE  = 6;
const int MENU_HALL     = 7;
const int MENU_CREDITS  = 8;
const int MENU_LEVELS   = 9;
const int MENU_HELP     = 10;
const int MENU_LANGUAGE = 11;

class ConfirmDialog : public QDialog {
	Q_OBJECT

private:
	QLabel* label;
	GameStock* stock;
	int result;

public:
	ConfirmDialog(QWidget* parent=NULL, Qt::WindowFlags flags=0);
	
	int execute(const QString& qstring);

public slots:
	void onNoClicked();
	void onOkClicked();
};

class GameMenu : public QStackedWidget {
	Q_OBJECT

private:
	Ui::GameMenu ui;
	QPixmap backgroundPicture;

	QStringList levelPacks;

	bool startAfterSelection;
	bool active;

	ConfirmDialog* dialog;
	GameProfile* profile;

public:
	QRect buttonProfile;
	QRect buttonLevelPack;

public:
	GameMenu(QWidget *parent = 0);
	~GameMenu();

	void initGraphics();
	void activate(bool active);
	bool isActive();

	void setMenuPage(int index);
	void setBackground(QPixmap pixmap);
	void addProfile(GameProfile* profile);
	void addConfirmDialog(ConfirmDialog* confirmDialog);

	void checkMenuMouseActions(const QPoint& position);

private slots:
	void onButtonNewClicked();
	void onButtonContinueClicked();
	void onButtonOptionsClicked();
	void onButtonHelpClicked();
	void onButtonExitClicked();

	void onButtonOptionGraphicsClicked();
	void onButtonOptionProfileClicked();
	void onButtonOptionLevelsClicked();
	void onButtonOptionBackClicked();

	void onButtonPauseGraphicsClicked();
	void onButtonPauseRestartClicked();
	void onButtonPauseExitClicked();

	void onRefreshGraphicModesClicked();
	void onButtonGraphicsOkClicked();
	void onButtonGraphicsBackClicked();

	void onGameStartClicked();
	void onGameBackClicked();

	void onButtonProfileSelectClicked();
	void onButtonProfileRenameClicked();
	void onButtonProfileDeleteClicked();
	void onButtonProfileBackClicked();
	void onPlayerListItemClicked(QListWidgetItem* item);

	void onButtonLevelsOkClicked();
	void onButtonLevelsBackClicked();
	void onLevelsItemClicked(QListWidgetItem* item);

	void onPuzzleToggled(bool on);
	void onCurrentChanged(int);

signals:
	void menuNew();
	void menuContinue();
	void menuExit();
	void menuRestartLevel();
	void menuVideoModeChanged();
	void menuLevelPack();
	void menuGameExit();
	void menuGameStart();
};

#endif
