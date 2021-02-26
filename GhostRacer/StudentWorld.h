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
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addActor(Actor* actor);
    void saveSoul() { --m_soulsToSave; }
    double getGhostRacerX() const;
    double getGhostRacerY() const;
    double getGhostRacerVertSpeed() const;
    bool checkOverlappingGhostRacer(const Actor* other) const;
    void spinGhostRacer();
    void damageGhostRacer(int damage);
    void healGhostRacer(int health);
    void rechargeGhostRacer(int sprays);
    void logHitHuman() { m_hitHuman = true; }
    template<typename F> Actor* getClosestCollisionAvoidanceWorthyActorInLane(const Actor* actor, F filter) const;
private:
    GhostRacer* m_player;
    std::list<Actor*> m_actors;
    const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    const int LANE_WIDTH = ROAD_WIDTH / 3;
    int m_soulsToSave;
    int m_bonus;
    bool m_hitHuman;
    bool checkOverlap(const Actor* a1, const Actor* a2) const;
    double calcDist(const Actor* a1, const Actor* a2) const;
    Lane getCurrentLane(const Actor* actor) const;
    Actor* getClosestCollisionAvoidanceWorthyActorInLane(Lane lane) const;
    void addRoadMarkers();
    void addZombieCabs();
    void addOilSlicks();
    void addZombiePeds();
    void addHumanPeds();
    void addHolyWaterRefillGoodies();
    void addLostSoulGoodies();
    int calcSoulsToSave() const;
    void updateDisplayText();
};

template <typename F>
Actor* StudentWorld::getClosestCollisionAvoidanceWorthyActorInLane(const Actor* actor, F filter) const {
    Lane lane = getCurrentLane(actor);
    if (lane == Lane::offroad)
        return nullptr;
    Actor* closestActor = nullptr;
    double closestDist = 999;
    double laneLeft = LEFT_EDGE + LANE_WIDTH * static_cast<int>(lane);
    double laneRight = LEFT_EDGE + LANE_WIDTH * (1 + static_cast<int>(lane));
    for (auto other : m_actors) {
        if (other->isCollisionAvoidanceWorthy() && other->getX() > laneLeft && other->getX() < laneRight 
            && filter(other->getY(), actor->getY())) {
            if (closestActor != nullptr) {
                double currDist = calcDist(actor, other);
                if (currDist < closestDist) {
                    closestActor = other;
                    closestDist = currDist;
                }
            }
            else {
                closestActor = other;
                closestDist = calcDist(actor, closestActor);
            }
        }
    }
    return closestActor;
}
#endif // STUDENTWORLD_H_
