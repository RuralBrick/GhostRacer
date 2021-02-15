#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

#pragma region Actor
Actor::Actor(int imageID, double x, double y, int dir, double size, unsigned int depth, 
	double xSpeed, double ySpeed, bool collisionAvoidanceWorthy)
	: GraphObject(imageID, x, y, dir, size, depth), 
	m_xSpeed(xSpeed), m_ySpeed(ySpeed), m_alive(true), 
	m_collisionAvoidanceWorthy(collisionAvoidanceWorthy) {

}

void Actor::doSomething() {
	double new_x = GraphObject::getX() + m_xSpeed;
	double new_y = GraphObject::getY() + m_ySpeed;
	GraphObject::moveTo(new_x, new_y);
	if (GraphObject::getX() < 0 || GraphObject::getX() > VIEW_WIDTH
		|| GraphObject::getY() < 0 || GraphObject::getY() > VIEW_HEIGHT) {
		m_alive = false;
		return;
	}
}
#pragma endregion Actor

#pragma region HPActor
HPActor::HPActor(int imageID, double x, double y, int dir, double size, unsigned int depth, 
	double xSpeed, double ySpeed, int hp, bool collisionAvoidanceWorthy)
	: Actor(imageID, x, y, dir, size, depth, xSpeed, ySpeed, collisionAvoidanceWorthy),
	m_hp(hp) {

}
#pragma endregion HPActor

#pragma region GhostRacer
GhostRacer::GhostRacer(double x, double y)
	: HPActor(IID_GHOST_RACER, x, y, 90, 4.0, 0, 0.0, 0.0, 100, true),
	m_sprays(10) {

}

void GhostRacer::doSomething() {
	if (!Actor::alive() || HPActor::hp() <= 0)
		return;
	
}
#pragma endregion GhostRacer

#pragma region BorderLine
BorderLine* BorderLine::m_lastWhiteBorderLine = nullptr;
BorderLine::BorderLine(double x, double y, Color color) 
	: Actor(color == Color::yellow ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE, 
		x, y, 0, 2.0, 1, 0.0, -4.0, true) {
	if (color == Color::white)
		m_lastWhiteBorderLine = this;
}

void BorderLine::doSomething() {
	Actor::doSomething();
}
#pragma endregion BorderLine