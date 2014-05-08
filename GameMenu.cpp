#include "GameMenu.h"
#include "GameMenu.h"
#include "DisplayWrapper.h"
#include "GameWidget.h"
#include "GameHelper.h"

GameMenu::GameMenu(QWidget *parent) : QStackedWidget(parent) {
	ui.setupUi(this);

	setWindowOpacity(0.7);

	ui.twHall->setColumnWidth(0, 220);
	ui.twHall->horizontalHeader()->setStretchLastSection(true);
	ui.twHall->setRowCount(10);

	for(int i = 0; i < 10; i++) {
		ui.twHall->setItem(i, 0, new QTableWidgetItem(""));
		ui.twHall->setItem(i, 1, new QTableWidgetItem(""));
	}

	QString stylePath = ":/styles/style.css";
	QString style = "";
	QFile file(stylePath);

	if (file.open(QIODevice::ReadOnly)) {
		QTextStream textStream(&file);
		style = textStream.readAll().simplified();
		file.close();
	}

	qApp->setStyleSheet(style);
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
	
	setCurrentIndex(MENU_MAIN);
	startAfterSelection = false;
	setMouseTracking(false);
	active = false;

	//load video modes
	ui.lwVideoMode->addItems(DisplayWrapper::listModes());
	initGraphics();
}

GameMenu::~GameMenu() {}

void GameMenu::initGraphics() {
	onCurrentChanged(0);
	//QRect buttonProfile = NULL;
	//QRect buttonLevelPack = NULL;
}

void GameMenu::activate(bool act) {
	this->active = act;
	if(this->active)
		showNormal();
	else
		hide();
}

bool GameMenu::isActive() {
	return active;
}

void GameMenu::setMenuPage(int index) {
	setCurrentIndex(index);
	setFocus();
}

void GameMenu::setBackground(QPixmap pixmap) {
	backgroundPicture = pixmap;
}

void GameMenu::checkMenuMouseActions(const QPoint& position) {
	
	if(this->isActive() && this->profile->isGameStarted()) {
		if(buttonProfile.contains(position)){
			onButtonOptionProfileClicked();
		} else if(buttonLevelPack.contains(position)){
			onButtonOptionLevelsClicked();
		}
	}
}

void GameMenu::addProfile(GameProfile* profile) {
	this->profile = profile;
}

void GameMenu::addConfirmDialog(ConfirmDialog* confirmDialog) {
	this->dialog = confirmDialog;
}

void GameMenu::onButtonNewClicked() {
	qDebug() << "button clicked";
	emit menuNew();
}

void GameMenu::onButtonContinueClicked() {
	emit menuContinue();
}

void GameMenu::onButtonOptionsClicked() {
	setCurrentIndex(MENU_OPTIONS);
}

void GameMenu::onButtonHelpClicked() {
	setCurrentIndex(MENU_HELP);
}

void GameMenu::onButtonExitClicked() {
	emit menuExit();
}

void GameMenu::onButtonOptionGraphicsClicked() {
	setCurrentIndex(MENU_GRAPHICS);

	/*
	QString mode(QString("%1 x %2")
				.arg(profile->screenWidth())
				.arg(profile->screenHeight()));
	QList<QListWidgetItem*> items = ui.lwVideoMode->findItems(mode, Qt::MatchExactly);
	if(items.count())
		ui.lwVideoMode->setCurrentItem(items.first());
	*/
}

void GameMenu::onButtonOptionProfileClicked() {
	setCurrentIndex(MENU_PROFILE);
	QString name = profile->getCurrentPlayerInfo()->name;
	
	ui.lwPlayersList->clear();
	ui.lwPlayersList->addItem("[New Player]");
	ui.lwPlayersList->addItems(profile->getPlayerNames());

	QList<QListWidgetItem* > foundItems = ui.lwPlayersList->findItems(name, Qt::MatchFixedString);
	if(foundItems.count()) {
		ui.lwPlayersList->setCurrentItem(foundItems.first());
	} else
		ui.lwPlayersList->setCurrentRow(0);
}

