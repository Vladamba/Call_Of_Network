#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Network.hpp>
#include "Player.hpp"

using namespace sf;

class Client
{
public:
	TcpSocket socket;
	bool connected, playing, teamed, team;
	Player player;

	Client(Level level)
	{		
		connected = false;
		playing = false;
		teamed = false;
		player = Player(level);
	}

	void disconnect()
	{
		socket.disconnect();
		connected = false;
		playing = false;
		teamed = false;
	}

	void createPacket(Packet* packet)
	{
		*packet << team;
		player.createPacket(packet);
	}

	void newPlayer(Level level, int _health)
	{
		player.newPlayer(level, team, _health);
	}

	Vector2f getBulletVec()
	{
		return Vector2f(player.left ? player.rect.left - BULLET_WIDTH - 1 : player.rect.left + player.rect.width + 1, player. rect.top  + player.rect.height / 2);
	}
};

#endif CLIENT_H