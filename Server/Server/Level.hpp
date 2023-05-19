#ifndef LEVEL_H
#define LEVEL_H

#include "tinyxml2.h"
#include <iostream>

using namespace tinyxml2;
using namespace sf;

enum ObjectType { None, Solid, Ladder, Spawner1, Spawner2 };

class Level
{
public:    
    int mapWidth, mapHeight, tileWidth, tileHeight;
    ObjectType** objects;    
    std::string mapFilename, tilesetFilename, tilesetTsxFilename, backgroundFilename;

    Level(std::string _mapFilename, std::string _tilesetFilename, std::string _backgroundFilename)
    {
        mapFilename = _mapFilename;
        tilesetFilename = _tilesetFilename;
        backgroundFilename = _backgroundFilename;

        XMLDocument levelFile;
        if (levelFile.LoadFile(mapFilename.c_str()) != XML_SUCCESS)
        {
            std::cout << "Loading level file failed!";
        }

        XMLElement* mapElement;
        mapElement = levelFile.FirstChildElement("map");

        tilesetTsxFilename = "files/";
        tilesetTsxFilename += mapElement->FirstChildElement("tileset")->Attribute("source");

        mapWidth = atoi(mapElement->Attribute("width"));
        mapHeight = atoi(mapElement->Attribute("height"));
        tileWidth = atoi(mapElement->Attribute("tilewidth"));
        tileHeight = atoi(mapElement->Attribute("tileheight"));

        objects = new ObjectType * [mapHeight];
        for (int i = 0; i < mapHeight; i++)
        {
            objects[i] = new ObjectType[mapWidth];
            for (int j = 0; j < mapWidth; j++)
            {
                objects[i][j] = ObjectType::None;
            }
        }

        XMLElement* objectGroupElement;
        objectGroupElement = mapElement->FirstChildElement("objectgroup");
        if (objectGroupElement == NULL)
        {
            std::cout << "No object layers found!";
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
                                if (name == "Spawner1")
                                {
                                    objects[i][j] = ObjectType::Spawner1;
                                }
                                else
                                {
                                    if (name == "Spawner2")
                                    {
                                        objects[i][j] = ObjectType::Spawner2;
                                    }
                                    else
                                    {
                                        std::cout << "Found incorrect object!";
                                    }
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
        return Vector2f(tileWidth, tileHeight);
    }
};

#endif LEVEL_H