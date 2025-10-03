#pragma once
#include "Scene.hpp"
#include <citro3d.h>
#include "inputs.hpp"
class SceneManager
{
public:
    static Scene *currentScene;
    static C3D_RenderTarget *top;
    static C3D_RenderTarget *bottom;

    static u32 now;
    static u32 lastTime;

    static bool CloseApp;

    static void initScreens();
    static void load(Scene *newScene);
    static void render();
    static void update();
    static void initConsole();
    static void deinitConsole();
    static bool appShouldRun();
};
