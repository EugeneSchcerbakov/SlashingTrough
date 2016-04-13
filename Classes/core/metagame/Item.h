//
//  Item.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#ifndef __SlashingTrough__Item__
#define __SlashingTrough__Item__

#include <string>
#include <memory>
#include <vector>

class Hero;
class Entity;

class Item
{
public:
    typedef std::shared_ptr<Item> Ptr;
    typedef std::weak_ptr<Item> WeakPtr;
    
    enum class Type
    {
        NONE,
        WEAPON,
        ARMOR,
        CRYSTALL,
        SHARD
    };
    
public:
    Item(Type t);
    virtual ~Item();

    int getPrice() const;
    Type getType() const;
    
    virtual const std::string& getId() const;
    virtual const std::string& getDesc() const;
    virtual const std::string& getIcon() const;
    virtual const std::string& getName() const;

    bool isType(Type t) const;
    
protected:
    int price;
    std::string id;
    std::string desc;
    std::string icon;
    std::string name;
    const Type type;
    
    friend class Store;
};

class ItemWeapon : public Item
{
public:
    struct TrailInfo
    {
        float length;
        float width;
        float posYCoeff;
        float opacity;
        std::string texture;
    };
    
    static Item::Ptr create();
    static ItemWeapon* cast(Item::Ptr base);
    
public:
    ItemWeapon();
    
    float getDamage() const;
    float getSpeed() const;
    float getDistance() const;
    
    const std::string& getSprite() const;
    const TrailInfo& getTrailInfo() const;
    
protected:
    float damage;
    float speed;
    float distance;
    std::string sprite;
    TrailInfo trail;
    
    friend class Store;
};

class ItemArmor : public Item
{
public:
    static Item::Ptr create();
    static ItemArmor* cast(Item::Ptr base);
    
public:
    ItemArmor();
    
    float getExtraHealth() const;
    const std::string& getSprite() const;
    
protected:
    float addHealth;
    std::string sprite;
    
    friend class Store;
};

class Ability
{
public:
    typedef std::shared_ptr<Ability> Ptr;
    typedef std::weak_ptr<Ability> WeakPtr;
    
public:
    Ability();
    
    virtual void init(Hero *p);
    virtual void update(float dt);
    
    virtual void swipeLeft();
    virtual void swipeRight();
    virtual void swipeBack();
    virtual void swipeForward();
    
    virtual void hit(Entity *e);
    virtual void kill(Entity *e);
    virtual float damage(float receivedDamage);
    
protected:
    Hero *_hero;
};

class Crystall : public Item
{
public:
    static Item::Ptr create();
    static Crystall* cast(Item::Ptr base);
    static const int LockTier;
    
    enum Kind
    {
        NONE = -1,
        WEAPON = 0,
        ARMOR = 1
    };
    
    struct TierData
    {
        Ability::Ptr ability;
        std::string desc;
        std::string icon;
        int shards;
    };
    
    class Shard : public Item
    {
    public:
        static Item::Ptr create();
        static Shard* cast(Item::Ptr base);
        
    public:
        Shard();
    };
    
public:
    Crystall();
    
    void onInit(Hero *hero);
    void onUpdate(float dt);
    
    void onSwipeLeft();
    void onSwipeRight();
    void onSwipeBack();
    void onSwipeForward();
    
    void onHit(Entity *entity);
    void onKill(Entity *entity);
    float onDamage(float receivedDamage);
    
    const std::string& getDesc() const override;
    const std::string& getIcon() const override;
    
    void refreshState();
    
    TierData getCurrentTierData() const;
    std::string getShardId() const;
    int getShardsToNextTier() const;
    int getMaxTier() const;
    int getTier() const;
    
    Crystall::Kind getKind() const;
    
    bool isKind(Crystall::Kind which) const;
    bool isUnlocked() const;
    
protected:
    void nextTier();
    
    int tier;
    Kind kind;
    
    std::vector<TierData> tiersData;
    
private:
    Hero *_hero;
    
    friend class Store;
};

#endif /* defined(__SlashingTrough__Item__) */
