#pragma once
#include "mvector.h"

/* Lasers */

class weapon {
private:

	mpoint2d position;
	mvector2d force;
	float travel_angle;
	bool actv;

public:
	weapon();
	~weapon();

	bool is_active() const;
	mpoint2d get_position() const; 
	float get_angle() const;

	void activate(mpoint2d position, mvector2d force, float angle);
	void deactivate();

	int update();
};