#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
using namespace std;

#pragma region Instantiation
GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), m_soulsToSave(0), m_bonus(0), m_hitHuman(false)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}
#pragma endregion Instantiation

#pragma region Required Functions
int StudentWorld::init()
{
    m_soulsToSave = calcSoulsToSave();
    m_bonus = 5000;
    m_hitHuman = false;
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
        if (!m_player->isAlive() || m_hitHuman) {
            GameWorld::decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_soulsToSave <= 0) {
            increaseScore(m_bonus);
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
    addZombieCabs();
    addOilSlicks();
    addZombiePeds();
    addHumanPeds();
    addHolyWaterRefillGoodies();
    addLostSoulGoodies();

    /* Update display text */
    updateDisplayText();
    if (m_bonus > 0)
        --m_bonus;

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
void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}

double StudentWorld::getGhostRacerX() const {
    return m_player->getX();
}

double StudentWorld::getGhostRacerY() const {
    return m_player->getY();
}

double StudentWorld::getGhostRacerVertSpeed() const {
    return m_player->getYSpeed();
}

bool StudentWorld::checkOverlappingGhostRacer(const Actor* other) const {
    return checkOverlap(other, m_player);
}

void StudentWorld::spinGhostRacer() {
    m_player->spin();
}

void StudentWorld::damageGhostRacer(int damage) {
    m_player->takeDamage(damage);
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

void StudentWorld::addZombieCabs() {
    int chance_vehicle = max(100 - GameWorld::getLevel() * 10, 20);
    if (randInt(0, chance_vehicle - 1) == 0) {
        Lane lane = static_cast<Lane>(randInt(0, 2));
        for (int n = 0; n < 3; ++n) {
            Actor* lowestActor = getClosestCollisionAvoidanceWorthyActorInLane(lane);
            if (lowestActor == nullptr || lowestActor->getY() > VIEW_HEIGHT / 3) {

            }
        }
    }
}

void StudentWorld::addOilSlicks() {
    int chance_oilSlick = max(150 - GameWorld::getLevel() * 10, 40);
    if (randInt(0, chance_oilSlick - 1) == 0)
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addZombiePeds() {
    int chance_zombiePed = max(100 - GameWorld::getLevel() * 10, 20);
    if (randInt(0, chance_zombiePed - 1) == 0)
        m_actors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
}

void StudentWorld::addHumanPeds() {
    int chance_humanPed = max(200 - GameWorld::getLevel() * 10, 30);
    if (randInt(0, chance_humanPed - 1) == 0)
        m_actors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
}

void StudentWorld::addHolyWaterRefillGoodies() {
    int chance_holyWater = 100 + 10 * GameWorld::getLevel();
    if (randInt(0, chance_holyWater - 1) == 0)
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addLostSoulGoodies() {
    int chance_lostSoul = 100;
    if (randInt(0, chance_lostSoul - 1) == 0)
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}
#pragma endregion Add Functions

#pragma region Other Private Functions
bool StudentWorld::checkOverlap(const Actor* a1, const Actor* a2) const {
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double rad_sum = a1->getRadius() + a2->getRadius();
    if (delta_x < rad_sum * 0.25 && delta_y < rad_sum * 0.6)
        return true;
    return false;
}

double StudentWorld::calcDist(const Actor* a1, const Actor* a2) const {
    double dx = a1->getX() - a2->getX();
    double dy = a1->getY() - a2->getY();
    return sqrt(dx * dx + dy * dy);
}

StudentWorld::Lane StudentWorld::getCurrentLane(const Actor* actor) const {
    if (actor->getX() < LEFT_EDGE)
        return Lane::offroad;
    else if (actor->getX() < LEFT_EDGE + LANE_WIDTH)
        return Lane::left;
    else if (actor->getX() < LEFT_EDGE + 2 * LANE_WIDTH)
        return Lane::middle;
    else if (actor->getX() < LEFT_EDGE + 3 * LANE_WIDTH)
        return Lane::right;
    else
        return Lane::offroad;
}

Actor* StudentWorld::getClosestCollisionAvoidanceWorthyActorInLane(Lane lane) const {
    if (lane == Lane::offroad)
        return nullptr;
    Actor* lowestActor = nullptr;
    double lowestY = 999;
    double laneLeft = LEFT_EDGE + LANE_WIDTH * static_cast<int>(lane);
    double laneRight = LEFT_EDGE + LANE_WIDTH * (1 + static_cast<int>(lane));
    for (auto it : m_actors)
        if (it->isCollisionAvoidanceWorthy() && it->getX() > laneLeft && it->getX() < laneRight)
            if (lowestActor != nullptr) {
                if (it->getY() < lowestY) {
                    lowestActor = it;
                    lowestY = lowestActor->getY();
                }
            }
            else {
                lowestActor = it;
                lowestY = lowestActor->getY();
            }
    return lowestActor;
}

int StudentWorld::calcSoulsToSave() const {
    return 2 * GameWorld::getLevel() + 5;
}

void StudentWorld::updateDisplayText() {
    ostringstream displayOut;
    displayOut << "Score: " << GameWorld::getScore();
    displayOut << "  Lvl: " << GameWorld::getLevel();
    displayOut << "  Souls2Save: " << m_soulsToSave;
    displayOut << "  Lives: " << GameWorld::getLives();
    displayOut << "  Health: " << m_player->getHp();
    displayOut << "  Sprays: " << m_player->getSprays();
    displayOut << "  Bonus: " << m_bonus;
    GameWorld::setGameStatText(displayOut.str());
}
#pragma endregion Other Private Functions