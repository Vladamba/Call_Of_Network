#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

using namespace sf;
using namespace tinyxml2;

struct Object
{
    std::string type;
    IntRect rect;
};

struct Layer
{
    int opacity;
    std::vector<Sprite> tiles;
};

class Level
{
public:
    int width, height, tileWidth, tileHeight;
    int firstTileID;
    Rect<float> drawingBounds;
    Texture tilesetImage;
    std::vector<Object> objects;
    std::vector<Layer> layers;

    Level(const char* fileName, Texture& t)
    {       
        XMLDocument levelFile;
        if (levelFile.LoadFile(fileName) != XML_SUCCESS)
        {
            printf("Loading level failed.");
        }

        XMLElement* map;       
        map = levelFile.FirstChildElement("map");

        width = atoi(map->Attribute("width"));
        height = atoi(map->Attribute("height"));
        tileWidth = atoi(map->Attribute("tilewidth"));
        tileHeight = atoi(map->Attribute("tileheight"));

        XMLElement* tilesetElement;
        tilesetElement = map->FirstChildElement("tileset");
        firstTileID = atoi(tilesetElement->Attribute("firstgid"));
        
        XMLElement* imageElement;
        imageElement = tilesetElement->FirstChildElement("image");
        const char* imagePath = imageElement->Attribute("source");
        
        Image image;
        if (!image.loadFromFile(imagePath))
        {
            printf("Failed to load tile sheet.");
            return false;
        }

        image.createMaskFromColor(Color(255, 255, 255));
        tilesetImage.loadFromImage(image);
        tilesetImage.setSmooth(false);

        int columns = tilesetImage.getSize().x / tileWidth;
        int rows = tilesetImage.getSize().y / tileHeight;

        std::vector<Rect<int> > subRects;

        for (int y = 0; y < rows; y++)
            for (int x = 0; x < columns; x++)
            {
                Rect<int> rect;

                rect.top = y * tileHeight;
                rect.height = tileHeight;
                rect.left = x * tileWidth;
                rect.width = tileWidth;

                subRects.push_back(rect);
            }

        XMLElement* layerElement;
        layerElement = map->FirstChildElement("layer");
        while (layerElement)
        {
            Layer layer;
            
            if (layerElement->Attribute("opacity") != NULL)
            {
                float opacity = strtod(layerElement->Attribute("opacity"), NULL);
                layer.opacity = 255 * opacity;
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
                return false;
            }
            
            XMLElement* tileElement;
            tileElement = layerDataElement->FirstChildElement("tile");

            if (tileElement == NULL)
            {
                printf("Bad map. No tile information found.");
                return false;
            }

            int x = 0;
            int y = 0;

            while (tileElement)
            {
                int tileGID = atoi(tileElement->Attribute("gid"));
                int subRectToUse = tileGID - firstTileID;

                if (subRectToUse >= 0)
                {
                    Sprite sprite;
                    sprite.setTexture(tilesetImage);
                    sprite.setTextureRect(subRects[subRectToUse]);
                    sprite.setPosition(x * tileWidth, y * tileHeight);
                    sprite.setColor(Color(255, 255, 255, layer.opacity));

                    layer.tiles.push_back(sprite);
                }

                tileElement = tileElement->NextSiblingElement("tile");

                x++;
                if (x >= width)
                {
                    x = 0;
                    y++;
                    if (y >= height)
                        y = 0;
                }
            }
            layers.push_back(layer);

            layerElement = layerElement->NextSiblingElement("layer");
        }
        
        XMLElement* objectGroupElement;

        if (map->FirstChildElement("objectgroup") != NULL)
        {
            objectGroupElement = map->FirstChildElement("objectgroup");
            while (objectGroupElement)
            {
                XMLElement* objectElement;
                objectElement = objectGroupElement->FirstChildElement("object");

                while (objectElement)
                {                                    
                    int x = atoi(objectElement->Attribute("x"));
                    int y = atoi(objectElement->Attribute("y"));

                    int width, height;

                    Sprite sprite;
                    sprite.setTexture(tilesetImage);
                    sprite.setTextureRect(Rect<int>(0, 0, 0, 0));
                    sprite.setPosition(x, y);

                    if (objectElement->Attribute("width") != NULL)
                    {
                        width = atoi(objectElement->Attribute("width"));
                        height = atoi(objectElement->Attribute("height"));
                    }
                    else
                    {
                        width = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].width;
                        height = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].height;
                        sprite.setTextureRect(subRects[atoi(objectElement->Attribute("gid")) - firstTileID]);
                    }

                    Object object;
                    object.type = objectElement->Attribute("name");
                    object.sprite = sprite;

                    Rect<float> objectRect;
                    objectRect.top = y;
                    objectRect.left = x;
                    objectRect.height = height;
                    objectRect.width = width;
                    object.rect = objectRect;

                    objects.push_back(object);
                    //printf("%s", objects[objects.size() - 1].name);

                    objectElement = objectElement->NextSiblingElement("object");
                }
                objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
            }
        }
        else
        {
            printf("No object layers found...");
        }

        return true;
    }


    Object getObject(std::string name)
    {
        // Только первый объект с заданным именем        
        int i;
        for (i = 0; i < objects.size(); i++)
        {                      
            if (objects[i].type == name)
            {
                break;              
            }                
        }         
        return objects[i];
    }

    std::vector<Object> getObjects(std::string name)
    {
        // Все объекты с заданным именем        
        std::vector<Object> vec;
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i].type == name)
            {
                vec.push_back(objects[i]);
            }              
        }            
        return vec;
    }

    std::vector<Object> getAllObjects()
    {        
        return objects;
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