#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "Consts.hpp"
#include "Level.hpp"
#include <math.h>

class Player
{
public:
	int sHeight, sWidth;
	float g, vJump, vLadderJump, vRun, vClimb;
	float tShoot = 200.f;
	float tDead = 1000.f;

	enum Key { Left, Right, Up, Down, Space };
	std::map<Key, bool> keys;
	unsigned char state;

	FloatRect rect;
	float dx, dy;
	float shootTimer, deadTimer;
	bool left, isAlive, onGround, onLadder, shoot, respawn;
	int health;

	Player(){}

	Player(Level level, int _health)
	{
		sWidth = level.tileWidth;
		sHeight = level.tileHeight;
		g = 2.f * (float)sHeight / (200.f * 200.f); // 0.0016
		vJump = sqrt(2.f * g * 2.5f * (float)sHeight); // 0.505964
		vLadderJump = sqrt(2.f * g * 1.5f * (float)sHeight); // 0.391918
		vRun = g * 2.5f * (float)sWidth / vJump; // 0.252982
		vClimb = vRun / 2.f; // 0.126491

		rect.width = PLAYER_WIDTH;
		rect.height = PLAYER_HEIGHT;

		newPlayer(level, true, _health);
	}

	void newPlayer(Level level, bool team, int _health)
	{
		Vector2f vec;
		if (team)
		{
			vec = level.getObjectCoord(ObjectType::PlayerSpawner);
		}
		else
		{
			vec = level.getObjectCoord(ObjectType::PlayerSpawner);
		}
		rect.left = vec.x;
		rect.top = vec.y;

		state = STATE_STAND;
		health = _health;
		left = false;

		dx = 0;
		dy = 0;
		onGround = false;
		onLadder = false;

		shootTimer = 0;
		shoot = false;

		isAlive = true;
		deadTimer = 0;
		respawn = false;
		//hit = false;
	}

	void updateKeys()
	{
		if (keys[Key::Left])
		{
			left = true;
			if (state == STATE_CLIMB)
			{
				dx = -vClimb;
			}
			else
			{
				dx = -vRun;
			}

			if (state == STATE_STAND)
			{
				state = STATE_RUN;
			}
		}
		else
		{
			if (keys[Key::Right])
			{
				left = false;
				if (state == STATE_CLIMB)
				{
					dx = vClimb;
				}
				else
				{
					dx = vRun;
				}

				if (state == STATE_STAND)
				{
					state = STATE_RUN;
				}
			}
			else
			{
				dx = 0;
			}
		}


		if (keys[Key::Up])
		{
			if (onLadder)
			{
				dy = -vClimb;
				state = STATE_CLIMB;
			}
			else
			{
				if (onGround)
				{
					if (state == STATE_STAND || state == STATE_RUN)
					{
						dy = -vJump;
						state = STATE_JUMP;
					}
				}
				/*if (state == STATE_CRAWL)
				{
					state = STATE_STAND;
				}*/
			}
		}
		else
		{
			if (keys[Key::Down])
			{
				if (onLadder)
				{
					dy = vClimb;
					state = STATE_CLIMB;
				}
				/*else
				{
					if (state == STATE_STAND || state == STATE_RUN)
					{
						state = STATE_CRAWL;
					}
				}*/
			}
			else
			{
				if (state == STATE_CLIMB)
				{
					dy = 0;
				}
			}
		}

		if (keys[Key::Space] && state != STATE_CLIMB)
		{
			shoot = true;
		}
		else
		{
			shoot = false;
		}
		//keys[Key::Left] = keys[Key::Right] = keys[Key::Up] = keys[Key::Down] = keys[Key::Space] = false;
	}

