#include "pch.h"

namespace coreutility {
    namespace log {
        bool init_logging() {
            if (!std::filesystem::exists("logs/")) {
                std::filesystem::create_directory("logs");
            }

            try {
                auto fl = spdlog::rotating_logger_mt("fl", "logs/logfile", 1024 * 1024 * 5, 3);
            } catch (const spdlog::spdlog_ex &ex) {
                std::cout << ex.what() << std::endl;
                return false;
            }

            auto loggr = spdlog::get("fl");
            loggr->flush_on(spdlog::level::err);
            loggr->info("============ Server started =============");
            loggr->flush();
            return true;
        }
    }

    namespace winapi {
        void cls() {
            COORD                      topLeft = {0, 0};
            HANDLE                     console = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO screen;
            DWORD                      written;

            GetConsoleScreenBufferInfo(console, &screen);
            FillConsoleOutputCharacterA(
                    console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
            );
            FillConsoleOutputAttribute(
                    console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
                    screen.dwSize.X * screen.dwSize.Y, topLeft, &written
            );
            SetConsoleCursorPosition(console, topLeft);
        }
    }
}