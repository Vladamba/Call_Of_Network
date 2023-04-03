#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"


class Bullet : public Entity
{
public:

	Bullet(AnimationManager& a, Level level, int _health) :
		Entity(a, level.getObjectVector("bullet"), 0, _health)
	{
		animationManager.currentAnimation = AnimationType::Move;
		objects = level.getAllObjects();
	}

	void update(float time)
	{
		rect.left += dx * time;

		for (int i = 0; i < objects.size(); i++)
		{
			if (rect.intersects(objects[i].rect))
			{
				health = 0;
			}
		}

		if (dx == 0)
		{
			if (animationManager.isPlaying() == false)
			{
				isAlive = false;
			}
		}

		if (health == 0) {
			animationManager.currentAnimation = AnimationType::Explode;
			animationManager.loop(false);
			dx = 0;

		}

		animationManager.update(time);
	}
};

#endif BULLET_H
