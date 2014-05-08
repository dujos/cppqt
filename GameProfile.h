#ifndef GAMEPROFILE_H
#define GAMEPROFILE_H

#include <QtGui\qstringlistmodel.h>
#include <QtCore\qobject.h>
#include <QtCore\qxmlstream.h>

struct ItemInfo {
	ItemInfo() : totalCount(0), localCount(0) {}

	int totalCount;
	int localCount;
};

struct LevelInfo {
	QString levelPack;
	QList<ItemInfo> items;

	int level, score, mode;

	LevelInfo(const QString& qlevel) :
		levelPack(qlevel),
		level(1) {}

	void initLevel();
	bool isPuzzle();
};

struct PlayerInfo {
	QString name;
	QString currentLevelPack;
	int width, height, x, y, screen;
	bool fullscreen;
	QList<LevelInfo*> levelPacks;

	PlayerInfo(const QString& qname) :
		name(qname), width(0), height(0), 
		x(0), y(0), screen(0) {
	}

	LevelInfo* getCurrentLevelInfo();
	void addLevelPack(const QString& qlevelPack);
	void addLevelPack(LevelInfo* levelInfo);
	void setCurrentLevelPack(const QString& qlevelPack);
};

struct PlayerScore {
	QString qname;
	int score;

	PlayerScore(const QString& qname, int score) :
		qname(qname), score(score) {
	}
};

class GameProfile : public QObject {

//Q_OBJECT

private:
	int current;
	PlayerInfo* playerInfo;
	QString userPath;

	bool started;
	bool paused;

	QList<PlayerInfo*> players;
	QList<ItemInfo*> items;

public:
	GameProfile(QObject* parent);
	~GameProfile();

	PlayerInfo* getCurrentPlayerInfo();

	int levelPackCount(const QString& name);
	QString levelPackName(const QString& name);
	QString getLevelPath(const QString name);

	void setCurrentLevelPack(const QString& name);
	QString getCurrentLevelPack();

	void removePlayerInfo(const QString& qname);
	void setPlayerInfo(const QString& qname);
	void renamePlayerInfo(const QString& before, const QString& after);

	void saveProfile();
	void loadProfile();
	void loadOldProfile();

	void saveXml();
	void loadXml();

	void readXmlPlayer(QXmlStreamReader& xstream);
	void readXmlElement(QXmlStreamReader& xstream);
	void readXmlUnknownElement(QXmlStreamReader& xstream);
	void readXmlLevelInfo(QXmlStreamReader& xstream, PlayerInfo* playerInfo);
	void readXmlLevelItem(QXmlStreamReader& xstream, LevelInfo* levelItem);

	void setPlayerNames(const QStringList& listNames);
	QStringList getPlayerNames();

	void setGameStarted(bool startGame);
	void setGamePaused(bool pauseGame);
	bool isGameStarted();
	bool isGamePaused();

	void setFullScreen(bool fullScreen);
	bool isFullScreen();

	inline int screenWidth() { return playerInfo->width; }
	inline int screenHeight() { return playerInfo->height; }
	
	inline void setVideoMode(int width, int height) { playerInfo->width = width;
													  playerInfo->height = height; }
	
	inline void setWindowPosition(int xx, int yy) { getCurrentPlayerInfo()->x = xx;
													getCurrentPlayerInfo()->y = yy; }
	
	inline void setScreen(int screen) { getCurrentPlayerInfo()->screen = screen; }

signals:
	void profileChanged();
};

extern GameProfile* gameProfile;

#endif