#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.hpp"

class Player : public Entity
{
public:
	enum class Key { Left, Right, Up, Down, Space };
	enum class State { Stand, Run, Jump, Crawl, Climb } state;
	float shootTimer;
	bool onGround, onLadder, shoot, hit;
	std::map<Key, bool> keys;

	Player(AnimationManager a, Level level, int _health) : 
		Entity(a, level.getObjectVector("player"), _health)
	{
		state = State::Stand;
		animationManager.set(AnimationType::Stand);
		animationManager.loop(AnimationType::Jump, false);
		animationManager.loop(AnimationType::Fall, false);

		shootTimer = 0;
		onGround = false;
		onLadder = false;
		shoot = false;
		hit = false;
		left = false;
		objects = level.getAllObjects();
	}

	void updateKeys()
	{
		if (keys[Key::Left])
		{
			left = true;	
			dx = -0.1f;
			if (state == State::Stand)
			{
				state = State::Run;
			}
			if (state == State::Climb)
			{
				dx = -0.05f;
			}
		}
		else
		{
			if (keys[Key::Right])
			{
				left = false;
				dx = 0.1f;
				if (state == State::Stand)
				{
					state = State::Run;
				}
				if (state == State::Climb)
				{
					dx = 0.05f;
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
				dy = -0.05f;
				state = State::Climb;
			} 
			else
			{
				if (state == State::Stand || state == State::Run)
				{
					dy = -0.27f;
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
					dy = 0.05f;
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
				state = State::Jump;
				dy = -0.27f;
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

	void update(float time)
	{
		updateKeys();

		if (state != State::Climb)
		{
			dy += 0.0005 * time;
		}

		onGround = false;
		onLadder = false;

		rect.left += dx * time;
		Collision(0);

		rect.top += dy * time;
		Collision(1);

		if (state == State::Climb)
		{
			if (!onLadder)
			{
				state = State::Jump;
				dy = -0.27f;
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
			if (shootTimer > 4000)
			{
				shootTimer = 400;
			}
		}

		updateAnimation(time);
	}

	void Collision(int num)
	{
		for (int i = 0; i < objects.size(); i++)
		{
			if (rect.intersects(objects[i].rect))
			{
				if (objects[i].name == "solid")
				{
					if (num == 1)
					{
						if (dy > 0)
						{
							rect.top = objects[i].rect.top - rect.height;
							dy = 0;				
							onGround = true;
						}
						if (dy < 0)
						{
							rect.top = objects[i].rect.top + objects[i].rect.height;
							dy = 0;
						}
					}
					else
					{
						if (dx > 0)
						{
							rect.left = objects[i].rect.left - rect.width;
						}
						if (dx < 0)
						{
							rect.left = objects[i].rect.left + objects[i].rect.width;							
						}
						dx = 0;
					}
				}

				if (objects[i].name == "ladder")
				{
					onLadder = true;
					//if (animationManager.currentAnimation == AnimationType::Climb)
					//{
					//	rect.left = objects[i].rect.left - 10;
					//}
				}

				if (objects[i].name == "SlopeLeft")
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
				}
			}
		}
	}
};

#endif PLAYER_H