#pragma once

struct SceneFlags {
    bool grab_mouse = false;
};

class Scene {
public:
    virtual ~Scene() {}

    virtual void init() {}

    virtual void enable() {}
    virtual void disable() {}

    virtual void update(float dt) {}
    virtual void render() {}

    virtual SceneFlags get_options() {
        return SceneFlags();
    }
};
