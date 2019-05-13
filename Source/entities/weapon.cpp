#include "weapon.h"
#include "../general/macros.h"

weapon::weapon(){}

weapon::~weapon(){}


bool weapon::is_active() const { return actv; }
mpoint2d weapon::get_position() const { return position; }
float weapon::get_angle() const { return travel_angle; }

void weapon::activate(mpoint2d position, mvector2d force, float angle) {
	
	actv = true;
	
	this->position = position;
	this->force = force;
	travel_angle = angle;
}

void weapon::deactivate() { actv = false; }


int weapon::update() {
	position.x += force.getX();
	position.y += force.getY();

	if (position.x >= math_h_positive_bound || position.x <= math_h_negative_bound) {
		actv = false;
		return 1;
	}
	if (position.y >= math_v_positive_bound || position.y <= math_v_negative_bound) {
		actv = false;
		return 1;
	}
	return 0;
}


