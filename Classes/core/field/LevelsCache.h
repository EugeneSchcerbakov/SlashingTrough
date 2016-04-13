//
//  LevelsCache.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 24/08/15.
//
//

#ifndef __SlashingTrough__LevelsCache__
#define __SlashingTrough__LevelsCache__

#include <map>

#include "FieldLevel.h"

class LevelsCache
{
public:
    static LevelsCache& getInstance();
    
public:
    bool load(const std::string &filename);
    
    FieldLevel::WeakPtr getLevelById(const std::string &id);
    FieldLevel::WeakPtr getLevelByIndex(int index);
    
    int getLevelsAmount() const;
    
private:
    std::map<std::string, FieldLevel::Ptr> _levels;
};

#endif /* defined(__SlashingTrough__LevelsCache__) */
