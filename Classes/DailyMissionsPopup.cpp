//
//  DailyMissionsPopup.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/10/15.
//
//

#include "DailyMissionsPopup.h"
#include "DailyMissions.h"
#include "Store.h"

class DailyTaskWidget : public cocos2d::Node
{
public:
    static DailyTaskWidget* create(DailyTaskBase::WeakPtr taskPtr)
    {
        DailyTaskWidget *widget = new DailyTaskWidget();
        if (widget && widget->init(taskPtr)) {
            widget->autorelease();
        } else {
            delete widget;
            widget = nullptr;
        }
        return widget;
    }
    
protected:
    DailyTaskWidget()
    {
        
    }
    
    virtual ~DailyTaskWidget()
    {
        
    }
    
    bool init(DailyTaskBase::WeakPtr taskPtr)
    {
        if (!cocos2d::Node::init() || taskPtr.expired()) {
            return false;
        }
        
        DailyTaskBase::Ptr task = taskPtr.lock();
        
        auto titleText = cocos2d::ui::Text::create();
        titleText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
        titleText->setString(task->getInfo().description);
        titleText->setFontName("font_prototype.ttf");
        titleText->setFontSize(18);
        titleText->setTextColor(cocos2d::Color4B::WHITE);
        titleText->setTextAreaSize(cocos2d::Size(150.0f, 0.0f));
        titleText->ignoreContentAdaptWithSize(false);
        
        std::string iconFile = Store::getInstance().getItemById(task->getInfo().lootRewardId)->getIcon();
        
        auto lootIcon = cocos2d::Sprite::create(iconFile);
        lootIcon->setScale(0.5f);
        lootIcon->setPositionX(250.0f);
        
        addChild(titleText);
        addChild(lootIcon);
        
        return true;
    }
};

DailyMissionPopup* DailyMissionPopup::create()
{
    DailyMissionPopup *popup = new DailyMissionPopup();
    if (popup && popup->init()) {
        popup->autorelease();
    } else {
        delete popup;
        popup = nullptr;
    }
    return popup;
}

bool DailyMissionPopup::hitTest(const cocos2d::Vec2 &pt)
{
    return _panel->getBoundingBox().containsPoint(pt);
}

void DailyMissionPopup::showEffect()
{
    Popup::showEffect();
    
    if (_panel->getNumberOfRunningActions() > 0) {
        return;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    float x = origin.x + size.width - _panel->getContentSize().width * _panel->getScale() * 0.5f;
    float y = _panel->getPositionY();
    
    auto move = cocos2d::MoveTo::create(0.2f, cocos2d::Vec2(x, y));
    
    _panel->runAction(move);
}

float DailyMissionPopup::hideEffect()
{
    Popup::hideEffect();
    
    if (_panel->getNumberOfRunningActions() > 0) {
        return 0.0f;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    float time = 0.1f;
    float x = origin.x + size.width + _panel->getContentSize().width * _panel->getScale() * 0.5f;
    float y = _panel->getPositionY();
    
    auto move = cocos2d::MoveTo::create(time, cocos2d::Vec2(x, y));
    
    _panel->runAction(move);
    
    return time;
}

DailyMissionPopup::DailyMissionPopup()
: Popup()
{
}

DailyMissionPopup::~DailyMissionPopup()
{
}

bool DailyMissionPopup::init()
{
    if (!Popup::init()) {
        return false;
    }
    
    auto daily = DailyMissions::getInstance();
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    const float panelScale = 1.75;
    _panel = cocos2d::Sprite::create("ui/ui_panel_daily-missions.png");
    _panel->setScale(panelScale);
    _panel->setPositionX(origin.x + size.width + _panel->getContentSize().width * panelScale * 0.5f);
    _panel->setPositionY(origin.y + size.height * 0.5f + 80.0);
    
    auto titleText = cocos2d::ui::Text::create("Daily goals", "font_prototype.ttf", 23);
    titleText->setTextColor(cocos2d::Color4B::YELLOW);
    titleText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    titleText->setPositionX(30.0f);
    titleText->setPositionY(_panel->getContentSize().height - titleText->getContentSize().height * 0.5f - 15.0f);
    
    _panel->addChild(titleText);
    
    cocos2d::Vec2 goalPos;
    goalPos.x = 30.0;
    goalPos.y = _panel->getContentSize().height - 100.0f;
    
    for (DailyTaskBase::Ptr task : daily.getTodayMissions())
    {
        DailyTaskWidget *widget = DailyTaskWidget::create(task);
        widget->setPosition(goalPos);
        _panel->addChild(widget);
        
        goalPos.y -= 80.0f;
    }
    
    addChild(_panel, 1);
    
    return true;
}
