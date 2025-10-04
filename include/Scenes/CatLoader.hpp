#pragma once
#include <3ds.h>
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include "Scene.hpp"
#include "Object.hpp"
#include "SceneManager.hpp"
#include "inputs.hpp"
#include "CatLoader/result.hpp"
#include "CatLoader/Thread.hpp"
#include "CatLoader/download.hpp"
#include "MainMenu.hpp"
#include "debugg.hpp"

struct CachedImage {
    u8* rgba = nullptr;
    int w = 0;
    int h = 0;
    size_t bytes = 0;
    bool has = false;
};

class CatLoaderList : public Scene
{
private:
    
    u64 splashScreenTimer = 0;
    bool splashScreen = true;

    C2D_SpriteSheet SplashSheet = nullptr;
    C2D_Image topSplash = {nullptr, nullptr};
    C2D_Image botSplash = {nullptr, nullptr};

    C2D_Image loadingImg = {nullptr, nullptr};

    Object *downloadButton = nullptr;
    Object *profileButton = nullptr;
    Object *moreButton = nullptr;
    Object *togglerToScratch = nullptr;
    Object *togglerToBox = nullptr;

    C2D_Image nameField = {nullptr, nullptr};

    C2D_TextBuf projectsBuf;
    C2D_Text projectsText[6];
    C2D_Font font = nullptr;


    bool scratchBox = false;

    std::string studioID = "36475140";
    int page = 0;
    int siteEnd;

    bool loadedFromCache = false;

    s32 mainPrio = 0;

    bool requestSceneChange = false;
public:
    int selectedIdx;
    static std::string error;
    static std::string tip;
    static std::array<Project, 6> projects;
    CatLoaderList();
    ~CatLoaderList();
    void updateList();
    ApiResult updateText();

    void render() override;
    void update(float delta) override;
};

class DownloadAnimation : public Scene {
private:
    C2D_TextBuf textBuf;
    C2D_Text msgTexts[2];
    C2D_Font font = nullptr;

    bool scratchBox = false;
    std::string project_id = "";
    std::string project = "";
public:
    DownloadAnimation(bool scratchBox, std::string id, std::string name);
    ~DownloadAnimation();

    void render() override;
    void update(float delta) override;
};