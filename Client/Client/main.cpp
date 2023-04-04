#include <SFML/Graphics.hpp>
#include <list>
#include "Level.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Bullet.hpp"
#include "Player.hpp"
//#include "MovingPlatform.hpp"
//#include "HealthBar.hpp"

using namespace sf;

int main()
{
	RenderWindow window(VideoMode(450, 280), "Call Of Network");

	View view(FloatRect(0, 0, 450, 280));

	Level level("files/images/tileset2.png", "files/mymap.tmx");
	
	Texture enemy_t, moveplatform_t, bg;
	bg.loadFromFile("files/images/bg.png");

	moveplatform_t.loadFromFile("files/images/movingPlatform.png");


	//AnimationManager anim4;
	//anim4.create("move", moveplatform_t, 0, 0, 95, 22, 1, 0);*/

	Sprite background(bg);
	background.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);

	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;


	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	AnimationManager playerAnimationManager("files/images/megaman.png", "files/myanim.xml");
	AnimationManager bulletAnimationManager("files/images/bullet.png", "files/bullet.xml");
	Player Mario(playerAnimationManager, level, 100);

	//HealthBar healthBar;

	Clock clock;
	
	while (window.isOpen())
	{
		if (clock.getElapsedTime().asMilliseconds() > 10)
		{
			float time = clock.getElapsedTime().asMicroseconds();
			clock.restart();

			time = time / 500;

			if (time > 40)
			{
				time = 40;
			}

			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					window.close();
				}
			}


			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				Mario.keys[Player::Key::Left] = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				Mario.keys[Player::Key::Right] = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Up))
			{
				Mario.keys[Player::Key::Up] = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Down))
			{
				Mario.keys[Player::Key::Down] = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				Mario.keys[Player::Key::Space] = true;

			}

			Mario.update(time);
			if (Mario.shoot)
			{
				entities.push_back(new Bullet(bulletAnimationManager, level, 
					Vector2f(Mario.rect.left + Mario.rect.width / 2, Mario.rect.top), 1, Mario.left));
			}

			it = entities.begin();
			while (it != entities.end())
			{
				(*it)->update(time);
				if ((*it)->isAlive == false)
				{
					it = entities.erase(it);
				}
				else it++;
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
			}*/


			view.setCenter(Mario.rect.left, Mario.rect.top);
			window.setView(view);

			background.setPosition(view.getCenter());
			window.draw(background);

			level.draw(window);

			for (it = entities.begin(); it != entities.end(); it++)
			{
				(*it)->draw(window);
			}

			Mario.draw(window);
			//healthBar.draw(window);

			window.display();
		}
	}
}