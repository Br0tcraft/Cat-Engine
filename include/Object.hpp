#pragma once
#include <citro2d.h>
#include <string>
#include "types.hpp"
#include "inputs.hpp"
#include "debugg.hpp"

class Scene; // Forward

class Object {
public:
    std::string name = "";

    Scene* parent = nullptr;
    vec2i position = vec2i(0,0);
    vec2 scale = vec2(1.0f,1.0f);
    vec2i size = vec2i(1,1);
    C2D_Image image;

    Object(Scene* newParent) : parent(newParent), image{nullptr, nullptr} {}
    virtual ~Object() {}

    void render() {
        if(image.tex && image.subtex) C2D_DrawImageAt(image, position.x, position.y, 0.0f, nullptr, scale.x, scale.y);
        //if(image.tex && image.subtex) C2D_DrawImageAt(image, 0, 0, 0.0f, nullptr, 1.0f, 1.0f);
    }

    virtual void update(float deltaTime) {}

    bool isPointInside(vec2i point) {
        return (point.x >= position.x && point.x <= position.x + size.x * scale.x &&
                point.y >= position.y && point.y <= position.y + size.y * scale.y);
    }

    bool isClicked() {
        return InputManager::isMousePressed() && isPointInside(InputManager::getMousePos());
    }
};
