namespace cpp pathing

enum Result{
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
	INVALID_CONTINENTS = 10,
}

struct Position
{
	1:double x,
	2:double y,
	3:double z,
	4:i32    continent,
	5:bool   isInAir;
}

struct BlacklistedArea
{
	1:Position position,
	2:double radius,
}

struct RouteParameters
{
	1:bool canFly,
	2:i32 faction,
	3:i32 randomVariance,
	4:list<BlacklistedArea> blacklistedPositions,
}

struct Response
{
	1:Result code,
	2:list<Position> route,
}

service Pathing
{
	Response GetRoute(1:Position A, 2:Position B, 3:RouteParameters params),
}