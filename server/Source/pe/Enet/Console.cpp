#include "pe/Enet/Console.h"
#include "pe/Enet/Client.h"
#include "pe/Enet/Server.h"
#include <cstdio>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pe {
namespace enet {

    static std::deque<std::string> split(const std::string& str, const char delim)
    {
        std::stringstream ss(str);
        std::string s;

        std::deque<std::string> out;

        while (std::getline(ss, s, delim)) {
            out.push_back(s);
        }
        return out;
    }

    struct Command {
        std::string name;
        std::string description;
        std::string syntax;
        std::function<void(const std::deque<std::string>&)> func;
    };

    Console* Console::sInstance = nullptr;

    Console::Console(Server* server)
        : mServer(server)
    {
        sInstance = this;
    }

    void Console::processCommand(const std::deque<std::string>& args)
    {
        static Command sCommands[] = {
            { "help", "Shows this menu" },
            {
                "list",
                "Shows the list of clients",
                "",
                [this](const std::deque<std::string>& args) {
                    for (auto entry : mServer->mClients) {
                        Client& client = entry.second;
                        printf("%s:\n", client.mName);
                        printf("  Connection: %s:%d\n", inet_ntoa(in_addr { client.mPeer->address.host }), client.mPeer->address.port);
                        printf("  User ID: ");
                        PE_PRINT_UUID(client.mUid, printf);
                        printf("\n");
                    }
                },

            },
            {
                "save",
                "Saves save data",
                "",
                [this](const std::deque<std::string>& args) {
                    mServer->getSaveData().save();
                },
            },
            {
                "resetsave",
                "Resets save data",
                "",
                [this](const std::deque<std::string>& args) {
                    mServer->getSaveData().reset();
                    mServer->getSaveData().save();
                    printf("Successfully reset save data\n");
                },
            },
            {
                "setshines",
                "Set amount of shines",
                "<amount>",
                [this](const std::deque<std::string>& args) {
                    if (args.size() != 2) {
                        printf("setshines <amount>\n");
                        return;
                    }

                    int amount = 0;
                    try {
                        amount = std::stoi(args[1]);
                    } catch (const std::exception&) {
                        printf("%s is not a number!", args[1].c_str());
                        return;
                    }

                    mServer->getSaveData().numCollectedGoalItems = amount;
                    mServer->getSaveData().save();

                    ToC_GoalItemInfo packet;
                    packet.numCollectedGoalItems = amount;
                    packet.isCollectNewShine = false;
                    packet.user = {};
                    mServer->sendPacketToAll(&packet);

                    printf("Set shine count to %d\n", amount);
                },
            },
            {
                "tpall",
                "Teleport all players to specified player",
                "<player>",
                [this](const std::deque<std::string>& args) {
                    if (args.size() != 2) {
                        printf("tpall <player>\n");
                        return;
                    }

                    Client* player = mServer->searchClient(args[1].c_str());
                    if (player == nullptr) {
                        printf("No player found with query \"%s\"\n", args[1].c_str());
                        return;
                    }

                    ToC_CheatPacket packet;
                    packet.type = ToC_CheatPacket::Type::Teleport;
                    packet.teleport.trans = player->mTrans;
                    mServer->sendPacketToAllExcept(player, &packet);

                    printf("Teleported all players to %s\n", player->mName);
                },
            }
        };

        sCommands[0].func = [this](const std::deque<std::string>& args) {
            printf("Help Menu\n=========\n");
            for (const Command& cmd : sCommands) {
                printf("  %s: %s\n", cmd.name.c_str(), cmd.description.c_str());
                printf("    Syntax: %s %s\n", cmd.name.c_str(), cmd.syntax.c_str());
            }
        };

        for (const Command& cmd : sCommands) {
            if (cmd.name == args[0]) {
                cmd.func(args);
                return;
            }
        }

        printf("Unknown command. Type 'help' for help.\n");
    }

    void Console::threadFunc()
    {
        while (mServer->isAlive()) {
            printf("> ");
            fflush(stdout);

            std::string cmd;
            std::getline(std::cin, cmd);
            auto args = split(cmd, ' ');
            processCommand(args);
        }
    }

    void Console::start()
    {
        mThread = std::thread(&Console::threadFunc, this);
    }

} // namespace enet
} // namespace pe
