#ifndef BULLET_H
#define BULLET_H

class Bullet
{
public:
	float vMove = 0.5f;

	FloatRect rect;
	float dx;
	bool isAlive, left, team;
	int health;

	Bullet(Vector2f vec, int _health, bool _left, bool _team)
	{		
		rect.width = BULLET_WIDTH;
		rect.height = BULLET_HEIGHT;

		newBullet(vec, _health, _left, _team);
		isAlive = false;
	}

	void newBullet(Vector2f vec, int _health, bool _left, bool _team)
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
		team = _team;
	}

	Vector2i update(signed __int32 _time, Level level, Vector2f *playersCoord)
	{
		float time = (float)_time;
		rect.left += dx * time;

		Vector2i vec = NULL_VECTOR2I;
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
					return vec;
				}
			}
		}
		
		return vec;
	}

	void createPacket(Packet* packet)
	{
		*packet << rect.left;
		*packet << rect.top;
		*packet << left;
	}
};

#endif BULLET_H