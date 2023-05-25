#ifndef BULLET_H
#define BULLET_H

class Bullet
{
public:
	AnimationManager animationManager;
	unsigned char state;
	float x, y;
	bool left, isAlive;

	Bullet(AnimationManager a)
	{
		animationManager = a;
		animationManager.set(AnimationType::Move);
		animationManager.loop(AnimationType::Move, false);
		animationManager.loop(AnimationType::Explode, false);
		state = STATE_STAND;

		isAlive = true; // No explode animation					
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
		*packet >> x;
		*packet >> y;
		*packet >> left;
	}

	void draw(RenderWindow& window)
	{
		animationManager.draw(window, x, y);
	}
};

#endif BULLET_H
