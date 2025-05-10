#include "pe/Execute/ExecuteDirectorExtensions.h"
#include "al/Execute/ExecuteOrder.h"
#include "al/Execute/ExecuteTableHolderUpdate.h"
#include "hk/hook/InstrUtil.h"
#include "hk/ro/RoUtil.h"
#include "imgui.h"
#include "pe/Util/Log.h"

namespace pe {

    static const al::ExecuteOrder sCustomUpdateTableOrder[] = {
        { "OceanWaterRequest", "Execute", 1, "システム", "Ocean Water Update Request" },
        { "TimerManager", "Execute", 1, "システム", "Timer Management" },
        { "ライト管理", "Execute", 2, "システム", "LightDirector" },
        //{ "Multiplayer Manager", "Execute", 1, "システム", "MultiplayerManager" },
        { "Clipping", "Execute", 1, "システム", "ClippingDirector" },
        { "ModelUpdate", "ActorModelUpdate", 1024, "システム", "ModelUpdate" },
        { "センサー", "Execute", 1, "システム", "Hit Sensor Director" },
        { "ポインティング", "Execute", 8, "システム", "pointer" },
        { "DRCアシスト", "ActorMovementCalcAnim", 2, "システム", "DRCassist" },
        { "DRCアシスト同期グループ", "ActorMovement", 1, "システム", "DRCassist" },
        { "空", "ActorMovementCalcAnim", 8, "地形", "DRCassistSyncGroup" },
        { "遠景", "ActorMovementCalcAnim", 8, "地形", "DistantView" },
        { "コリジョン地形", "ActorMovementCalcAnim", 256, "地形", "Terrain" },
        { "コリジョン地形[Movement]", "ActorMovement", 128, "地形", "Terrain[Movement]" },
        { "コリジョン地形装飾", "ActorMovementCalcAnim", 32, "地形", "TerrainDecoration" },
        { "コリジョン地形装飾[Movement]", "ActorMovement", 32, "地形", "TerrainDecoration[Movement]" },
        { "コリジョン地形オブジェ", "ActorMovementCalcAnim", 64, "地形オブジェ", "TerrainObj" },
        { "コリジョン地形オブジェ[Movement]", "ActorMovement", 32, "地形オブジェ", "TerrainObj[Movement]" },
        { "コリジョンアイテム", "ActorMovementCalcAnim", 32, "アイテム", "CollisionItem" },
        { "コリジョンディレクター", "Execute", 1, "システム", "Collision Director" },
        { "地形オブジェ", "ActorMovementCalcAnim", 128, "地形オブジェ", nullptr },
        { "地形オブジェ[Movement]", "ActorMovement", 32, "地形オブジェ", nullptr },
        { "EnemyMapObj[Movement]", "ActorMovement", 32, "地形オブジェ", nullptr },
        { "エフェクトオブジェ", "ActorMovement", 32, "エフェクト", nullptr },
        { "地形オブジェ装飾", "ActorMovementCalcAnim", 32, "地形オブジェ", nullptr },
        { "乗り物", "ActorMovementCalcAnim", 32, "乗り物", nullptr },
        { "乗り物[Movement]", "ActorMovement", 32, "乗り物", nullptr },
        { "乗り物装飾", "ActorMovementCalcAnim", 32, "乗り物", nullptr },
        { "デモプレイヤーロケーター", "ActorMovementCalcAnim", 4, "プレイヤー", nullptr },
        { "デモプレイヤー前処理", "ActorMovement", 16, "プレイヤー", nullptr },
        { "プレイヤー前処理", "Functor", 1, "プレイヤー", nullptr },
        { "プレイヤー[Movement]", "ActorMovement", 1, "プレイヤー", nullptr },
        { "プレイヤー", "ActorMovementCalcAnim", 64, "プレイヤー", nullptr },
        { "プレイヤー後処理", "Functor", 1, "プレイヤー", nullptr },
        { "プレイヤー装飾", "ActorMovementCalcAnim", 64, "プレイヤー", nullptr },
        { "プレイヤー装飾２", "ActorMovementCalcAnim", 64, "プレイヤー", nullptr },
        { "ゴーストプレイヤー記録", "Functor", 1, "敵", nullptr },
        { "敵", "ActorMovementCalcAnim", 128, "敵", nullptr },
        { "敵[Movement]", "ActorMovement", 32, "敵", nullptr },
        { "敵装飾", "ActorMovementCalcAnim", 32, "敵", nullptr },
        { "敵装飾[Movement]", "ActorMovement", 32, "敵", nullptr },
        { "デモ管理者", "Execute", 64, "地形", nullptr },
        { "デモ", "ActorMovementCalcAnim", 64, "地形", nullptr },
        { "デモオブジェクト", "ActorMovement", 32, "地形", nullptr },
        { "ＮＰＣ", "ActorMovementCalcAnim", 32, "ＮＰＣ", nullptr },
        { "ＮＰＣ装飾", "ActorMovementCalcAnim", 32, "ＮＰＣ", nullptr },
        { "コインローテータ", "Execute", 1, "アイテム", nullptr },
        { "ゴーストディレクター", "Execute", 1, "プレイヤー", nullptr },
        { "エコーエミッター管理", "Execute", 1, "地形オブジェ", nullptr },
        { "アイテム", "ActorMovementCalcAnim", 64, "アイテム", nullptr },
        { "アイテム装飾", "ActorMovementCalcAnim", 32, "アイテム", nullptr },
        { "シャドウマスク", "ActorMovement", 64, "影", nullptr },
        { "グラフィックス要求者", "ActorMovement", 64, "地形オブジェ", nullptr },
        { "監視オブジェ", "ActorMovement", 32, "地形オブジェ", nullptr },
        { "フォグディレクター", "Execute", 1, "システム", "Fog Director" },
        { "ステージスイッチディレクター", "Execute", 1, "システム", "Stage Switch Director" },
        { "カメラ振動", "Functor", 1, "システム", nullptr },
        { "２Ｄ", "LayoutUpdate", 256, "レイアウト", "2D" },
        { "2D StopScene", "LayoutUpdate", 8, "レイアウト", "2D" },
        { "2D StallScene", "LayoutUpdate", 4, "レイアウト", "2DAboveBlur2" },
        { "２Ｄ（ポーズ無視）", "LayoutUpdate", 96, "レイアウト", "2D(IgnorePose)" },
        { "エフェクト（前処理）", "Functor", 1, "エフェクト", nullptr },
        { "エフェクト（３Ｄ）", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（プレイヤー）", "Execute", 1, "エフェクト", nullptr },
        { "Effect (HitStop)", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（カメラデモ）", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（ベース２Ｄ）", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（２Ｄ）", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（下画面ベース２Ｄ）", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（下画面２Ｄ）", "Execute", 1, "エフェクト", nullptr },
        { "Effect (2DAboveBlur)", "Execute", 1, "エフェクト", nullptr },
        { "エフェクト（後処理）", "Functor", 1, "エフェクト", nullptr },
        { "Stamp", "ActorMovementCalcAnim", 2, "システム", nullptr },
        { "Water", "ActorMovement", 1, "地形オブジェ", "Water Movement" },
        { "HitSensorThreadUpdate", "Execute", 1, "システム", "Hit Sensor Director MultiThreaded" },
        { "ClippingDirectorRequest", "Execute", 1, "システム", "Clipping Director Request" }
    };

    static void updateTableInitHook(al::ExecuteTableHolderUpdate* thisPtr, const al::ExecuteSystemInitInfo& info, const al::ExecuteOrder* order, int size, bool something) {
        thisPtr->init(info, sCustomUpdateTableOrder, IM_ARRAYSIZE(sCustomUpdateTableOrder), something);
    }

    void initExecuteDirectorExtensionHooks() {
        hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x00808a44, updateTableInitHook);
    }

} // namespace pe
