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
#include <fstream>

const int mspf = 1000 / 120;

using namespace sf;

int main()
{
	enum Stage { Connection, CheckFiles, TeamAsk, TeamAnswer, NameAsk, NameAnswer, PortAsk, PortAnswer, Playing, Error };
	IpAddress serverIp;
	unsigned short serverPort, myPort;

	Stage stage = Stage::Connection;
	TcpSocket tcpSocket;
	UdpSocket udpSocket;
	Packet rPacket, sPacket;

	float offsetX, offsetY;	
	RenderWindow window;
	int windowWidthHalf = 0, windowHeightHalf = 0, mapWidth = 0, mapHeight = 0;
	View view;
	Level level;

	//moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	//AnimationManager anim4;
	//anim4.create("move", moveplatform_t, 0, 0, 95, 22, 1, 0);*/

	HealthBar healthBar("files/images/healthBar.png");
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
	unsigned char playersNumber = 0, bulletsNumber = 0, myIndex = 0, playerState = 0;
	std::string mapFileName, tilesetFileName, backgroundFileName, myName;

	Clock clock;
	signed __int32 time;

	while (true)
	{
		switch (stage)
		{
			case Stage::Error:
			{
				rPacket.clear();
				tcpSocket.disconnect();
				new(&tcpSocket) TcpSocket;
				std::cout << "Disconnected from the server.\n\n";
				stage = Stage::Connection;
				break;
			}

			case Stage::Connection:		
			{
				std::cout.flush();
				std::cout << "Input server's IP: ";
				std::string sServerIp;
				std::cin >> sServerIp;
				serverIp = IpAddress(sServerIp);
				std::cout << "Input server's port: ";
				std::cin >> serverPort;

				Socket::Status s = tcpSocket.connect(serverIp, serverPort, seconds(5));
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
				{
					tcpSocket.setBlocking(false);
					stage = Stage::CheckFiles;
				}
				break;
			}

			case Stage::CheckFiles:
			{
				if (tcpSocket.receive(rPacket) == Socket::Done && rPacket)
				{
					rPacket >> mapFileName;
					std::ifstream file("files/" + mapFileName);
					if (!file.is_open())
					{
						std::cout << "You miss the map file. Go download it from https://CallOfNetwork.com/maps\n";
						stage = Stage::Error;
						break;
					}
					else
					{
						file.close();
					}

					rPacket >> tilesetFileName;
					new(&file) std::ifstream("files/images/" + tilesetFileName);
					if (!file.is_open())
					{
						std::cout << "You miss the tileset file. Go download it from https://CallOfNetwork.com/tilesets\n";
						stage = Stage::Error;
						break;
					}
					else
					{
						file.close();
					}

					rPacket >> backgroundFileName;
					rPacket.clear();
					new(&file) std::ifstream("files/images/" + backgroundFileName);
					if (!file.is_open())
					{
						std::cout << "You miss the background file. Go download it from https://CallOfNetwork.com/backgrounds\n";
						stage = Stage::Error;
						break;
					}
					else
					{
						file.close();
					}
					stage = Stage::TeamAsk;
				}
				break;
			}

			case Stage::TeamAsk:
			{
				if (tcpSocket.receive(rPacket) == Socket::Done && rPacket)
				{
					unsigned char uTeam = 0;
					int iTeam;
					rPacket >> uTeam;
					iTeam = uTeam;
					std::cout << "There are " << iTeam << " players in the first team and ";

					rPacket >> uTeam;
					rPacket.clear();
					iTeam = uTeam;
					std::cout << iTeam << " players in the second one. \nChoose your team (input 1 or 2): ";
					std::cin >> iTeam;

					bool team = iTeam == 1;
					sPacket << team;
					tcpSocket.send(sPacket);
					sPacket.clear();
					stage = Stage::TeamAnswer;
				}
				break;
			}

			case Stage::TeamAnswer:
			{
				if (tcpSocket.receive(rPacket) == Socket::Done && rPacket)
				{
					bool autoBalanced;
					rPacket >> autoBalanced;
					rPacket.clear();

					if (autoBalanced)
					{
						std::cout << "You were autobalanced into another team.\n";
					}
					stage = Stage::NameAsk;
				}
				break;
			}

			case Stage::NameAsk:	
			{
				std::cout << "Enter you name: ";
				std::cin >> myName;

				sPacket << myName;
				tcpSocket.send(sPacket);
				sPacket.clear();

				stage = Stage::NameAnswer;
				break;
			}

			case Stage::NameAnswer:
			{
				if (tcpSocket.receive(rPacket) == Socket::Done && rPacket)
				{
					bool anotherName;
					rPacket >> anotherName;
					if (anotherName)
					{
						std::cout << "Your name changed to: ";
						rPacket >> myName;
						std::cout << myName;
					}
					rPacket.clear();

					stage = Stage::PortAsk;
				}
				break;
			}

			case Stage::PortAsk:
			{								
				udpSocket.setBlocking(false);
				udpSocket.bind(Socket::AnyPort);
					
				myPort = udpSocket.getLocalPort();
				sPacket << myPort;
				tcpSocket.send(sPacket);
				sPacket.clear();

				stage = Stage::PortAnswer;
				break;
			}

			case Stage::PortAnswer:
			{
				if (tcpSocket.receive(rPacket) == Socket::Done && rPacket)
				{
					rPacket >> serverPort;
					rPacket.clear();

					playerState = 0;
					sPacket << playerState;
					udpSocket.send(sPacket, serverIp, serverPort);
					sPacket.clear();

					//printf("ip: %s, port: %d, my port: %d", serverIp.toString().c_str(), serverPort, udpSocket.getLocalPort());

					offsetX = 0;
					offsetY = 0;
					new(&window) RenderWindow(VideoMode(400, 200), "Call Of Network");
					windowWidthHalf = window.getSize().x / 2;
					windowHeightHalf = window.getSize().y / 2;
					new(&view) View(FloatRect(0, 0, window.getSize().x, window.getSize().y));

					new(&level) Level("files/" + mapFileName, "files/images/" + tilesetFileName, "files/images/" + backgroundFileName);
					mapWidth = level.mapWidth * level.tileWidth;
					mapHeight = level.mapHeight * level.tileHeight;

					std::cout << "Press Right Shift to respawn.\n";
					stage = Stage::Playing;
				}
				break;
			}

			case Stage::Playing:
			{
				Event event;
				while (window.pollEvent(event))
				{
					if (event.type == Event::Closed)
					{
						window.close();
						stage = Stage::Error;
					}
				}
				if (udpSocket.receive(rPacket, serverIp, myPort) == Socket::Done && rPacket)
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
				//}

				//if (clock.getElapsedTime().asMilliseconds() > mspf)
				//{
					playerState = 0;
					if (Keyboard::isKeyPressed(Keyboard::Left))
					{
						playerState = playerState | KEY_LEFT;
					}
					if (Keyboard::isKeyPressed(Keyboard::Right))
					{
						playerState = playerState | KEY_RIGHT;
					}

					if (Keyboard::isKeyPressed(Keyboard::Up))
					{
						playerState = playerState | KEY_UP;
					}
					if (Keyboard::isKeyPressed(Keyboard::Down))
					{
						playerState = playerState | KEY_DOWN;
					}

					if (Keyboard::isKeyPressed(Keyboard::Space))
					{
						playerState = playerState | KEY_SPACE;
					}
					if (Keyboard::isKeyPressed(Keyboard::RShift))
					{
						playerState = playerState | KEY_RSHIFT;
					}				
					sPacket << playerState;
					/*playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Left) << 4);
					playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Right) << 3);
					playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Up) << 2);
					playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Down) << 1);
					playerState = playerState | ((unsigned char)Keyboard::isKeyPressed(Keyboard::Space));*/
					udpSocket.send(sPacket, serverIp, serverPort);					
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

					window.clear();
					view.setCenter(players[myIndex]->x + offsetX, players[myIndex]->y + offsetY);
					window.setView(view);

					level.draw(window, view.getCenter());

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
}