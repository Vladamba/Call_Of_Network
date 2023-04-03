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

	Animation(){}

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

enum class AnimationType {Stand, Run, Jump, Fall, Crawl, Climb, Move, Explode};


class AnimationManager
{
public:
	Texture texture;
	AnimationType currentAnimation;
	std::map<AnimationType, Animation> animationList;

	AnimationManager(){}

	AnimationManager(const char* image, const char* file)
	{
		if (!texture.loadFromFile(image))
		{
			printf("Loading animation image failed!");
		}
		texture.setSmooth(false);

		XMLDocument animationFile;
		if (animationFile.LoadFile(file) != XML_SUCCESS)
		{
			printf("Loading animation file failed!");			
		}

		XMLElement* animationElement;
		animationElement = animationFile.FirstChildElement("sprites")->FirstChildElement("animation");
		while (animationElement)
		{						
			std::string title = animationElement->Attribute("title");	
			if (title == "Stand")
			{
				currentAnimation = AnimationType::Stand;
			}
			else
			{
				if (title == "Run")
				{
					currentAnimation = AnimationType::Run;
				}
				else
				{
					if (title == "Jump")
					{
						currentAnimation = AnimationType::Jump;
					}
					else
					{
						if (title == "Fall")
						{
							currentAnimation = AnimationType::Fall;
						}
						else
						{
							if (title == "Crawl")
							{
								currentAnimation = AnimationType::Crawl;
							}
							else
							{
								if (title == "Climb")
								{
									currentAnimation = AnimationType::Climb;
								}
								else
								{
									if (title == "Move")
									{
										currentAnimation = AnimationType::Move;
									}
									else
									{
										if (title == "Explode")
										{
											currentAnimation = AnimationType::Explode;
										}
										else
										{
											printf("Found incorrect animation!");
										}
									}
								}
							}
						}						
					}
				}
			}		

			Animation animation(atoi(animationElement->Attribute("delay")), texture);
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

			animationList[currentAnimation] = animation;

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

	void left(bool left)
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

	void play(AnimationType name)
	{
		animationList[name].isPlaying = true;
	}

	void loop(bool loop)
	{
		animationList[currentAnimation].loop = loop;
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