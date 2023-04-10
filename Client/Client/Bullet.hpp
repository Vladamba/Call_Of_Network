#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"

class Bullet : public Entity
{
private:
	const float move = 0.5;

public:

	Bullet(AnimationManager a, Vector2f vec, int _health, bool _left) :
		Entity(a, vec, _health)
	{
		animationManager.set(AnimationType::Move);
		rect.width = (float)animationManager.getWidth();
		rect.height = (float)animationManager.getHeight();
		animationManager.loop(AnimationType::Move, false);
		animationManager.loop(AnimationType::Explode, false);
						
		left = _left;		
		if (left)
		{
			dx = -move;
		}
		else
		{
			dx = move;
		}		
	}

	Vector2i update(signed __int32 _time, Level level, std::vector<FloatRect> playerRects)
	{
		float time = (float)_time;
		rect.left += dx * time;

		Vector2i vec(0, 0);
		if (health > 0)
		{
			for (int i = rect.top / level.tileHeight; i < (rect.top + rect.height) / level.tileHeight; i++)
			{
				for (int j = rect.left / level.tileWidth; j < (rect.left + rect.width) / level.tileWidth; j++)
				{
					if (i < level.mapHeight && i >= 0 && j < level.mapWidth && j >= 0)
					{
						if (level.objects[i][j] == ObjectType::Solid)
						{
							health = 0;
							goto collisionHappened;
						}
					}
					else
					{
						//When the bullet is out of the map, it is better just to destroy it
						health = 0;
						goto collisionHappened;
					}
				}
			}

			for (int i = 0; i < playerRects.size(); i++)
			{
				if (rect.intersects(playerRects[i]))
				{
					vec.x = health;
					vec.y = i;
					health = 0;
					break;
				}
			}
		}

		collisionHappened:

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
		return vec;
	}
};

#endif BULLET_H
