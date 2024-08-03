#pragma once

#include <sead/heap/seadHeap.h>
#include <sead/heap/seadHeapMgr.h>

namespace al {

class SceneHeapSetter : sead::ScopedCurrentHeapSetter {
    sead::Heap* _8;

public:
    SceneHeapSetter();
};

sead::Heap* getStationedHeap();
sead::Heap* getSequenceHeap();
sead::Heap* getSceneResourceHeap();
sead::Heap* getSceneHeap();
sead::Heap* getCourseSelectResourceHeap();
sead::Heap* getCourseSelectHeap();
sead::Heap* tryFindNamedHeap(const char* name);

void createSceneHeap(const char* stageName);
void createSceneResourceHeap(const char*);
bool isCreatedSceneResourceHeap();
void destroySceneHeap(bool);
void createCourseSelectHeap();
void destroyCourseSelectHeap();
sead::Heap* findNamedHeap(const char* name);

} // namespace al
