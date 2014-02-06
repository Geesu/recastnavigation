// FlyPathing.cpp

#include "FlyPathing.h"
#include "Coords.h"

#include "../lib/glm/glm.hpp"
#include "../lib/PlistCpp/src/pugixml.hpp"

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif
#include "DetourCommon.h"


enum SamplePolyAreas{
/*    SAMPLE_POLYAREA_GROUND,
    SAMPLE_POLYAREA_WATER,
    SAMPLE_POLYAREA_ROAD,
    SAMPLE_POLYAREA_DOOR,
    SAMPLE_POLYAREA_GRASS,
    SAMPLE_POLYAREA_JUMP,*/
    SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
    SAMPLE_POLYAREA_FIRE,
    SAMPLE_POLYAREA_MOB,
	SAMPLE_POLYAREA_MOBS,
    SAMPLE_POLYAREA_HIGH_SLOPE,
};

enum SamplePolyFlags{
    SAMPLE_POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
    SAMPLE_POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
    SAMPLE_POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
    SAMPLE_POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
    SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
    SAMPLE_POLYFLAGS_ALL		= 0xffff	// All abilities.
};

using namespace std;

//
// class World
//
World::World() {
    continentIDToName = NULL;
    continentNameToID = NULL;
}

World::~World() {
    free();
}

bool World::init(char const *continentsPath) {
    continentIDToName = new IDToNameType;
    continentNameToID = new NameToIDType;

    char appPath[PATH_MAX] = {0};

#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)appPath, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);

    //chdir(path);
    //std::cout << "Current Path: " << path << std::endl;
#endif

    string strPath(appPath);
    strPath += "/";
    string strMapsPath = strPath + "Maps.xml";

    // 2. create continent name<->id mapping
    pugi::xml_document doc;
    if (!doc.load_file(strMapsPath.c_str())) {
        return false;
    }
    pugi::xml_node tools = doc.child("data");
    for (pugi::xml_node tool = tools.first_child(); tool; tool = tool.next_sibling())
    {
        std::string name;
        int32_t continentID = 0;

        for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
        {
            std::string AttrName(attr.name());

            if ( AttrName == "Name" ){
                name = attr.value();
            }
            else if ( AttrName == "ID" ){
                continentID = atoi(attr.value());
            }
        }
        // save it
        continentIDToName->insert(make_pair(continentID, name));
        continentNameToID->insert(make_pair(name, continentID));
    }
    // load continents' meshes
    FileList fl;
    //scanDirectory(continentsPath, "*", fl);
    scanFilesInDirectory(continentsPath, fl);
    for (int fileIdx = 0; fileIdx < fl.size; ++fileIdx)
    {
        string fileName(fl.files[fileIdx]);
        NameToIDType::const_iterator contIter = continentNameToID->find(fileName);
        if (contIter == continentNameToID->end())
        {
            continue;
        }
        char contPath[256];
        sprintf(contPath, "%s%s", continentsPath, fl.files[fileIdx]);
        Continent* continent = new Continent;
        if (continent->init(contPath, contIter->second))
        {
            continents.insert(make_pair(fileName, continent));
        } else
        {
            delete continent;
        }
    }
    /*for (ContinentMapType::const_iterator contIter = continentIDToName->begin(); contIter != continentIDToName->end(); ++contIter)
    {
        string contPath(contIter->second);
        Continent* continent = new Continent;
        if (continent->init(contPath))
        {
            continents.insert(ContinentMapPair(fl.files[fileIdx], continent));
        } else
        {
            delete continent;
        }
    }*/
    return true;
}

void World::free() {
    for (ContinentMapType::iterator contIter = continents.begin(); contIter != continents.end(); ++contIter)
    {
        Continent * pCont = contIter->second;
        delete pCont;
    }
    continents.clear();
    continentIDToName->clear();
    delete continentIDToName;
    continentIDToName = NULL;
    continentNameToID->clear();
    delete continentNameToID;
    continentNameToID = NULL;
}

