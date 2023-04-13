#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.hpp"

class Player : public Entity
{
public:
	unsigned char state;
	int health;
	bool dy;

	Player() : Entity()
	{
	}

	Player(AnimationManager a, int _health) :
		Entity(a)
	{
		state = STATE_STAND;
		animationManager.set(AnimationType::Stand);
		rect.width = (float)animationManager.getWidth();
		rect.height = (float)animationManager.getHeight();
		animationManager.loop(AnimationType::Jump, false);

		health = _health;
		dy = true;
	}

	void update(signed __int32 _time)
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

	void receivePacket(Packet* packet)
	{
		*packet >> rect.left;
		*packet >> rect.top;
		*packet >> left;
		*packet >> dy;
		*packet >> state;
	}
};

#endif PLAYER_H