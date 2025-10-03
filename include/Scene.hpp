#pragma once
#include <string>
#include <citro2d.h>

class Scene {
public:
    std::string name;
    C2D_SpriteSheet spritesheet;

    Scene() : name("Scene"), spritesheet(nullptr) {}
    virtual ~Scene() {}

    virtual void render() = 0;
    virtual void update(float deltaTime) {}
};
