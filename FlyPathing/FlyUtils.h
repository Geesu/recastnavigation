#ifndef __FlyUtils_H_
#define __FlyUtils_H_

#include "Pathing.h"

class FlyUtils {
public:
    FlyUtils();
    ~FlyUtils();
    bool connect(const float *startPos, const int startCont, const float *endPos, const int endCont, int& responseCode);
    void addBlacklistedArea(const pathing::Position &position, float radius);

    const std::vector<pathing::Position> & getRoute() { return route; }
    
    void setCanFly(bool value) { canFly = value; }
    void setFactionId(int value) { factionId = value; }
    void setRandomVariance(int value) { randomVariance = value; }
private:
    std::vector<pathing::Position> route;
    std::vector<pathing::BlacklistedArea> blacklistedAreas;
    bool canFly;
    int  factionId;
    int  randomVariance;
};

#endif //__FlyUtils_H_
