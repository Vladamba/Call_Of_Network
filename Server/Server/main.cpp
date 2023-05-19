#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <thread>
#include <iostream>
#include <fstream>

#include "Consts.hpp"
#include "Level.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Client.hpp"

using namespace sf;

void newClient(Level* level, Client** clients, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber);
void updateClients(Level* level, Client** clients, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber);

int main()
{	
	std::string mapFilename = "", tilesetFilename = "", backgroundFilename = "";

	while (backgroundFilename == "")
	{		
		bool f = true;
		std::string path;
		while (f)
		{
			if (mapFilename == "")
			{
				std::cout << "Input map filename: ";
			}
			else
			{
				if (tilesetFilename == "")
				{
					std::cout << "Input tileset filename: ";
				}
				else
				{
					std::cout << "Input background filename: ";
				}
			}
			
			std::cin >> path;
			std::ifstream file(path, std::ios::binary);
			if (file.good())
			{
				file.close();
				f = false;
			}
			else
			{
				std::cout << "No file found! Try again.\n";
			}
		}

		if (mapFilename == "")
		{
			mapFilename = path;
		}
		else
		{
			if (tilesetFilename == "")
			{
				tilesetFilename = path;
			}
			else
			{
				backgroundFilename = path;
			}
		}
	}

	Level level(mapFilename, tilesetFilename, backgroundFilename);
	mapFilename = "";
	tilesetFilename = "";
	backgroundFilename = "";


	Client** clients = new Client*[CLIENTS_SIZE];
	for (int i = 0; i < CLIENTS_SIZE; i++)
	{		
		clients[i] = new Client(level);
	}
	 
	unsigned char clientsNumber = 0, team1 = 0, team2 = 0;

	std::thread tNewClient(newClient, &level, clients, &team1, &team2, &clientsNumber);
	std::thread tUpdateClients(updateClients, &level, clients, &team1, &team2, &clientsNumber);
	
	tNewClient.join();
	tUpdateClients.join();
	return 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////

void newClient(Level* level, Client** clients, unsigned char* team1, unsigned char* team2, unsigned char* clientsNumber)
{
	TcpListener listener;
	Packet rPacket, sPacket;

	listener.listen(Socket::AnyPort);	
	std::cout << "My IP is: " <<  IpAddress::getLocalAddress().toString() << "\nMy port is: " << listener.getLocalPort();

	while (true)
	{
		for (int i = 0; i < CLIENTS_SIZE; i++)
		{
			switch (clients[i]->stage)
			{
				case Stage::Error:
				{
					rPacket.clear();
					std::cout << "\n" << clients[i]->name << " disconnected!";
					clients[i]->disconnect();
					break;
				}

				case Stage::Connection:
				{
					if (listener.accept(clients[i]->tcpSocket) == Socket::Done)
					{
						std::cout << "\nAccepted new player!";
						if (listener.isBlocking())
						{
							listener.setBlocking(false);
						}
						clients[i]->tcpSocket.setBlocking(false);
						
						clients[i]->filename = level->mapFilename;
						clients[i]->stage = Stage::FileAsk;
					}
					break;
				}					

				case Stage::FileAsk:
				{
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						clients[i]->stage = Stage::Error;
						break;
					}

					if (s == Socket::Done)
					{
						rPacket.clear();
						sPacket << clients[i]->filename;
						clients[i]->tcpSocket.send(sPacket);
						sPacket.clear();
						clients[i]->stage = Stage::FileAnswer;
					}
					break;
				}

				case Stage::FileAnswer:
				{
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						clients[i]->stage = Stage::Error;
						break;
					}

					if (s == Socket::Done)
					{
						bool need;
						rPacket >> need;
						rPacket.clear();

						if (need)
						{					
							clients[i]->tcpSocket.setBlocking(true);
							std::ifstream file(clients[i]->filename, std::ios::binary);
							file.seekg(0, std::ios::end);
							int fileSize = file.tellg();
							file.close();

							sPacket << fileSize;
							clients[i]->tcpSocket.send(sPacket);
							sPacket.clear();

							file.open(clients[i]->filename, std::ios::binary);
							char buffer[1024];
							while (file.good())
							{
								file.read(buffer, sizeof(buffer));
								clients[i]->tcpSocket.send(buffer, file.gcount());
							}
							clients[i]->tcpSocket.setBlocking(false);
						}

						if (clients[i]->filename == level->backgroundFilename)
						{
							clients[i]->stage = Stage::TeamAsk;
							break;
						}

						if (clients[i]->filename == level->tilesetTsxFilename)
						{
							clients[i]->filename = level->backgroundFilename;
						}

						if (clients[i]->filename == level->tilesetFilename)
						{
							clients[i]->filename = level->tilesetTsxFilename;
						}

						if (clients[i]->filename == level->mapFilename)
						{
							clients[i]->filename = level->tilesetFilename;
						}
						clients[i]->stage = Stage::FileAsk;
					}
					break;
				}

				case Stage::TeamAsk:
				{
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						clients[i]->stage = Stage::Error;
						break;
					}

					if (s == Socket::Done)
					{
						sPacket << *team1;
						sPacket << *team2;
						clients[i]->tcpSocket.send(sPacket);
						sPacket.clear();
						clients[i]->stage = Stage::TeamAnswer;
					}
					break;
				}

				case Stage::TeamAnswer:
				{
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
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
						clients[i]->tcpSocket.send(sPacket);
						sPacket.clear();
						clients[i]->stage = Stage::NameAsk;
					}
					break;
				}

				case Stage::NameAsk:
				{
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
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
								clients[i]->name = clients[i]->name + "_1";
								break;
							}
						}

						sPacket << anotherName;
						sPacket << clients[i]->name;
						clients[i]->tcpSocket.send(sPacket);
						sPacket.clear();

						clients[i]->stage = Stage::PortAsk;						
					}
					break;
				}

				case Stage::PortAsk:
				{
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						clients[i]->stage = Stage::Error;
						break;
					}

					if (s == Socket::Done)
					{
						rPacket >> clients[i]->clientPort;
						rPacket.clear();

						clients[i]->udpSocket.setBlocking(false);
						clients[i]->udpSocket.bind(Socket::AnyPort);

						clients[i]->ip = clients[i]->tcpSocket.getRemoteAddress();
						clients[i]->serverPort = clients[i]->udpSocket.getLocalPort();

						sPacket << clients[i]->serverPort;
						clients[i]->tcpSocket.send(sPacket);
						sPacket.clear();
					
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

	unsigned char bulletsNumber = 0, clientIndex = 0, score1 = 0, score2 = 0;

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
					Socket::Status s = clients[i]->tcpSocket.receive(rPacket);
					if (s == Socket::Disconnected || s == Socket::Error)
					{
						rPacket.clear();
						std::cout << "\n" << clients[i]->name << " disconnected!";
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

					IpAddress ip;
					if (clients[i]->udpSocket.receive(rPacket, ip, clients[i]->serverPort) == Socket::Done)
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
							bullets[i]->isAlive = false;
							if (clients[bulletHitVec.y]->player.hit(bulletHitVec.x))
							{
								if (bullets[i]->team)
								{
									score1++;
								}
								else
								{
									score2++;
								}
							}
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


			sPacket << score1;
			sPacket << score2;
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
					clients[i]->udpSocket.send(pPacket, clients[i]->ip, clients[i]->clientPort);
					clientIndex++;
				}
			}
			sPacket.clear();
		}
	}
}