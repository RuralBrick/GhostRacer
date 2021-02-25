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
    void addRoadMarkers();
    //void addZombieCabs();
    void addOilSlicks();
    void addZombiePeds();
    void addHumanPeds();
    void addHolyWaterRefillGoodies();
    void addLostSoulGoodies();
    int calcSoulsToSave() const;
    void updateDisplayText();
};

#endif // STUDENTWORLD_H_
