// FlyPathing.h
// interface for the function
#ifndef _FlyPathing_Included
#define _FlyPathing_Included

#include "../Recast/Include/Recast.h"
#include "../Detour/Include/DetourNavMesh.h"
#include "../RecastDemo/Include/Filelist.h"
#include "DetourNavMeshQuery.h"
//#include "SampleInterfaces.h"

#include <tr1/unordered_map>
#include <map>
#include <list>
#include <vector>

struct BlackListObject
{
	double  position[3];
	double  radius;
};

struct WorldPosition // simplyfied version of Position struct from Pathing application.
{
    double  position[3];
    int32_t continentId;
};

struct PortalObject
{
    int32_t	inContinentId;
	double	inPosition[3];
    int32_t outContinentId;
    double	outPosition[3];
};

typedef std::vector<WorldPosition> RouteType;

enum ResultCode{
        RES_UNKNOWN = 0,
        RES_SUCCESS = 1,
        RES_START_POLY_NOT_FOUND = 2,
        RES_END_POLY_NOT_FOUND = 3,
        RES_PARTIAL_ROUTE = 4,
    };

struct Result {
    enum type {
        UNKNOWN = 0,
        SUCCESS = 1,
        FINAZLIZED_FAILED = 2,
        UPDATE_FAILED = 3,
        SLICE_INIT_FAILED = 4,
        MESH_INIT_FAILED = 5,
        PARTIAL_ROUTE = 6,
        END_POLY_NOT_FOUND = 7,
        START_POLY_NOT_FOUND = 8,
        FIND_STRAIGHT_FAILED = 9,
        INVALID_CONTINENTS = 10
    };
};

#define GRID_SIZE       (533.0f + (1.0f / 3.0f))

struct Response
{
    Result::type    result;
    RouteType       route;
};


class Continent;

// shared world data which should be prepared at the start of the server
class World
{
public:
    World();
    ~World();

    bool    init(const char *continentsPath);
    void    free();

    int32_t calculateRoute(const WorldPosition &startPos, const WorldPosition &endPos, const bool canFly);
private:
    // a number of continents
    // each continent has a AABB tree and set of portals


    typedef std::tr1::unordered_map<std::string, Continent*> ContinentMapType;
    typedef std::pair<std::string, Continent*> ContinentMapPair;
    ContinentMapType continents;

    typedef std::map<int32_t, std::string> IDToNameType;
    IDToNameType *continentIDToName;
    typedef std::tr1::unordered_map<std::string, int32_t> NameToIDType;
    NameToIDType *continentNameToID;
};

class Continent
{
public:
    Continent();
    ~Continent();

    bool    init(const char *path, const int32_t continentId);
    void    free();

    int32_t calculateRoute(Response& response, const float *startPos, const float *endPos, const bool canFly);
    std::list<float*> * calculateRouteNonFly(float start[3], float end[3], int *result_code);
private:
    // Recast+Detour data
    dtNavMesh       *m_navMesh;
    dtNavMeshQuery  *m_navQuery;
    //BuildContext    *m_ctx;
    // internal fields
    int32_t m_continentId;
    bool    addTile(const char *fn);

};

// process route in multi-threaded environment
bool	CalculateRoute(const double *startPos, const int32_t startContinent, const double *endPos, const int32_t endContinent,
        const bool canFly, const int32_t faction, const int32_t randomVariance, const BlackListObject *blackListedPositions,
        const int32_t blackListedPosCount, World *world);


#endif //_FlyPathing_Included
