//
//  DailyMissionsPopup.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/10/15.
//
//

#include "DailyMissionsPopup.h"
#include "Store.h"
#include "EffectFlyingIcon.h"

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
    
    void setCompletedState(bool animated)
    {
        _completedState = true;
        
        auto text = getChildByName<cocos2d::ui::Text *>("titleText");
        auto icon = getChildByName<cocos2d::Sprite *>("lootIcon");
        
        if (!text || !icon) {
            return;
        }
        
        if (animated)
        {
            auto tint = cocos2d::TintTo::create(0.5f, 127, 127, 127);
            text->runAction(tint);
            icon->setVisible(false);
        }
        else
        {
            text->setTextColor(cocos2d::Color4B(127, 127, 127, 255));
            icon->setVisible(false);
        }
    }
    
    bool isCompletedState() const
    {
        return _completedState;
    }
    
protected:
    DailyTaskWidget()
    : _completedState(false)
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
        titleText->setFontSize(30);
        titleText->setTextColor(cocos2d::Color4B::WHITE);
        titleText->setTextAreaSize(cocos2d::Size(350.0f, 0.0f));
        titleText->ignoreContentAdaptWithSize(false);
        titleText->setName("titleText");
        
        std::string iconFile = Store::getInstance().getItemById(task->getInfo().lootRewardId)->getIcon();
        
        auto lootIcon = cocos2d::Sprite::create(iconFile);
        lootIcon->setScale(0.9f);
        lootIcon->setPositionX(440.0f);
        lootIcon->setName("lootIcon");
        
        addChild(titleText);
        addChild(lootIcon);
        
        return true;
    }
    
private:
    bool _completedState;
};

DailyMissionPopup* DailyMissionPopup::create(cocos2d::Layer *container)
{
    DailyMissionPopup *popup = new DailyMissionPopup(container);
    if (popup && popup->init()) {
        popup->autorelease();
    } else {
        delete popup;
        popup = nullptr;
    }
    return popup;
}

bool DailyMissionPopup::hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const
{
    return _panel->getBoundingBox().containsPoint(pt);
}

void DailyMissionPopup::addTaskRewardEffect(const std::string &taskId, cocos2d::Vec2 endp)
{
    auto data = std::pair<std::string, cocos2d::Vec2>(taskId, endp);
    _effects.push_back(data);
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
    
    auto callback = [this]()
    {
        _shown = true;
    };
    
    auto move = cocos2d::MoveTo::create(0.2f, cocos2d::Vec2(x, y));
    auto call = cocos2d::CallFunc::create(callback);
    auto seq = cocos2d::Sequence::create(move, call, nullptr);
    
    _panel->runAction(seq);
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
    
    auto callback = [this]()
    {
        _shown = false;
    };
    
    auto move = cocos2d::MoveTo::create(time, cocos2d::Vec2(x, y));
    auto call = cocos2d::CallFunc::create(callback);
    auto seq = cocos2d::Sequence::create(move, call, nullptr);
    
    _panel->runAction(seq);
    
    return time;
}

DailyMissionPopup::DailyMissionPopup(cocos2d::Layer *container)
: Popup()
, _globalEffects(container)
, _shown(false)
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
    
    _panel = cocos2d::Sprite::create("ui/ui_panel_daily-missions.png");
    _panel->setPositionX(origin.x + size.width + _panel->getContentSize().width * 0.5f);
    _panel->setPositionY(origin.y + size.height * 0.5f + 80.0);
    
    auto titleText = cocos2d::ui::Text::create("Daily goals", "font_prototype.ttf", 36);
    titleText->setTextColor(cocos2d::Color4B::YELLOW);
    titleText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    titleText->setPositionX(50.0f);
    titleText->setPositionY(_panel->getContentSize().height - titleText->getContentSize().height * 0.5f - 20.0f);
    
    _panel->addChild(titleText);
    
    cocos2d::Vec2 goalPos;
    goalPos.x = 50.0;
    goalPos.y = _panel->getContentSize().height - 160.0f;
    
    for (DailyTaskBase::Ptr task : daily.getTodayMissions())
    {
        DailyTaskWidget *widget = DailyTaskWidget::create(task);
        widget->setPosition(goalPos);
        widget->setName(task->getInfo().id);
        _panel->addChild(widget);
        
        goalPos.y -= 140.0f;
    }
    
    addChild(_panel, 1);
    scheduleUpdate();
    
    return true;
}

void DailyMissionPopup::update(float dt)
{
    checkInternalState();
    
    if (_shown)
    {
        while (!_effects.empty())
        {
            DailyMissions &daily = DailyMissions::getInstance();
            
            auto data = _effects.back();
            
            auto id = data.first;
            auto endp = data.second;
            
            DailyTaskBase::Ptr task = daily.findMissionInPool(id);
            
            if (task)
            {
                DailyTaskWidget *widget = _panel->getChildByName<DailyTaskWidget *>(task->getInfo().id);
                
                if (widget)
                {
                    Store &store = Store::getInstance();
                    Item::Ptr item = store.getItemById(task->getInfo().lootRewardId);
                    
                    auto icon = widget->getChildByName("lootIcon");
                    
                    auto scale = icon->getScale();
                    auto from = icon->convertToWorldSpace(cocos2d::Vec2::ZERO);
                    auto image = item->getIcon();
                    
                    from.x += icon->getContentSize().width * 0.5f * scale;
                    from.y += icon->getContentSize().height * 0.5f * scale;
                    
                    auto effect = EffectFlyingIcon::create(image, from, endp, scale);
                    _globalEffects->addChild(effect);
                    
                    widget->setCompletedState(true);
                }
            }
            
            _effects.pop_back();
        }
    }
}

void DailyMissionPopup::checkInternalState()
{
    DailyMissions &daily = DailyMissions::getInstance();
    
    for (DailyTaskBase::Ptr task : daily.getTodayMissions())
    {
        bool hasEffect = false;
        
        for (auto effectData : _effects) {
            if (effectData.first == task->getInfo().id) {
                hasEffect = true;
                break;
            }
        }
        
        if (!hasEffect)
        {
            DailyTaskWidget *widget = _panel->getChildByName<DailyTaskWidget *>(task->getInfo().id);
        
            if (task->isRewarded() && widget && !widget->isCompletedState())
            {
                widget->setCompletedState(false);
            }
        }
    }
}
