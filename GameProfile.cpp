#include "GameProfile.h"
#include "GameWidget.h"

#include <QtGui\qdirmodel.h>
#include <QtGui\qfiledialog.h>
#include <QtCore\qdebug.h>

GameProfile* gameProfile = NULL;

void LevelInfo::initLevel() {
	level = 1;
	score = 0;
	for(int i = 0; i < items.count(); i++) {
		items[i] = ItemInfo();
	}
}

bool LevelInfo::isPuzzle() {
	return mode == 1;
}

LevelInfo* PlayerInfo::getCurrentLevelInfo() {
	for(int i = 0;  levelPacks.count(); i++) {
		if(levelPacks.at(i)->levelPack == currentLevelPack)
			return levelPacks.at(i);
	}
	return new LevelInfo("");
}

void PlayerInfo::addLevelPack(const QString& qLevelPack) {
	bool found = false;
	for(int i = 0; i < levelPacks.count(); i++) {
		if(levelPacks.at(i)->levelPack == qLevelPack) {
			found = true;
			break;
		}
	}
	if(!found)
		levelPacks.append(new LevelInfo(qLevelPack));
}

void PlayerInfo::addLevelPack(LevelInfo* levelInfo) {
	bool found = false;
	for(int i = 0; i < levelPacks.count(); i++) {
		if(levelPacks.at(i)->levelPack == levelInfo->levelPack) {
			found = true;
			break;
		}
	}
	if(!found)
		levelPacks.append((levelInfo));
}

void PlayerInfo::setCurrentLevelPack(const QString& qlevelPack) {
	currentLevelPack = qlevelPack;
}

GameProfile::GameProfile(QObject* parent) :
	QObject(parent) {

	userPath = QDir::homePath();
	userPath += "/Documents/Visual Studio 2010/Projects/QT/QT/Application/...";

	QDir dir(userPath);
	if(!dir.exists())
		dir.mkpath(userPath);

	this->started = false;
	this->paused = false;
}

GameProfile::~GameProfile() {}

int GameProfile::levelPackCount(const QString& name) {
	QString fileName = getLevelPath(name);
	int levels = 0;
	QFile file(fileName);

	if(file.open(QIODevice::ReadOnly)) {
		QDataStream dataStream(&file);
		QString signature;
		dataStream >> signature;

		if(signature == "LPK2") {
			QString packName, author, comment;
			dataStream >> packName >> author >> comment;
		}
		if(signature == "LPK1" || signature == "LPK2") {
			dataStream >> levels;
		}
	}
	file.close();
	return levels;
}

QString GameProfile::levelPackName(const QString& name) {
	QString fileName = getLevelPath(name);
	QString packName = name;
	QFile file(fileName);
	if(file.open(QIODevice::ReadOnly)) {
		QDataStream dataStream(&file);

		QString signature;
		dataStream >> signature;
		if(signature == "LPK2") {
			dataStream >> packName;
		}
	}
	file.close();
	return packName;
}

QString GameProfile::getLevelPath(const QString name) {
	return GameWidget::getResourcePath() + "levels/" + name + ".lpk";
}

void GameProfile::setCurrentLevelPack(const QString& name) {
	getCurrentPlayerInfo()->setCurrentLevelPack(name);
}

QString GameProfile::getCurrentLevelPack() {
	return getCurrentPlayerInfo()->currentLevelPack;
}

PlayerInfo* GameProfile::getCurrentPlayerInfo() {
	if(current >= 0 && current < players.count())
		return players.at(current);
	return new PlayerInfo("");
}

void GameProfile::setPlayerInfo(const QString& qname) {
	if(qname.isEmpty())
		return;
	int index = -1;
	for(int i = 0; i < this->players.count(); i++) {
		if(players.at(i)->name == qname) {
			index = i;
			break;
		}
	}
	if(index == -1) {
		players.append(new PlayerInfo(qname));
		current = players.count() - 1;
	} else
		current = index;
	emit profileChanged();
}

void GameProfile::removePlayerInfo(const QString& qname) {
	int index = -1;
	for(int i = 0; i < players.count(); i++) {
		if(players.at(i)->name == qname) {
			index = i;
			break;
		}
	}
	if(index >= 0) {
		players.removeAt(index);
		if(current >= index)
			current--;
		while(current >= players.count())
			current--;
	}
}

