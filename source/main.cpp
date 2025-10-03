#include "debugg.hpp"
#include "SceneManager.hpp"
#include "inputs.hpp"
#include "Scenes/MainMenu.hpp"

int main() {
    SceneManager::initConsole();
    SceneManager::initScreens();

    createDirs("sdmc:/3ds/CatEngine");

    SceneManager::load(new SplashScreen());

    InputManager::loadInputs();


    
    while (SceneManager::appShouldRun())
    {
        InputManager::loadInputs();
        SceneManager::update();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        SceneManager::render();
        C3D_FrameEnd(0);
    }
    SceneManager::deinitConsole();
    return 0;
}