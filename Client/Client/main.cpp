#include <SFML/Graphics.hpp>
#include <list>
#include "Level.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
//#include "MovingPlatform.hpp"
//#include "HealthBar.hpp"

const int mspf = 1000 / 120;

using namespace sf;

int main()
{
	float offsetX = 0;
	float offsetY = 0;
	RenderWindow window(VideoMode(400, 200), "Call Of Network");
	int windowWidthHalf = window.getSize().x / 2;
	int windowHeightHalf = window.getSize().y / 2;
	View view(FloatRect(0, 0, window.getSize().x, window.getSize().y));

	Level level("files/images/tileset2.png", "files/mymap.tmx");
	int mapWidth = level.mapWidth * level.tileWidth;
	int mapHeight = level.mapHeight * level.tileHeight;
	
	Texture enemy_t, moveplatform_t, tBackground;
	tBackground.loadFromFile("files/images/bg.png");

	//moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	//AnimationManager anim4;
	//anim4.create("move", moveplatform_t, 0, 0, 95, 22, 1, 0);*/

	Sprite background(tBackground);
	background.setOrigin(tBackground.getSize().x / 2, tBackground.getSize().y / 2);

	std::list<Bullet> entities;
	std::list<Bullet>::iterator it;

	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	AnimationManager playerAnimationManager("files/images/megaman.png", "files/myanim.xml");
	AnimationManager bulletAnimationManager("files/images/bullet.png", "files/bullet.xml");
	Player Mario(playerAnimationManager, level, 100);

	std::vector<FloatRect> players;
	players.push_back(Mario.rect);

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

		Mario.keys[Player::Key::Left] = Keyboard::isKeyPressed(Keyboard::Left);
		Mario.keys[Player::Key::Right] = Keyboard::isKeyPressed(Keyboard::Right);
		Mario.keys[Player::Key::Up] = Keyboard::isKeyPressed(Keyboard::Up);
		Mario.keys[Player::Key::Down] = Keyboard::isKeyPressed(Keyboard::Down);
		Mario.keys[Player::Key::Space] = Keyboard::isKeyPressed(Keyboard::Space);

		if (clock.getElapsedTime().asMilliseconds() > mspf)
		{

			time = clock.getElapsedTime().asMilliseconds();
			clock.restart();

			if (time > mspf * 2)
			{
				time = mspf * 2;
			}

			//float time, ObjectType** map, int mapWidth, int mapHeight, int tileWidth, int tileHeight
			Mario.update(time, level);
			if (Mario.shoot)
			{
				entities.push_back(Bullet(bulletAnimationManager, Vector2f(Mario.rect.left + Mario.rect.width / 2, Mario.rect.top), 1, Mario.left));
			}

			it = entities.begin();
			while (it != entities.end())
			{
				(*it).update(time, level, players);
				if ((*it).isAlive == false)
				{
					it = entities.erase(it);
				}
				else it++;
			}

			if (Mario.rect.left < windowWidthHalf) {
				offsetX = windowWidthHalf - Mario.rect.left;
			}
			else
			{
				if (Mario.rect.left > mapWidth - windowWidthHalf) {
					offsetX = mapWidth - windowWidthHalf - Mario.rect.left;
				}
			}

			if (Mario.rect.top < windowHeightHalf) {
				offsetY = windowHeightHalf - Mario.rect.top;
			}
			else
			{
				if (Mario.rect.top > mapHeight - windowHeightHalf) {
					offsetY = mapHeight - windowHeightHalf - Mario.rect.top;
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
			view.setCenter(Mario.rect.left + offsetX, Mario.rect.top + offsetY);
			window.setView(view);

			background.setPosition(view.getCenter());
			window.draw(background);

			level.draw(window);

			for (it = entities.begin(); it != entities.end(); it++)
			{
				(*it).draw(window);
			}

			Mario.draw(window);
			//healthBar.draw(window);

			window.display();
		}
	}
}