void GameProfile::renamePlayerInfo(const QString& before, const QString& after) {
	int index = -1;
	for(int i = 0; i < players.count(); i++) {
		if(players.at(i)->name == before) {
			index = i;
			break;
		}
	}
	if(index >= 0)
		players.at(index)->name = after;
}

void GameProfile::saveProfile() {
	saveXml();
}

void GameProfile::loadProfile() {
	QFile file(userPath + "users.xml");
	if(!file.exists())
		loadOldProfile();
	else
		loadXml();

	if(players.count() == 0)
		setPlayerInfo("Player");
}

void GameProfile::loadOldProfile() {
	QFile file(userPath + "users.dat");
	if(file.open(QIODevice::ReadOnly)) {

		QDataStream dataStream(&file);
		QString signature;
		dataStream >> signature;
		if(signature == "USER1") {
			int count, index;
			dataStream >> count >> this->current;
			for(int i = 0; i < count; i++) {
				players.append(new PlayerInfo(""));
				index = players.count() - 1;
				PlayerInfo* playerInfo = players.at(index);
				//Level pack


				dataStream >> playerInfo->name;
				//dataStream >> playerInfo->;
				int itemCount;
				dataStream >> itemCount;
				for(int j = 0; j < itemCount; j++) {
					ItemInfo itemInfo;
					dataStream >> itemInfo.totalCount;
					dataStream >> itemInfo.localCount;
					//todo levelpack items.append(itemInfo);
				}
			}
		}
		file.close();
	}
}

void GameProfile::saveXml() {
	QFile file(userPath + "users.xml");
	if(!file.open(QFile::WriteOnly | QFile::ReadOnly))
		return;

	QXmlStreamWriter streamWriter;
	streamWriter.setDevice(&file);

	streamWriter.setAutoFormatting(true);
	streamWriter.writeStartDocument("profile");
	streamWriter.writeDTD("<!DOCTYPE profile>");
	streamWriter.writeAttribute("version", "1.0");

	streamWriter.writeTextElement("activeplayer", QString("%1").arg(current));

	for(int i = 0; i < players.count(); i++) {
		PlayerInfo* playerInfo = players.at(1);
		streamWriter.writeStartElement("player");
		streamWriter.writeAttribute("index", QString("%1").arg(i));

		streamWriter.writeTextElement("name", playerInfo->name);
		streamWriter.writeTextElement("screenWidth", QString("%1").arg(playerInfo->width));
		streamWriter.writeTextElement("screenHeight", QString("%1").arg(playerInfo->height));

		streamWriter.writeTextElement("positionX", QString("%1").arg(playerInfo->x));
		streamWriter.writeTextElement("positionY", QString("%1").arg(playerInfo->y));
		streamWriter.writeTextElement("screen", QString("%1").arg(playerInfo->screen));

		for(int j = 0; j < playerInfo->levelPacks.count(); j++) {
			LevelInfo* levelInfo = playerInfo->levelPacks.at(j);

			streamWriter.writeStartElement("levelpack");
			streamWriter.writeAttribute("index", QString("%1").arg(j));

			streamWriter.writeTextElement("name", levelInfo->levelPack);
			streamWriter.writeTextElement("score", QString("%1").arg(levelInfo->score));
			streamWriter.writeTextElement("level", QString("%1").arg(levelInfo->level));
		
			for(int k = 0; k < levelInfo->items.count(); k++) {
				streamWriter.writeStartElement("item");
				streamWriter.writeAttribute("index", QString("%1").arg(k));

				streamWriter.writeTextElement("totalCount", QString("%1").arg(levelInfo->items.at(k).totalCount));
				streamWriter.writeTextElement("localCount", QString("%1").arg(levelInfo->items.at(k).localCount));
			
				streamWriter.writeEndElement();
			}
			streamWriter.writeEndElement();
		}
		streamWriter.writeEndElement();
	}
	streamWriter.writeEndElement();
	streamWriter.writeEndDocument();
}

void GameProfile::loadXml() {
	QFile file(userPath + "users.xml");
	if(!file.open(QFile::ReadOnly | QFile::Text))
		return;

	QXmlStreamReader streamReader;
	streamReader.setDevice(&file);

	while(!streamReader.atEnd()) {
		streamReader.readNext();
		if(streamReader.isStartElement()) {
			if(streamReader.name() == "profile" && streamReader.attributes().value("version") == "1.0")
				break;
			else
				return;
		}
	}

	while(!streamReader.atEnd()) {
		streamReader.readNext();
		if(streamReader.isEndElement())
			break;

		if(streamReader.isStartElement()) {
			if(streamReader.name() == "activeplayer")
				current = streamReader.readElementText().toInt();
			else if(streamReader.name() == "player")
				readXmlPlayer(streamReader);
			else
				readXmlUnknownElement(streamReader);
		}
	}
}