int32_t World::calculateRoute(WorldPosition const & startPos, WorldPosition const & endPos, bool canFly)
{
    Response response;
    // TODO: use additional data for continents where user cannot fly
    if (startPos.continentId == endPos.continentId)
    {
        IDToNameType::iterator idIter = continentIDToName->find(startPos.continentId);
        if (idIter != continentIDToName->end())
        {
            string contName = idIter->second;
            ContinentMapType::iterator contIter = continents.find(contName);
            if (contIter != continents.end())
            {
                Continent* continent = contIter->second;
                float startPosF[3] = {(float)startPos.position[0], (float)startPos.position[1], (float)startPos.position[2]};
                float endPosF[3] = {(float)endPos.position[0], (float)endPos.position[1], (float)endPos.position[2]};
                int32_t count = continent->calculateRoute(response, startPosF, endPosF, canFly);
                return count;
            } else
            {
                // incorrect continent name
                return -2;
            }
        } else
        {
            // incorrect continent ID
            return -1;
        }
    }
    // unsupported
    return 0;
}

//
// class Continent
//
Continent::Continent() : m_navMesh(0), m_navQuery(0)
{
    m_navQuery = dtAllocNavMeshQuery();
    m_continentId = -1;
}

Continent::~Continent() {
    free();
}

struct NavMeshSetHeader
{
    int magic;
    int version;
    int numTiles;
    dtNavMeshParams params;
};

struct NavMeshTileHeader
{
    dtTileRef tileRef;
    int dataSize;
};

static const int NAVMESHSET_MAGIC = 'M'<<24 | 'S'<<16 | 'E'<<8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

bool    Continent::init(const char *path, const int32_t continentId) {
    m_navMesh = dtAllocNavMesh();
    dtNavMeshParams *params = new dtNavMeshParams ();
    params->tileWidth = (533.0f + (1.0f / 3.0f)) / 4;
    params->tileHeight = (533.0f + (1.0f / 3.0f)) / 4;
    params->orig[0] = -(32 * (533.0f + (1.0f / 3.0f)));
    params->orig[1] = -(32 * (533.0f + (1.0f / 3.0f)));
    params->orig[2] = -(32 * (533.0f + (1.0f / 3.0f)));
    params->maxTiles =  16384;//16*840;
    params->maxPolys =  4096;//1048576/8;

    dtStatus status = m_navMesh->init(params);
    if (dtStatusFailed(status)){
        //m_ctx->dumpLog("fail...");
        delete params;
        return false;
    }
    delete params;
    FileList fl;
    scanDirectory(path, "mesh", fl);
    for (int fileIdx = 0; fileIdx < fl.size; ++fileIdx)
    {
        char buff[256];
        sprintf(buff, "%s/%s", path, fl.files[fileIdx]);
        addTile(buff);
    }
    status = m_navQuery->init(m_navMesh, 748983);
    printf("Query init status: 0x%X\n", status);
    if (dtStatusFailed(status))
    {
        //m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
        return false;
    }
    m_continentId = continentId;
    return true;
}

void    Continent::free() {
    dtFreeNavMesh(m_navMesh);
    dtFreeNavMeshQuery(m_navQuery);
}

bool Continent::addTile(const char *fn)
{
    FILE* fp = fopen(fn, "rb");
    if ( !fp ){
        return false;
    }
    // Read header.
    NavMeshSetHeader setHeader;
    fread(&setHeader, sizeof(NavMeshSetHeader), 1, fp);

    if (setHeader.magic != NAVMESHSET_MAGIC) {
        fclose(fp);
        return false;
    }
    if (setHeader.version != NAVMESHSET_VERSION) {
        fclose(fp);
        return false;
    }
    // Read tiles.
    for (int i = 0; i < setHeader.numTiles; ++i) {

        NavMeshTileHeader header;
        fread(&header, sizeof(header), 1, fp);

        if (!header.dataSize)
            break;

        unsigned char* data = (unsigned char*)dtAlloc(header.dataSize, DT_ALLOC_PERM);
        if (!data) break;
        memset(data, 0, (size_t)header.dataSize);
        fread(data, (size_t)header.dataSize, 1, fp);

        dtTileRef tileRef = 0;
        dtStatus dtResult = m_navMesh->addTile(data, header.dataSize, DT_TILE_FREE_DATA, tileRef, &tileRef);
        if ( dtResult != DT_SUCCESS ){

            printf(" res: 0x%X\n", dtResult);

            if (dtStatusDetail(dtResult, DT_OUT_OF_MEMORY))
            {
                //Log?
            }
            dtFree(data);
            return false;
        }
    }
    fclose(fp);
    return true;
}

