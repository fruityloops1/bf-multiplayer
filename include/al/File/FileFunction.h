#pragma once

#include "resource/seadArchiveRes.h"
#include <sead/prim/seadSafeString.h>

namespace al {

bool isExistArchive(const sead::SafeString& archive);
sead::ArchiveRes* loadArchive(const sead::SafeString& archive);
sead::ArchiveRes* loadArchiveWithExt(const sead::SafeString& archive, const char* extension);

void makeLocalizedArchivePath(sead::BufferedSafeString* out, const sead::SafeString& archive);
void makeStageDataArchivePath(sead::BufferedSafeString* out, const char* stageName, int scenario, const char* type /* Design, Map, Sound */);

} // namespace al
