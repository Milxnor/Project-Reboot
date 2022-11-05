#pragma once

#include <iostream> // todo remove

/*

#include <vector>
#include <spdlog/spdlog.h>

enum ELogLevel : uint8_t
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Disabled,
    All = Trace
};

struct LogCategory
{
protected:
    spdlog::level::level_enum m_DefaultLevel;
    std::shared_ptr<spdlog::logger> m_Logger;
public:
    LogCategory(const char* CategoryName, ELogLevel DefaultLevel);
    ~LogCategory() { spdlog::drop(m_Logger->name()); };

    const std::string& GetName() const { return m_Logger->name(); }
    ELogLevel GetLevel() const { return (ELogLevel)m_Logger->level(); }
    std::shared_ptr<spdlog::logger> GetLogger() const { return m_Logger; }

    void Reset() { m_Logger->set_level(m_DefaultLevel); };
    void SetLevel(ELogLevel Level) { m_Logger->set_level((spdlog::level::level_enum)Level); }
};

class Log
{
private:
    static std::vector<spdlog::sink_ptr> m_LogSinks;
    static bool m_Initialized;
public:
    static bool IsInitialized() { return m_Initialized; }

    static void Init();
    static void Shutdown();

    static const std::vector<spdlog::sink_ptr>& GetLogSinks() { return m_LogSinks; }

    static void Log_Internal(std::shared_ptr<spdlog::logger> Logger, ELogLevel Level, const std::string& Message);
    static void Log_Internal(std::shared_ptr<spdlog::logger> Logger, ELogLevel Level, const std::wstring& Message);
};

#define LOG(Category, Level, ...) \
        Log::Log_Internal(Category.GetLogger(), Level, std::format(__VA_ARGS__));

#define DECLARE_LOG_CATEGORY(CategoryName, DefaultLevel) \
        struct LogCategory##CategoryName : public LogCategory \
        { \
            LogCategory##CategoryName() : LogCategory(#CategoryName, DefaultLevel) {} \
        }; \
        extern LogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY(CategoryName) \
        LogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY_STATIC(CategoryName, DefaultLevel) \
        static struct LogCategory##CategoryName : public LogCategory \
        { \
            LogCategory##CategoryName() : LogCategory(#CategoryName, DefaultLevel) {} \
        } CategoryName;


        */