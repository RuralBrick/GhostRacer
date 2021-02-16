#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), m_souls(0)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

#pragma region Required Functions
int StudentWorld::init()
{
    m_souls = 0;
    m_player = new GhostRacer(this, 128, 32);
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int j = 0; j < N; ++j) {
        m_actors.push_back(new BorderLine(this, LEFT_EDGE, j * SPRITE_HEIGHT, BorderLine::Color::yellow));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE, j * SPRITE_HEIGHT, BorderLine::Color::yellow));
    }
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    for (int j = 0; j < M; ++j) {
        m_actors.push_back(new BorderLine(this, LEFT_EDGE + LANE_WIDTH, j * (4.0 * SPRITE_HEIGHT), BorderLine::Color::white));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE - LANE_WIDTH, j * (4.0 * SPRITE_HEIGHT), BorderLine::Color::white));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_player->isAlive())
        m_player->doSomething();
    for (auto actor : m_actors) {
        if (actor->isAlive())
            actor->doSomething();
        if (!m_player->isAlive()) {
            GameWorld::decLives(); // TOOD: have to decLives()?
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_souls >= calcSoulsToSave()) {
            increaseScore(0 /*extra points*/); // TODO: bonus points
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if (!(*it)->isAlive()) {
            delete* it;
            it = m_actors.erase(it);
        }
        else
            ++it;
    }

    /* Add new actors */
    addRoadMarkers();
    //addZombieCabs();
    addOilSlicks();
    //addZombiePeds();
    //addHumanPeds();
    addHolyWaterRefillGoodies();
    addLostSoulGoodies();

    /* Update display text */
    updateDisplayText();

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        delete* it;
        it = m_actors.erase(it);
    }
}
#pragma endregion Required Functions

#pragma region Other Public Functions
double StudentWorld::getGhostRacerVertSpeed() const {
    if (m_player != nullptr)
        return m_player->getYSpeed();
    return 0;
}

bool StudentWorld::checkOverlap(const Actor* a1, const Actor* a2) const {
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double rad_sum = a1->getRadius() + a2->getRadius();
    if (delta_x < rad_sum * 0.25 && delta_y < rad_sum * 0.6)
        return true;
    return false;
}

bool StudentWorld::checkOverlappingGhostRacer(const Actor* other) const {
    return checkOverlap(other, m_player);
}

void StudentWorld::spinGhostRacer() {
    m_player->spin();
}

void StudentWorld::healGhostRacer(int health) {
    m_player->healDamage(health);
}

void StudentWorld::rechargeGhostRacer(int sprays) {
    m_player->addSprays(sprays);
}
#pragma endregion Other Public Functions

#pragma region Add Functions
void StudentWorld::addRoadMarkers() {
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - BorderLine::getLastWhiteBorderLineY();
    if (delta_y >= SPRITE_HEIGHT) {
        m_actors.push_back(new BorderLine(this, LEFT_EDGE, new_border_y, BorderLine::Color::yellow));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE, new_border_y, BorderLine::Color::yellow));
    }
    if (delta_y >= 4.0 * SPRITE_HEIGHT) {
        m_actors.push_back(new BorderLine(this, LEFT_EDGE + LANE_WIDTH, new_border_y, BorderLine::Color::white));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE - LANE_WIDTH, new_border_y, BorderLine::Color::white));
    }
}

void StudentWorld::addOilSlicks() {
    int chance_oilSlick = max(150 - GameWorld::getLevel() * 10, 40);
    if (randInt(0, chance_oilSlick) == 0)
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addHolyWaterRefillGoodies() {
    int chance_holyWater = 100 + 10 * GameWorld::getLevel();
    if (randInt(0, chance_holyWater) == 0)
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addLostSoulGoodies() {
    int chance_lostSoul = 100;
    if (randInt(0, chance_lostSoul) == 0)
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}
#pragma endregion Add Functions

int StudentWorld::calcSoulsToSave() const {
    return 2 * GameWorld::getLevel() + 5;
}

void StudentWorld::updateDisplayText() {
    ostringstream displayOut;
    displayOut << "Score: " << GameWorld::getScore();
    displayOut << "  Lvl: " << GameWorld::getLevel();
    displayOut << "  Souls2Save: " << calcSoulsToSave();
    displayOut << "  Lives: " << GameWorld::getLives();
    displayOut << "  Health: " << m_player->getHp();
    displayOut << "  Sprays: " << m_player->getSprays();
    displayOut << "  Bonus: "; // TODO: display bonus
    GameWorld::setGameStatText(displayOut.str());
}