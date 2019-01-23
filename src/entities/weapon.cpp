#include "weapon.h"
#include "macros.h"


mpoint2d weapon::getposition() { return position; }
mvector2d  weapon::getforce() { return force; }
bool  weapon::active() { return actv; }

void  weapon::setposition(mpoint2d pos) { position = pos; }
void  weapon::setforce(mvector2d force) { this->force = force; }
void  weapon::setstatus(bool act) { actv = act; }

int weapon::checkbounds() {
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

void weapon::updateposition() {
	position.x += force.getX();
	position.y += force.getY();
}

float  weapon::get_travel_angle() { return travel_angle; }
void  weapon::set_travel_angle(float angle) { travel_angle = angle; }