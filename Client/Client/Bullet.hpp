#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"

class Bullet : public Entity
{
public:

	Bullet(AnimationManager a, Level level, Vector2f vec, int _health, bool _left) :
		Entity(a, vec, _health)
	{
		animationManager.set(AnimationType::Move);
		animationManager.loop(AnimationType::Move, false);
		animationManager.loop(AnimationType::Explode, false);
				
		left = _left;		
		if (left)
		{
			dx = -0.2f;
		}
		else
		{
			dx = 0.2f;
		}
		objects = level.getAllObjects();
	}

	void update(float time)
	{
		rect.left += dx * time;

		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i].name == "solid")
			{
				if (rect.intersects(objects[i].rect))
				{
					health = 0;
				}
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
			animationManager.set(AnimationType::Explode);
			dx = 0;

		}

		animationManager.update(time, left);
	}
};

#endif BULLET_H
