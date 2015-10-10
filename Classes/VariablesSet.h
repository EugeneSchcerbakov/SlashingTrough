//
//  VariablesSet.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__VariablesSet__
#define __SlashingTrough__VariablesSet__

#include <string>
#include <map>

class VariablesSet
{
public:
    int getInt(const std::string &name, int def = 0) const;
    bool getBool(const std::string &name, bool def = false) const;
    float getFloat(const std::string &name, float def = 0.0f) const;
    std::string getString(const std::string &name, std::string def = "") const;
    
    void setInt(const std::string &name, int value);
    void setBool(const std::string &name, bool value);
    void setFloat(const std::string &name, float value);
    void setString(const std::string &name, const std::string &value);
    
    void incInt(const std::string &name, int amount = 1);
    void incFloat(const std::string &name, float amount = 1.0f);
    
    void clear();
    
    friend class PlayerInfo;
    
protected:
    std::map<std::string, int> _variablesInt;
    std::map<std::string, float> _variablesFloat;
    std::map<std::string, std::string> _variablesStr;
};

#endif /* defined(__SlashingTrough__VariablesSet__) */
