#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"

class Bullet : public Entity
{
public:

	Bullet(AnimationManager a, Vector2f vec, int _health, bool _left) :
		Entity(a, vec, _health)
	{
		animationManager.set(AnimationType::Move);
		rect.width = animationManager.getWidth();
		rect.height = animationManager.getHeight();
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
	}

	Vector2i update(float time, Level level, std::vector<FloatRect> playerRects)
	{
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
						// choto sdelat nado
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
