//
//  Cheats.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 20/12/15.
//
//

#ifndef Cheats_h
#define Cheats_h

#include <map>
#include <string>
#include <functional>

class Cheats
{
public:
    struct Info
    {
        std::string id;
        std::string title;
        std::function<void()> func;
        
        Info() = default;
    };
    
public:
    static Cheats& getInstance();
    
    void addCheat(const std::string &id, const std::string &title, std::function<void()> func);
    void runCheat(const std::string &id);
    
    bool isExist(const std::string &id) const;
    void clearAll();
    
    const std::map<std::string, Cheats::Info>& getAll() const;
    
    static void initBasicCheats();
    
protected:
    Cheats();
    
private:
    std::map<std::string, Cheats::Info> _cheats;
};

#endif /* Cheats_h */
