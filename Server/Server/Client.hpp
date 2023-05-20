#ifndef CLIENT_H
#define CLIENT_H

using namespace sf;
enum Stage { Connection, FileAsk, FileAnswer, TeamAsk, TeamAnswer, NameAsk, NameAnswer, PortAsk, PortAnswer, Playing, Error };
class Client
{
public:
	
	TcpSocket tcpSocket;
	Stage stage;
	UdpSocket udpSocket;
	IpAddress ip;
	unsigned short clientPort, serverPort;

	bool team;
	std::string filename, name;
	Player player;

	Client(Level level)
	{		
		stage = Stage::Connection;
		name = "New player";
		player = Player(level);
	}

	void disconnect()
	{
		tcpSocket.disconnect();
		udpSocket.unbind();
		stage = Stage::Connection;
		name = "New player";
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