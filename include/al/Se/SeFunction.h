#pragma once

#include "al/Audio/AudioKeeper.h"
#include "al/Audio/MeInfo.h"
#include <sead/prim/seadSafeString.h>

namespace al {

void startSe(const IUseAudioKeeper* user, const sead::SafeString& name, MeInfo* = nullptr);
bool tryStartSe(const IUseAudioKeeper* user, const sead::SafeString& name, MeInfo* = nullptr);
bool tryHoldSeWithParam(const IUseAudioKeeper* user, const sead::SafeString& name, float, MeInfo* = nullptr);
bool isExistSePlayNameInUserInfo(const IUseAudioKeeper* user, const char* name);
void setSeSeqLocalVariableDefault(const IUseAudioKeeper*, int, int);
bool tryStopSe(const IUseAudioKeeper* user, const sead::SafeString& name);
void stopSequenceBgm(const IUseAudioKeeper* user, const char* bgm, int);

} // namespace al
