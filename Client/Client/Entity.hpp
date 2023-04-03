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

	Entity(const char* image, const char* file, Vector2f vec, float speed, int _health)
	{
		animationManager = AnimationManager(image, file);		

		rect.left = vec.x;
		rect.top = vec.y;
		rect.width = animationManager.getWidth();
		rect.height = animationManager.getHeight();

		dx = speed;
		health = _health;

		dy = timer = timerEnd = 0;

		isAlive = true;
		left = false;
	}

	virtual void update(float time){};

	void draw(RenderWindow &window)
	{
		animationManager.draw(window, rect.left, rect.top);
	}
};

#endif ENTITY_H