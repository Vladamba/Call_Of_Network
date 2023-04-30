#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include <SFML/Graphics.hpp>
#include "Consts.hpp"

using namespace sf;

class HealthBar
{
public:
	RectangleShape rect;
	int greenOpacity, redOpacity;
	Texture texture;
	Sprite sprite;

	HealthBar(const char* image)
	{
		if (!texture.loadFromFile(image))
		{
			printf("Loading health bar image failed!");
		}
		texture.setSmooth(false);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, 64, 16));	

		rect.setSize(Vector2f(64, 16));
		greenOpacity = 255;
		redOpacity = 0;
		rect.setFillColor(Color(redOpacity, greenOpacity, 0, 255));
	}

	void update(int health)
	{
		//health * 255 /100
	}

	void draw(RenderWindow& window, Vector2f vec)
	{
		rect.setPosition(vec);
		window.draw(rect);
		sprite.setPosition(vec);
		window.draw(sprite);
	}
};

#endif HEALTHBAR_H