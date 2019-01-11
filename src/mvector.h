#pragma once

#include "math.h"
#include <stdio.h>

#define mpi 3.14159265358979323846264338327950288	//	PI 
#define mpi2 1.57079632679489661923132169163975144	//	PI / 2

typedef struct mpoint2d {
	double x;
	double y;
};

class mvector2d {
	double x;
	double y;
public:
	
	mvector2d() {};

	mvector2d(mpoint2d &P, mpoint2d &Q) { x = Q.x - P.x; y = Q.y - P.y; }
	
	mvector2d(double x, double y) { this->x = x; this->y = y; }

	double getX() const { return this->x; }
	double getY() const { return this->y; }

	void print() {
		printf("( %f , %f )\n", x, y);
	}

	double magnitude() {
		double mag = pow(x, 2) + pow(y, 2);
		return sqrt(mag);
	}

	//OPERATORS

	void operator += (mvector2d &v1) {
		x += v1.x;
		y += v1.y;
	}
	
	mvector2d operator + (mvector2d &v1) {
		mvector2d nv(x + v1.x, y + v1.y);
		return nv;
	}

	void operator -= (mvector2d &v1) {
			x -= v1.x;
		y -= v1.y;
	}
	
	mvector2d operator - (mvector2d &v1) {
		mvector2d nv(x - v1.x, y - v1.y);
		return nv;
	}
	
	void operator *= (double &scalar) {
			x *= scalar;
		y *= scalar;
	}

	mvector2d operator * (double &scalar) {
				mvector2d nv(x * scalar, y * scalar);
		return nv;
	}

	void operator /= (double &scalar) {
		x /= scalar;
		y /= scalar;
	}

	mvector2d operator / (double &scalar) {
		mvector2d nv(x / scalar, y / scalar);
		return nv;
	}

	double dot_product (mvector2d &v1) {
		return (x * v1.x + y * v1.y);
	}

	double angle() {
		double mag = magnitude();
		double radians = acos(x / mag);
		double degrees = (radians / mpi) * 180;

		if (y < 0.0)
		degrees *= -1;

		return degrees;
	}

	double angle(mvector2d &v1) {
		double radians = acos(dot_product(v1) / (magnitude() * v1.magnitude()));
		double degrees = (radians / mpi) * 180;
		return degrees;
	}

	void rotate(double degrees) {
		double radians = degrees * mpi / 180;
		double angle_cos = cos(radians);
		double angle_sin = sin(radians);

		double nx = x * angle_cos - y * angle_sin;
		double ny = x * angle_sin + y * angle_cos;

		x = nx;
		y = ny;
	}

};
