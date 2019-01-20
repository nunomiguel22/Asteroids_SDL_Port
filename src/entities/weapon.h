#pragma once
#include "mvector.h"

/* Lasers */

class weapon {
	mpoint2d position;
	mvector2d force;
	bool actv;
public:

	mpoint2d getposition();
	mvector2d getforce();
	bool active();

	void setposition(mpoint2d pos);
	void setforce(mvector2d force);
	void setstatus(bool act);

	int checkbounds();
	void updateposition();
};