#pragma once

#include <cstdio>
#include <deque>
#include <thread>
#include <string>

namespace pe {
namespace enet {
    class Server;

    class Console {
        Server* mServer = nullptr;
        std::thread mThread;

        void threadFunc();
        void processCommand(const std::deque<std::string>& args);

        static Console* sInstance;

    public:
        Console(Server* server);

        void start();
        template <typename... Args>
        static void log(const char* fmt, Args... args)
        {
            printf("\33[2K\r");
            printf(fmt, args...);
            printf("\n");
            printf("> ");
            fflush(stdout);
        }

        friend class Server;
    };

} // namespace enet
} // namespace pe
