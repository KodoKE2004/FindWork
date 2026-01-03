#pragma once
#include <filesystem>
#include <iostream>
#include <source_location>
#include <vector>
#include <windows.h>

enum class MessageColor
{
    Red,
    Green,
    Yellow,
    White
};


namespace Debug
{
    inline static void Assert(
        bool expression,
        std::string_view message,
        const std::source_location& loc = std::source_location::current())
    {
    #ifdef _DEBUG
        if (!expression)
        {
            std::filesystem::path fullPath = loc.file_name();
            std::filesystem::path cwd = std::filesystem::current_path();
            std::filesystem::path rel = fullPath.lexically_relative(cwd);

            // std::string に保持
            std::string pathStr = rel.empty() ? fullPath.string() : rel.string();

            std::cerr
                << "File     : " << pathStr << "\n"
                << "Function : " << loc.function_name() << "\n"
                << "Line     : " << loc.line() << "\n"
                << "Message  : " << message << std::endl;   // 改行 + フラッシュ

            std::terminate();
        }
    #endif
    }

    inline static void Assert(
        bool expression,
        std::wstring_view message,
        const std::source_location& loc = std::source_location::current())
    {
    #ifdef _DEBUG
        if (!expression)
        {
            namespace fs = std::filesystem;
            fs::path fullPath = loc.file_name();
            fs::path cwd = fs::current_path();
            fs::path rel = fullPath.lexically_relative(cwd);

            // 作業ディレクトリからの相対パスをワイド文字列で取得
            std::wstring pathStr = rel.empty() ? fullPath.wstring() : rel.wstring();

            // ワイド出力
            std::wcerr
                << L"File     : " << pathStr << L"\n"
                << L"Function : " << loc.function_name() << L"\n"
                << L"Line     : " << loc.line() << L"\n"
                << L"Message  : " << message << std::endl;

            std::terminate();
        }
    #endif
    }

    static void Log(std::string_view message)
    {
    #ifdef _DEBUG
        std::cout << message << std::endl;
    #endif
    }
    static void Log(std::string_view message, MessageColor color)
    {
    #ifdef _DEBUG
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        const WORD green  = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        const WORD red    = FOREGROUND_RED   | FOREGROUND_INTENSITY;
        const WORD yellow = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        const WORD white  = FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE;
        
        WORD originalAttrs;
        switch (color)
        {
        case MessageColor::Red   : originalAttrs = red;     break;
        case MessageColor::Green : originalAttrs = green;   break;
        case MessageColor::Yellow: originalAttrs = yellow;  break;
        case MessageColor::White : originalAttrs = white;   break;
        default: break;
        }

        SetConsoleTextAttribute(hConsole, originalAttrs);
        std::cout << message << std::endl;
        SetConsoleTextAttribute(hConsole,white);

    #endif
    }

    template<typename... Args>
    static void Log(std::format_string<Args...> format, Args&&... args)
    {
    #ifdef _DEBUG
        Log(std::string(std::format(format, std::forward<Args>(args)...)));
    #endif
    }
};

