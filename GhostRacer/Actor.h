#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth, 
		double xSpeed, double ySpeed, bool collisionAvoidanceWorthy);
	virtual void doSomething() = 0;
	double getXSpeed() const {
		return m_xSpeed;
	}
	double getYSpeed() const {
		return m_ySpeed;
	}
	void setXSpeed(double speed) {
		m_xSpeed = speed;
	}
	void setYSpeed(double speed) {
		m_ySpeed = speed;
	}
	void adjustXSpeed(double speed) {
		m_xSpeed += speed;
	}
	void adjustYSpeed(double speed) {
		m_ySpeed += speed;
	}
	bool alive() const {
		return m_alive;
	}
protected:
	StudentWorld* m_sw;
private:
	double m_xSpeed;
	double m_ySpeed;
	bool m_alive;
	bool m_collisionAvoidanceWorthy;
};

class HPActor : public Actor {
public:
	HPActor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth, 
		double xSpeed, double ySpeed, int hp, bool collisionAvoidanceWorthy);
	virtual void doSomething() = 0;
	int hp() const {
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
	GhostRacer(StudentWorld* sw, double x, double y);
	virtual void doSomething();
private:
	int m_sprays;
	void move();
};

class BorderLine : public Actor {
public:
	enum class Color { yellow, white };
	BorderLine(StudentWorld* sw, double x, double y, Color color);
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
