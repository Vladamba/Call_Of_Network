#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "Consts.hpp"
#include "Animation.hpp"
#include "Level.hpp"

using namespace sf;

class Entity
{
public:
	FloatRect rect;
	AnimationManager animationManager;	
	bool isAlive, left;

	Entity()
	{
	}

	Entity(AnimationManager a)
	{		
		rect.left = 0;
		rect.top = 100;
		animationManager = a;		
		isAlive = true;
		left = false;
	}	

	void draw(RenderWindow& window)
	{
		animationManager.draw(window, rect.left, rect.top);
	}
};

#endif ENTITY_H