#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Movable
{
protected:
	float radius;

	float velocity;
	float acceleration;

public:
	Movable();


	void setRadius(float r);

	void setVelocity(float v);
	void setAcceleration(float a);


	float getRadius();

	float getVelocity();
	float getAcceleration();


	virtual ~Movable();
};