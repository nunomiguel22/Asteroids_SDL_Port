#pragma once
#include "mvector.h"

/* Lasers */

class weapon {
	mpoint2d position;
	mvector2d force;
	float travel_angle;
	bool actv;
public:

	mpoint2d getposition();
	mvector2d getforce();
	bool active();
	float get_travel_angle();

	void setposition(mpoint2d pos);
	void setforce(mvector2d force);
	void setstatus(bool act);
	void set_travel_angle(float angle);

	int checkbounds();
	void updateposition();
};