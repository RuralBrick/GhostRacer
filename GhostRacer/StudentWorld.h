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
    double getGhostRacerVertSpeed() const;
private:
    GhostRacer* m_player;
    std::list<Actor*> m_actors;
    void addRoadMarkers();
    //void addZombieCabs();
    //void addOilSlicks();
    //void addZombiePeds();
    //void addHumanPeds();
    //void addHolyWaterRefillGoodies();
    //void addLostSoulGoodies();
};

#endif // STUDENTWORLD_H_
