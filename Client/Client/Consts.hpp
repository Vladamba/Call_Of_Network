#ifndef CONSTS_H
#define CONSTS_H

#include <SFML/Graphics.hpp>

using namespace sf;

const int MSPF = 1000 / 120;
const int CLIENTS_SIZE = 8;
const int BULLETS_SIZE = 64;
const Vector2f NULL_VECTOR = Vector2f(-1, -1);

const float PLAYER_WIDTH = 30.f;
const float PLAYER_HEIGHT = 40.0f;
const float BULLET_WIDTH = 6.f;
const float BULLET_HEIGHT = 6.0f;

const unsigned char STATE_STAND = 0;
const unsigned char STATE_RUN = 1;
const unsigned char STATE_JUMP = 2;
const unsigned char STATE_CRAWL = 3;
const unsigned char STATE_CLIMB = 4;

const unsigned char KEY_LEFT = 1;
const unsigned char KEY_RIGHT = 2;
const unsigned char KEY_UP = 4;
const unsigned char KEY_DOWN = 8;
const unsigned char KEY_SPACE = 16;
const unsigned char KEY_RSHIFT = 32;

#endif CONSTS_H