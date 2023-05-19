#ifndef SCORE_H
#define SCORE_H

using namespace sf;

class Score
{
public:
	Font font;
	Text text;
	String s;

	Score(const char* fontFile)
	{
		if (!font.loadFromFile(fontFile))
		{
			std::cout << "Loading font failed!\n";
		}
		new(&text) Text("0 | 0", font, 16);				
	}

	void update(unsigned char score1, unsigned char score2)
	{
		s = std::to_string(score1) + " | " + std::to_string(score2);
		text.setString(s);
	}

	void draw(RenderWindow& window, Vector2f vec)
	{
		text.setPosition(vec + Vector2f(-10, 10));
		window.draw(text);
	}
};

#endif SCORE_H