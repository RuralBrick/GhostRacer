#define _USE_MATH_DEFINES
#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <algorithm>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

#pragma region Actor
Actor::Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth,
	double xSpeed, double ySpeed, bool collisionAvoidanceWorthy, bool sprayable)
	: GraphObject(imageID, x, y, dir, size, depth), 
	m_sw(sw), m_xSpeed(xSpeed), m_ySpeed(ySpeed), m_alive(true), 
	m_collisionAvoidanceWorthy(collisionAvoidanceWorthy), m_sprayable(sprayable) {}

bool Actor::isOutOfBounds() const {
	if (GraphObject::getX() < 0 || GraphObject::getX() > VIEW_WIDTH
		|| GraphObject::getY() < 0 || GraphObject::getY() > VIEW_HEIGHT)
		return true;
	return false;
}

bool Actor::isOverlappingGhostRacer() const {
	return Actor::getWorld()->checkOverlappingGhostRacer(this);
}

// Returns bool to tell caller to return
bool Actor::move() {
	double new_x = GraphObject::getX() + m_xSpeed;
	double new_y = GraphObject::getY() + m_ySpeed - Actor::getWorld()->getGhostRacerVertSpeed();
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
HPActor::HPActor(StudentWorld* sw, int imageID, double x, double y, int dir, double size,
	double xSpeed, double ySpeed, int hp, bool sprayable)
	: Actor(sw, imageID, x, y, dir, size, 0, xSpeed, ySpeed, true, sprayable),
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
	: HPActor(sw, IID_GHOST_RACER, x, y, 90, 4.0, 0.0, 0.0, 100, false),
	m_sprays(10) {}

bool GhostRacer::move() {
	const double max_shift_per_tick = 4.0;
	double delta_x = std::cos(GraphObject::getDirection() * M_PI / 180.0) * max_shift_per_tick;
	GraphObject::moveTo(GraphObject::getX() + delta_x, GraphObject::getY());
	return false;
}

void GhostRacer::die() {
	Actor::die();
	Actor::getWorld()->playSound(SOUND_PLAYER_DIE);
}

void GhostRacer::doSomething() {
	if (!Actor::isAlive())
		return;
	if (GraphObject::getX() <= ROAD_CENTER - ROAD_WIDTH / 2.0) {
		if (GraphObject::getDirection() > 90)
			HPActor::takeDamage(10);
		GraphObject::setDirection(82);
		Actor::getWorld()->playSound(SOUND_VEHICLE_CRASH);
		goto lbl_move;
	}
	if (GraphObject::getX() >= ROAD_CENTER + ROAD_WIDTH / 2.0) {
		if (GraphObject::getDirection() < 90)
			HPActor::takeDamage(10);
		GraphObject::setDirection(98);
		Actor::getWorld()->playSound(SOUND_VEHICLE_CRASH);
		goto lbl_move;
	}
	int key;
	if (Actor::getWorld()->getKey(key)) {
		if (key == KEY_PRESS_SPACE && m_sprays >= 1) {
			double x = GraphObject::getX() + SPRITE_HEIGHT * std::cos(GraphObject::getDirection() * M_PI / 180.0);
			double y = GraphObject::getY() + SPRITE_HEIGHT * std::sin(GraphObject::getDirection() * M_PI / 180.0);
			Actor::getWorld()->addActor(new HolyWaterProjectile(Actor::getWorld(), x, y, GraphObject::getDirection()));
			Actor::getWorld()->playSound(SOUND_PLAYER_SPRAY);
			--m_sprays;
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
		x, y, 0, 2.0, 2, 0.0, -4.0, false, false) {
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
	double xSpeed, double ySpeed, int hp)
	: HPActor(sw, iid, x, y, dir, size, xSpeed, ySpeed, hp, true),
	m_moveDist(0) {}

void AIActor::doSomething() {
	// Check if need to return after each step
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
	: AIActor(sw, iid, x, y, 0, size, 0.0, -4.0, 2) {}

void Pedestrian::planMove() {
	Actor::setXSpeed(randInt(0, 1) == 0 ? randInt(-3, -1) : randInt(1, 3));
	AIActor::setMoveDist(randInt(4, 32));
	GraphObject::setDirection(Actor::getXSpeed() < 0 ? 180 : 0);
}
#pragma endregion Pedestrian

#pragma region HumanPedestrian
HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
	: Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0) {}

bool HumanPedestrian::interactWithGhostRacer() {
	Actor::getWorld()->logHitHuman();
	return true;
}

void HumanPedestrian::doDamageEffect() {
	Actor::setXSpeed(Actor::getXSpeed() * -1);
	GraphObject::setDirection(GraphObject::getDirection() == 180 ? 0 : 180);
	Actor::getWorld()->playSound(SOUND_PED_HURT);
}

void HumanPedestrian::getSprayed(int damage) {
	doDamageEffect();
}
#pragma endregion HumanPedestrian

#pragma region ZombiePedestrian
ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
	: Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3.0),
	m_ticksTillGrunt(0) {}

bool ZombiePedestrian::interactWithGhostRacer() {
	Actor::getWorld()->damageGhostRacer(5);
	HPActor::takeDamage(2);
	return true;
}

void ZombiePedestrian::die() {
	Actor::die();
	Actor::getWorld()->playSound(SOUND_PED_DIE);
	if (!Actor::isOverlappingGhostRacer())
		if (randInt(0, 4) == 0)
			Actor::getWorld()->addActor(new HealingGoodie(Actor::getWorld(), GraphObject::getX(), GraphObject::getY()));
	Actor::getWorld()->increaseScore(150);
}

void ZombiePedestrian::doDamageEffect() {
	Actor::getWorld()->playSound(SOUND_PED_HURT);
}

bool ZombiePedestrian::actBeforeMove() {
	if (std::abs(GraphObject::getX() - Actor::getWorld()->getGhostRacerX()) <= 30
		&& GraphObject::getY() > Actor::getWorld()->getGhostRacerY()) {
		GraphObject::setDirection(270);
		if (GraphObject::getX() < Actor::getWorld()->getGhostRacerX())
			Actor::setXSpeed(1);
		else if (GraphObject::getX() > Actor::getWorld()->getGhostRacerX())
			Actor::setXSpeed(-1);
		else
			Actor::setXSpeed(0);
		if (--m_ticksTillGrunt <= 0) {
			Actor::getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			m_ticksTillGrunt = 20;
		}
	}
	return false;
}

void ZombiePedestrian::getSprayed(int damage) {
	HPActor::takeDamage(damage);
}
#pragma endregion ZombiePedestrian

#pragma region ZombieCab
ZombieCab::ZombieCab(StudentWorld* sw, double x, double y, double ySpeed)
	: AIActor(sw, IID_ZOMBIE_CAB, x, y, 90, 4.0, 0.0, ySpeed, 3),
	m_hitGhostRider(false) {}

bool ZombieCab::interactWithGhostRacer() {
	if (m_hitGhostRider)
		return false;
	Actor::getWorld()->playSound(SOUND_VEHICLE_CRASH);
	Actor::getWorld()->damageGhostRacer(20);
	if (GraphObject::getX() <= Actor::getWorld()->getGhostRacerX()) {
		Actor::setXSpeed(-5);
		GraphObject::setDirection(120 + randInt(0, 19));
	}
	else {
		Actor::setXSpeed(5);
		GraphObject::setDirection(60 - randInt(0, 19));
	}
	m_hitGhostRider = true;
	return false;
}

void ZombieCab::die() {
	Actor::die();
	Actor::getWorld()->playSound(SOUND_VEHICLE_DIE);
	if (randInt(0, 4) == 0)
		Actor::getWorld()->addActor(new OilSlick(Actor::getWorld(), GraphObject::getX(), GraphObject::getY()));
	Actor::getWorld()->increaseScore(200);
}

void ZombieCab::doDamageEffect() {
	Actor::getWorld()->playSound(SOUND_VEHICLE_HURT);
}

double ZombieCab::calcDist(const Actor* other) {
	return Actor::getWorld()->calcDist(this, other);
}

bool ZombieCab::checkInFront(double y) {
	return y > GraphObject::getY();
}

bool ZombieCab::checkBehind(double y) {
	return y < GraphObject::getY();
}

bool ZombieCab::actAfterMove() {
	if (Actor::getYSpeed() > Actor::getWorld()->getGhostRacerVertSpeed()) {
		// Get closest actor in front
		Actor* closestActor = Actor::getWorld()->getClosestCollisionAvoidanceWorthyActorInLane(
			this, Actor::getWorld()->getCurrentLane(this), &ZombieCab::calcDist, &ZombieCab::checkInFront);
		if (closestActor != nullptr && (closestActor->getY() - GraphObject::getY()) < 96) {
			Actor::adjustYSpeed(-0.5);
			return true;
		}
	}
	else {
		// Get closest actor behind
		Actor* closestActor = Actor::getWorld()->getClosestCollisionAvoidanceWorthyActorInLane(
			this, Actor::getWorld()->getCurrentLane(this), &ZombieCab::calcDist, &ZombieCab::checkBehind);
		if (closestActor != nullptr && (GraphObject::getY() - closestActor->getY()) < 96 && !Actor::getWorld()->isGhostRacer(closestActor)) {
			Actor::adjustYSpeed(0.5);
			return true;
		}
	}
	return false;
}

void ZombieCab::planMove() {
	AIActor::setMoveDist(randInt(4, 32));
	Actor::adjustYSpeed(randInt(-2, 2));
}

void ZombieCab::getSprayed(int damage) {
	HPActor::takeDamage(damage);
}
#pragma endregion ZombieCab

#pragma region Item
Item::Item(StudentWorld* sw, int iid, double x, double y, int dir, double size, bool sprayable)
	: Actor(sw, iid, x, y, dir, size, 2, 0.0, -4.0, false, sprayable) {}

void Item::doSomething() {
	if (Actor::move())
		return;
	if (isOverlappingGhostRacer())
		interactWithGhostRacer();
	doStuffAfter();
}
#pragma endregion Item

#pragma region OilSlick
OilSlick::OilSlick(StudentWorld* sw, double x, double y)
	: Item(sw, IID_OIL_SLICK, x, y, 0, randInt(2, 5), false) {}

void OilSlick::interactWithGhostRacer() {
	Actor::getWorld()->playSound(SOUND_OIL_SLICK);
	Actor::getWorld()->spinGhostRacer();
}
#pragma endregion OilSlick

#pragma region Goodie
Goodie::Goodie(StudentWorld* sw, int iid, double x, double y, int dir, double size, bool sprayable)
	: Item(sw, iid, x, y, dir, size, sprayable) {}

void Goodie::interactWithGhostRacer() {
	Actor::die();
	Actor::getWorld()->playSound(SOUND_GOT_GOODIE);
}
#pragma endregion Goodie

#pragma region HealingGoodie
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
	: Goodie(sw, IID_HEAL_GOODIE, x, y, 0, 1.0, true) {}

void HealingGoodie::interactWithGhostRacer() {
	Actor::getWorld()->healGhostRacer(10);
	Goodie::interactWithGhostRacer();
	Actor::getWorld()->increaseScore(250);
}

void HealingGoodie::getSprayed(int damage) {
	Actor::die();
}
#pragma endregion HealingGoodie

#pragma region HolyWaterGoodie
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
	: Goodie(sw, IID_HOLY_WATER_GOODIE, x, y, 90, 2.0, true) {}

void HolyWaterGoodie::interactWithGhostRacer() {
	Actor::getWorld()->rechargeGhostRacer(10);
	Goodie::interactWithGhostRacer();
	Actor::getWorld()->increaseScore(50);
}

void HolyWaterGoodie::getSprayed(int damage) {
	Actor::die();
}
#pragma endregion HolyWaterGoodie

#pragma region SoulGoodie
SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
	: Goodie(sw, IID_SOUL_GOODIE, x, y, 0, 4.0, false) {}

void SoulGoodie::interactWithGhostRacer() {
	Actor::getWorld()->saveSoul();
	Actor::die();
	Actor::getWorld()->playSound(SOUND_GOT_SOUL);
	Actor::getWorld()->increaseScore(100);
}

void SoulGoodie::doStuffAfter() {
	GraphObject::setDirection(GraphObject::getDirection() - 10);
}
#pragma endregion SoulGoodie

#pragma region HolyWaterProjectile
HolyWaterProjectile::HolyWaterProjectile(StudentWorld* sw, double x, double y, int dir)
	: Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, 0.0, 0.0, false, false),
	m_travelDist(160) {}

void HolyWaterProjectile::doSomething() {
	if (!Actor::isAlive())
		return;
	Actor* hitActor = Actor::getWorld()->getFirstOverlappingSprayableActor(this);
	if (hitActor != nullptr) {
		hitActor->getSprayed(1);
		Actor::die();
		return;
	}
	GraphObject::moveForward(SPRITE_HEIGHT);
	m_travelDist -= SPRITE_HEIGHT;
	if (Actor::isOutOfBounds() || m_travelDist <= 0)
		Actor::die();
}
#pragma endregion HolyWaterProjectile
