#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.hpp"

const float speed = 0.1f;
const float jump = 0.27f;
const float climb = 0.05f;
const float fall = 0.0005f;

class Player : public Entity
{
public:
	enum class Key { Left, Right, Up, Down, Space };
	enum class State { Stand, Run, Jump, Crawl, Climb } state;
	float shootTimer;
	bool onGround, onLadder, shoot, hit;
	std::map<Key, bool> keys;

	Player(AnimationManager a, Level level, int _health) :
		Entity(a, level.getObjectCoord(ObjectType::Player), _health)
	{
		state = State::Stand;
		animationManager.set(AnimationType::Stand);
		rect.width = animationManager.getWidth();
		rect.height = animationManager.getHeight();
		animationManager.loop(AnimationType::Jump, false);
		animationManager.loop(AnimationType::Fall, false);

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
			dx = -speed;
			if (state == State::Stand)
			{
				state = State::Run;
			}
			if (state == State::Climb)
			{
				dx = -climb;
			}
		}
		else
		{
			if (keys[Key::Right])
			{
				left = false;
				dx = speed;
				if (state == State::Stand)
				{
					state = State::Run;
				}
				if (state == State::Climb)
				{
					dx = climb;
				}
			}
			else
			{
				dx = 0;
				if (state == State::Run)
				{
					state = State::Stand;
				}
			}
		}


		if (keys[Key::Up])
		{
			if (onLadder)
			{
				dy = -climb;
				state = State::Climb;
			}
			else
			{
				if (state == State::Stand || state == State::Run)
				{
					dy = -jump;
					state = State::Jump;
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
					dy = climb;
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

		if (keys[Key::Space])
		{
			if (state == State::Climb)
			{
				dy = -jump;
				state = State::Jump;				
				onLadder = false;
			}
			else
			{
				shoot = true;
			}
		}
		else
		{
			shoot = false;
		}

		keys[Key::Left] = keys[Key::Right] = keys[Key::Up] = keys[Key::Down] = keys[Key::Space] = false;
	}

	void updateAnimation(float time)
	{
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
			break;
		}
		/*if (state == State::Climb)
		{
			animationManager.set("climb");
			animationManager.pause();
			if (dy != 0)
			{
				animationManager.play("climb");
			}
		}

		if (shoot) {
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
			dy += fall * time;
		}

		if (dy != 0)
		{
			rect.top += dy * time;
			collision(false, time, level);
		}

		if (state == State::Climb)
		{
			if (!onLadder)
			{
				dy = jump;
				state = State::Jump;
			}
		}

		if (onGround && dx == 0 && state != State::Crawl)
		{
			state = State::Stand;
		}

		shootTimer += time;
		if (shoot)
		{
			if (shootTimer > 400)
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
				shootTimer = 400;
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
						//goto collisionHappened;
					}
				}
				else
				{
					rect.left = level.tileWidth;
					rect.top = level.tileHeight;
				}
			}
		}
		//collisionHappened:




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