	void update(signed __int32 _time, Level level)
	{
		float time = (float)_time;

		if (isAlive)
		{
			updateKeys();

			onGround = false;
			onLadder = false;

			if (dx != 0)
			{
				rect.left += dx * time;
				collision(true, time, level);
			}

			if (state != STATE_CLIMB)
			{
				dy += g * time;
				rect.top += dy * time + g * time * time / 2.f;
			}
			else
			{
				rect.top += dy * time;
			}
			collision(false, time, level);

			if (state == STATE_CLIMB && !onLadder)
			{
				dy = -vLadderJump;
				state = STATE_JUMP;
			}

			//if (onGround && state != STATE_CRAWL)
			if (onGround)
			{
				if (dx == 0)
				{
					state = STATE_STAND;
				}
				else
				{
					state = STATE_RUN;
				}
			}

			shootTimer += time;
			if (shoot)
			{
				if (shootTimer >= tShoot)
				{
					shootTimer = 0;
					shoot = true;
				}
				else
				{
					shoot = false;
				}
			}
			else
			{
				if (shootTimer > 10000) //Just to avoid overflowing
				{
					shootTimer = tShoot;
				}
			}
		}
		else
		{
			deadTimer += time;
			if (deadTimer >= tDead)
			{
				respawn = true;
			}
		}
	}


	void collision(bool checkX, float time, Level level)
	{
		for (int i = rect.top / level.tileHeight; i < (rect.top + rect.height) / level.tileHeight; i++)
		{
			for (int j = rect.left / level.tileWidth; j < (rect.left + rect.width) / level.tileWidth; j++)
			{
				if (i < level.mapHeight && i >= 0 && j < level.mapWidth && j >= 0)
				{
					if (level.objects[i][j] == ObjectType::Ladder)
					{
						onLadder = true;
					}

					if (level.objects[i][j] == ObjectType::Solid)
					{
						if (checkX)
						{
							if (dx > 0)
							{
								rect.left = j * level.tileWidth - rect.width;
							}
							else
							{
								rect.left = j * level.tileWidth + level.tileWidth;
							}
							dx = 0;
						}
						else
						{
							if (dy > 0)
							{
								rect.top = i * level.tileHeight - rect.height;
								onGround = true;
							}
							else
							{
								rect.top = i * level.tileHeight + level.tileHeight;
							}
							dy = 0;
						}
						return;
					}
				}
				else
				{
					//When the player is out of the map, it is better just to kill him
					isAlive = false;
				}
			}
		}

		/*if (objects[i].name == "SlopeLeft")
		{
			onGround = true;
			FloatRect r = objects[i].rect;
			int y0 = (rect.left + rect.width / 2 - r.left) * r.height / r.width + r.top - rect.height;
			if (rect.top > y0)
			{
				if (rect.left + rect.width / 2 > r.left)
				{
					rect.top = y0;
					dy = 0;
				}
			}
		}

		if (objects[i].name == "SlopeRight")
		{
			onGround = true;
			FloatRect r = objects[i].rect;
			int y0 = -(rect.left + rect.width / 2 - r.left) * r.height / r.width + r.top + r.height - rect.height;
			if (rect.top > y0)
			{
				if (rect.left + rect.width / 2 < r.left + r.width)
				{
					rect.top = y0;
					dy = 0;
				}
			}
		}*/
	}

	Vector2f getVec()
	{
		return Vector2f(rect.left, rect.top);
	}

	void hit(int damage)
	{
		health -= damage;
		if (health <= 0)
		{
			isAlive = false;			
		}
	}

	void receivePacket(Packet* packet)
	{
		*packet >> keys[Player::Key::Left];
		*packet >> keys[Player::Key::Right];
		*packet >> keys[Player::Key::Up];
		*packet >> keys[Player::Key::Down];
		*packet >> keys[Player::Key::Space];
	}

	void createPacket(Packet* packet)
	{
		*packet << rect.left;
		*packet << rect.top;
		*packet << left;
		bool f = dy == 0;
		*packet << f;
		*packet << state;
		*packet << isAlive;
	}
};

#endif PLAYER_H