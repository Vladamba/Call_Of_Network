#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

using namespace sf;
using namespace tinyxml2;

struct Object
{
    std::string name;
    FloatRect rect;
};

struct Layer
{
    std::vector<Sprite> tiles;
    int opacity;
};

class Level
{
public:
    Texture texture;
    int width, height, tileWidth, tileHeight;
    FloatRect drawingBounds;
    std::vector<Object> objects;
    std::vector<Layer> layers;

    Level(const char* image, const char* file)
    {
        if (!texture.loadFromFile(image))
        {
            printf("Loading level image failed!");
        }
        texture.setSmooth(false);

        XMLDocument levelFile;
        if (!levelFile.LoadFile(file))
        {
            printf("Loading level file failed.");
        }

        XMLElement* mapElement;       
        mapElement = levelFile.FirstChildElement("map");

        width = atoi(mapElement->Attribute("width"));
        height = atoi(mapElement->Attribute("height"));
        tileWidth = atoi(mapElement->Attribute("tilewidth"));
        tileHeight = atoi(mapElement->Attribute("tileheight"));       

        XMLElement* layerElement;
        layerElement = mapElement->FirstChildElement("layer");
        while (layerElement)
        {
            Layer layer;
            if (layerElement->Attribute("opacity") != NULL)
            {
                layer.opacity = 255 * strtof(layerElement->Attribute("opacity"), NULL);
            }
            else
            {
                layer.opacity = 255;
            }

            XMLElement* layerDataElement;
            layerDataElement = layerElement->FirstChildElement("data");
            if (layerDataElement == NULL)
            {
                printf("Bad map. No layer information found.");
            }
            
            XMLElement* tileElement;
            tileElement = layerDataElement->FirstChildElement("tile");
            if (tileElement == NULL)
            {
                printf("Bad map. No tile information found.");
            }

            int columns = texture.getSize().x / tileWidth;
            int x = 0;
            int y = 0;
            IntRect rect;
            rect.width = tileWidth;
            rect.height = tileHeight;
            while (tileElement)
            {
                int tileGid = atoi(tileElement->Attribute("gid"));
                if (tileGid > 0)
                {
                    rect.left = (tileGid % columns - 1) * tileWidth;
                    rect.top = (tileGid / columns) * tileHeight;

                    Sprite sprite;
                    sprite.setTexture(texture);
                    sprite.setTextureRect(rect);
                    sprite.setPosition(x * tileWidth, y * tileHeight);
                    sprite.setColor(Color(255, 255, 255, layer.opacity));
                    layer.tiles.push_back(sprite);
                }

                x++;
                if (x >= width)
                {
                    x = 0;
                    y++;
                    if (y >= height)
                        y = 0;
                }

                tileElement = tileElement->NextSiblingElement("tile");
            }
            layers.push_back(layer);

            layerElement = layerElement->NextSiblingElement("layer");
        }

        
        XMLElement* objectGroupElement;
        objectGroupElement = mapElement->FirstChildElement("objectgroup");
        if (objectGroupElement == NULL)
        {
            printf("No object layers found");
        }
        while (objectGroupElement)
        {
            XMLElement* objectElement;
            objectElement = objectGroupElement->FirstChildElement("object");
            while (objectElement)
            {                
                Object object;
                object.name = objectElement->Attribute("name");
                object.rect.left = atoi(objectElement->Attribute("x"));
                object.rect.top = atoi(objectElement->Attribute("y"));

                if (objectElement->Attribute("width") != NULL)
                {
                    object.rect.width = atoi(objectElement->Attribute("width"));
                    object.rect.height = atoi(objectElement->Attribute("height"));
                }
                else
                {
                    object.rect.width = 0;
                    object.rect.height = 0;
                }

                objects.push_back(object);

                objectElement = objectElement->NextSiblingElement("object");
            }
            objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
        }
    }


    Vector2f getObjectVector(std::string name)
    {      
        for (int i = 0; i < objects.size(); i++)
        {                      
            if (objects[i].name == name)
            {
                return Vector2f(objects[i].rect.left, objects[i].rect.top);
            }                
        }       
        return Vector2f(0, 0);
    }

    std::vector<Object> getObjects(std::string name)
    {
        // Все объекты с заданным именем        
        std::vector<Object> vec;
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i].name == name)
            {
                vec.push_back(objects[i]);
            }              
        }            
        return vec;
    }

    std::vector<Object> getAllObjects()
    {        
        std::vector<Object> vec;
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i].name != "player")
            {
                vec.push_back(objects[i]);
            }
        }
        return vec;
    }

    Vector2i getTileSize()
    {
        return Vector2i(tileWidth, tileHeight);
    }

    void draw(RenderWindow& window)
    {
        // Рисуем все тайлы (объекты НЕ рисуем!)
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