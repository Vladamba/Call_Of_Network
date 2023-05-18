#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

using namespace sf;
using namespace tinyxml2;

struct Layer
{
    std::vector<Sprite> tiles;
    int opacity;
};

class Level
{
public:
    Texture tTileset, tBackground;
    int mapWidth, mapHeight, tileWidth, tileHeight;    
    std::vector<Layer> layers;
    Sprite sBackground;  

    Level() {}

    Level(std::string map, std::string tileset, std::string background)
    {
        if (!tBackground.loadFromFile(background.c_str()))
        {
            printf("Loading background failed!");
        }
        tBackground.setSmooth(false);
        sBackground.setTexture(tBackground);
        sBackground.setOrigin(tBackground.getSize().x / 2, tBackground.getSize().y / 2);

        if (!tTileset.loadFromFile(tileset.c_str()))
        {
            printf("Loading tileset failed!");
        }
        tTileset.setSmooth(false);

        XMLDocument levelFile;
        if (levelFile.LoadFile(map.c_str()) != XML_SUCCESS)
        {
            printf("Loading map failed!");
        }

        XMLElement* mapElement;       
        mapElement = levelFile.FirstChildElement("map");

        mapWidth = atoi(mapElement->Attribute("width"));
        mapHeight = atoi(mapElement->Attribute("height"));
        tileWidth = atoi(mapElement->Attribute("tilewidth"));
        tileHeight = atoi(mapElement->Attribute("tileheight"));         

        XMLElement* layerElement;
        layerElement = mapElement->FirstChildElement("layer");
        while (layerElement)
        {
            Layer layer;
            if (layerElement->Attribute("opacity") != NULL)
            {
                layer.opacity = (int)255.0f * strtof(layerElement->Attribute("opacity"), NULL);
            }
            else
            {
                layer.opacity = 255;
            }

            XMLElement* layerDataElement;
            layerDataElement = layerElement->FirstChildElement("data");
            if (layerDataElement == NULL)
            {
                printf("Bad map. No layer information found!");
            }
            
            XMLElement* tileElement;
            tileElement = layerDataElement->FirstChildElement("tile");
            if (tileElement == NULL)
            {
                printf("Bad map. No tile information found!");
            }

            int columns = tTileset.getSize().x / tileWidth;
            int x = 0;
            int y = 0;
            IntRect rect;
            rect.width = tileWidth;
            rect.height = tileHeight;
            while (tileElement)
            {
                int tileGid = 0;
                if (tileElement->Attribute("gid") != NULL)
                {
                    tileGid = atoi(tileElement->Attribute("gid"));
                }                 
                if (tileGid > 0)
                {
                    rect.left = (tileGid % columns - 1) * tileWidth;
                    rect.top = (tileGid / columns) * tileHeight;

                    Sprite sprite;
                    sprite.setTexture(tTileset);
                    sprite.setTextureRect(rect);
                    sprite.setPosition(x * tileWidth, y * tileHeight);
                    sprite.setColor(Color(255, 255, 255, layer.opacity));
                    layer.tiles.push_back(sprite);                    
                }

                x++;
                if (x >= mapWidth)
                {
                    x = 0;
                    y++;
                    if (y >= mapHeight)
                        y = 0;
                }

                tileElement = tileElement->NextSiblingElement("tile");
            }
            layers.push_back(layer);

            layerElement = layerElement->NextSiblingElement("layer");
        }
    }


    void draw(RenderWindow& window, Vector2f vec)
    {
        sBackground.setPosition(vec);
        window.draw(sBackground);

        for (int layer = 0; layer < layers.size(); layer++)
        {
            for (int tile = 0; tile < layers[layer].tiles.size(); tile++)
            {
                window.draw(layers[layer].tiles[tile]);
            }
        }
    }
};

#endif LEVEL_H