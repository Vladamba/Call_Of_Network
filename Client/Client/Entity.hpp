#ifndef ENTITY_H
#define ENTITY_H

#include <math.h>
#include <SFML/Graphics.hpp>
#include "Animation.hpp"
#include "Level.hpp"

using namespace sf;

class Entity
{
public:
	FloatRect rect;
	float dx, dy;
	AnimationManager animationManager;	
	bool isAlive, left;
	int health;

	Entity(AnimationManager a, Vector2f vec, int _health)
	{		
		animationManager = a;		

		rect.left = vec.x;
		rect.top = vec.y;
		health = _health;
		dx = 0;
		dy = 0;
		isAlive = true;
	}	

	void draw(RenderWindow& window)
	{
		//animationManager.draw(window, rect.left, rect.top + rect.height);
		animationManager.draw(window, rect.left, rect.top);
	}
};

#endif ENTITY_H