#include "Actor.h"
#include "StudentWorld.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <stdlib.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

#pragma region Actor
Actor::Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth,
	double xSpeed, double ySpeed, bool collisionAvoidanceWorthy)
	: GraphObject(imageID, x, y, dir, size, depth), 
	m_sw(sw), m_xSpeed(xSpeed), m_ySpeed(ySpeed), m_alive(true), 
	m_collisionAvoidanceWorthy(collisionAvoidanceWorthy) {}

bool Actor::isOutOfBounds() const {
	if (GraphObject::getX() < 0 || GraphObject::getX() > VIEW_WIDTH
		|| GraphObject::getY() < 0 || GraphObject::getY() > VIEW_HEIGHT)
		return true;
	return false;
}

bool Actor::isOverlappingGhostRacer() const {
	return m_sw->checkOverlappingGhostRacer(this);
}

bool Actor::move() {
	double new_x = GraphObject::getX() + m_xSpeed;
	double new_y = GraphObject::getY() + m_ySpeed - m_sw->getGhostRacerVertSpeed();
	GraphObject::moveTo(new_x, new_y);
	if (isOutOfBounds()) {
		m_alive = false;
		return true;
	}
	return false;
}

void Actor::die() {
	m_alive = false;
}
#pragma endregion Actor

#pragma region HPActor
HPActor::HPActor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth,
	double xSpeed, double ySpeed, int hp, bool collisionAvoidanceWorthy)
	: Actor(sw, imageID, x, y, dir, size, depth, xSpeed, ySpeed, collisionAvoidanceWorthy),
	m_hp(hp) {}

void HPActor::takeDamage(int damage) {
	m_hp -= damage;
	if (m_hp <= 0)
		die();
	else
		doDamageEffect();
}
#pragma endregion HPActor

#pragma region GhostRacer
GhostRacer::GhostRacer(StudentWorld* sw, double x, double y)
	: HPActor(sw, IID_GHOST_RACER, x, y, 90, 4.0, 0, 0.0, 0.0, 100, true),
	m_sprays(10) {}

bool GhostRacer::move() {
	const double max_shift_per_tick = 4.0;
	double delta_x = cos(GraphObject::getDirection() * M_PI / 180.0) * max_shift_per_tick;
	GraphObject::moveTo(GraphObject::getX() + delta_x, GraphObject::getY());
	return false;
}

void GhostRacer::die() {
	Actor::die();
	m_sw->playSound(SOUND_PLAYER_DIE);
}

void GhostRacer::doSomething() {
	if (!Actor::isAlive())
		return;
	if (GraphObject::getX() <= ROAD_CENTER - ROAD_WIDTH / 2.0) {
		if (GraphObject::getDirection() > 90)
			HPActor::takeDamage(10);
		GraphObject::setDirection(82);
		m_sw->playSound(SOUND_VEHICLE_CRASH);
		goto lbl_move;
	}
	if (GraphObject::getX() >= ROAD_CENTER + ROAD_WIDTH / 2.0) {
		if (GraphObject::getDirection() < 90)
			HPActor::takeDamage(10);
		GraphObject::setDirection(98);
		m_sw->playSound(SOUND_VEHICLE_CRASH);
		goto lbl_move;
	}
	int key;
	if (m_sw->getKey(key)) {
		if (key == KEY_PRESS_SPACE) {
			// TODO: shoot holy water
			goto lbl_move;
		}
		if (key == KEY_PRESS_LEFT && GraphObject::getDirection() < 114) {
			GraphObject::setDirection(GraphObject::getDirection() + 8);
			goto lbl_move;
		}
		if (key == KEY_PRESS_RIGHT && GraphObject::getDirection() > 66) {
			GraphObject::setDirection(GraphObject::getDirection() - 8);
			goto lbl_move;
		}
		if (key == KEY_PRESS_UP && Actor::getYSpeed() < 5) {
			Actor::adjustYSpeed(1);
			goto lbl_move;
		}
		if (key == KEY_PRESS_DOWN && Actor::getYSpeed() > -1) {
			Actor::adjustYSpeed(-1);
			goto lbl_move;
		}
	}
lbl_move:
	move();
}

void GhostRacer::healDamage(int hp) {
	HPActor::setHp(std::min(HPActor::getHp() + hp, MAX_HP));
}

void GhostRacer::spin() {
	int newDir = GraphObject::getDirection();
	if (randInt(0, 1) == 0)
		newDir -= randInt(5, 20);
	else
		newDir += randInt(5, 20);
	GraphObject::setDirection(std::clamp(newDir, 60, 120));
}
#pragma endregion GhostRacer

#pragma region BorderLine
BorderLine* BorderLine::m_lastWhiteBorderLine = nullptr;
BorderLine::BorderLine(StudentWorld* sw, double x, double y, Color color)
	: Actor(sw, color == Color::yellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE,
		x, y, 0, 2.0, 2, 0.0, -4.0, true) {
	if (color == Color::white)
		m_lastWhiteBorderLine = this;
}

void BorderLine::doSomething() {
	Actor::move();
}

double BorderLine::getLastWhiteBorderLineY() {
	if (m_lastWhiteBorderLine != nullptr)
		return BorderLine::m_lastWhiteBorderLine->getY();
	return 0;
}
#pragma endregion BorderLine

#pragma region AIActor
AIActor::AIActor(StudentWorld* sw, int iid, double x, double y, int dir, double size,
	double xSpeed, double ySpeed, int hp, bool collisionAvoidanceWorthy)
	: HPActor(sw, iid, x, y, dir, size, 0, xSpeed, ySpeed, hp, collisionAvoidanceWorthy),
	m_moveDist(0) {}

