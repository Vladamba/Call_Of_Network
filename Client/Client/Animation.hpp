#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

using namespace sf;
using namespace tinyxml2;

class Animation
{
public:
	std::vector<IntRect> frames_right, frames_left;
	float currentFrame, speed;
	bool loop, left, isPlaying;
	Sprite sprite;

	Animation(int delay, Texture& t)
	{		
		currentFrame = 0;
		speed = 1.0f / delay;
		loop = true;
		left = false;
		isPlaying = true;	
		sprite.setTexture(t);
	}

	void update(float time)
	{
		if (isPlaying)
		{
			currentFrame += speed * time;
			if (currentFrame > frames_right.size())
			{
				currentFrame -= frames_right.size();
			}
			
			if (left)
			{
				sprite.setTextureRect(frames_left[(int)currentFrame]);
			}
			else
			{
				sprite.setTextureRect(frames_right[(int)currentFrame]);
			}
		}		
	}
};


std::string AnimationType[] = {"stand", "run", "jump", "crawl", "climb", 
	"standShoot","runShoot", "jumpShoot", "crawlShoot"};


class AnimationManager
{
public:
	std::string currentAnimation;
	std::map<std::string, Animation> animationList;

	AnimationManager(){}

	AnimationManager(const char* fileName, Texture& t)
	{
		XMLDocument animationFile;
		if (animationFile.LoadFile(fileName) != XML_SUCCESS)
		{
			printf("Loading animation failed!");			
		}

		XMLElement* animationElement;
		animationElement = animationFile.FirstChildElement("sprites")->FirstChildElement("animation");
		while (animationElement)
		{			
			currentAnimation = animationElement->Attribute("title");	
			bool correctAnimation = false;
			for (int i = 0; i < AnimationType->size(); i++) 
			{
				if (currentAnimation == AnimationType[i])
				{
					correctAnimation = true;
					break;
				}
			}
			if (correctAnimation)
			{
				Animation animation(atoi(animationElement->Attribute("delay")), t);
				XMLElement* cut;
				cut = animationElement->FirstChildElement("cut");
				while (cut)
				{
					int x = atoi(cut->Attribute("x"));
					int y = atoi(cut->Attribute("y"));
					int w = atoi(cut->Attribute("w"));
					int h = atoi(cut->Attribute("h"));

					animation.frames_right.push_back(IntRect(x, y, w, h));
					animation.frames_left.push_back(IntRect(x + w, y, -w, h));

					cut = cut->NextSiblingElement("cut");
				}
				//animation.sprite.setOrigin(0, animation.frames_right[0].height);
				animationList[currentAnimation] = animation;
			}
			else
			{
				printf("Found incorrect animation!");
			}	
			animationElement = animationElement->NextSiblingElement("animation");
		}
	}

	void draw(RenderWindow& window, float x, float y)
	{
		animationList[currentAnimation].sprite.setPosition(x, y);
		window.draw(animationList[currentAnimation].sprite);
	}

	bool isPlaying() {
		return animationList[currentAnimation].isPlaying;
	}

	void set(std::string name)
	{
		currentAnimation = name;
		//animationList[currentAnimation].left = false;
	}

	void flip(bool left)
	{
		animationList[currentAnimation].left = left;
	}

	void update(float time)
	{
		animationList[currentAnimation].update(time);
	}

	void pause()
	{
		animationList[currentAnimation].isPlaying = false;
	}

	void play(std::string name)
	{
		animationList[name].isPlaying = true;
	}

	int getWidth()
	{
		return animationList[currentAnimation].frames_right[(int)animationList[currentAnimation].currentFrame].width;
	}

	int getHeight()
	{
		return animationList[currentAnimation].frames_right[(int)animationList[currentAnimation].currentFrame].height;
	}
};

#endif ANIMATION_H