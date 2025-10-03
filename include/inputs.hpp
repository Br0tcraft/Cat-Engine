#pragma once
#include <3ds.h>
#include "types.hpp"

enum Keys {
    A = KEY_A,
    B = KEY_B,
    X = KEY_X,
    Y = KEY_Y,
    L = KEY_L,
    R = KEY_R,
    ZL = KEY_ZL,
    ZR = KEY_ZR,
    START = KEY_START,
    SELECT = KEY_SELECT,
    DRIGHT = KEY_DRIGHT,
    DLEFT = KEY_DLEFT,
    DUP = KEY_DUP,
    DDOWN = KEY_DDOWN,
    TOUCH = KEY_TOUCH,
    UP = KEY_UP,
    DOWN = KEY_DOWN,
    RIGHT = KEY_RIGHT,
    LEFT = KEY_LEFT,
};


class InputManager {
public:
    static u32 inputsDown;
    static u32 inputsUp;
    static u32 inputsHeld;


    static void loadInputs();
    static vec2i getMousePos();
    static bool isMousePressed();
    static bool isMouseReleased();
    static bool isMouseDown();
    static bool isKeyPressed(Keys key);
    static bool isKeyReleased(Keys key);
    static bool isKeyDown(Keys key);
};