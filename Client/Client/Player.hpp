#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.hpp"

class Player : public Entity
{
public:
	enum class Key
	{
		Left, Right, Up, Down, Space
	};
	
	bool onLadder, shoot, hit;
	std::map<Key, bool> keys;

	Player(const char* image, const char* file, Texture& t, Level level, int _health) : 
		Entity(image, file, t, level.getObjectVector("player"), 0, _health)
	{
		animationManager.set(AnimationType::Stand);
		onLadder = false;
		shoot = false;
		hit = false;
		objects = level.getAllObjects();
	}

	void updateKeyboard()
	{
		if (keys[Key::Left])
		{
			left = true;
			if (state != State::Crawl)
			{
				dx = -0.1;
			}
			if (state == State::Stand)
			{
				state = State::Run;
			}
		}

		if (keys[Key::Right])
		{
			left = false;
			if (state != State::Crawl)
			{
				dx = 0.1;
			}
			if (state == State::Stand)
			{
				state = State::Run;
			}
		}

		if (keys["Up"])
		{
			if (onLadder)
			{
				state = State::Climb;
			}
			if (state == State::Stand || state == State::Run) 
			{ 
				dy = -0.27; 
				state = State::Jump; 
				animationManager.play("jump"); 
			}
			if (state == State::Climb) 
			{
				if (keys["L"] || keys["R"])
				{
					state = State::Stand;
				}
				else
				{
					dy = -0.05;
				}				
			}			
		}

		if (keys["Down"])
		{
			if (state == State::Stand || state == State::Run) 
			{ 
				state = State::Crawl; 
				dx = 0; 
			}
			if (state == State::Climb) {
				dy = 0.05;
			}
		}

		if (keys["Space"])
		{
			shoot = true;
		}

		/////////////////////если клавиша отпущена///////////////////////////
		if (!(keys["R"] || keys["L"]))
		{
			dx = 0;
			if (state == State::Run)
			{
				state = State::Stand;
			}
		}

		if (!(keys["Up"] || keys["Down"]))
		{
			if (state == State::Climb) 
			{
				dy = 0;
			}
		}

		if (!keys["Down"])
		{
			if (state == State::Crawl) 
			{ 
				state = State::Stand;
			}
		}

		if (!keys["Space"])
		{
			shoot = false;
		}

		keys["R"] = keys["L"] = keys["Up"] = keys["Down"] = keys["Space"] = false;
	}

	void updateAnimation(float time)
	{
		if (state == State::Stand)
		{
			animationManager.set("stand");
		}
		if (state == State::Run)
		{
			animationManager.set("run");
		}
		if (state == State::Jump)
		{
			animationManager.set("jump");
		}
		if (state == State::Crawl)
		{
			animationManager.set("crawl");
		}
		if (state == State::Climb) 
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

		if (left)
		{
			animationManager.flip(true);
		}

		animationManager.update(time);
	}

	void update(float time)
	{
		updateKeyboard();

		updateAnimation(time);

		if (state == State::Climb && !onLadder)
		{
			state = State::Stand;
		}
		if (state != State::Climb)
		{
			dy += 0.0005 * time;
		}
		onLadder = false;

		x += dx * time;
		Collision(0);

		y += dy * time;
		Collision(1);
	}

	void Collision(int num)
	{
		for (int i = 0; i < objects.size(); i++)
		{
			if (getRect().intersects(objects[i].rect))
			{
				if (objects[i].name == "solid")
				{
					if (num == 1)
					{
						if (dy > 0)
						{
							y = objects[i].rect.top - h;
							dy = 0;
							state = State::Stand;
						}
						if (dy < 0)
						{
							y = objects[i].rect.top + objects[i].rect.height;
							dy = 0;
						}
					}
					else
					{
						if (dx > 0)
						{
							x = objects[i].rect.left - w;
						}
						if (dx < 0)
						{
							x = objects[i].rect.left + objects[i].rect.width;
						}
					}
				}

				if (objects[i].name == "ladder")
				{
					onLadder = true;
					if (state == State::Climb)
					{
						x = objects[i].rect.left - 10;
					}
				}

				if (objects[i].name == "SlopeLeft")
				{
					FloatRect r = objects[i].rect;
					int y0 = (x + w / 2 - r.left) * r.height / r.width + r.top - h;
					if (y > y0)
					{
						if (x + w / 2 > r.left)
						{
							y = y0; dy = 0; 
							state = State::Stand;
						}
					}
				}

				if (objects[i].name == "SlopeRight")
				{
					FloatRect r = objects[i].rect;
					int y0 = -(x + w / 2 - r.left) * r.height / r.width + r.top + r.height - h;
					if (y > y0)
					{
						if (x + w / 2 < r.left + r.width)
						{
							y = y0; dy = 0; 
							state = State::Stand;
						}
					}
				}
			}
		}
	}
};

#endif PLAYER_H