void GameMenu::onButtonOptionLevelsClicked() {
	setCurrentIndex(MENU_LEVELS);

	QStringList extension;
	extension << "*lpk";
	levelPacks = QDir(GameWidget::getResourcePath() + "/levels/").entryList(extension);
	ui.lwLevels->clear();
	for(int i = 0; i < levelPacks.count(); i++) {
		QString name = levelPacks.at(i).section('.', 0, 0);
		int packCount = profile->levelPackCount(name);
		QString packName = profile->levelPackName(name);
		ui.lwLevels->addItem(tr("%1 {%2 levels}").arg(packName).arg(packCount));
	}
	QList<QListWidgetItem* > foundItems = ui.lwLevels->findItems(profile->getCurrentLevelPack(),
		Qt::MatchFixedString);
	if(foundItems.count())
		ui.lwLevels->setCurrentItem(foundItems.first());
	else
		ui.lwLevels->setCurrentRow(0);
}

void GameMenu::onButtonOptionBackClicked() {
	setCurrentIndex(MENU_MAIN);
	startAfterSelection = false;
}

void GameMenu::onButtonPauseGraphicsClicked() {
	onButtonOptionGraphicsClicked();
}

void GameMenu::onButtonPauseRestartClicked() {
	emit menuRestartLevel();
}

void GameMenu::onButtonPauseExitClicked() {
	emit menuGameExit();
}

void GameMenu::onRefreshGraphicModesClicked() {
	DisplayWrapper::init();

	ui.lwVideoMode->clear();
	ui.lwVideoMode->addItems(DisplayWrapper::listModes());
}

void GameMenu::onButtonGraphicsOkClicked() {
	bool fs = ui.cbFullscreenMode->isChecked();

	QString temp;
	QString mode(ui.lwVideoMode->currentItem()->text());
	int width, height;
	QTextStream textStream(&mode);

	textStream >> width >> temp >> height;

	if(fs != profile->isFullScreen()
		|| width != profile->screenWidth()
		|| height != profile->screenHeight()) {
		
		profile->setFullScreen(fs);
		profile->setVideoMode(width, height);
		emit menuVideoModeChanged();
	}
	onButtonGraphicsBackClicked();
}

void GameMenu::onButtonGraphicsBackClicked() {
	setCurrentIndex(profile->isGameStarted() ? MENU_PAUSE : MENU_OPTIONS);
}

void GameMenu::onGameStartClicked() {
	LevelInfo* levelInfo = profile->getCurrentPlayerInfo()->getCurrentLevelInfo();
	levelInfo->mode = ui.rbPuzzle->isChecked() ? 1 : 0;
	
	emit menuGameStart();
}

void GameMenu::onGameBackClicked() {
	setCurrentIndex(MENU_MAIN);
}

void GameMenu::onButtonProfileSelectClicked() {
	if(ui.lwPlayersList->currentRow() == 0) {
		bool ok;
		QString name = QInputDialog::getText(this, tr("New Player"), tr("Enter new Player name:"),
			QLineEdit::Normal, tr("Player"), &ok, Qt::FramelessWindowHint);
		if(ok && !name.isEmpty()) {
			QList<QListWidgetItem* > foundItem = ui.lwPlayersList->findItems(name, Qt::MatchFixedString);
			if(foundItem.isEmpty()) {
				ui.lwPlayersList->addItem(name);
				ui.lwPlayersList->setCurrentRow(ui.lwPlayersList->count()-1);
				profile->setPlayerInfo(name);
			} else
				QMessageBox::critical(this, tr("Player already exists"), 
					tr("Player exists change player name"));
		}
	} else {
		if(ui.lwPlayersList->currentItem()) {
			QString name = ui.lwPlayersList->currentItem()->text();
			if(!name.isEmpty()) {
				bool fs = profile->isFullScreen();
				int width = profile->screenWidth();
				int height = profile->screenHeight();

				profile->setPlayerInfo(name);

				if(fs != profile->isFullScreen()
					|| width != profile->screenWidth()
					|| height != profile->screenHeight()) {
						emit menuVideoModeChanged();
				}

				if(startAfterSelection)
					emit menuNew();
				else
					this->onButtonProfileBackClicked();
			}
		}
	}
}

