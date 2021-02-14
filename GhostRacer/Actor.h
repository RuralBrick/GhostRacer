#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
public:
	Actor();
	virtual void doSomething() = 0;
	bool alive() {
		return m_alive;
	}
private:
	bool m_alive;
};

class GhostRacer : public Actor {
public:
	GhostRacer(double x, double y);
	virtual void doSomething();
};

class WhiteBorderLine;

class BorderLine : public Actor {
public:
	BorderLine(double x, double y);
	virtual void doSomething();
	static double lastWhiteBorderLineY() {
		return lastWhiteBorderLine->getY();
	}
protected:
	static WhiteBorderLine* lastWhiteBorderLine;
};

class YellowBorderLine : public BorderLine {
public:
	YellowBorderLine(double x, double y);
};

class WhiteBorderLine : public BorderLine {
public:
	WhiteBorderLine(double x, double y);
};

#endif // ACTOR_H_
