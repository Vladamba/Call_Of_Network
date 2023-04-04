#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "Animation.hpp"
#include "Level.hpp"

using namespace sf;

class Entity
{
public:
	FloatRect rect;
	float dx, dy, timer, timerEnd;
	AnimationManager animationManager;
	std::vector<Object> objects;
	bool isAlive, left;
	int health;

	Entity(AnimationManager a, Vector2f vec, int _health)
	{		
		animationManager = a;

		rect.left = vec.x;
		rect.top = vec.y;
		rect.width = animationManager.getWidth();
		rect.height = animationManager.getHeight();

		health = _health;

		dx = dy = timer = timerEnd = 0;

		isAlive = true;
	}

	virtual void update(float time){};

	void draw(RenderWindow& window)
	{
		animationManager.draw(window, rect.left, rect.top + rect.height);
	}
};

#endif ENTITY_H