#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "Consts.hpp"
#include "Animation.hpp"
#include "Level.hpp"

class Player
{
public:
	AnimationManager animationManager;
	unsigned char state;
	float x, y;
	bool left, isAlive, dy, team;	
	int health;

	Player() {}

	Player(AnimationManager a, int _health)
	{	
		animationManager = a;
		animationManager.set(AnimationType::Stand);
		animationManager.loop(AnimationType::Jump, false);
		state = STATE_STAND;

		x = 0;
		y = 0;
		left = false;
		isAlive = true;
		dy = true;

		health = _health;	
	}

	void update(signed __int32 _time, bool myPlayer)
	{
		float time = (float)_time;
		switch (state)
		{
		case STATE_STAND:
			animationManager.set(AnimationType::Stand);
			break;
		case STATE_RUN:
			animationManager.set(AnimationType::Run);
			break;
		case STATE_JUMP:
			animationManager.set(AnimationType::Jump);
			break;
		case STATE_CRAWL:
			animationManager.set(AnimationType::Crawl);
			break;
		case STATE_CLIMB:
			animationManager.set(AnimationType::Climb);
			if (dy)
			{
				animationManager.pause();
			}
			else
			{
				animationManager.play();
			}
			break;
		}

		/*if (hit) {
			timer += time;
			if (timer > 1000)
			{
				hit = false;
				timer = 0;
			}
			animationManager.set("hit");
		}*/

		if (!isAlive)
		{
			animationManager.setColor(Color::Red);
		}
		else
		{
			if (myPlayer)
			{
				animationManager.setColor(Color(0, 64, 64, 255));
			}
			else
			{
				if (team)
				{
					animationManager.setColor(Color(0, 128, 0, 128));
				}
				else
				{
					animationManager.setColor(Color(0, 0, 128, 255));
				}
			}			
		}
		animationManager.update(time, left);
	}

	void receivePacket(Packet* packet)
	{
		*packet >> team;
		*packet >> x;
		*packet >> y;
		*packet >> left;
		*packet >> health;
		*packet >> dy;
		*packet >> state;
		*packet >> isAlive;
	}

	void draw(RenderWindow& window)
	{
		animationManager.draw(window, x, y);
	}
};

#endif PLAYER_H