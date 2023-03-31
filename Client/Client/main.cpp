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

	Level lvl;
	lvl.loadFromXML("files\\Level2.tmx");
	
	Texture enemy_t, moveplatform_t, megaman_t, bullet_t, bg;
	bg.loadFromFile("files/images/bg.png");
	enemy_t.loadFromFile("files/images/enemy.png");
	moveplatform_t.loadFromFile("files/images/movingPlatform.png");
	megaman_t.loadFromFile("files/images/megaman.png");
	bullet_t.loadFromFile("files/images/bullet.png");

	AnimationManager anim("files\\anim_megaman.xml", megaman_t);
	//if (!anim.loadFromXML("files\\anim_megaman.xml", megaman_t))
	{
		return 2;
	}
	anim.animationList["jump"].loop = 0;

	AnimationManager anim2("files\\bullet.xml", bullet_t);
	//if (!anim2.loadFromXML("files\\bullet.xml", bullet_t))
	{
		return 2;
	}

	/*AnimationManager anim3;
	anim3.create("move", enemy_t, 0, 0, 16, 16, 2, 0.002, 18);
	anim3.create("dead", enemy_t, 58, 0, 16, 16, 1, 0);

	AnimationManager anim4;
	anim4.create("move", moveplatform_t, 0, 0, 95, 22, 1, 0);*/

	Sprite background(bg);
	background.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);

	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;

	//std::vector<Object> e = lvl.GetObjects("enemy");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new ENEMY(anim3, lvl, e[i].rect.left, e[i].rect.top));

	//e = lvl.GetObjects("MovingPlatform");
	//for (int i = 0; i < e.size(); i++)
		//entities.push_back(new MovingPlatform(anim4, lvl, e[i].rect.left, e[i].rect.top));

	Object pl = lvl.getObject("player");
	Player Mario(anim, lvl, pl.rect.left, pl.rect.top);

	//HealthBar healthBar;

	Clock clock;

	/////////////////// �������� ����  /////////////////////
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		time = time / 500;  // ����� ���������� �������� ����

		if (time > 40) time = 40;

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Space)
				{
					entities.push_back(new Bullet(anim2, lvl, Mario.x + 18, Mario.y + 18, Mario.left));
				}
			}
		}


		if (Keyboard::isKeyPressed(Keyboard::Left)) Mario.key["L"] = true;
		if (Keyboard::isKeyPressed(Keyboard::Right)) Mario.key["R"] = true;
		if (Keyboard::isKeyPressed(Keyboard::Up)) Mario.key["Up"] = true;
		if (Keyboard::isKeyPressed(Keyboard::Down)) Mario.key["Down"] = true;
		if (Keyboard::isKeyPressed(Keyboard::Space)) Mario.key["Space"] = true;


		for (it = entities.begin(); it != entities.end();)
		{
			Entity* b = *it;
			b->update(time);
			if (b->isAlive == false) 
			{ 
				it = entities.erase(it); 
				delete b; 
			}
			else it++;
		}

		Mario.update(time);
		//healthBar.update(Mario.Health);

		/*for (it = entities.begin(); it != entities.end(); it++)
		{
			//1. �����
			if ((*it)->Name == "Enemy")
			{
				Entity* enemy = *it;

				if (enemy->Health <= 0) continue;

				if (Mario.getRect().intersects(enemy->getRect()))
					if (Mario.dy > 0) { enemy->dx = 0; Mario.dy = -0.2; enemy->Health = 0; }
					else if (!Mario.hit) {
						Mario.Health -= 5; Mario.hit = true;
						if (Mario.dir) Mario.x += 10; else Mario.x -= 10;
					}
					

				for (std::list<Entity*>::iterator it2 = entities.begin(); it2 != entities.end(); it2++)
				{
					Entity* bullet = *it2;
					if (bullet->name == "Bullet")
					{					
						if (bullet->health > 0)
						{
							if (bullet->getRect().intersects(enemy->getRect()))
							{
								bullet->health = 0; enemy->health -= 5;
							}
						}
					}
				}
			}

			//2. ���������� ���������
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

			//3.. � �.�.
		}*/


		/////////////////////���������� �� �����/////////////////////
		view.setCenter(Mario.x, Mario.y);
		window.setView(view);

		background.setPosition(view.getCenter());
		window.draw(background);

		lvl.draw(window);

		for (it = entities.begin(); it != entities.end(); it++)
		{
			(*it)->draw(window);
		}

		Mario.draw(window);
		//healthBar.draw(window);

		window.display();
	}
}