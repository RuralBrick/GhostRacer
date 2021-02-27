#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth, 
		double xSpeed, double ySpeed, bool collisionAvoidanceWorthy, bool sprayable);
	virtual void doSomething() = 0;
	double getXSpeed() const { return m_xSpeed;  }
	double getYSpeed() const { return m_ySpeed; }
	bool isAlive() const { return m_alive; }
	bool isCollisionAvoidanceWorthy() const { return m_collisionAvoidanceWorthy; }
	bool isSprayable() const { return m_sprayable; }
	virtual void getSprayed(int damage) = 0;
protected:
	StudentWorld* getWorld() const { return m_sw; }
	void setXSpeed(double speed) { m_xSpeed = speed; }
	void setYSpeed(double speed) { m_ySpeed = speed; }
	void adjustXSpeed(double speed) { m_xSpeed += speed; }
	void adjustYSpeed(double speed) { m_ySpeed += speed; }
	bool isOutOfBounds() const;
	bool isOverlappingGhostRacer() const;
	bool move();
	virtual void die();
private:
	StudentWorld* m_sw;
	double m_xSpeed;
	double m_ySpeed;
	bool m_alive;
	bool m_collisionAvoidanceWorthy;
	bool m_sprayable;
};

class HPActor : public Actor {
public:
	HPActor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, 
		double xSpeed, double ySpeed, int hp, bool sprayable);
	int getHp() const { return m_hp; }
	void takeDamage(int damage);
protected:
	void setHp(int hp) { m_hp = hp; }
private:
	int m_hp;
	virtual void doDamageEffect() = 0;
};

class GhostRacer : public HPActor {
public:
	GhostRacer(StudentWorld* sw, double x, double y);
	void doSomething();
	void healDamage(int health);
	int getSprays() { return m_sprays; }
	void addSprays(int sprays) { m_sprays += sprays; }
	void spin();
private:
	const int MAX_HP = 100;
	int m_sprays;
	bool move();
	void die();
	void doDamageEffect() {}
	void getSprayed(int damage) {}
};

class BorderLine : public Actor {
public:
	enum class Color { yellow, white };
	static double getLastWhiteBorderLineY();
	BorderLine(StudentWorld* sw, double x, double y, Color color);
private:
	static BorderLine* m_lastWhiteBorderLine;
	void doSomething();
	void getSprayed(int damage) {}
};

class AIActor : public HPActor {
public:
	AIActor(StudentWorld* sw, int iid, double x, double y, int dir, double size,
		double xSpeed, double ySpeed, int hp);
protected:
	void setMoveDist(int dist) { m_moveDist = dist; }
private:
	int m_moveDist;
	virtual void doSomething();
	virtual bool interactWithGhostRacer() = 0;
	virtual bool actBeforeMove() = 0;
	virtual bool actAfterMove() = 0;
	virtual void planMove() = 0;
};

class Pedestrian : public AIActor {
public:
	Pedestrian(StudentWorld* sw, int iid, double x, double y, double size);
private:
	bool actAfterMove() { return false; }
	virtual void planMove();
};

class HumanPedestrian : public Pedestrian {
public:
	HumanPedestrian(StudentWorld* sw, double x, double y);
private:
	void doDamageEffect();
	bool interactWithGhostRacer();
	bool actBeforeMove() { return false; }
	void getSprayed(int damage);
};

class ZombiePedestrian : public Pedestrian {
public:
	ZombiePedestrian(StudentWorld* sw, double x, double y);
private:
	int m_ticksTillGrunt;
	void die();
	void doDamageEffect();
	bool interactWithGhostRacer();
	bool actBeforeMove();
	void getSprayed(int damage);
};

class ZombieCab : public AIActor {
public:
	ZombieCab(StudentWorld* sw, double x, double y, double ySpeed);
private:
	bool m_hitGhostRider;
	void die();
	void doDamageEffect();
	bool interactWithGhostRacer();
	bool actBeforeMove() { return false; }
	bool actAfterMove();
	void planMove();
	double calcDist(const Actor* other);
	bool checkInFront(double y);
	bool checkBehind(double y);
	void getSprayed(int damage);
};

class Item : public Actor {
public:
	Item(StudentWorld* sw, int iid, double x, double y, int dir, double size, bool sprayable);
private:
	virtual void doSomething();
	virtual void interactWithGhostRacer() = 0;
	virtual void doStuffAfter() = 0;
};

class OilSlick : public Item {
public:
	OilSlick(StudentWorld* sw, double x, double y);
private:
	void interactWithGhostRacer();
	void doStuffAfter() {}
	void getSprayed(int damage) {}
};

class Goodie : public Item {
public:
	Goodie(StudentWorld* sw, int iid, double x, double y, int dir, double size, bool sprayable);
protected:
	virtual void interactWithGhostRacer();
private:
	void doStuffAfter() {}
};

class HealingGoodie : public Goodie {
public:
	HealingGoodie(StudentWorld* sw, double x, double y);
private:
	void interactWithGhostRacer();
	void getSprayed(int damage);
};

class HolyWaterGoodie : public Goodie {
public:
	HolyWaterGoodie(StudentWorld* sw, double x, double y);
private:
	void interactWithGhostRacer();
	void getSprayed(int damage);
};

class SoulGoodie : public Goodie {
public:
	SoulGoodie(StudentWorld* sw, double x, double y);
private:
	void interactWithGhostRacer();
	void doStuffAfter();
	void getSprayed(int damage) {}
};

class HolyWaterProjectile : public Actor {
public:
	HolyWaterProjectile(StudentWorld* sw, double x, double y, int dir);
private:
	double m_travelDist;
	void doSomething();
	void getSprayed(int damage) {}
};

#endif // ACTOR_H_
