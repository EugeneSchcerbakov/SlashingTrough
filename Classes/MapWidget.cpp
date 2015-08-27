//
//  MapWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#include "MapWidget.h"

#include "ScreenChanger.h"
#include "LevelsCache.h"
#include "Log.h"

MapWidget* MapWidget::create(const std::string &mapDesc)
{
    MapWidget *map = new MapWidget();
    if (map && map->init(mapDesc)) {
        map->autorelease();
    } else {
        delete map;
        map = nullptr;
    }
    return map;
}

MapWidget::MapWidget()
{
}

MapWidget::~MapWidget()
{
}

bool MapWidget::init(const std::string &mapDesc)
{
    if (!cocos2d::ui::ScrollView::init()) {
        return false;
    }
    
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    cocos2d::Size size = cocos2d::Director::getInstance()->getVisibleSize();
    
    float scale = 1.0f;
    
    _map = cocos2d::Sprite::create("map/map_bg.png");
    _map->setAnchorPoint(cocos2d::Vec2(0.5f, 0.0f));
    _map->setPositionX(_map->getContentSize().width * scale * 0.5f + 0.0f);
    _map->setScale(scale);
    
    if (!initLevelMarkers(mapDesc)) {
        return false;
    }
    
    auto input = cocos2d::EventListenerTouchOneByOne::create();
    input->onTouchBegan = CC_CALLBACK_2(MapWidget::touchBegan, this);
    input->onTouchEnded = CC_CALLBACK_2(MapWidget::touchEnded, this);
    input->onTouchCancelled = CC_CALLBACK_2(MapWidget::touchCanceled, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(input, _map);
    
    setDirection(cocos2d::ui::ScrollView::Direction::BOTH);
    setContentSize(size);
    setInnerContainerSize(cocos2d::Size(_map->getContentSize()) * scale);
    setBounceEnabled(true);
    addChild(_map);
    
    scrollToBottom(0.0001f, false);
    
    return true;
}

bool MapWidget::initLevelMarkers(const std::string &mapFile)
{
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(mapFile);
    std::string buff = fileUtils->getStringFromFile(path);
    
    LevelsCache &levelsCache = LevelsCache::getInstance();
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        auto root = document.RootElement();
        auto node = root->FirstChildElement();
        while (node) {
            std::string name = node->Name();
            if (name == "LevelMark") {
                std::string levelId = node->Attribute("levelId");
                std::string text = node->Attribute("text");
                float x = node->FloatAttribute("x");
                float y = node->FloatAttribute("y");
                FieldLevel::WeakPtr level = levelsCache.getLevelById(levelId);
                MapLevelMark *mark = MapLevelMark::create(level);
                mark->setLevelText(text);
                mark->setPosition(cocos2d::Vec2(x, y));
                _map->addChild(mark);
                _levelMarkers.push_back(mark);
            } else {
                WRITE_WARN("Unknown map element: " + name);
            }
            node = node->NextSiblingElement();
        }
        
        WRITE_INIT("Map successfully crated.");
        return true;
    }
    else
    {
        WRITE_ERR("Failed to read map file");
        return false;
    }
}

bool MapWidget::touchBegan(cocos2d::Touch *touch, cocos2d::Event *e)
{
    return true;
}

void MapWidget::touchEnded(cocos2d::Touch *touch, cocos2d::Event *e)
{
    cocos2d::Vec2 p = _map->convertTouchToNodeSpace(touch);
    for (auto mark : _levelMarkers) {
        if (mark->getBoundingBox().containsPoint(p)) {
            ScreenChanger::beginRunAndSlash(mark->getLevelId());
            break;
        }
    }
}

void MapWidget::touchCanceled(cocos2d::Touch *touch, cocos2d::Event *e)
{
    
}
