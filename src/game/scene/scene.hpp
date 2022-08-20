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
    virtual void update_worker() {}
    virtual void update_keyboard(float dt, const bool* pressed) {}
    virtual void render() {}

    virtual SceneFlags get_options() {
        return SceneFlags();
    }
};
