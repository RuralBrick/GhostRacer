#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

#pragma region Required Functions
int StudentWorld::init()
{
    m_player = new GhostRacer(this, 128, 32);
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2.0;
    double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2.0;
    for (int j = 0; j < N; ++j) {
        m_actors.push_back(new BorderLine(this, LEFT_EDGE, j * SPRITE_HEIGHT, BorderLine::Color::yellow));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE, j * SPRITE_HEIGHT, BorderLine::Color::yellow));
    }
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    for (int j = 0; j < M; ++j) {
        m_actors.push_back(new BorderLine(this, LEFT_EDGE + ROAD_WIDTH / 3.0, j * (4.0 * SPRITE_HEIGHT), BorderLine::Color::white));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE - ROAD_WIDTH / 3.0, j * (4.0 * SPRITE_HEIGHT), BorderLine::Color::white));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_player->alive())
        m_player->doSomething();
    for (auto actor : m_actors) {
        if (actor->alive())
            actor->doSomething();
        if (!m_player->alive())
            return GWSTATUS_PLAYER_DIED;
        if (false /*completed level*/) {
            increaseScore(0 /*extra points*/);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if (!(*it)->alive()) {
            delete* it;
            it = m_actors.erase(it);
        }
        else
            ++it;
    }

    /* Add new actors */
    addRoadMarkers();
    //addZombieCabs();
    //addOilSlicks();
    //addZombiePeds();
    //addHumanPeds();
    //addHolyWaterRefillGoodies();
    //addLostSoulGoodies();

    /* Update display text */

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
#pragma endregion Other Public Functions

#pragma region Add Functions
void StudentWorld::addRoadMarkers() {
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - BorderLine::lastWhiteBorderLineY();
    if (delta_y >= SPRITE_HEIGHT) {
        m_actors.push_back(new BorderLine(this, ROAD_CENTER - ROAD_WIDTH / 2.0, new_border_y, BorderLine::Color::yellow));
        m_actors.push_back(new BorderLine(this, ROAD_CENTER + ROAD_WIDTH / 2.0, new_border_y, BorderLine::Color::yellow));
    }
    if (delta_y >= 4.0 * SPRITE_HEIGHT) {
        m_actors.push_back(new BorderLine(this, ROAD_CENTER - ROAD_WIDTH / 2.0 + ROAD_WIDTH / 3.0, new_border_y, BorderLine::Color::white));
        m_actors.push_back(new BorderLine(this, ROAD_CENTER + ROAD_WIDTH / 2.0 - ROAD_WIDTH / 3.0, new_border_y, BorderLine::Color::white));
    }
}
#pragma endregion Add Functions