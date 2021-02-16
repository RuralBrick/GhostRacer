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
	double getXSpeed() const { return m_xSpeed;  }
	double getYSpeed() const { return m_ySpeed; }
	void setXSpeed(double speed) { m_xSpeed = speed; }
	void setYSpeed(double speed) { m_ySpeed = speed; }
	void adjustXSpeed(double speed) { m_xSpeed += speed; }
	void adjustYSpeed(double speed) { m_ySpeed += speed; }
	bool isAlive() const { return m_alive; }
	virtual void die();
protected:
	StudentWorld* m_sw;
	bool isOutOfBounds() const;
	bool isOverlappingGhostRacer() const;
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
	int getHp() const { return m_hp; }
	virtual void doDamageEffect() = 0;
	virtual void takeDamage(int damage);
protected:
	void setHp(int hp) { m_hp = hp; }
private:
	int m_hp;
};

class GhostRacer : public HPActor {
public:
	GhostRacer(StudentWorld* sw, double x, double y);
	virtual void doSomething();
	virtual void die();
	virtual void doDamageEffect() {}
	void healDamage(int health);
	int getSprays() { return m_sprays; }
	void addSprays(int sprays) { m_sprays += sprays; }
	void spin();
private:
	const int MAX_HP = 100;
	int m_sprays;
	void move();
};

class BorderLine : public Actor {
public:
	enum class Color { yellow, white };
	BorderLine(StudentWorld* sw, double x, double y, Color color);
	virtual void doSomething();
	static double getLastWhiteBorderLineY();
private:
	static BorderLine* m_lastWhiteBorderLine;
};

class Item : public Actor {
public:
	Item(StudentWorld* sw, int iid, double x, double y, double size, unsigned int depth);
	virtual void doSomething();
	virtual void interactWithGhostRacer() = 0;
};

class Goodie : public Item {
public:
	Goodie(StudentWorld* sw, int iid, double x, double y, double size);
	virtual void interactWithGhostRacer() = 0;
};

class OilSlick : public Item {
public:
	OilSlick(StudentWorld* sw, double x, double y);
	virtual void interactWithGhostRacer();
};

class HealingGoodie : public Goodie {
public:
	HealingGoodie(StudentWorld* sw, double x, double y);
	virtual void interactWithGhostRacer();
};

class HolyWaterGoodie : public Goodie {
public:
	HolyWaterGoodie(StudentWorld* sw, double x, double y);
	virtual void interactWithGhostRacer();
};

class SoulGoodie : public Goodie {
public:
	SoulGoodie(StudentWorld* sw, double x, double y);
	virtual void doSomething();
	virtual void interactWithGhostRacer();
};

#endif // ACTOR_H_
