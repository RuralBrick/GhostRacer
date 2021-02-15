#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
public:
	Actor(int imageID, double x, double y, int dir, double size, unsigned int depth, 
		double xSpeed, double ySpeed, bool collisionAvoidanceWorthy);
	virtual void doSomething() = 0;
	bool alive() {
		return m_alive;
	}
private:
	double m_xSpeed;
	double m_ySpeed;
	bool m_alive;
	bool m_collisionAvoidanceWorthy;
};

class HPActor : public Actor {
public:
	HPActor(int imageID, double x, double y, int dir, double size, unsigned int depth, 
		double xSpeed, double ySpeed, int hp, bool collisionAvoidanceWorthy);
	virtual void doSomething() = 0;
	int hp() {
		return m_hp;
	}
	void takeDamage(int damage) {
		m_hp -= damage;
		// TODO: maybe add `if m_hp <= 0`
	}
private:
	int m_hp;
};

class GhostRacer : public HPActor {
public:
	GhostRacer(double x, double y);
	virtual void doSomething();
private:
	int m_sprays;
};

class BorderLine : public Actor {
public:
	enum class Color { yellow, white };
	BorderLine(double x, double y, Color color);
	virtual void doSomething();
	static double lastWhiteBorderLineY() {
		if (m_lastWhiteBorderLine != nullptr)
			return BorderLine::m_lastWhiteBorderLine->getY();
		return 0;
	}
private:
	static BorderLine* m_lastWhiteBorderLine;
};

#endif // ACTOR_H_
