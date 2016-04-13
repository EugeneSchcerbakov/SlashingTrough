//
//  VariablesSet.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "VariablesSet.h"


int VariablesSet::getInt(const std::string &name, int def) const
{
    std::map<std::string, int>::const_iterator it;
    it = _variablesInt.find(name);
    return it != _variablesInt.end() ? (*it).second : def;
}

bool VariablesSet::getBool(const std::string &name, bool def) const
{
    return getInt(name, (int)def) > 0;
}

float VariablesSet::getFloat(const std::string &name, float def) const
{
    std::map<std::string, float>::const_iterator it;
    it = _variablesFloat.find(name);
    return it != _variablesFloat.end() ? (*it).second : def;
}

std::string VariablesSet::getString(const std::string &name, std::string def) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = _variablesStr.find(name);
    return it != _variablesStr.end() ? (*it).second : def;
}

void VariablesSet::setInt(const std::string &name, int value)
{
    _variablesInt[name] = value;
}

void VariablesSet::setBool(const std::string &name, bool value)
{
    setInt(name, (int)value);
}

void VariablesSet::setFloat(const std::string &name, float value)
{
    _variablesFloat[name] = value;
}

void VariablesSet::setString(const std::string &name, const std::string &value)
{
    _variablesStr[name] = value;
}

void VariablesSet::incInt(const std::string &name, int amount)
{
    std::map<std::string, int>::iterator it;
    it = _variablesInt.find(name);
    
    if (it != _variablesInt.end()) {
        (*it).second += amount;
    } else {
        setInt(name, amount);
    }
}

void VariablesSet::incFloat(const std::string &name, float amount )
{
    std::map<std::string, float>::iterator it;
    it = _variablesFloat.find(name);
    
    if (it != _variablesFloat.end()) {
        (*it).second += amount;
    } else {
        setFloat(name, amount);
    }
}

void VariablesSet::clear()
{
    _variablesInt.clear();
    _variablesFloat.clear();
    _variablesStr.clear();
}
