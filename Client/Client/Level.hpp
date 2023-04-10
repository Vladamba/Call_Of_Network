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

enum class ObjectType { None, Solid, Ladder, Player };

class Level
{
public:
    Texture texture;
    int mapWidth, mapHeight, tileWidth, tileHeight;    
    ObjectType** objects;
    std::vector<Layer> layers;

    Level(const char* image, const char* file)
    {
        if (!texture.loadFromFile(image))
        {
            printf("Loading level image failed!");
        }
        texture.setSmooth(false);

        XMLDocument levelFile;
        if (levelFile.LoadFile(file) != XML_SUCCESS)
        {
            printf("Loading level file failed.");
        }

        XMLElement* mapElement;       
        mapElement = levelFile.FirstChildElement("map");

        mapWidth = atoi(mapElement->Attribute("width"));
        mapHeight = atoi(mapElement->Attribute("height"));
        tileWidth = atoi(mapElement->Attribute("tilewidth"));
        tileHeight = atoi(mapElement->Attribute("tileheight"));      

        objects = new ObjectType*[mapHeight];
        for (int i = 0; i < mapHeight; i++)
        {
            objects[i] = new ObjectType[mapWidth];
            for (int j = 0; j < mapWidth; j++)
            {
                objects[i][j] = ObjectType::None;
            }
        }        

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
                    sprite.setTexture(texture);
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
                std::string name = objectElement->Attribute("name");
                int x = atoi(objectElement->Attribute("x"));
                int y = atoi(objectElement->Attribute("y"));
                for (int i = y / tileHeight; i < (y + atoi(objectElement->Attribute("height"))) / tileHeight; i++)
                {
                    for (int j = x / tileWidth; j < (x + atoi(objectElement->Attribute("width"))) / tileWidth; j++)
                    {
                        if (name == "Solid")
                        {
                            objects[i][j] = ObjectType::Solid;
                        }    
                        else
                        {
                            if (name == "Ladder")
                            {
                                objects[i][j] = ObjectType::Ladder;
                            }
                            else
                            {
                                if (name == "Player")
                                {
                                    objects[i][j] = ObjectType::Player;
                                }
                                else
                                {
                                    printf("Found incorrect object!");
                                }
                            }
                        }
                    }
                }

                objectElement = objectElement->NextSiblingElement("object");
            }
            
            objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
        }
    }


    Vector2f getObjectCoord(ObjectType type)
    {      
        for (int i = 0; i < mapHeight; i++)
        {            
            for (int j = 0; j < mapWidth; j++)
            {
                if (objects[i][j] == type)
                {
                    return Vector2f(j * tileWidth, i * tileHeight);
                }
            }
        }  
        return Vector2f(1, 1);
    }

    /*
    
    for (int i = 0; i < mapHeight; i++)
    {
        for (int j = 0; j < mapWidth; j++)
        {
            if (objects[i][j] == ObjectType::None)
            {
                printf("n");
            }
            if (objects[i][j] == ObjectType::Solid)
            {
                printf("s");
            }
            if (objects[i][j] == ObjectType::Ladder)
            {
                printf("l");
            }
            if (objects[i][j] == ObjectType::Player)
            {
                printf("p");
            }
        }
        printf("\n");
    }
    printf("\n");

    
    
    
    
    
    
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
    }*/

    void draw(RenderWindow& window)
    {
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