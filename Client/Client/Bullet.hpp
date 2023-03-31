#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"


class Bullet : public Entity
{
public:

	Bullet(AnimationManager& a, Level& lev, int x, int y, bool dir) :Entity(a, x, y)
	{
		option("Bullet", 0.3, 10, "move");

		if (dir)
		{
			dx = -0.3;
		}
		objects = lev.getObjects("solid");
	}

	void update(float time)
	{
		x += dx * time;

		for (int i = 0; i < objects.size(); i++)
		{
			if (getRect().intersects(objects[i].rect))
			{
				health = 0;
			}
		}

		if (health <= 0) {
			animationManager.set("explode"); 
			dx = 0;
			if (animationManager.isPlaying() == false)
			{
				isAlive = false;
			}
		}

		animationManager.update(time);
	}

};

#endif BULLET_H