int32_t Continent::calculateRoute(Response& response, const float *startPos, const float *endPos, const bool canFly)
{
    float startPosF[3] = {startPos[0], startPos[1], startPos[2]};
    float endPosF[3] = {endPos[0], endPos[1], endPos[2]};
    int result = RES_UNKNOWN;
    list<float*> *retListPtr = NULL;
    if (!canFly)
    {
        retListPtr = calculateRouteNonFly(startPosF, endPosF, &result);
    }
    if (retListPtr == NULL)
    {
        return -1;
    }
    for (list<float*>::iterator posIter = retListPtr->begin(); posIter != retListPtr->end(); ++posIter)
    {
        float *pos = *posIter;

        WorldPosition NewPos;
        NewPos.position[0] = pos[0];
        NewPos.position[1] = pos[1];
        NewPos.position[2] = pos[2];
        NewPos.continentId = m_continentId;

        response.route.push_back(NewPos);
    }
    int32_t posCount = retListPtr->size();
    delete retListPtr;
    if (result == RES_SUCCESS && posCount > 0)
    {
        response.result = Result::SUCCESS;
    }
    return posCount;
}

// passing the opp coords
list<float*> *Continent::calculateRouteNonFly(float start[3], float end[3], int *result_code) {

    Coords StartCoords, EndCoords;
    StartCoords.SetOPPCoords(start[0], start[1], start[2]);
    EndCoords.SetOPPCoords(end[0], end[1], end[2]);

    float NavMeshOri[3];
    NavMeshOri[0] = -(32 * GRID_SIZE);
    NavMeshOri[1] = -(32 * GRID_SIZE);
    NavMeshOri[2] = -(32 * GRID_SIZE);
    int startTileY = (int)(((-(StartCoords.oppX())) - (-(32 * GRID_SIZE))) / GRID_SIZE);
    int startTileX = (int)(((-(StartCoords.oppY())) - (-(32 * GRID_SIZE))) / GRID_SIZE);
    int endTileY = (int)(((-(EndCoords.oppX())) - (-(32 * GRID_SIZE))) / GRID_SIZE);
    int endTileX = (int)(((-(EndCoords.oppY())) - (-(32 * GRID_SIZE))) / GRID_SIZE);

    printf( "Start tile: (%d, %d)\n", startTileX, startTileY);
    printf( "End tile: (%d, %d)\n", endTileX, endTileY);

    int x1, x2, y1, y2;
    if ( startTileX <= endTileX ){
        x1 = startTileX;
        x2 = endTileX;
    }
    else{
        x1 = endTileX;
        x2 = startTileX;
    }
    if ( startTileY <= endTileY ){
        y1 = startTileY;
        y2 = endTileY;
    }
    else{
        y1 = endTileY;
        y2 = startTileY;
    }

    // *** START ROUTE FROM

    //  From MaiN
    //  MaxPolys = 4096,
    //  MaxTiles = 16384,
    static const int MAX_POLYS = 4096;

    //mesh querier
    class dtNavMeshQuery* m_navQuery;
    m_navQuery = dtAllocNavMeshQuery();

    dtQueryFilter m_filter = dtQueryFilter();
    m_filter.setIncludeFlags(0xffff);
    m_filter.setExcludeFlags(0);
    m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f) ;
    //m_filter.setAreaCost(SAMPLE_POLYAREA_WATER, 100.0f) ;
    //m_filter.setAreaCost(SAMPLE_POLYFLAGS_SWIM, 100.0f) ;

    //poly refs required for route calculation
    dtPolyRef m_startRef = NULL;
    dtPolyRef m_endRef = NULL;

    //starting and ending position xyz ?
    float m_spos[3];
    float m_epos[3];

    m_spos[0] = StartCoords.adtX();
    m_spos[1] = StartCoords.adtY();
    m_spos[2] = StartCoords.adtZ();
    m_epos[0] = EndCoords.adtX();
    m_epos[1] = EndCoords.adtY();
    m_epos[2] = EndCoords.adtZ();

    printf("Going from {%0.2f, %0.2f, %0.2f} to {%0.2f, %0.2f, %0.2f}\n", m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2]);
    //number of points in our straight path?
    int m_nstraightPath = 0;
    //final path
    float m_straightPath[MAX_POLYS*3];

    //no fucking clue
    float m_polyPickExt[3];
    m_polyPickExt[0] = 2;
    m_polyPickExt[1] = 4;
    m_polyPickExt[2] = 2;

    //swim, jump, door? not really applicable
    unsigned char m_straightPathFlags[MAX_POLYS];

    //the navmesh polys that are used for the path?
    dtPolyRef m_straightPathPolys[MAX_POLYS];

    dtPolyRef m_polys[MAX_POLYS];

    //get the nav mesh and init a query object
    dtStatus status = m_navQuery->init(m_navMesh, 748983);        // 20 MB

    // set the mesh directory
    //m_navQuery->setMeshPath(_meshPath);

    //printf("0x%X 0x%X 0x%X\n", DT_FAILURE, DT_SUCCESS, DT_IN_PROGRESS);
    //0x80000000 0x40000000 0x20000000
    //printf("0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n", DT_WRONG_MAGIC, DT_WRONG_VERSION, DT_OUT_OF_MEMORY, DT_INVALID_PARAM, DT_BUFFER_TOO_SMALL, DT_OUT_OF_NODES, DT_PARTIAL_RESULT);
    // 0x1 0x2 0x4 0x8 0x10 0x20 0x40

    if ( dtStatusFailed(status) ){
        *result_code = Result::UNKNOWN;
        return NULL;
    }

    dtStatus stat = m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
    dtStatus stat2 = m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

    printf("Start poly: 0x%X 0x%X\n", m_startRef, stat);
    printf("End poly: 0x%X 0x%X\n", m_endRef, stat2);

    if ( m_startRef == 0 ){
        *result_code = Result::START_POLY_NOT_FOUND;
        return NULL;
    }
    else if ( m_endRef == 0 ){
        *result_code = Result::END_POLY_NOT_FOUND;
        return NULL;
    }

    status = m_navQuery->initSlicedFindPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter);
    if ( dtStatusFailed(status) ){
        *result_code = Result::SLICE_INIT_FAILED;
        return NULL;
    }

    int doneIters = 0;
    while ( 1 ){

        status = m_navQuery->updateSlicedFindPath( 10000, &doneIters);

        if ( dtStatusFailed(status) ){
            *result_code = Result::UPDATE_FAILED;
            return NULL;
        }
        else if ( dtStatusSucceed(status) ){
            break;
        }
    }

    // dtStatus finalizeSlicedFindPath(dtPolyRef* path, int* pathCount, const int maxPath);
    int m_npolys = 0;
    status = m_navQuery->finalizeSlicedFindPath(m_polys, &m_npolys, 10000);

    if ( dtStatusFailed(status) ){
        *result_code = Result::FINAZLIZED_FAILED;
        return NULL;
    }

    if ( m_npolys ){
        status = m_navQuery->findStraightPath(m_spos, m_epos, m_polys, m_npolys, m_straightPath, m_straightPathFlags, m_straightPathPolys, &m_nstraightPath, MAX_POLYS);
        if ( dtStatusFailed(status) ){
            *result_code = Result::FIND_STRAIGHT_FAILED;
            return NULL;
        }
    }

    if ( m_nstraightPath < 2 ){
        *result_code = Result::PARTIAL_ROUTE;
        return NULL;
    }

    // our list of waypoints!
    list<float*> *positions = new list<float*>();

    // build an array!
    for (int i = 0; i < m_nstraightPath-1; ++i) {

        // OPP coordinates
        float *pos = new float[3];
        pos[0] = -1*m_straightPath[i*3+2];
        pos[1] = -1*m_straightPath[i*3];
        pos[2] = m_straightPath[i*3+1];

        //printf("{%0.2f, %0.2f, %0.2f}\n", -1*m_straightPath[i*3+2], -1*m_straightPath[i*3], m_straightPath[i*3+1] );
        positions->push_back(pos);
    }

    // Add the final one!
    float *pos = new float[3];
    pos[0] = end[0];
    pos[1] = end[1];
    pos[2] = end[2];
    positions->push_back(pos);

    *result_code = Result::SUCCESS;
    return positions;
}

// CalculateRoute

bool	CalculateRoute(const double *startPos, const int32_t startContinent, const double *endPos, const int32_t endContinent,
        const bool canFly, const int32_t faction, const int32_t randomVariance, const BlackListObject *blackListedPositions,
        const int32_t blackListedPosCount, World *world)
{
    if (!canFly)
    {
        // return 2D recast+detour path

    } else
    {
        //!!!
    }
    return false;
}
