#include "Coords.h"

#include <string>
#include <sstream>

#include <math.h>

using namespace std;

Coords::Coords() {
	_x = 0.0f;
	_y = 0.0f;
	_z = 0.0f;
}

void Coords::SetOPPCoords(float x, float y, float z){
	_x = x;
	_y = y;
	_z = z;
}

void Coords::SetADTCoords(float x, float y, float z){
	_x = -z;
	_y = -x;
	_z = y;
}

float Coords::oppX(){
	return _x;
}

float Coords::oppY(){
	return _y;
}

float Coords::oppZ(){
	return _z;
}

float Coords::adtX(){
	return -_y;
}

float Coords::adtY(){
	return _z;
}

float Coords::adtZ(){
	return -_x;
}

std::string Coords::tile(){
	std::stringstream Stream;
	Stream << floor(32.0-(_x/533.33333));
	Stream << "_";
	Stream << floor(32.0-(_y/533.33333));
	return Stream.str();
}

Coords::~Coords() {
	// TODO Auto-generated destructor stub
}
