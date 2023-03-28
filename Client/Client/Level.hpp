#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include "tinyxml2.h"

using namespace sf;
using namespace tinyxml2;

class Object
{
public:
    const char* name;
    const char* type;
    Rect<float> rect;
    std::map<const char*, const char*> properties;
    Sprite sprite;


    int getPropertyInt(const char* name)
    {
        return atoi(properties[name]);
    }

    float getPropertyFloat(const char* name)
    {
        return strtof(properties[name], NULL);
    }

    const char* getPropertyString(const char* name)
    {
        return properties[name];
    }
};

struct Layer
{
    int opacity;
    std::vector<Sprite> tiles;
};

static class Level
{
public://private
    int width, height, tileWidth, tileHeight;
    int firstTileID;
    Rect<float> drawingBounds;
    Texture tilesetImage;
    static std::vector<Object> objects;
    std::vector<Layer> layers;

    bool loadFromFile(const char* fileName)
    {
        XMLDocument levelFile = new XMLDocument();

        if (!levelFile.LoadFile(fileName))
        {
            //std::cout << "Loading level \"" << filename << "\" failed." << std::endl;
            return false;
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
            //std::cout << "Failed to load tile sheet." << std::endl;
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
                // std::cout << "Bad map. No layer information found." << std::endl;
                return false;
            }
            
            XMLElement* tileElement;
            tileElement = layerDataElement->FirstChildElement("tile");

            if (tileElement == NULL)
            {
                //std::cout << "Bad map. No tile information found." << std::endl;
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
                    const char* objectType;
                    if (objectElement->Attribute("type") != NULL)
                    {
                        objectType = objectElement->Attribute("type");
                    }
                    const char* objectName;
                    if (objectElement->Attribute("name") != NULL)
                    {
                        objectName = objectElement->Attribute("name");
                    }
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
                    object.name = objectName;
                    object.type = objectType;
                    object.sprite = sprite;

                    Rect<float> objectRect;
                    objectRect.top = y;
                    objectRect.left = x;
                    objectRect.height = height;
                    objectRect.width = width;
                    object.rect = objectRect;

                    XMLElement* properties;
                    properties = objectElement->FirstChildElement("properties");
                    if (properties != NULL)
                    {
                        XMLElement* property;
                        property = properties->FirstChildElement("property");
                        if (property != NULL)
                        {
                            while (property)
                            {
                                const char* propertyName = property->Attribute("name");
                                const char* propertyValue = property->Attribute("value");

                                object.properties[propertyName] = propertyValue;

                                property = property->NextSiblingElement("property");
                            }
                        }
                    }

                    objects.push_back(object);

                    objectElement = objectElement->NextSiblingElement("object");
                }
                objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
            }
        }
        else
        {
            //std::cout << "No object layers found..." << std::endl;
        }

        return true;
    }


    Object getObject(const char* name)
    {
        // Только первый объект с заданным именем
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i].name == name)
            {
                return objects[i];
            }                
        }            
    }

    std::vector<Object> getObjects(const char* name)
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

    static std::vector<Object> getAllObjects()
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