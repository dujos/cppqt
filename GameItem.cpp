#include "GameItem.h"

/*
GameItem::GameItem(quint8 ix) :
  ix(ix), state(ItemState::StandingState), dx(0), dy(0),
  opacity(1), x(0), y(0) {
}
*/

GameItem::GameItem(int ix) :
  ix(ix), state(ItemState::StandingState), dx(0), dy(0),
  opacity(1), x(0), y(0) {
}

GameItem::~GameItem() {}

void GameItem::standing() {
	state = ItemState::StandingState;
	setOpacity(1);
	dx = dy = x = y = 0;
}

bool GameItem::isStanding() const {
	return state == ItemState::StandingState;
}

void GameItem::dying() {
	state = ItemState::DyingState;
	setOpacity(1);
	count = 5;
}

bool GameItem::isDying() const {
	return state == ItemState::DyingState;
}

void GameItem::die() {
	state = ItemState::DeadState;
}

bool GameItem::isDead() const {
	return state == ItemState::DeadState;
}

void GameItem::falling(int dx, int dy) {
	state = ItemState::FallingState;
	setOpacity(1);
	this->dx = dx;
	this->dy = dy;
	x = y = 0;
	count = 5;
}

bool GameItem::isFalling() const {
	return state == ItemState::FallingState;
}

void GameItem::starting() {
	state = ItemState::StartingState;
	setOpacity(0);
	count = 5;
}

bool GameItem::isStarting() const {
	return state == ItemState::StartingState;
}

void GameItem::select() {
	state = ItemState::SelectedState;
	setOpacity(1);
	count = 10;
}

bool GameItem::isSelected() {
	return state == ItemState::SelectedState;
}

void GameItem::next() {
	switch(state) {
		case ItemState::DyingState:
			opacity -= 0.5;
			break;
		case ItemState::FallingState:
			if(!(--count)) {
				standing();
			} else {
				x = -20*dx*count;
				y = -20*dy*count;
			}
			break;
		case ItemState::StartingState:
			opacity += 0.5;
			if(!(--count)) 
				standing();
			break;
		case ItemState::SelectedState:
			opacity -= 0.5;
			if(!(--count)) {
				opacity = 1;
				count = 10;
			}
			break;
		default:
			break;
	}
}