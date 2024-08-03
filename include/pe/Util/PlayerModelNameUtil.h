#pragma once

#include "Game/Player/PlayerAnimFunc.h"
#include <sead/basis/seadTypes.h>

namespace pe {

namespace util {

    constexpr const char* const sPlayerNames[] = { "Mario", "Luigi", "Peach", "Kinopio", "Rosetta" };
    constexpr const char* const sPowerUpNames[10] = { "", "Mini", "Fire", "Climb", "RaccoonDog", "Boomerang", "RaccoonDogWhite", "", "ClimbWhite", "ClimbGiga" };
    constexpr const char* const sPlayerArchiveNames[5][10] = {
        { "Mario", "MarioMini", "MarioFire", "MarioClimb", "MarioRaccoonDog", "MarioBoomerang", "MarioRaccoonDogWhite", "MarioClimb", "MarioClimbWhite", "MarioClimbGiga" },
        { "Luigi", "LuigiMini", "LuigiFire", "LuigiClimb", "LuigiRaccoonDog", "LuigiBoomerang", "LuigiRaccoonDogWhite", "LuigiClimb", "LuigiClimbWhite", "LuigiClimb" },
        { "Peach", "PeachMini", "PeachFire", "PeachClimb", "PeachRaccoonDog", "PeachBoomerang", "PeachRaccoonDogWhite", "PeachClimb", "PeachClimbWhite", "PeachClimb" },
        { "Kinopio", "KinopioMini", "KinopioFire", "KinopioClimb", "KinopioRaccoonDog", "KinopioBoomerang", "KinopioRaccoonDogWhite", "KinopioClimb", "KinopioClimbWhite", "KinopioClimb" },
        { "Rosetta", "RosettaMini", "RosettaFire", "RosettaClimb", "RosettaRaccoonDog", "RosettaBoomerang", "RosettaRaccoonDogWhite", "RosettaClimb", "RosettaClimbWhite", "RosettaClimb" }
    };

    enum class PlayerType : u8 {
        Mario = 0,
        Luigi = 1,
        Peach = 2,
        Kinopio = 3,
        Rosetta = 4
    };

    constexpr const char* const getPlayerName(PlayerType type) { return sPlayerNames[(u8)type]; }
    constexpr const char* const getPowerUpName(EPlayerFigure type) { return sPowerUpNames[(u8)type]; }
    constexpr const char* const getPlayerArchiveName(PlayerType playerType, EPlayerFigure powerUpType) { return sPlayerArchiveNames[(u8)playerType][(u8)powerUpType]; }

    constexpr PlayerType getPlayerType(const char* playerName)
    {
        switch (playerName[0]) {
        case 'M':
            return PlayerType::Mario;
            break;
        case 'L':
            return PlayerType::Luigi;
            break;
        case 'P':
            return PlayerType::Peach;
            break;
        case 'K':
            return PlayerType::Kinopio;
            break;
        case 'R':
            return PlayerType::Rosetta;
            break;
        default:
            return PlayerType::Mario;
        }
    }

    inline bool isFigureClimb(EPlayerFigure figure)
    {
        return figure == EPlayerFigure::Climb || figure == EPlayerFigure::ClimbGiga || figure == EPlayerFigure::ClimbWhite || figure == EPlayerFigure::Manekineko;
    }
    inline bool isFigureHaveTail(EPlayerFigure figure)
    {
        return isFigureClimb(figure) || figure == EPlayerFigure::RaccoonDog || figure == EPlayerFigure::RaccoonDogWhite;
    }

} // namespace util

} // namespace pe