#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Consts.hpp"
#include "Level.hpp"

using namespace sf;

class Entity
{
public:
	FloatRect rect;
	float dx;	
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