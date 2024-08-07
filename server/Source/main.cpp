#include "pe/Enet/ProjectPacketHandler.h"
#include "pe/Enet/Server.h"
#include "pe/Util.h"
#include <nlohmann/json.hpp>

#define BUDDY_ALLOC_IMPLEMENTATION
#include "buddy_alloc.h"

#include "main.h"

static buddy* sBuddy = nullptr;

void* buddyMalloc(size_t size)
{
    return buddy_malloc(sBuddy, size);
}

void buddyFree(void* ptr)
{
    return buddy_free(sBuddy, ptr);
}

static void memoryFull()
{
    fprintf(stderr, "balls");
    abort();
}

int main()
{
    u16 port = 7089;

    std::string file = pe::readStringFromFile("Config.json");
    nlohmann::json data;
    try {
        data = nlohmann::json::parse(file);
        port = data["port"];
        printf("Using port %d\n", port);
    } catch (const nlohmann::json::parse_error& e) {
        printf("No config file. Using port 7089\n");
    }

    const size_t arenaSize = 1024 * 1024 * 256;
    void* buddyMetadata = malloc(buddy_sizeof(arenaSize));
    void* buddyArena = malloc(arenaSize);
    sBuddy = buddy_init(reinterpret_cast<u8*>(buddyMetadata), reinterpret_cast<u8*>(buddyArena), arenaSize);

    ENetCallbacks callbacks { buddyMalloc, buddyFree, memoryFull };

    pe::enet::ProjectPacketHandler handler;
    pe::enet::Server server({ ENET_HOST_ANY, port }, handler, callbacks);
    server.start();
    int returnCode = server.join();

    free(buddyMetadata);
    free(buddyArena);

    return returnCode;
}
