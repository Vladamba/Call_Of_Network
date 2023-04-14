#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Network.hpp>
#include "Player.hpp"

using namespace sf;

class Client
{
public:
	TcpSocket socket;
	Player player;
	bool connected;	

	Client(Level level, int health)
	{
		player = Player(level, health);
		connected = false;
	}

	void updateState(Packet* packet)
	{
		*packet >> player.keys[Player::Key::Left];
		*packet >> player.keys[Player::Key::Right];
		*packet >> player.keys[Player::Key::Up];
		*packet >> player.keys[Player::Key::Down];
		*packet >> player.keys[Player::Key::Space];
	}

	bool updatePlayer(signed __int32 time, Level level)
	{
		player.update(time, level);
		return player.shoot;
	}

	void playerHit(int damage)
	{
		player.hit(damage);
	}

	Vector2f getBulletVec()
	{
		return Vector2f(player.left ? player.rect.left - BULLET_WIDTH - 1 : player.rect.left + player.rect.width + 1, player. rect.top  + player.rect.height / 2);
	}

	void createPacket(Packet *packet)
	{
		*packet << player.rect.left;
		*packet << player.rect.top;
		*packet << player.left;
		bool f = player.dy == 0;
		*packet << f;
		*packet << player.state;
	}
};

#endif CLIENT_H