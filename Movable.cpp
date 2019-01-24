#include "Movable.h"



Movable::Movable() : radius(0.f), velocity(0.f), acceleration(0.f)
{
}



void Movable::setRadius(float r)
{
	radius = r;
}

void Movable::setVelocity(float v)
{
	velocity = v;
}

void Movable::setAcceleration(float a)
{
	acceleration = a;
}



float Movable::getRadius()
{
	return radius;
}

float Movable::getVelocity()
{
	return velocity;
}

float Movable::getAcceleration()
{
	return acceleration;
}



Movable::~Movable()
{
}