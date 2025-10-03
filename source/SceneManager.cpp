#include <3ds.h>
#include <citro2d.h>

#include "Scene.hpp"
#include "SceneManager.hpp"
#include "debugg.hpp"


Scene* SceneManager::currentScene = nullptr;
C3D_RenderTarget* SceneManager::top = nullptr;
C3D_RenderTarget* SceneManager::bottom = nullptr;

u32 SceneManager::lastTime = 0;
u32 SceneManager::now = 0;

bool SceneManager::CloseApp = false;

void SceneManager::initScreens() {
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    lastTime = osGetTime();
    log("Screens initialized\n");
}

void SceneManager::load(Scene* newScene) {
    log("Switching Scene");
    if (currentScene) {
        log(("from: " + currentScene->name).c_str());
        delete currentScene;
    } else {
        log("from: nullptr");
    }
    log(("to: " + newScene->name).c_str());

    currentScene = newScene;
}

void SceneManager::render() {

    if(!currentScene) return;
    currentScene->render();
}

void SceneManager::update() {
    now = osGetTime();
    float deltaTime = (now - lastTime) / 1000.0f;
    lastTime = now;
    if(!currentScene) return;
    currentScene->update(deltaTime);
}

void SceneManager::initConsole() {
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    romfsInit();
    CloseApp = false;
}

void SceneManager::deinitConsole() {
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
}

bool SceneManager::appShouldRun() {
    if (InputManager::isKeyDown(Keys::START)) CloseApp = true;
    return !CloseApp && aptMainLoop();
}
