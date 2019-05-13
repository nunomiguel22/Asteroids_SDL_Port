#pragma once

#include "math.h"
#include <stdio.h>
#include <stdint.h>

const float mpi = 3.14159265358979323846264338327950288f;	//	PI 
const float mpi2 = 1.57079632679489661923132169163975144f;	//	PI / 2

typedef struct {
	float x;
	float y;
} mpoint2d;

class mvector2d {
	float x;
	float y;
public:
	
	mvector2d() {};

	mvector2d(mpoint2d &P, mpoint2d &Q) { x = Q.x - P.x; y = Q.y - P.y; }
	
	mvector2d(float x, float y) { this->x = x; this->y = y; }

	float getX() const { return this->x; }
	float getY() const { return this->y; }

	void setX(float x) { this->x = x; };
	void setY(float y) { this->y = y; };

	void print() {
		printf("( %f , %f )\n", x, y);
	}

	float magnitude() {
		float mag = (float)(pow(x, 2) + pow(y, 2));
		return (float)sqrt(mag);
	}

	float rsqrt() {

		float mag = (float)(pow(x, 2) + pow(y, 2));

		const float threehalfs = 1.5F;
		const float x2 = mag * 0.5F;

		float res = mag;
		uint32_t& i = *reinterpret_cast<uint32_t *>(&res);    // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);                             // what the fuck?
		res = res * (threehalfs - (x2 * res * res));

		return res;
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
	
	void operator *= (float scalar) {
			x *= scalar;
		y *= scalar;
	}

	mvector2d operator * (float &scalar) {
				mvector2d nv(x * scalar, y * scalar);
		return nv;
	}

	void operator /= (float &scalar) {
		x /= scalar;
		y /= scalar;
	}

	mvector2d operator / (float &scalar) {
		mvector2d nv(x / scalar, y / scalar);
		return nv;
	}

	float dot_product (mvector2d &v1) {
		return (x * v1.x + y * v1.y);
	}

	float angle() {
		float mag = magnitude();
		float radians = (float)(acos(x / mag));
		float degrees = (radians / mpi) * 180;

		if (y < 0.0)
		degrees *= -1;

		return degrees;
	}

	float angle(mvector2d &v1) {
		float radians =(float) (acos(dot_product(v1) / (magnitude() * v1.magnitude())));
		float degrees = (radians / mpi) * 180;
		return degrees;
	}

	void rotate(float degrees) {
		float radians = degrees * mpi / 180;
		float angle_cos = (float)cos(radians);
		float angle_sin = (float)sin(radians);

		float nx = x * angle_cos - y * angle_sin;
		float ny = x * angle_sin + y * angle_cos;

		x = nx;
		y = ny;
	}

	void limit(float limit) {

		float magn = this->magnitude();
		if (magn > limit)
			*this *= (limit / magn);
	}

	mvector2d versor() {

		float inverse_sqrt = rsqrt();
		
		mvector2d versor = *this;
		versor *= inverse_sqrt;
		return versor;
	}
};