void AIActor::doSomething() {
	if (!Actor::isAlive())
		return;
	if (Actor::isOverlappingGhostRacer())
		if (interactWithGhostRacer())
			return;
	if (actBeforeMove())
		return;
	if (Actor::move())
		return;
	if (actAfterMove())
		return;
	if (--m_moveDist > 0)
		return;
	planMove();
}
#pragma endregion AIActor

#pragma region Pedestrian
Pedestrian::Pedestrian(StudentWorld* sw, int iid, double x, double y, double size)
	: AIActor(sw, iid, x, y, 0, size, 0.0, -4.0, 2, true) {}

void Pedestrian::planMove() {
	Actor::setXSpeed(randInt(0, 1) == 0 ? randInt(-3, -1) : randInt(1, 3));
	m_moveDist = randInt(4, 32);
	GraphObject::setDirection(Actor::getXSpeed() < 0 ? 180 : 0);
}
#pragma endregion Pedestrian

#pragma region HumanPedestrian
HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
	: Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0) {}

bool HumanPedestrian::interactWithGhostRacer() {
	m_sw->logHitHuman();
	return true;
}

void HumanPedestrian::doDamageEffect() {
	Actor::setXSpeed(Actor::getXSpeed() * -1);
	GraphObject::setDirection(GraphObject::getDirection() == 180 ? 0 : 180);
	m_sw->playSound(SOUND_PED_HURT);
}
#pragma endregion HumanPedestrian

#pragma region ZombiePedestrian
ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
	: Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3.0), m_ticksTillGrunt(0) {}

bool ZombiePedestrian::interactWithGhostRacer() {
	m_sw->damageGhostRacer(5);
	HPActor::takeDamage(2);
	return true;
}

void ZombiePedestrian::die() {
	Actor::die();
	m_sw->playSound(SOUND_PED_DIE);
	if (!Actor::isOverlappingGhostRacer())
		if (randInt(0, 4) == 0)
			m_sw->addActor(new HealingGoodie(m_sw, GraphObject::getX(), GraphObject::getY()));
	m_sw->increaseScore(150);
}

void ZombiePedestrian::doDamageEffect() {
	m_sw->playSound(SOUND_PED_HURT);
}

bool ZombiePedestrian::actBeforeMove() {
	if (abs(GraphObject::getX() - m_sw->getGhostRacerX()) <= 30 && GraphObject::getY() > m_sw->getGhostRacerY()) {
		GraphObject::setDirection(270);
		if (GraphObject::getX() < m_sw->getGhostRacerX())
			Actor::setXSpeed(1);
		else if (GraphObject::getX() > m_sw->getGhostRacerX())
			Actor::setXSpeed(-1);
		else
			Actor::setXSpeed(0);
		if (--m_ticksTillGrunt <= 0) {
			m_sw->playSound(SOUND_ZOMBIE_ATTACK);
			m_ticksTillGrunt = 20;
		}
	}
	return false;
}
#pragma endregion ZombiePedestrian

#pragma region ZombieCab

#pragma endregion ZombieCab

#pragma region Item
Item::Item(StudentWorld* sw, int iid, double x, double y, int dir, double size)
	: Actor(sw, iid, x, y, dir, size, 2, 0.0, -4.0, false) {}

void Item::doSomething() {
	if (Actor::move())
		return;
	if (isOverlappingGhostRacer())
		interactWithGhostRacer();
	doStuffAfter();
}
#pragma endregion Item

#pragma region Goodie
Goodie::Goodie(StudentWorld* sw, int iid, double x, double y, int dir, double size)
	: Item(sw, iid, x, y, dir, size) {}

void Goodie::interactWithGhostRacer() {
	Actor::die();
	m_sw->playSound(SOUND_GOT_GOODIE);
}
#pragma endregion Goodie

#pragma region OilSlick
OilSlick::OilSlick(StudentWorld* sw, double x, double y)
	: Item(sw, IID_OIL_SLICK, x, y, 0, randInt(2, 5)) {}

void OilSlick::interactWithGhostRacer() {
	m_sw->playSound(SOUND_OIL_SLICK);
	m_sw->spinGhostRacer();
}
#pragma endregion OilSlick

#pragma region HealingGoodie
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
	: Goodie(sw, IID_HEAL_GOODIE, x, y, 0, 1.0) {}

void HealingGoodie::interactWithGhostRacer() {
	m_sw->healGhostRacer(10);
	Goodie::interactWithGhostRacer();
	m_sw->increaseScore(250);
}
#pragma endregion HealingGoodie

#pragma region HolyWaterGoodie
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
	: Goodie(sw, IID_HOLY_WATER_GOODIE, x, y, 90, 2.0) {}

void HolyWaterGoodie::interactWithGhostRacer() {
	m_sw->rechargeGhostRacer(10);
	Goodie::interactWithGhostRacer();
	m_sw->increaseScore(50);
}
#pragma endregion HolyWaterGoodie

#pragma region SoulGoodie
SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
	: Goodie(sw, IID_SOUL_GOODIE, x, y, 0, 4.0) {}

void SoulGoodie::interactWithGhostRacer() {
	m_sw->saveSoul();
	Actor::die();
	m_sw->playSound(SOUND_GOT_SOUL);
	m_sw->increaseScore(100);
}

void SoulGoodie::doStuffAfter() {
	GraphObject::setDirection(GraphObject::getDirection() - 10);
}
#pragma endregion SoulGoodie
