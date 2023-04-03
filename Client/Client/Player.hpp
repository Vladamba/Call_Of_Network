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

	Player(const char* image, const char* file, Level level, int _health) : 
		Entity(image, file, level.getObjectVector("player"), 0, _health)
	{
		animationManager.currentAnimation = AnimationType::Stand;
		onLadder = false;
		shoot = false;
		hit = false;
		objects = level.getAllObjects();
	}

	void updateKeys()
	{
		if (keys[Key::Left])
		{
			left = true;		
			dx = -0.1f;
			if (animationManager.currentAnimation == AnimationType::Stand)
			{
				animationManager.currentAnimation = AnimationType::Run;
			}
			if (animationManager.currentAnimation == AnimationType::Climb)
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
				if (animationManager.currentAnimation == AnimationType::Stand)
				{
					animationManager.currentAnimation = AnimationType::Run;
				}
				if (animationManager.currentAnimation == AnimationType::Climb)
				{
					dx = 0.05f;
				}
			}
			else
			{
				dx = 0;
				if (animationManager.currentAnimation == AnimationType::Run)
				{
					animationManager.currentAnimation = AnimationType::Stand;
				}
			}	
		}

		if (keys[Key::Up])
		{
			if (onLadder)
			{
				dy = -0.05f;
				animationManager.currentAnimation = AnimationType::Climb;			
			}
			if (animationManager.currentAnimation == AnimationType::Stand || 
				animationManager.currentAnimation == AnimationType::Run)
			{ 
				dy = -0.27f; 
				animationManager.currentAnimation = AnimationType::Jump;
			}
			if (animationManager.currentAnimation == AnimationType::Crawl)
			{
				animationManager.currentAnimation = AnimationType::Stand;
			}
					
		}
		else
		{
			if (keys[Key::Down])
			{
				if (onLadder)
				{
					dy = 0.05f;
					animationManager.currentAnimation = AnimationType::Climb;
				}
				if (animationManager.currentAnimation == AnimationType::Stand ||
					animationManager.currentAnimation == AnimationType::Run)
				{
					animationManager.currentAnimation = AnimationType::Crawl;
				}
			}
			else
			{
				if (animationManager.currentAnimation == AnimationType::Climb)
				{
					dy = 0;
				}
			}
		}	

		if (keys[Key::Space])
		{
			if (animationManager.currentAnimation == AnimationType::Climb)
			{
				animationManager.currentAnimation = AnimationType::Jump;
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

		animationManager.left(left);


		animationManager.update(time);
	}

	void update(float time)
	{
		updateKeys();

		updateAnimation(time);

		if (animationManager.currentAnimation == AnimationType::Jump)
		{
			if (dy >= 0)
			{
				//animationManager.currentAnimation = AnimationType::Fall;
			}
		}

		//if (state == State::Climb && !onLadder)
		//{
		//	state = State::Stand;
		//}
		if (animationManager.currentAnimation != AnimationType::Climb)
		{
			dy += 0.0005 * time;
		}

		onLadder = false;

		rect.left += dx * time;
		Collision(0);

		rect.top += dy * time;
		Collision(1);

		if (animationManager.currentAnimation == AnimationType::Climb)
		{
			if (!onLadder)
			{
				animationManager.currentAnimation = AnimationType::Jump;
				dy = -0.27f;
			}		
		}
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
							animationManager.currentAnimation = AnimationType::Stand;
						}
						if (dy < 0)
						{
							rect.top = objects[i].rect.top + objects[i].rect.height;
							dy = 0;
							//animationManager.currentAnimation = AnimationType::Fall;
						}
					}
					else
					{
						if (dx > 0)
						{
							rect.left = objects[i].rect.left - rect.width;
							animationManager.currentAnimation = AnimationType::Stand;
						}
						if (dx < 0)
						{
							rect.left = objects[i].rect.left + objects[i].rect.width;
							animationManager.currentAnimation = AnimationType::Stand;
						}
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
					FloatRect r = objects[i].rect;
					int y0 = (rect.left + rect.width / 2 - r.left) * r.height / r.width + r.top - rect.height;
					if (rect.top > y0)
					{
						if (rect.left + rect.width / 2 > r.left)
						{
							rect.top = y0; 
							dy = 0; 
							//animationManager.currentAnimation = AnimationType::Stand;
						}
					}
				}

				if (objects[i].name == "SlopeRight")
				{
					FloatRect r = objects[i].rect;
					int y0 = -(rect.left + rect.width / 2 - r.left) * r.height / r.width + r.top + r.height - rect.height;
					if (rect.top > y0)
					{
						if (rect.left + rect.width / 2 < r.left + r.width)
						{
							rect.top = y0; 
							dy = 0; 
							//animationManager.currentAnimation = AnimationType::Stand;
						}
					}
				}
			}
		}
	}
};

#endif PLAYER_H