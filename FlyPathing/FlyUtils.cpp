#include "FlyUtils.h"

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

FlyUtils::FlyUtils()
{
    //
}

FlyUtils::~FlyUtils()
{
    //
}

bool    FlyUtils::connect(const float *startPos, const int startCont, const float *endPos, const int endCont, int& responseCode)
{
    using namespace apache::thrift;
    
    bool res = false;
    boost::shared_ptr<transport::TSocket> socket(new transport::TSocket("localhost", 9000));
    boost::shared_ptr<transport::TFramedTransport> transport(new transport::TFramedTransport(socket));
    boost::shared_ptr<protocol::TBinaryProtocol> protocol(new protocol::TBinaryProtocol(transport));
    pathing::PathingClient pathingClient(protocol);
    try {
        transport->open();
        pathing::Position A, B;
        A.x = startPos[0];//-5038.887;
        A.y = startPos[1];//-810.3184;
        A.z = startPos[2];// 495.1288;
        A.continent = startCont;//0;
        A.isInAir = false;// start from the surface
        B.x = endPos[0];//-8998.929;
        B.y = endPos[1];//482.6484;
        B.z = endPos[2];//96.62222;
        B.continent = endCont;//0;
        B.isInAir = false;// end on the surface
        
        pathing::RouteParameters routeParams;
        routeParams.canFly = canFly;
        routeParams.faction = factionId;
        routeParams.randomVariance = randomVariance;
        routeParams.blacklistedPositions = blacklistedAreas;
        
        pathing::Response response;
        
        pathingClient.GetRoute(response, A, B, routeParams);
        responseCode = response.code;
        printf("Pathing response code: %d\n" , response.code);
        size_t pathSize = response.route.size();
        printf("Route size: %zd\n", pathSize);
        route.clear();
        for (std::vector<pathing::Position>::iterator iter = response.route.begin(); iter != response.route.end(); ++iter)
        {
            pathing::Position pos = *iter;
            printf("point: [%5.7f, %5.7f, %5.7f], continent %d, inAir: %d\n", pos.x, pos.y, pos.z, pos.continent, (int)pos.isInAir);
            route.push_back(pos);
        }
        
        transport->close();
        res = true;
    } catch (TException &tx) {
        printf("TRANSPORT ERROR: %s\n", tx.what());
    }
    return res;
}

void FlyUtils::addBlacklistedArea(const pathing::Position &position, float radius)
{
    pathing::BlacklistedArea area;
    area.position = position;
    area.radius = radius;
    blacklistedAreas.push_back(area);
}

