#pragma once

#include "pe/Enet/DataPacket.h"
#include "pe/Enet/RedistPacket.h"
#include "pe/Enet/Types.h"

namespace pe {
namespace enet {

    // Greeting packet
    struct ToS_Hello : DataPacket<ToS_Hello> {
        nn::account::Uid uid { 0 };
        nn::account::Nickname name { 0 };
        bool isGhost = false;
    };

    struct ToS_SingleModeSceneEnter : DataPacket<ToS_SingleModeSceneEnter> {
        u8 playerType = 0;
    };

    struct PlayerFigureChangeData {
        u8 playerFigure;
    };

    using ToS_PlayerFigureChange = RedistPacket<PlayerFigureChangeData>::ServerBound;
    using ToC_PlayerFigureChange = RedistPacket<PlayerFigureChangeData>::ClientBound;

    struct ToS_GoalItemInfo : DataPacket<ToS_GoalItemInfo> {
        int islandID, shineID;
    };

    struct ToC_GoalItemInfo : DataPacket<ToC_GoalItemInfo> {
        nn::account::Uid user { 0 };
        int numCollectedGoalItems; // global counter to be updated

        // new shine data
        bool isCollectNewShine;
        int islandID, shineID;
    };

    struct PhaseUnlockData {
        int unlockedPhase;
        bool endCurrentPhase;
        u32 bossDefeatedFlag;
        bool isNewToPhase1;
        bool isNewToPhase2;
        bool isNewToPhase3;
        bool isNewToPhase4;
        bool isNewToPhase2Boss;
        bool isNewToPhase3Boss;
        bool isNewToPhase1BowserIntro;
        bool isNewToPhase2BowserIntro;
        bool isNewToPhase3BowserIntro;
        bool isNewToPhase1BowserExit;
        bool isNewToPhase2BowserExit;
        bool isNewToPhase3BowserExit;
        int phase1DarkBowserHitPoint;
        int phase2DarkBowserHitPoint;
        int phase3DarkBowserHitPoint;
        int phase4DarkBowserHitPoint;
        int phase4DarkBowserHitPointFinal;
        int phase3DarkBowserHitPointPreBattle;
        int phase4DarkBowserHitPointPreBattle;
    };

    using ToS_PhaseUnlock = RedistPacket<PhaseUnlockData>::ServerBound;
    using ToC_PhaseUnlock = RedistPacket<PhaseUnlockData>::ClientBound;

    struct ScenarioUpdateData {
        char islandName[128] { 0 };
        int scenario;
        bool unk;
    };

    using ToS_ScenarioUpdate = RedistPacket<ScenarioUpdateData>::ServerBound;
    using ToC_ScenarioUpdate = RedistPacket<ScenarioUpdateData>::ClientBound;

    struct DisasterModeControllerInternalUpdateData {
        int mFramesOfProsperityTransition;
        int mFramesOfProsperity;
        int mFramesOfAnticipationTransition;
        int mFramesOfAnticipation;
        int mFramesOfDisasterTransition;
        int mFramesOfDisaster;
        int mFramesUntilDisaster;
        int mProgressFrames;
        bool mDisableClock;
        bool mIsRaining;
        int mPostBossPeaceTime;
        bool isNewNerve;
        uintptr_t controllerNerve;
    };

    using ToS_DisasterModeControllerInternalUpdate = RedistPacket<DisasterModeControllerInternalUpdateData>::ServerBound;
    using ToC_DisasterModeControllerInternalUpdate = RedistPacket<DisasterModeControllerInternalUpdateData>::ClientBound;

    struct ToC_SyncClockUpdate : DataPacket<ToC_SyncClockUpdate> {
        u64 microseconds = 0;
    };

    struct MiscMapSyncData {
        enum class Type {
            KillActorByObjId,
            ShardCollect,
            DarkBowserHealth,
            ToadUnlock
        };

        Type type;
        union {
            struct {
                char objId[128];
            } killActorByObjId;
            struct {
                int zoneID, shardID;
            } shardCollect;
            struct {
                int health;
            } darkBowserHealth;
            struct {
                int character;
            } toadUnlock;
        };
    };

    using ToS_MiscMapSync = RedistPacket<MiscMapSyncData>::ServerBound;
    using ToC_MiscMapSync = RedistPacket<MiscMapSyncData>::ClientBound;

    struct JumpFlipData {
        bool flipState;
    };

    using ToS_JumpFlip = RedistPacket<JumpFlipData>::ServerBound;
    using ToC_JumpFlip = RedistPacket<JumpFlipData>::ClientBound;

    struct ToC_CheatPacket : DataPacket<ToC_CheatPacket> {
        ToC_CheatPacket() { }

        enum class Type {
            Teleport
        };

        Type type;
        union {
            struct {
                sead::Vector3f trans { 0, 0, 0 };
            } teleport;
        };
    };

    constexpr size_t sSaveDataChunkSize = 0x2100;

    struct SaveDataChunk : DataPacket<SaveDataChunk> {
    public:
        int chunkIndex = 0;
        u16 size = 0;
        u8 data[sSaveDataChunkSize] { 0 };
    };

} // namespace enet
} // namespace pe