void GameMenu::onButtonProfileRenameClicked() {
	bool ok;
	QString name = QInputDialog::getText(this, tr("Rename Player"), 
		tr("Change Player name:"), QLineEdit::Normal, ui.lwPlayersList->currentItem()->text(),
		&ok, Qt::FramelessWindowHint);
	if(ok && !name.isEmpty()) {
		QList<QListWidgetItem* > foundItem = ui.lwPlayersList->findItems(name, Qt::MatchFixedString);
		if(foundItem.isEmpty()) {
			profile->renamePlayerInfo(ui.lwPlayersList->currentItem()->text(), name);
			ui.lwPlayersList->currentItem()->setText(name);
		} else
			QMessageBox::critical(this, tr("Player exists"),
				tr("Cannot rename existing name!"));
	}
}

void GameMenu::onButtonProfileDeleteClicked() {
	if(ui.lwPlayersList->currentRow() <= 0)
		return;

	if(QMessageBox::question(this, tr("Remove Player"), 
		tr("Ok to remove player %1").arg(ui.lwPlayersList->currentItem()->text()),
		QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
		return;

	int row = ui.lwPlayersList->currentRow();
	QListWidgetItem* playerItem = ui.lwPlayersList->takeItem(row);
	
	if(playerItem) {
		profile->removePlayerInfo(playerItem->text());
		delete playerItem;
	}

	if(row > 0 && row < ui.lwPlayersList->count())
		ui.lwPlayersList->setCurrentRow(row);
}

void GameMenu::onButtonProfileBackClicked() {
	setCurrentIndex(MENU_OPTIONS);
}

void GameMenu::onPlayerListItemClicked(QListWidgetItem* item) {
	onButtonProfileSelectClicked();
}

void GameMenu::onButtonLevelsOkClicked() {
	emit menuLevelPack();
}

void GameMenu::onButtonLevelsBackClicked() {
	setCurrentIndex(MENU_OPTIONS);
}

void GameMenu::onLevelsItemClicked(QListWidgetItem* item) {
	onButtonLevelsOkClicked();
}

void GameMenu::onPuzzleToggled(bool on) {
	profile->getCurrentPlayerInfo()->getCurrentLevelInfo()->mode = on ? 1 : 0;
}

void GameMenu::onCurrentChanged(int) {
	int dy = DY(150);
	this->move((WIDTH-width())/2, dy);
}

ConfirmDialog::ConfirmDialog(QWidget* parent, Qt::WindowFlags flags) :
	QDialog(parent, Qt::FramelessWindowHint) {
	QVBoxLayout* vbl = new QVBoxLayout();
	
	vbl->addSpacerItem(new QSpacerItem(1, DY(10)));
	label = new QLabel("");
	label->setAlignment(Qt::AlignCenter);
	vbl->addWidget(label);

	vbl->addSpacerItem(new QSpacerItem(1, DY(10)));
	QHBoxLayout* hbl = new QHBoxLayout();
	QPushButton* buttonNo = new QPushButton(tr("no"));
	buttonNo->setFixedHeight(DY(40));
	connect(buttonNo, SIGNAL(clicked()), this, SLOT(onNoClicked()));
	hbl->addWidget(buttonNo);

	hbl->addSpacerItem(new QSpacerItem(DX(40), 1));
	QPushButton* buttonYes = new QPushButton(tr("yes"));
	buttonYes->setFixedHeight(DY(40));
	connect(buttonYes, SIGNAL(clicked()), this, SLOT(onOkClicked()));
	hbl->addWidget(buttonYes);
	
	vbl->addLayout(hbl);
	setLayout(vbl);

	setWindowOpacity(1);
}
	
int ConfirmDialog::execute(const QString& qstring) {
	setFixedSize(DX(540), DY(200));
	move((WIDTH-DX(540))/2, (HEIGHT-DY(200))/2);
	//label->setFont(stock->font12);

	update();
	QApplication::processEvents();

	result = 0;
	label->setText(qstring);

	QDialog::exec();
	return result;
}

void ConfirmDialog::onNoClicked() {
	result = 0;
	close();
}

void ConfirmDialog::onOkClicked() {
	result = 1;
	close();
}