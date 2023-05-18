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
	bool loop, isPlaying;
	Sprite sprite;

	Animation() {}

	Animation(int delay, Texture& t)
	{		
		currentFrame = 0;
		speed = 1.0f / delay;
		loop = true;
		isPlaying = true;	
		sprite.setTexture(t);
	}

	void update(float time, bool left)
	{
		if (left)
		{
			sprite.setTextureRect(frames_left[currentFrame]);
		}
		else
		{
			sprite.setTextureRect(frames_right[currentFrame]);
		}

		if (isPlaying)
		{
			currentFrame += speed * time;
			if (currentFrame >= frames_right.size())
			{				
				if (loop)
				{
					currentFrame -= frames_right.size();
				}
				else
				{
					currentFrame = frames_right.size() - 1;
					isPlaying = false;
				}
			}		
		}		
	}
};

enum class AnimationType { Stand, Run, Jump, Climb, Move, Explode };

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
									animationElement = animationElement->NextSiblingElement("animation");
									continue;
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

	void set(AnimationType t)
	{
		if (t != currentAnimation)
		{
			animationList[currentAnimation].isPlaying = true;
			currentAnimation = t;
			animationList[currentAnimation].currentFrame = 0;
		}
	}

	void update(float time, bool left)
	{		
		animationList[currentAnimation].update(time, left);
	}

	void pause()
	{
		animationList[currentAnimation].isPlaying = false;
	}

	void play()
	{
		animationList[currentAnimation].isPlaying = true;
	}

	void loop(AnimationType a, bool loop)
	{
		animationList[a].loop = loop;
	}

	void setColor(Color color)
	{
		animationList[currentAnimation].sprite.setColor(color);
	}

	/*bool isPlaying() {
		return animationList[currentAnimation].isPlaying;
	}
	
	int getWidth()
	{
		return animationList[currentAnimation].frames_right[(int)animationList[currentAnimation].currentFrame].width;
	}

	int getHeight()
	{
		return animationList[currentAnimation].frames_right[(int)animationList[currentAnimation].currentFrame].height;
	}*/
};

#endif ANIMATION_H