#include "log.h"

/*

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>

LogCategory::LogCategory(const char* CategoryName, ELogLevel level)
    : m_DefaultLevel((spdlog::level::level_enum)level)
{
    // if (false)
    {
        if (!Log::IsInitialized())
            Log::Init();

        auto& logSinks = Log::GetLogSinks();
        m_Logger = std::make_shared<spdlog::logger>(CategoryName, logSinks.begin(), logSinks.end());
        spdlog::register_logger(m_Logger);

        m_Logger->set_level(m_DefaultLevel);
        m_Logger->flush_on(m_DefaultLevel);
    }
}

std::vector<spdlog::sink_ptr> Log::m_LogSinks = {};
bool Log::m_Initialized = false;

static std::string GenerateLogFileName()
{
    std::string logFileDirectory = "Content/Logs";

    auto t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);

    std::ostringstream date;
    date << std::put_time(&tm, "%D %T");
    std::string dateFormatted = date.str();

    int filesWithCurrentDate = 0;

    for (const auto& file : std::filesystem::directory_iterator(logFileDirectory))
        if (file.path().filename().string().contains(dateFormatted))
            filesWithCurrentDate++;

    std::string filename = std::format("{}/{}-{}.log", logFileDirectory, dateFormatted, filesWithCurrentDate);

    while (std::filesystem::exists(filename))
    {
        filesWithCurrentDate--;
        filename = std::format("{}/{}-{}.log", logFileDirectory, dateFormatted, filesWithCurrentDate);
    }

    return filename;
}

void Log::Init()
{
    AllocConsole();

    FILE* stream;
    // freopen_s(&stream, "CONIN$", "r", stdin);
    freopen_s(&stream, "CONOUT$", "w+", stdout);
    // freopen_s(&stream, "CONERR$", "w", stderr);

    SetConsoleTitleA("Project Reboot V2");

    std::string logName = "reboot.log"; // GenerateLogFileName();

    m_LogSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>())->set_pattern("[%D-%T] %n: %^%v%$");
    m_LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logName, true))->set_pattern("[%D-%T] %n: %l: %v");

    m_Initialized = true;
}

void Log::Shutdown()
{
    FreeConsole();

    spdlog::shutdown();

    m_Initialized = false;
}

void Log::Log_Internal(std::shared_ptr<spdlog::logger> Logger, ELogLevel Level, const std::string& Message)
{
    switch (Level)
    {
	case Trace:
        Logger->trace(Message);
		break;
    case Debug:
        Logger->debug(Message);
        break;
    case Info:
		Logger->info(Message);
        break;
    case Warning:
		Logger->warn(Message);
        break;
	case Error:
		Logger->error(Message);
        break;
    case Critical:
        Logger->critical(Message);
        MessageBoxA(GetConsoleWindow(), Message.c_str(), std::format("Project Reboot - {}", Logger->name()).c_str(), MB_OK);
        abort();
        break;
    }
}

void Log::Log_Internal(std::shared_ptr<spdlog::logger> Logger, ELogLevel Level, const std::wstring& Message)
{
    Log_Internal(Logger, Level, std::string(Message.begin(), Message.end()));
}

*/