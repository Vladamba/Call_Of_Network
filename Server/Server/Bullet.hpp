#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"

class Bullet : public Entity
{
public:
	float vMove = 0.5;

	Bullet(Vector2f vec, int _health, bool _left) :
		Entity(vec, _health)
	{		
		rect.width = 6.f;
		rect.height = 6.f;

		left = _left;
		if (left)
		{
			dx = -vMove;
		}
		else
		{
			dx = vMove;
		}
		isAlive = false;
	}

	Vector2i update(signed __int32 _time, Level level, Vector2f *playersCoord)
	{
		float time = (float)_time;
		rect.left += dx * time;

		Vector2i vec = NULL_VECTOR2I;
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
							isAlive = false;
							return vec;
						}
					}
					else
					{
						//When the bullet is out of the map, it is better just to destroy it
						isAlive = false;
						return vec;
					}
				}
			}

			for (int i = 0; i < CLIENTS_SIZE; i++)
			{
				if (playersCoord[i] != NULL_VECTOR2f)
				{
					if (rect.left + rect.width >= playersCoord[i].x && rect.left <= playersCoord[i].x + PLAYER_WIDTH &&
						rect.top + rect.height >= playersCoord[i].y && rect.top <= playersCoord[i].y + PLAYER_HEIGHT)
					{
						vec.x = health;
						vec.y = i;
						isAlive = false;
						return vec;
					}
				}
			}
		}
		return vec;
	}

	void newBullet(Vector2f vec, int _health, bool _left)
	{
		rect.left = vec.x;
		rect.top = vec.y;
		health = _health;

		left = _left;
		if (left)
		{
			dx = -vMove;
		}
		else
		{
			dx = vMove;
		}
		isAlive = true;
	}

	void createPacket(Packet* packet)
	{
		*packet << rect.left;
		*packet << rect.top;
		*packet << left;
	}
};

#endif BULLET_H
