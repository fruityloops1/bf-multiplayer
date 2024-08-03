#pragma once

namespace al {

class Scene;
class SceneInitInfo;

class LayoutInitInfo {
};

void initLayoutInitInfo(al::LayoutInitInfo* info, const al::Scene* scene, const al::SceneInitInfo& sceneInfo);

} // namespace al