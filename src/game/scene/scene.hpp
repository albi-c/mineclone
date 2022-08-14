#pragma once

class Scene {
public:
    virtual ~Scene() {}

    virtual void init() {}

    virtual void update(float dt) {}
    virtual void render() {}
};
