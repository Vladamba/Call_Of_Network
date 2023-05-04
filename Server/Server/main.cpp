#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Consts.hpp"
#include "Level.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Client.hpp"
#include <thread>
//#include "MovingPlatform.hpp"
//#include "HealthBar.hpp"

using namespace sf;

void newClient(Level* level, Client** clients, unsigned short port, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber);
void updateClients(Level* level, Client** clients, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber);

int main()
{
	unsigned short port = 2000;
	printf("My IP is: %s\nMy port is: %d", IpAddress::getLocalAddress().toString(), port);

	Level level("files/" + MAP_FILENAME);
	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	Client** clients = new Client*[CLIENTS_SIZE];
	for (int i = 0; i < CLIENTS_SIZE; i++)
	{		
		clients[i] = new Client(level);
	}
	 
	unsigned char clientsNumber = 0, team1 = 0, team2 = 0;

	std::thread tNewClient(newClient, &level, clients, port, &team1, &team2, &clientsNumber);
	std::thread tUpdateClients(updateClients, &level, clients, &team1, &team2, &clientsNumber);
	
	//tNewClient.detach();
	tNewClient.join();
	//tUpdateClients.detach();
	tUpdateClients.join();
	return 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

void newClient(Level* level, Client** clients, unsigned short port, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber)
{
	TcpListener listener;
	Packet rPacket, sPacket;

	listener.listen(port);
	while (true)
	{
		for (int i = 0; i < CLIENTS_SIZE; i++)
		{
			switch (clients[i]->stage)
			{
				case Stage::Error:
				{
					rPacket.clear();
					printf("\n%s disconnected!", clients[i]->name);
					clients[i]->disconnect();
					break;
				}

				case Stage::Connection:
				{
					if (listener.accept(clients[i]->socket) == Socket::Done)
					{
						printf("\nAccepted new player!");
						if (listener.isBlocking())
						{
							listener.setBlocking(false);
						}
						if (clients[i]->socket.isBlocking())
						{
							clients[i]->socket.setBlocking(false);
						}
						clients[i]->stage = Stage::CheckFiles;
					}
					break;
				}					

				case Stage::CheckFiles:
				{
					sPacket << MAP_FILENAME;
					sPacket << TILESET_FILENAME;
					sPacket << BACKGROUND_FILENAME;
					clients[i]->socket.send(sPacket);
					sPacket.clear();
					clients[i]->stage = Stage::TeamAsk;
					break;
				}

				case Stage::TeamAsk:
				{
					sPacket << *team1;
					sPacket << *team2;
					clients[i]->socket.send(sPacket);
					sPacket.clear();
					clients[i]->stage = Stage::TeamAnswer;
					break;
				}

				case Stage::TeamAnswer:
				{
					Socket::Status s = clients[i]->socket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						clients[i]->stage = Stage::Error;
						break;
					}

					if (s == Socket::Done)
					{
						rPacket >> clients[i]->team;
						rPacket.clear();

						bool autoBalanced = false;
						if (clients[i]->team)
						{
							(*team1)++;
							if (*team1 - *team2 > 1)
							{
								(*team1)--;
								(*team2)++;
								clients[i]->team = false;
								autoBalanced = true;
							}
						}
						else
						{
							(*team2)++;
							if (*team2 - *team1 > 1)
							{
								(*team2)--;
								(*team1)++;
								clients[i]->team = true;
								autoBalanced = true;
							}
						}

						sPacket << autoBalanced;
						clients[i]->socket.send(sPacket);
						sPacket.clear();
						clients[i]->stage = Stage::NameAsk;
					}
					break;
				}

				case Stage::NameAsk:
				{
					Socket::Status s = clients[i]->socket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						clients[i]->stage = Stage::Error;
						break;
					}

					if (s == Socket::Done)
					{
						rPacket >> clients[i]->name;
						rPacket.clear();

						bool anotherName = false;
						for (int j = 0; j < CLIENTS_SIZE; j++)
						{
							if (i != j && clients[i]->name == clients[j]->name)
							{
								anotherName = true;
								clients[i]->name = clients[i]->name + "_2";
								break;
							}
						}

						sPacket << anotherName;
						sPacket << clients[i]->name;
						clients[i]->socket.send(sPacket);
						sPacket.clear();

						//clients[i]->newPlayer(*level, 100);
						clients[i]->stage = Stage::Playing;
						(*clientsNumber)++;
					}
					break;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void updateClients(Level* level, Client** clients, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber)
{	
	Packet rPacket, sPacket;

	Bullet** bullets = new  Bullet * [BULLETS_SIZE];
	for (int i = 0; i < BULLETS_SIZE; i++)
	{
		bullets[i] = new Bullet(NULL_VECTOR2f, 10, false, true);
	}

	Vector2f playersCoord[CLIENTS_SIZE];
	Vector2i bulletHitVec;

	unsigned char bulletsNumber = 0, clientIndex = 0;

	Clock clock;
	signed __int32 time;
	while (true)
	{
		if (clock.getElapsedTime().asMilliseconds() > MSPF)
		{
			time = clock.getElapsedTime().asMilliseconds();
			clock.restart();

			if (time > MSPF * 2)
			{
				time = MSPF * 2;
			}

			for (int i = 0; i < CLIENTS_SIZE; i++)
			{
				if (clients[i]->stage == Stage::Playing)
				{
					Socket::Status s = clients[i]->socket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						rPacket.clear();
						printf("\n%s Disconnected!", clients[i]->name);
						clients[i]->disconnect();
						if (clients[i]->team)
						{
							(*team1)--;
						}
						else
						{
							(*team2)--;
						}
						(*clientsNumber)--;
						break;
					}

					if (s == Socket::Done)
					{						
						clients[i]->player.receivePacket(&rPacket);
						rPacket.clear();
					}

					clients[i]->player.update(time, *level);

					if (clients[i]->player.respawn)
					{
						clients[i]->newPlayer(*level, 100);
					}

					if (clients[i]->player.shoot)
					{
						for (int j = 0; j < BULLETS_SIZE; j++)
						{
							if (!bullets[j]->isAlive)
							{
								bullets[j]->newBullet(clients[i]->getBulletVec(), 10, clients[i]->player.left, clients[i]->team);
								bulletsNumber++;
								break;
							}
						}
					}
					playersCoord[i] = clients[i]->player.getVec();
				}
				else
				{
					playersCoord[i] = NULL_VECTOR2f;
				}
			}

			for (int i = 0; i < BULLETS_SIZE; i++)
			{
				if (bullets[i]->isAlive)
				{
					bulletHitVec = bullets[i]->update(time, *level, playersCoord);
					if (bulletHitVec != NULL_VECTOR2I)
					{
						if (clients[bulletHitVec.y]->team != bullets[i]->team)
						{
							clients[bulletHitVec.y]->player.hit(bulletHitVec.x);
							bulletsNumber--;
						}
					}
					else
					{
						if (!bullets[i]->isAlive)
						{
							bulletsNumber--;
						}
					}
				}
			}

			//Send
			sPacket << *clientsNumber;
			for (int i = 0; i < CLIENTS_SIZE; i++)
			{
				if (clients[i]->stage == Stage::Playing)
				{
					clients[i]->createPacket(&sPacket);
				}
			}

			sPacket << bulletsNumber;
			for (int i = 0; i < BULLETS_SIZE; i++)
			{
				if (bullets[i]->isAlive)
				{
					bullets[i]->createPacket(&sPacket);
				}
			}

			clientIndex = 0;
			for (int i = 0; i < CLIENTS_SIZE; i++)
			{
				if (clients[i]->stage == Stage::Playing)
				{
					Packet pPacket = sPacket;
					pPacket << clientIndex;
					clients[i]->socket.send(pPacket);
					clientIndex++;
				}
			}
			sPacket.clear();
		}
	}
}