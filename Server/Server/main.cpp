#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Consts.hpp"
#include "Level.hpp"
#include "Entity.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
#include "Client.hpp"
//#include "MovingPlatform.hpp"
//#include "HealthBar.hpp"

using namespace sf;

int main()
{
	TcpListener listener;
	Packet rPacket, sPacket;

	Level level("files/mymap.tmx");
	int mapWidth = level.mapWidth * level.tileWidth;
	int mapHeight = level.mapHeight * level.tileHeight;

	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	Client** clients = new Client*[CLIENTS_SIZE];
	for (int i = 0; i < CLIENTS_SIZE; i++)
	{		
		clients[i] = new Client(level, 100);
	}
	int clientsNumber = 0;

	Vector2f playersCoord[CLIENTS_SIZE];

	Bullet** bullets = new  Bullet*[BULLETS_SIZE];	
	for (int i = 0; i < BULLETS_SIZE; i++)
	{
		bullets[i] = new Bullet(NULL_VECTOR2f, 10, false);
	}		
	int bulletsNumber = 0;

	Vector2i bulletHitVec;

	int clientIndex = 0;

	Clock clock;
	signed __int32 time;

	listener.listen(2000);
	while (true)
	{
		for (int i = 0; i < CLIENTS_SIZE; i++)
		{
			if (!clients[i]->connected)
			{
				if (listener.accept(clients[i]->socket) == Socket::Done)
				{
					printf("\nAccepted!");
					if (listener.isBlocking())
					{
						listener.setBlocking(false);
					}
					if (clients[i]->socket.isBlocking())
					{
						clients[i]->socket.setBlocking(false);
					}
					clients[i]->connected = true;
					clientsNumber++;
				}
			}			
		}
		
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
				if (clients[i]->connected)
				{
					if (clients[i]->socket.isBlocking())
					{
						clients[i]->socket.setBlocking(false);
					}

					Socket::Status s = clients[i]->socket.receive(rPacket);
					if (s == Socket::Disconnected)
					{
						printf("\nDisconnected!");
						clients[i]->socket.disconnect();
						clients[i]->connected = false;
						clientsNumber--;
						rPacket.clear();
						continue;
					}
					if (s == Socket::Done)
					{
						//printf("\nReceived!");
						clients[i]->updateState(&rPacket);
						rPacket.clear();
					}

					if (clients[i]->updatePlayer(time, level))
					{
						for (int j = 0; j < BULLETS_SIZE; j++)
						{
							if (!bullets[j]->isAlive)
							{
								bullets[j]->newBullet(clients[i]->getBulletVec(), 10, clients[i]->player.left);
								bulletsNumber++;
								break;
							}
						}
					}
					if (clients[i]->player.respawn)
					{
						clients[i]->player.newPlayer(level, 100);
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
					bulletHitVec = bullets[i]->update(time, level, playersCoord);
					if (bulletHitVec != NULL_VECTOR2I)
					{
						clients[bulletHitVec.y]->playerHit(bulletHitVec.x);
						bulletsNumber--;
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

			sPacket << clientsNumber;
			for (int i = 0; i < CLIENTS_SIZE; i++)
			{
				if (clients[i]->connected)
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
				if (clients[i]->connected)
				{
					Packet pPacket = sPacket;
					pPacket << clientIndex;
					clients[i]->socket.send(pPacket);
					clientIndex++;
				}
			}
			sPacket.clear();

			/*
				healthBar.update(Mario.Health);

				for (it = entities.begin(); it != entities.end(); it++)
				{
					//2. движущиеся платформы
					if ((*it)->Name == "MovingPlatform")
					{
						Entity* movPlat = *it;
						if (Mario.getRect().intersects(movPlat->getRect()))
							if (Mario.dy > 0)
								if (Mario.y + Mario.h < movPlat->y + movPlat->h)
								{
									Mario.y = movPlat->y - Mario.h + 3; Mario.x += movPlat->dx * time; Mario.dy = 0; Mario.STATE = PLAYER::stay;
								}
					}
				}
			*/
		}
	}
}