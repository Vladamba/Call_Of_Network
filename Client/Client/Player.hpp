#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.hpp"

class Player : public Entity
{
public:
	int sHeight, sWidth;
	float g, vJump, vLadderJump, vRun, vClimb;
	float tShoot = 200.f;

	enum class Key { Left, Right, Up, Down, Space };
	enum class State { Stand, Run, Jump, Crawl, Climb } state;
	float shootTimer;
	bool onGround, onLadder, shoot, hit;
	std::map<Key, bool> keys;

	Player(AnimationManager a, Level level, int _health) :
		Entity(a, level.getObjectCoord(ObjectType::Player), _health)
	{
		sHeight = level.tileHeight;
		sWidth = level.tileWidth;
		g = 2.f * (float)sHeight / (200.f * 200.f);// 0.0016
		vJump = sqrt(2.f * g * 2.5f * (float)sHeight); // 0.505964
		vLadderJump = sqrt(2.f * g * 1.5f * (float)sHeight); // 0.391918
		vRun = g * 2.5f * (float)sWidth / vJump; // 0.252982
		vClimb = vRun / 2.f; // 0.126491
		/*printf("g=%f", g);
		printf("  vJump=%f", vJump);
		printf("  vLadderJump=%f", vLadderJump);
		printf("  vRun=%f", vRun);
		printf("  vClimb=%f", vClimb);*/
		state = State::Stand;
		animationManager.set(AnimationType::Stand);
		rect.width = (float)animationManager.getWidth();
		rect.height = (float)animationManager.getHeight();
		animationManager.loop(AnimationType::Jump, false);

		shootTimer = 0;
		onGround = false;
		onLadder = false;
		shoot = false;
		hit = false;
		left = false;
	}

	void updateKeys()
	{
		if (keys[Key::Left])
		{
			left = true;
			if (state == State::Climb)
			{
				dx = -vClimb;
			}
			else
			{
				dx = -vRun;
			}

			if (state == State::Stand)
			{
				state = State::Run;
			}		
		}
		else
		{
			if (keys[Key::Right])
			{
				left = false;
				if (state == State::Climb)
				{
					dx = vClimb;
				}
				else
				{
					dx = vRun;
				}

				if (state == State::Stand)
				{
					state = State::Run;
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
				state = State::Climb;
			}
			else
			{
				if (onGround)
				{
					if (state == State::Stand || state == State::Run)
					{
						dy = -vJump;
						state = State::Jump;
					}
				}			
				if (state == State::Crawl)
				{
					state = State::Stand;
				}
			}
		}
		else
		{
			if (keys[Key::Down])
			{
				if (onLadder)
				{
					dy = vClimb;
					state = State::Climb;
				}
				else
				{
					if (state == State::Stand || state == State::Run)
					{
						state = State::Crawl;
					}
				}
			}
			else
			{
				if (state == State::Climb)
				{
					dy = 0;
				}
			}
		}

		if (keys[Key::Space] && state != State::Climb)
		{
			shoot = true;
		}
	}

	void updateAnimation(signed __int32 _time)
	{
		float time = (float)_time;
		switch (state)
		{
		case State::Stand:
			animationManager.set(AnimationType::Stand);
			break;
		case State::Run:
			animationManager.set(AnimationType::Run);
			break;
		case State::Jump:
			animationManager.set(AnimationType::Jump);
			break;
		case State::Crawl:
			animationManager.set(AnimationType::Crawl);
			break;
		case State::Climb:
			animationManager.set(AnimationType::Climb);
			if (dy == 0)
			{
				animationManager.pause();
			}
			else
			{
				animationManager.play();
			}
			break;
		}

		/*if (shoot) {
			animationManager.set("shoot");
			if (state == State::Run)
			{
				animationManager.set("shootAndWalk");
			}
		}

		if (hit) {
			timer += time;
			if (timer > 1000)
			{
				hit = false;
				timer = 0;
			}
			animationManager.set("hit");
		}
		*/

		animationManager.update(time, left);
	}

	void update(float time, Level level)
	{
		//rect.width = animationManager.getWidth();
		//rect.height = animationManager.getHeight();
		updateKeys();

		onGround = false;
		onLadder = false;

		if (dx != 0)
		{
			rect.left += dx * time;
			collision(true, time, level);
		}

		if (state != State::Climb)
		{
			dy += g * time;			
			rect.top += dy * time + g * time * time / 2.f;
		}
		else
		{
			rect.top += dy * time;
		}

		collision(false, time, level);

		if (state == State::Climb && !onLadder)
		{
			dy = -vLadderJump;
			state = State::Jump;
		}

		if (onGround && state != State::Crawl)
		{
			if (dx == 0)
			{
				state = State::Stand;
			}
			else
			{
				state = State::Run;
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
			if (shootTimer > 10000)
			{
				shootTimer = tShoot;
			}
		}

		updateAnimation(time);
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
					//When the player is out of the map, do not know what to do
					rect.left = level.tileWidth;
					rect.top = level.tileHeight;
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
};

#endif PLAYER_H