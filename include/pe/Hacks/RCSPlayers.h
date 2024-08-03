#pragma once

#include "pe/Util/PlayerModelNameUtil.h"

namespace pe {

class RCSPlayers {
public:
    static void initHooks();
    static util::PlayerType& getCurrentPlayerType();
    static void setPlayerType(util::PlayerType type);
    static void readPlayerTypeFromFile();
};

} // namespace