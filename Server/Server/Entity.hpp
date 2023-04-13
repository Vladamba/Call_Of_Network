#ifndef ENTITY_H
#define ENTITY_H

#include <math.h>
#include "Consts.hpp"
#include <SFML/Graphics.hpp>
#include "Level.hpp"

using namespace sf;

class Entity
{
public:
	FloatRect rect;
	float dx, dy;	
	bool isAlive, left;
	int health;

	Entity()
	{
	}

	Entity(Vector2f vec, int _health)
	{
		rect.left = vec.x;
		rect.top = vec.y;
		health = _health;
	}
};

#endif ENTITY_H