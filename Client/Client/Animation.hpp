#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

using namespace sf;
using namespace tinyxml2;

class Animation
{
public:
	std::vector<IntRect> frames, framesFliped;
	float currentFrame, speed;
	bool loop, flip, isPlaying;
	Sprite sprite;

	Animation()
	{		
		currentFrame = 0;
		flip = false;
		isPlaying = true;	
	}

	void update(float time)
	{
		if (isPlaying)
		{
			currentFrame += speed * time;
			if (currentFrame > frames.size())
			{
				currentFrame -= frames.size();
			}

			sprite.setTextureRect(frames[currentFrame]);
			if (flip)
			{
				sprite.setTextureRect(framesFliped[currentFrame]);
			}
		}		
	}
};


/*enum class AnimationType
{
	stand, standShoot,
	run, runShoot,
	crawl, crawlShoot,
	jump, jumpShoot,
	climb
};*/


class AnimationManager
{
public:
	const char* currentAnimation;
	std::map<const char*, Animation> animationList;

	AnimationManager()
	{
	}

	~AnimationManager()
	{
		animationList.clear();
	}

	void loadFromXML(const char* fileName, Texture& t)
	{
		XMLDocument animationFile = new XMLDocument();
		animationFile.LoadFile(fileName);		

		XMLElement* head;
		head = animationFile.FirstChildElement("sprites");

		XMLElement * animationElement;
		animationElement = head->FirstChildElement("animation");
		while (animationElement)
		{
			Animation a;
			currentAnimation = animationElement->Attribute("title");			
			a.speed = 1.0f / atoi(animationElement->Attribute("delay"));
			a.sprite.setTexture(t);

			XMLElement* cut;
			cut = animationElement->FirstChildElement("cut");
			while (cut)
			{
				int x = atoi(cut->Attribute("x"));
				int y = atoi(cut->Attribute("y"));
				int w = atoi(cut->Attribute("w"));
				int h = atoi(cut->Attribute("h"));
				
				a.frames.push_back(IntRect(x, y, w, h));
				a.framesFliped.push_back(IntRect(x + w, y, -w, h));
				cut = cut->NextSiblingElement("cut");
			}

			a.sprite.setOrigin(0, a.frames[0].height);

			animationList[currentAnimation] = a;
			animationElement = animationElement->NextSiblingElement("animation");
		}
	}

	void draw(RenderWindow& window, int x, int y)
	{
		animationList[currentAnimation].sprite.setPosition(x, y);
		window.draw(animationList[currentAnimation].sprite);
	}

	void set(const char* name)
	{
		currentAnimation = name;
		animationList[currentAnimation].flip = false;
	}

	void flip(bool f)
	{
		animationList[currentAnimation].flip = f;
	}

	void update(float time)
	{
		animationList[currentAnimation].update(time);
	}

	void pause()
	{
		animationList[currentAnimation].isPlaying = false;
	}

	void play(const char* name)
	{
		animationList[name].isPlaying = true;
	}

	float getH()
	{
		return animationList[currentAnimation].frames[animationList[currentAnimation].currentFrame].height;
	}

	float getW()
	{
		return animationList[currentAnimation].frames[animationList[currentAnimation].currentFrame].width;
	}
};

#endif ANIMATION_H