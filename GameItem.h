#ifndef GAMEITEM_H
#define GAMEITEM_H

#include "Defines.h"
#include <QtCore\qdebug.h>

class GameItem {

public:
	enum ItemState { StandingState, DeadState, DyingState, FallingState, StartingState, SelectedState };

private:
	//quint8 ix;
	int ix;
	int dx;
	int dy;
	int x;
	int y;
	int count;
	qreal opacity;

	ItemState state;

public:
	//GameItem(quint8 ix);
	GameItem(int ix);
	~GameItem();

	//inline quint8 getIx() const { return ix; }
	inline int getIx() const { return ix; }
	inline void setIx(quint8 ixx) { ix = ixx; }

	inline int getdx() const { return dx; }
	inline int getdy() const { return dy; }

	inline qreal getOpacity() const { return opacity; }
	inline void setOpacity(qreal o) { opacity = o; }
	
	inline void setCount(qreal c) { count += c; }
	inline qreal getCount() { return count; }

	void standing();
	bool isStanding() const;
	
	void dying();
	bool isDying() const;
	
	void die();
	bool isDead() const;
	
	void falling(int dx, int dy);
	bool isFalling() const;	
	
	void starting();
	bool isStarting() const;
	
	void select();
	bool isSelected();

	inline ItemState getState() { return state; }

	void next();
};

#endif