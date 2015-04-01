//
//  GameInfo.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#ifndef __SlashingTrough__GameInfo__
#define __SlashingTrough__GameInfo__

#include <string>
#include <map>

class GameInfo
{
public:
    static GameInfo& Instance();
    
    bool LoadInfo(const std::string &filename);
    
    int GetInt(const std::string &name, int def = 0) const;
    bool GetBool(const std::string &name, bool def = false) const;
    float GetFloat(const std::string &name, float def = 0.0f) const;
    std::string GetString(const std::string &name, std::string def = "") const;
    
    void SetInt(const std::string &name, int value);
    void SetBool(const std::string &name, bool value);
    void SetFloat(const std::string &name, float value);
    void SetString(const std::string &name, const std::string &value);
    
private:
    std::map<std::string, int> _variablesInt;
    std::map<std::string, float> _variablesFloat;
    std::map<std::string, std::string> _variablesStr;
};

#endif /* defined(__SlashingTrough__GameInfo__) */
