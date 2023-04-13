#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Consts.hpp"
#include "Level.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
//#include "MovingPlatform.hpp"
//#include "HealthBar.hpp"

const int mspf = 1000 / 120;

using namespace sf;

enum class State { Connection, Informarion, Game };
int main()
{
	State state = State::Connection;
	TcpSocket socket;
	Packet rPacket, sPacket;

	float offsetX = 0;
	float offsetY = 0;
	RenderWindow window(VideoMode(400, 200), "Call Of Network");
	int windowWidthHalf = window.getSize().x / 2;
	int windowHeightHalf = window.getSize().y / 2;
	View view(FloatRect(0, 0, window.getSize().x, window.getSize().y));

	Level level("files/images/tileset2.png", "files/mymap.tmx"); // should send just file without image
	int mapWidth = level.mapWidth * level.tileWidth;
	int mapHeight = level.mapHeight * level.tileHeight;
	
	Texture enemy_t, moveplatform_t, tBackground;
	tBackground.loadFromFile("files/images/bg.png");

	//moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	//AnimationManager anim4;
	//anim4.create("move", moveplatform_t, 0, 0, 95, 22, 1, 0);*/

	Sprite background(tBackground);
	background.setOrigin(tBackground.getSize().x / 2, tBackground.getSize().y / 2);

	AnimationManager playerAnimationManager("files/images/megaman.png", "files/myanim.xml");
	AnimationManager bulletAnimationManager("files/images/bullet.png", "files/bullet.xml");

	Player** players = new Player*[CLIENTS_SIZE];
	for (int i = 0; i < CLIENTS_SIZE; i++)
	{
		players[i] = new Player(playerAnimationManager, 100);
	}
	int playersNumber = 0;

	Bullet** bullets = new  Bullet*[BULLETS_SIZE];
	for (int i = 0; i < BULLETS_SIZE; i++)
	{
		bullets[i] = new Bullet(bulletAnimationManager);
	}
	int bulletsNumber = 0;

	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	unsigned char playerState;
	int myIndex = 0;

	//HealthBar healthBar;

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

		if (state == State::Connection)
		{
			if (socket.connect(IpAddress("192.168.62.172"), 2000, seconds(5)) == Socket::Done)
			{				
				socket.setBlocking(false);
				state = State::Informarion;		
			}
		}

		if (state == State::Informarion)
		{
			if (socket.receive(rPacket) == Socket::Done && rPacket)
			{		
				printf("\nReceived!");
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
				state = State::Game;
				continue;
			}
		}

		if (state == State::Game)
		{
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
				playerState = 0;
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
					players[i]->update(time);
				}

				for (int i = 0; i < bulletsNumber; i++)
				{
					bullets[i]->update(time);
				}


				if (players[myIndex]->rect.left < windowWidthHalf) {
					offsetX = windowWidthHalf - players[myIndex]->rect.left;
				}
				else
				{
					if (players[myIndex]->rect.left > mapWidth - windowWidthHalf) {
						offsetX = mapWidth - windowWidthHalf - players[myIndex]->rect.left;
					}
				}

				if (players[myIndex]->rect.top < windowHeightHalf) {
					offsetY = windowHeightHalf - players[myIndex]->rect.top;
				}
				else
				{
					if (players[myIndex]->rect.top > mapHeight - windowHeightHalf) {
						offsetY = mapHeight - windowHeightHalf - players[myIndex]->rect.top;
					}
				}


				//healthBar.update(Mario.Health);

				/*for (it = entities.begin(); it != entities.end(); it++)
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

				if (((int)characters[0].character->getRect().left > wWidth / 2) &&
					((int)characters[0].character->getRect().left < mWidth * tsWidth - wWidth / 2)) {
					offsetX = (characters[0].character->getRect().left - (float)wWidth / 2.0f);
				}
				if (((int)characters[0].character->getRect().top > wHeight / 2) &&
					((int)characters[0].character->getRect().top < mHeight * tsHeight - wHeight / 2)) {
					offsetY = (characters[0].character->getRect().top - (float)wHeight / 2.0f);
				}
				*/

				//window.clear();
				view.setCenter(players[myIndex]->rect.left + offsetX, players[myIndex]->rect.top + offsetY);
				window.setView(view);

				background.setPosition(view.getCenter());
				window.draw(background);

				level.draw(window);

				for (int i = 0; i < playersNumber; i++)
				{
					players[i]->draw(window);
				}

				for (int i = 0; i < bulletsNumber; i++)
				{
					bullets[i]->draw(window);
				}

				//healthBar.draw(window);

				window.display();
			}
		}
	}
}