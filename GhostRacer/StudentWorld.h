#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
public:
    enum class Lane {offroad = -1, left = 0, middle = 1, right = 2};
    static double calcDist(const Actor* a1, const Actor* a2);
    static Lane getCurrentLane(const Actor* actor);
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addActor(Actor* actor);
    void saveSoul() { --m_soulsToSave; }
    double isGhostRacer(const Actor* actor) { return actor == (Actor*)m_player; }
    double getGhostRacerX() const;
    double getGhostRacerY() const;
    double getGhostRacerVertSpeed() const;
    bool checkOverlappingGhostRacer(const Actor* other) const;
    void spinGhostRacer();
    void damageGhostRacer(int damage);
    void healGhostRacer(int health);
    void rechargeGhostRacer(int sprays);
    void logHitHuman() { m_hitHuman = true; }
    template<typename Class, typename F1, typename F2>
        Actor* getClosestCollisionAvoidanceWorthyActorInLane(const Class caller, Lane lane, F1 calcDist, F2 filter) const;
private:
    static const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    static const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    static const int LANE_WIDTH = ROAD_WIDTH / 3;
    static bool checkOverlap(const Actor* a1, const Actor* a2);
    GhostRacer* m_player;
    std::list<Actor*> m_actors;
    int m_soulsToSave;
    int m_bonus;
    bool m_hitHuman;
    void addRoadMarkers();
    void addZombieCabs();
    void addOilSlicks();
    void addZombiePeds();
    void addHumanPeds();
    void addHolyWaterRefillGoodies();
    void addLostSoulGoodies();
    int calcSoulsToSave() const;
    void updateDisplayText();
    bool returnTrue(double a) const;
    double calcDistFromBottom(const Actor* actor) const;
    double calcDistFromTop(const Actor* actor) const;
};

template <typename Class, typename F1, typename F2>
Actor* StudentWorld::getClosestCollisionAvoidanceWorthyActorInLane(const Class caller, Lane lane, F1 calcDist, F2 filter) const {
    if (lane == Lane::offroad)
        return nullptr;
    Actor* closestActor = nullptr;
    double closestDist = 999;
    double laneLeft = LEFT_EDGE + LANE_WIDTH * static_cast<int>(lane);
    double laneRight = LEFT_EDGE + LANE_WIDTH * (1 + static_cast<int>(lane));
    if (m_player->getX() >= laneLeft && m_player->getX() < laneRight && (caller->*filter)(m_player->getY())) {
        closestActor = m_player;
        closestDist = (caller->*calcDist)(closestActor);
    }
    for (auto actor : m_actors) {
        if (actor->isCollisionAvoidanceWorthy() && actor->getX() >= laneLeft && actor->getX() < laneRight
            && (caller->*filter)(actor->getY())) {
            if (closestActor != nullptr) {
                double currDist = (caller->*calcDist)(actor);
                if (currDist < closestDist) {
                    closestActor = actor;
                    closestDist = currDist;
                }
            }
            else {
                closestActor = actor;
                closestDist = (caller->*calcDist)(closestActor);
            }
        }
    }
    return closestActor;
}
#endif // STUDENTWORLD_H_
