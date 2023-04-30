#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Consts.hpp"
#include "Level.hpp"
#include "Animation.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "HealthBar.hpp"
//#include "MovingPlatform.hpp"
#include <iostream>

const int mspf = 1000 / 120;

using namespace sf;

enum class Stage { Connection, TeamAsk, TeamAnswer, PlayersInfo, Game };
int main()
{
	std::string serverIP;
	unsigned short serverPort;

	Stage stage = Stage::Connection;
	TcpSocket socket;
	Packet rPacket, sPacket;

	float offsetX = 0;
	float offsetY = 0;
	RenderWindow window(VideoMode(400, 200), "Call Of Network");
	int windowWidthHalf = window.getSize().x / 2;
	int windowHeightHalf = window.getSize().y / 2;
	View view(FloatRect(0, 0, window.getSize().x, window.getSize().y));

	Level level("files/images/tileset2.png", "files/mymap.tmx");
	int mapWidth = level.mapWidth * level.tileWidth;
	int mapHeight = level.mapHeight * level.tileHeight;
	
	Texture moveplatform_t, tBackground;
	tBackground.loadFromFile("files/images/bg.png");
	Sprite background(tBackground);
	background.setOrigin(tBackground.getSize().x / 2, tBackground.getSize().y / 2);

	HealthBar healthBar("files/images/healthBar.png");

	//moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	//AnimationManager anim4;
	//anim4.create("move", moveplatform_t, 0, 0, 95, 22, 1, 0);*/

	AnimationManager playerAnimationManager("files/images/megaman.png", "files/myanim.xml");
	AnimationManager bulletAnimationManager("files/images/bullet.png", "files/bullet.xml");

	Player** players = new Player*[CLIENTS_SIZE];
	for (int i = 0; i < CLIENTS_SIZE; i++)
	{
		players[i] = new Player(playerAnimationManager, 100);
	}

	Bullet** bullets = new  Bullet*[BULLETS_SIZE];
	for (int i = 0; i < BULLETS_SIZE; i++)
	{
		bullets[i] = new Bullet(bulletAnimationManager);
	}

	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	//unsigned char playerState;	
	unsigned char playersNumber = 0, bulletsNumber = 0, myIndex = 0;

	Clock clock;
	signed __int32 time;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
		}

		switch (stage)
		{
		case Stage::Connection:
			std::cout << "Input server's IP: ";
			std::cin >> serverIP;
			std::cout << "Input server's port: ";
			std::cin >> serverPort;

			if (socket.connect(IpAddress(serverIP), serverPort, seconds(5)) == Socket::Done)
			{
				socket.setBlocking(false);
				stage = Stage::TeamAsk;
			}
			break;

		case Stage::TeamAsk:
			if (socket.receive(rPacket) == Socket::Done && rPacket)
			{
				unsigned char team1 = 0, team2 = 0;
				rPacket >> team1;
				rPacket >> team2;
				rPacket.clear();

				std::cout << "There are " << team1 << " players in the first team and " << team2 << " players in the second one. \nChoose your team (input 1 or 2): ";
				std::cin >> team1;

				bool team = team1 == 1;
				sPacket << team;
				socket.send(sPacket);					
				sPacket.clear();
				stage = Stage::TeamAnswer;
			}			
			break;

		case Stage::TeamAnswer:
			// Maybe autobalance
			stage = Stage::PlayersInfo;
			break;

		case Stage::PlayersInfo:
			if (socket.receive(rPacket) == Socket::Done && rPacket)
			{
				printf("Connected!");
				rPacket >> playersNumber;
				for (int i = 0; i < playersNumber; i++)
				{
					players[i]->receivePacket(&rPacket);
				}

				rPacket >> bulletsNumber;
				for (int i = 0; i < bulletsNumber; i++)
				{
					bullets[i]->receivePacket(&rPacket);
				}

				rPacket >> myIndex;
				rPacket.clear();
				stage = Stage::Game;
			}
			break;

		case Stage::Game:
			if (socket.receive(rPacket) == Socket::Done && rPacket)
			{
				rPacket >> playersNumber;
				for (int i = 0; i < playersNumber; i++)
				{
					players[i]->receivePacket(&rPacket);
				}

				rPacket >> bulletsNumber;
				for (int i = 0; i < bulletsNumber; i++)
				{
					bullets[i]->receivePacket(&rPacket);
				}

				rPacket >> myIndex;
				rPacket.clear();
			}


			if (clock.getElapsedTime().asMilliseconds() > mspf)
			{
				//playerState = 0;
				/*playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Left) << 4);
				playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Right) << 3);
				playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Up) << 2);
				playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Down) << 1);
				playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Space));*/
				//printf("%d", playerState);
				//printf("Sent!\n");
				sPacket << Keyboard::isKeyPressed(Keyboard::Left);
				sPacket << Keyboard::isKeyPressed(Keyboard::Right);
				sPacket << Keyboard::isKeyPressed(Keyboard::Up);
				sPacket << Keyboard::isKeyPressed(Keyboard::Down);
				sPacket << Keyboard::isKeyPressed(Keyboard::Space);
				socket.send(sPacket);
				sPacket.clear();

				time = clock.getElapsedTime().asMilliseconds();
				clock.restart();

				if (time > mspf * 2)
				{
					time = mspf * 2;
				}

				for (int i = 0; i < playersNumber; i++)
				{
					players[i]->update(time, i == myIndex);
				}

				for (int i = 0; i < bulletsNumber; i++)
				{
					bullets[i]->update(time);
				}

				if (players[myIndex]->x < windowWidthHalf) {
					offsetX = windowWidthHalf - players[myIndex]->x;
				}
				else
				{
					if (players[myIndex]->x > mapWidth - windowWidthHalf) {
						offsetX = mapWidth - windowWidthHalf - players[myIndex]->x;
					}
				}

				if (players[myIndex]->y < windowHeightHalf) {
					offsetY = windowHeightHalf - players[myIndex]->y;
				}
				else
				{
					if (players[myIndex]->y > mapHeight - windowHeightHalf) {
						offsetY = mapHeight - windowHeightHalf - players[myIndex]->y;
					}
				}

				/*for (it = entities.begin(); it != entities.end(); it++)
				{
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

				window.clear();
				view.setCenter(players[myIndex]->x + offsetX, players[myIndex]->y + offsetY);
				window.setView(view);

				background.setPosition(view.getCenter());
				window.draw(background);

				level.draw(window);

				for (int i = 0; i < playersNumber; i++)
				{
					if (i != myIndex)
					{
						players[i]->draw(window);
					}
				}
				players[myIndex]->draw(window);

				for (int i = 0; i < bulletsNumber; i++)
				{
					bullets[i]->draw(window);
				}

				healthBar.update(players[myIndex]->health);
				healthBar.draw(window, view.getCenter() - Vector2f(200, 100));

				window.display();
			}
			break;
		}
	}
}