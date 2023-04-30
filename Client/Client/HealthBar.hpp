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

		rect.setSize(Vector2f(48, 16));		
		redOpacity = 0;
		greenOpacity = 255;	
		rect.setFillColor(Color(redOpacity, greenOpacity, 0, 255));
	}

	void update(int health)
	{
		rect.setSize(Vector2f(health * 48.f / 100.f, 16));
		greenOpacity = health * 255.f / 100.f;
		redOpacity = 255 - health * 255.f / 100.f;
		rect.setFillColor(Color(redOpacity, greenOpacity, 0, 255));
	}

	void draw(RenderWindow& window, Vector2f vec)
	{
		rect.setPosition(vec + Vector2f(16, 0));
		window.draw(rect);
		sprite.setPosition(vec);
		window.draw(sprite);
	}
};

#endif HEALTHBAR_H