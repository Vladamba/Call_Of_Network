#ifndef BULLET_H
#define BULLET_H

#include "Entity.hpp"

class Bullet : public Entity
{
private:
	const float move = 0.5;

public:

	Bullet(AnimationManager a) :
		Entity(a)
	{
		animationManager.set(AnimationType::Move);
		rect.width = BULLET_WIDTH;
		rect.height = BULLET_HEIGHT;
		animationManager.loop(AnimationType::Move, false);
		animationManager.loop(AnimationType::Explode, false);			
	}

	void update(signed __int32 _time)
	{
		float time = (float)_time;

		if (!isAlive) {
			animationManager.set(AnimationType::Explode);
		}

		animationManager.update(time, left);
	}

	void receivePacket(Packet* packet)
	{
		*packet >> rect.left;
		*packet >> rect.top;
		*packet >> left;
	}
};

#endif BULLET_H
