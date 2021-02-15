#include "Actor.h"
#include "StudentWorld.h"
#define _USE_MATH_DEFINES
#include <math.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

#pragma region Actor
Actor::Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth,
	double xSpeed, double ySpeed, bool collisionAvoidanceWorthy)
	: GraphObject(imageID, x, y, dir, size, depth), 
	m_sw(sw), m_xSpeed(xSpeed), m_ySpeed(ySpeed), m_alive(true), 
	m_collisionAvoidanceWorthy(collisionAvoidanceWorthy) {

}

void Actor::doSomething() {
	double new_x = GraphObject::getX() + m_xSpeed;
	double new_y = GraphObject::getY() + m_ySpeed - m_sw->getGhostRacerVertSpeed();
	GraphObject::moveTo(new_x, new_y);
	if (GraphObject::getX() < 0 || GraphObject::getX() > VIEW_WIDTH
		|| GraphObject::getY() < 0 || GraphObject::getY() > VIEW_HEIGHT) {
		m_alive = false;
		return;
	}
}
#pragma endregion Actor

#pragma region HPActor
HPActor::HPActor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth,
	double xSpeed, double ySpeed, int hp, bool collisionAvoidanceWorthy)
	: Actor(sw, imageID, x, y, dir, size, depth, xSpeed, ySpeed, collisionAvoidanceWorthy),
	m_hp(hp) {

}
#pragma endregion HPActor

#pragma region GhostRacer
GhostRacer::GhostRacer(StudentWorld* sw, double x, double y)
	: HPActor(sw, IID_GHOST_RACER, x, y, 90, 4.0, 0, 0.0, 0.0, 100, true),
	m_sprays(10) {

}

void GhostRacer::move() {
	const double max_shift_per_tick = 4.0;
	double delta_x = cos(GraphObject::getDirection() * M_PI / 180.0) * max_shift_per_tick;
	GraphObject::moveTo(GraphObject::getX() + delta_x, GraphObject::getY());
}

void GhostRacer::doSomething() {
	if (!Actor::alive() || HPActor::hp() <= 0)
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
#pragma endregion GhostRacer

#pragma region BorderLine
BorderLine* BorderLine::m_lastWhiteBorderLine = nullptr;
BorderLine::BorderLine(StudentWorld* sw, double x, double y, Color color)
	: Actor(sw, color == Color::yellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE, 
		x, y, 0, 2.0, 1, 0.0, -4.0, true) {
	if (color == Color::white)
		m_lastWhiteBorderLine = this;
}

void BorderLine::doSomething() {
	Actor::doSomething();
}
#pragma endregion BorderLine