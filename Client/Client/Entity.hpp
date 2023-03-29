#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include "Animation.hpp"
#include "Level.hpp"
#include <vector>

using namespace sf;

class Entity
{
public:
	float x, y, dx, dy, w, h, timer, timerEnd;
	AnimationManager animationManager;
	std::vector<Object> objects;
	bool isAlive, dir;
	const char* name;
	int health;

	Entity(AnimationManager &a, int _x, int _y)
	{
		animationManager = a;
		x = _x;
		y = _y;		

		isAlive = true;
		dir = false;

		dx = dy = timer = timerEnd = 0;
	}

	virtual void update(float time) = 0;

	void draw(RenderWindow& window)
	{
		animationManager.draw(window, x, y + h);
	}

	FloatRect getRect()
	{
		return FloatRect(x, y, w, h);
	}

	void option(const char* _name, float speed, int _health, const char* animation = "")
	{
		name = _name;
		dx = speed;
		health = _health;

		if (animation != "")
		{
			animationManager.set(animation);
		}
		w = animationManager.getW();
		h = animationManager.getH();
	}
};

#endif ENTITY_H