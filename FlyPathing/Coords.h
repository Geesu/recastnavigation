#ifndef COORDS_H_
#define COORDS_H_

#include <string>

class Coords {
private:
	// stored as opp coords
	float _x, _y, _z;

public:
	float oppX();
	float oppY();
	float oppZ();
	float adtX();
	float adtY();
	float adtZ();

	std::string tile();

	Coords();
	void SetOPPCoords(float x, float y, float z);
	void SetADTCoords(float x, float y, float z);
	virtual ~Coords();
};

#endif /* COORDS_H_ */
