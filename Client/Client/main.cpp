#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <fstream>

#include "Consts.hpp"
#include "Level.hpp"
#include "Animation.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "HealthBar.hpp"
#include "Score.hpp"

using namespace sf;

int main()
{
	enum Stage { Connection, FileAsk, FileAnswer, TeamAsk, TeamAnswer, NameAsk, NameAnswer, PortAsk, PortAnswer, Playing, Error };
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

	Score score("files/OpenSans-Regular.ttf");
	HealthBar healthBar("files/images/healthBar.png");
	AnimationManager playerAnimationManager("files/images/megaman.png", "files/megaman.xml");
	AnimationManager bulletAnimationManager("files/images/bullet.png", "files/bullet.xml");	

	Player** players = new Player*[CLIENTS_SIZE];
	for (int i = 0; i < CLIENTS_SIZE; i++)
	{
		players[i] = new Player(playerAnimationManager);
	}

	Bullet** bullets = new  Bullet*[BULLETS_SIZE];
	for (int i = 0; i < BULLETS_SIZE; i++)
	{
		bullets[i] = new Bullet(bulletAnimationManager);
	}
	
	unsigned char playersNumber = 0, bulletsNumber = 0, myIndex = 0, playerState = 0, score1 = 0, score2 = 0;
	std::string mapFilename, tilesetFilename, backgroundFilename, myName;

	Clock clock;	
	signed __int32 time;

	while (true)
	{
		switch (stage)
		{
			case Stage::Error:
			{
				window.close();
				rPacket.clear();
				tcpSocket.disconnect();
				new(&tcpSocket) TcpSocket;
				std::cout << "Disconnected from the server.\n\n";
				stage = Stage::Connection;
				break;
			}

			case Stage::Connection:		
			{				
				std::fflush(stdin);
				std::fflush(stdout);
				std::cout << "Input server's IP: ";
				std::string sServerIp;
				std::cin >> sServerIp;
				serverIp = IpAddress(sServerIp);
				std::cout << "Input server's port: ";
				std::cin >> serverPort;

				Socket::Status s = tcpSocket.connect(serverIp, serverPort, seconds(2));
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
				{					
					mapFilename = "";
					tilesetFilename = "";
					backgroundFilename = "";
					myName = "";

					stage = Stage::FileAsk;
					sPacket << true;
					tcpSocket.send(sPacket);
					sPacket.clear();					
				}
				break;
			}

			case Stage::FileAsk:
			{
				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
				{
					bool need = true;
					std::string path;
					if (mapFilename == "") 
					{
						rPacket >> mapFilename;
						path = mapFilename;
					}
					else
					{
						if (tilesetFilename == "")
						{
							rPacket >> tilesetFilename;
							path = tilesetFilename;
						}
						else
						{
							// Use myName to store the path to tileset.tsx because it will not be needed
							if (myName == "")
							{
								rPacket >> myName;
								path = myName;
							}
							else
							{
								rPacket >> backgroundFilename;
								path = backgroundFilename;
							}
						}
					}
					rPacket.clear();
					
					std::ifstream file(path, std::ios::binary);
					if (file.good())
					{																		
						file.close();
						need = false;
					}
					else
					{			
						std::cout << "Downloading needed file.\n";
						stage = Stage::FileAnswer;
					}
					
					sPacket << need;
					tcpSocket.send(sPacket);
					sPacket.clear();

					if (!need)
					{
						sPacket << true;
						tcpSocket.send(sPacket);
						sPacket.clear();

						if (backgroundFilename != "")
						{
							stage = Stage::TeamAsk;
						}
					}
				}
				break;
			}

			case Stage::FileAnswer:
			{
				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
				{
					int fileSize;
					rPacket >> fileSize;
					rPacket.clear();

					std::string path;
					if (backgroundFilename != "")
					{
						path += backgroundFilename;
					}
					else
					{
						if (myName != "")
						{
							path += myName;
						}
						else
						{
							if (tilesetFilename != "")
							{
								path += tilesetFilename;
							}
							else
							{
								path += mapFilename;
							}
						}						
					}

					char buffer[1024];
					std::size_t recieved = 0;
					std::ofstream file(path, std::ios::binary);
					while (true)
					{
						Socket::Status s = tcpSocket.receive(buffer, sizeof(buffer), recieved);
						if (s == Socket::Disconnected || s == Socket::Error)
						{
							file.close();
							std::remove(path.c_str());							
							stage = Stage::Error;
							break;
						}
						if (s == Socket::Done)
						{						
							file.write(buffer, recieved);							

							fileSize -= recieved;
							if (fileSize <= 0)
							{
								file.close();
								if (backgroundFilename == "")
								{									
									stage = Stage::FileAsk;
								}
								else
								{
									stage = Stage::TeamAsk;
								}

								sPacket << true;
								tcpSocket.send(sPacket);
								sPacket.clear();
								break;
							}
						}
					}
				}
				break;
			}

			case Stage::TeamAsk:
			{
				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
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

					while (iTeam != 1 && iTeam != 2)
					{
						std::cout << "Incorrect input! Try again: ";
						std::cin >> iTeam;
					}

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
				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
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

				while (myName.length() > 8)
				{
					if (myName.length() > 8)
					{
						std::cout << "Name is too long! Try again: ";
					}				
					std::cin >> myName;
				}

				sPacket << myName;
				tcpSocket.send(sPacket);
				sPacket.clear();

				stage = Stage::NameAnswer;
				break;
			}

			case Stage::NameAnswer:
			{
				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
				{
					bool anotherName;
					rPacket >> anotherName;
					if (anotherName)
					{
						std::cout << "Your name changed to: ";
						rPacket >> myName;
						std::cout << myName + "\n";
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
				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				if (s == Socket::Done)
				{
					rPacket >> serverPort;
					rPacket.clear();

					tcpSocket.setBlocking(false);	

					playerState = 0;
					sPacket << playerState;
					udpSocket.send(sPacket, serverIp, serverPort);
					sPacket.clear();

					offsetX = 0;
					offsetY = 0;
					new(&window) RenderWindow(VideoMode(400, 200), "Call Of Network");
					windowWidthHalf = window.getSize().x / 2;
					windowHeightHalf = window.getSize().y / 2;
					new(&view) View(FloatRect(0, 0, window.getSize().x, window.getSize().y));

					new(&level) Level(mapFilename, tilesetFilename, backgroundFilename);
					mapWidth = level.mapWidth * level.tileWidth;
					mapHeight = level.mapHeight * level.tileHeight;

					new(&clock) Clock();

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

				Socket::Status s = tcpSocket.receive(rPacket);
				if (s == Socket::Disconnected || s == Socket::Error)
				{
					stage = Stage::Error;
					break;
				}

				IpAddress ip;
				if (udpSocket.receive(rPacket, ip, myPort) == Socket::Done)
				{
					rPacket >> score1;
					rPacket >> score2;
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
					udpSocket.send(sPacket, serverIp, serverPort);
					sPacket.clear();
				}
			
				if (clock.getElapsedTime().asMilliseconds() > MSPF)
				{		
					time = clock.getElapsedTime().asMilliseconds();
					clock.restart();

					if (time > MSPF * 2)
					{
						time = MSPF * 2;
					}

					for (int i = 0; i < playersNumber; i++)
					{
						players[i]->update(time, i == myIndex);
					}

					for (int i = 0; i < bulletsNumber; i++)
					{
						bullets[i]->update(time);
					}

					if (players[myIndex]->x <= windowWidthHalf) {
						offsetX = windowWidthHalf - players[myIndex]->x;
					}
					else
					{
						if (players[myIndex]->x >= mapWidth - windowWidthHalf) {
							offsetX = mapWidth - windowWidthHalf - players[myIndex]->x;
						}
					}

					if (players[myIndex]->y <= windowHeightHalf) {
						offsetY = windowHeightHalf - players[myIndex]->y;
					}
					else
					{
						if (players[myIndex]->y >= mapHeight - windowHeightHalf) {
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
					score.update(score1, score2);
					score.draw(window, view.getCenter() - Vector2f(0, 100));

					window.display();					
				}
				break;
			}
		}
	}
}