#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.hpp"

class Player : public Entity
{
public:
	enum class State
	{
		stand, standShoot,
		run, runShoot,
		crawl, crawlShoot,
		jump, jumpShoot,
		climb
	} state;
	bool onLadder, shoot, hit;
	std::map<std::string, bool> key;

	Player(AnimationManager &a, Level &lev, int x, int y) : Entity(a, x, y)
	{
		option("Player", 0, 100, "stand");
		state = State::stand; 
		hit = false;
		objects = lev.getAllObjects();
	}

	void updateKeyboard()
	{
		if (key["L"])
		{
			left = 1;
			if (state != State::crawl)
			{
				dx = -0.1;
			}
			if (state == State::stand)
			{
				state = State::run;
			}
		}

		if (key["R"])
		{
			left = 0;
			if (state != State::crawl)
			{
				dx = 0.1;
			}
			if (state == State::stand)
			{
				state = State::stand;
			}
		}

		if (key["Up"])
		{
			if (onLadder)
			{
				state = State::climb;
			}
			if (state == State::stand || state == State::run) 
			{ 
				dy = -0.27; 
				state = State::jump; 
				animationManager.play("jump"); 
			}
			if (state == State::climb) 
			{
				if (key["L"] || key["R"])
				{
					state = State::stand;
				}
				else
				{
					dy = -0.05;
				}				
			}			
		}

		if (key["Down"])
		{
			if (state == State::stand || state == State::run) 
			{ 
				state = State::crawl; 
				dx = 0; 
			}
			if (state == State::climb) {
				dy = 0.05;
			}
		}

		if (key["Space"])
		{
			shoot = true;
		}

		/////////////////////если клавиша отпущена///////////////////////////
		if (!(key["R"] || key["L"]))
		{
			dx = 0;
			if (state == State::run)
			{
				state = State::stand;
			}
		}

		if (!(key["Up"] || key["Down"]))
		{
			if (state == State::climb) 
			{
				dy = 0;
			}
		}

		if (!key["Down"])
		{
			if (state == State::crawl) 
			{ 
				state = State::stand;
			}
		}

		if (!key["Space"])
		{
			shoot = false;
		}

		key["R"] = key["L"] = key["Up"] = key["Down"] = key["Space"] = false;
	}

	void updateAnimation(float time)
	{
		if (state == State::stand)
		{
			animationManager.set("stand");
		}
		if (state == State::run)
		{
			animationManager.set("run");
		}
		if (state == State::jump)
		{
			animationManager.set("jump");
		}
		if (state == State::crawl)
		{
			animationManager.set("crawl");
		}
		if (state == State::climb) 
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
			if (state == State::run)
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

		if (state == State::climb && !onLadder)
		{
			state = State::stand;
		}
		if (state != State::climb)
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
				if (objects[i].type == "solid")
				{
					if (num == 1)
					{
						if (dy > 0)
						{
							y = objects[i].rect.top - h;
							dy = 0;
							state = State::stand;
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

				if (objects[i].type == "ladder")
				{
					onLadder = true;
					if (state == State::climb)
					{
						x = objects[i].rect.left - 10;
					}
				}

				if (objects[i].type == "SlopeLeft")
				{
					FloatRect r = objects[i].rect;
					int y0 = (x + w / 2 - r.left) * r.height / r.width + r.top - h;
					if (y > y0)
					{
						if (x + w / 2 > r.left)
						{
							y = y0; dy = 0; 
							state = State::stand;
						}
					}
				}

				if (objects[i].type == "SlopeRight")
				{
					FloatRect r = objects[i].rect;
					int y0 = -(x + w / 2 - r.left) * r.height / r.width + r.top + r.height - h;
					if (y > y0)
					{
						if (x + w / 2 < r.left + r.width)
						{
							y = y0; dy = 0; 
							state = State::stand;
						}
					}
				}
			}
		}
	}
};

#endif PLAYER_H