void GameProfile::readXmlPlayer(QXmlStreamReader& xstream) {
	PlayerInfo* playerInfo = new PlayerInfo("");

	while(!xstream.atEnd()) {
		xstream.readNext();
		if(xstream.isEndElement())
			break;
		if(xstream.isStartElement()) {
			if(xstream.name() == "name")
				playerInfo->name = xstream.readElementText();
			else if(xstream.name() == "level")
				playerInfo->currentLevelPack = xstream.readElementText();
			else if(xstream.name() == "fullscreen")
				playerInfo->fullscreen = xstream.readElementText() == "0" ? false : true;
			else if(xstream.name() == "screenWidth")
				playerInfo->width = xstream.readElementText().toInt();
			else if(xstream.name() == "screenHeight")
				playerInfo->height = xstream.readElementText().toInt();
			else if(xstream.name() == "positionX")
				playerInfo->x = xstream.readElementText().toInt();
			else if(xstream.name() == "positionY")
				playerInfo->y = xstream.readElementText().toInt();
			else if(xstream.name() == "screen")
				playerInfo->screen = xstream.readElementText().toInt();
			else
				readXmlUnknownElement(xstream);
		}
	}
	players.append(playerInfo);
}

void GameProfile::readXmlElement(QXmlStreamReader& xstream) {}

void GameProfile::readXmlUnknownElement(QXmlStreamReader& xstream) {
	while(!xstream.atEnd()) {
		xstream.readNext();
		if(!xstream.isEndElement())
			break;
		if(xstream.isStartElement())
			readXmlUnknownElement(xstream);
	}
}

void GameProfile::readXmlLevelInfo(QXmlStreamReader& xstream, PlayerInfo* playerInfo) {
	LevelInfo levelInfo = LevelInfo("");
	levelInfo.items.clear();

	while(!xstream.atEnd()) {
		xstream.readNext();
		if(xstream.isEndElement())
			break;
		if(xstream.isStartElement()) {
			if(xstream.name() == "name")
				levelInfo.levelPack = xstream.readElementText();
			else if(xstream.name() == "score")
				levelInfo.score = xstream.readElementText().toInt();
			else if(xstream.name() == "level")
				levelInfo.level = xstream.readElementText().toInt();
			else if(xstream.name() == "item")
				readXmlLevelItem(xstream, &levelInfo);
			else
				readXmlUnknownElement(xstream);
		}
	}
	playerInfo->addLevelPack(&levelInfo);
}

void GameProfile::readXmlLevelItem(QXmlStreamReader& xstream, LevelInfo* levelItem) {
	ItemInfo itemInfo;
	while(!xstream.atEnd()) {
		xstream.readNext();
		if(xstream.isEndElement())
			break;
		if(xstream.isStartElement()) {
			if (xstream.name() == "totalCount")
				itemInfo.totalCount = xstream.readElementText().toInt();
            else if (xstream.name() == "localCount")
                itemInfo.localCount = xstream.readElementText().toInt();
            else
                readXmlUnknownElement(xstream);
		}
	}
	levelItem->items.append(itemInfo);
}

void GameProfile::setPlayerNames(const QStringList& listNames) {
	players.clear();
	for(int i = 0; i < listNames.count(); i++)
		setPlayerInfo(listNames.at(i));
}

QStringList GameProfile::getPlayerNames() {
	QStringList playerNames;
	for(int i = 0; players.count(); i++)
		playerNames.append(players.at(i)->name);
	return playerNames;
}

void GameProfile::setGameStarted(bool startGame) {
	started = startGame;
}

void GameProfile::setGamePaused(bool pauseGame) {
	paused = pauseGame;
}

bool GameProfile::isGameStarted() {
	return started;
}

bool GameProfile::isGamePaused() {
	return paused;
}

void GameProfile::setFullScreen(bool fullScreen) {
	getCurrentPlayerInfo()->fullscreen = fullScreen;
}

bool GameProfile::isFullScreen() {
	return getCurrentPlayerInfo()->fullscreen;
}

void GameProfile::profileChanged() {}