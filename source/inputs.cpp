#include <3ds.h>
#include <citro2d.h>
#include "inputs.hpp"
#include "types.hpp"

u32 InputManager::inputsDown = 0;
u32 InputManager::inputsUp = 0;
u32 InputManager::inputsHeld = 0;

vec2i InputManager::getMousePos() {
    touchPosition touch;
    hidTouchRead(&touch);
    return vec2i(touch.px, touch.py);
}

bool InputManager::isMousePressed() {
    return inputsDown & Keys::TOUCH;
}

bool InputManager::isMouseReleased() {
    return inputsUp & Keys::TOUCH;
}

bool InputManager::isMouseDown() {
    return inputsHeld & Keys::TOUCH;
}

bool InputManager::isKeyPressed(Keys key) {
    return inputsDown & key;
}

bool InputManager::isKeyReleased(Keys key) {
    return inputsUp & key;
}

bool InputManager::isKeyDown(Keys key) {
    return inputsHeld & key;
}

void InputManager::loadInputs() {
    hidScanInput();
    InputManager::inputsDown = hidKeysDown();
    InputManager::inputsHeld = hidKeysHeld();
    InputManager::inputsUp = hidKeysUp();
}