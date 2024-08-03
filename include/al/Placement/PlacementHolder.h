#pragma once

namespace al {

class PlacementHolder {
    const char* _0;
    const char* _8;
    const char* _10;
    const char* _18;
    int mZoneID;
    int _24;

public:
    // FAKE
    const char* mUnitConfigName = nullptr;
    const char* mIdClone = nullptr;
    const char* mModelNameClone = nullptr;
    bool mKilledByHook = false;

    PlacementHolder();
    PlacementHolder(const char*, const char* unitConfigName, const char* id);
    ~PlacementHolder();

    bool isEqual(const PlacementHolder& rhs) const;
    static bool isEqual(const PlacementHolder& lhs, const PlacementHolder& rhs);

    void copyFromParent(const PlacementHolder& parent);
};

} // namespace al
