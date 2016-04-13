//
//  Log.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 09/08/15.
//
//

#ifndef __SlashingTrough__Log__
#define __SlashingTrough__Log__

#include <string>

#ifdef _DEBUG
#include <assert.h>
#endif

/**
 * Class that realised writing log on several platforms
 * Realised as Phoenix singlton
 */
class Log
{
public:
    static Log& getInstance();
    
    void writeError(const std::string& text);
    void writeWarning(const std::string& text);
    void writeLog(const std::string& text);
    void writeInit(const std::string& text);
    
private:
    /** Write line with timestamp */
    void writeLine(const std::string& text);
    
    std::ofstream *logFileStream;
    
    /** Construct information about singlton */
    static void create();
    
    /** Function that run when smd trying to use destroyed singlton */
    static void onDeadReference();
    
    /** Destruct singlton with ability to rebuild it */
    static void killPhoenixSingletone();
    
    static Log* singleInstance;
    
    static bool isDestroyed;
    static bool isFirstLife;
    
    Log();
    ~Log();
    Log(const Log&);
    void operator=(const Log&);
};

#ifdef _DEBUG
#define DEBUG_ASSERT { assert(false); }(void)1
#else
#define DEBUG_ASSERT
#endif

#define WRITE_INIT(message) {				\
Log::getInstance().writeInit(message);		\
}(void)1

#define WRITE_LOG(message) {				\
Log::getInstance().writeLog(message);		\
}(void)1

#define WRITE_WARN(message) {				\
Log::getInstance().writeWarning(message);	\
DEBUG_ASSERT;							\
}(void)1

#define WRITE_ERR(message) {				\
Log::getInstance().writeError(message);	\
DEBUG_ASSERT;							\
}(void)1

#ifdef _DEBUG
#define WARN_IF(condition, message) {	\
if (condition) {					\
WRITE_WARN(message);			\
}									\
}(void)1
#else
#define WARN_IF(condition, message)
#endif

#endif /* defined(__SlashingTrough__Log__) */
