#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void BorderLine::doSomething() {
	
}

WhiteBorderLine::WhiteBorderLine(double x, double y) : BorderLine(x, y) {
	lastWhiteBorderLine = this;
}