#include "Scenes/CatLoader.hpp"

#define WORKER_STACKSIZE (64 * 1024)

std::string CatLoaderList::error = "";
std::string CatLoaderList::tip = "";
std::array<Project, 6> CatLoaderList::projects;


CatLoaderList::CatLoaderList()
{
    name = "Cat Loader List";
    spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/CatLoader.t3x");

    SplashSheet = C2D_SpriteSheetLoad("romfs:/gfx/SplashScreen-CatLoader.t3x");
    topSplash = C2D_SpriteSheetGetImage(SplashSheet, 0);
    botSplash = C2D_SpriteSheetGetImage(SplashSheet, 1);

    splashScreenTimer = osGetTime();

    nameField = C2D_SpriteSheetGetImage(spritesheet, 0);
    loadingImg = C2D_SpriteSheetGetImage(spritesheet, 7);

    downloadButton = new Object(this);
    downloadButton->image = C2D_SpriteSheetGetImage(spritesheet, 1);
    downloadButton->position = vec2i(5, 45);
    downloadButton->size = vec2i(308, 52);
    downloadButton->scale = vec2(1.0f, 1.0f);

    profileButton = new Object(this);
    profileButton->image = C2D_SpriteSheetGetImage(spritesheet, 2);
    profileButton->position = vec2i(5, 103);
    profileButton->size = vec2i(308, 52);
    profileButton->scale = vec2(1.0f, 1.0f);

    moreButton = new Object(this);
    moreButton->position = vec2i(5, 161);
    moreButton->size = vec2i(308, 52);
    moreButton->scale = vec2(1.0f, 1.0f);
    moreButton->image = C2D_SpriteSheetGetImage(spritesheet, 3);

    togglerToScratch = new Object(this);
    togglerToScratch->image = C2D_SpriteSheetGetImage(spritesheet, 4);
    togglerToScratch->position = vec2i(0, 221);
    togglerToScratch->size = vec2i(326, 44);
    togglerToScratch->scale = vec2(1.0f, 1.0f);

    togglerToBox = new Object(this);
    togglerToBox->image = C2D_SpriteSheetGetImage(spritesheet, 5);
    togglerToBox->position = vec2i(160, 228);
    togglerToBox->size = vec2i(326, 44);
    togglerToBox->scale = vec2(1.0f, 1.0f);

    font = C2D_FontLoad("romfs:/gfx/font.bcfnt");
    projectsBuf = C2D_TextBufNew(4096);


    ApiResult resHttp = initHttp();
    if (!resHttp.success)
    {
        error = resHttp.message;
        tip = resHttp.tip;
    }
    selectedIdx = 0;
    page = 0;
    workerPage = 0;
    siteEnd = 0;
    workerIndex = -1;
    workerPage = -1;
    workerBusy = false;
    workerDone = false;
    workerThread = nullptr;
    workerRGBA = nullptr;
    workerW = 0;
    workerH = 0;

    ApiResult res = getProjects(!scratchBox, getAPIExplore(!scratchBox, studioID, page), projects);
    log(res.message.c_str());
    if (res.success)
    {
        log("SUCCESS");
        siteEnd = 0;
        for (const auto &p : projects)
        {   
            log("TITLE:");
            log(p.title.c_str());
            if (p.id != "0")
                siteEnd++;
        }
        workerDone = false;
        workerBusy = false;
    }
    else
    {
        log("FAIL");
        error = res.message;
        tip = res.tip;
        requestSceneChange = true;
    }
    CatLoaderList::updateText();

    svcGetThreadPriority(&mainPrio, CUR_THREAD_HANDLE);
}

CatLoaderList::~CatLoaderList()
{
    joinAndFreeWorker();

    if (spritesheet)
        C2D_SpriteSheetFree(spritesheet);
    nameField = {nullptr, nullptr};

    for (auto &p : projects)
    {
        if (p.image.tex && p.image.subtex)
        {
            C3D_TexDelete(p.image.tex);
            p.image = {nullptr, nullptr};
        }
        p.loaded = false;
    }
    delete downloadButton;
    delete profileButton;
    delete moreButton;
    delete togglerToScratch;
    delete togglerToBox;
    if (font) {
        C2D_FontFree(font);
        font = nullptr;
    }
    if (projectsBuf) {
        C2D_TextBufDelete(projectsBuf);
        projectsBuf = nullptr;
    }

    selectedIdx = 0;
    page = 0;
    siteEnd = 0;
    workerIndex = -1;
    workerPage = -1;
    workerBusy = false;
    workerDone = false;
    workerThread = nullptr;
    workerW = 0;
    workerH = 0;
}

void CatLoaderList::render()
{
    if (splashScreen) {
        C2D_TargetClear(SceneManager::top, C2D_Color32(87, 145, 184, 255));
        C2D_SceneBegin(SceneManager::top);
        C2D_DrawImageAt(topSplash, 0, 0, 0, nullptr, 1.0f, 1.0f);
        C2D_TargetClear(SceneManager::bottom, C2D_Color32(87, 145, 184, 255));
        C2D_SceneBegin(SceneManager::bottom);
        C2D_DrawImageAt(botSplash, 0, 0, 0, nullptr, 1.0f, 1.0f);
        return;
    }
    C2D_TargetClear(SceneManager::top, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::top);
    for (int i = 0; i < 6; i++)
    {
        if (i >= siteEnd)
            break;
        int row = i / 3;
        int col = i % 3;
        float x = 10 + col * (120 + 10);
        float y = 10 + row * (90 + 10 + 12);

        if (i == selectedIdx)
        {
            C2D_DrawRectSolid(x - 2, y - 2, 0, 120 + 4, 90 + 4, C2D_Color32(255, 255, 255, 255));
        }

        if (projects[i].loaded and projects[i].image.tex)
        {
            C2D_DrawImageAt(projects[i].image, x, y, 0, nullptr, 1.0f, 1.0f);
        }
        else
        {   
            C2D_DrawImageAt(loadingImg, x, y, 0, nullptr, 1.0f, 1.0f);
        }

        
    }

    C2D_TargetClear(SceneManager::bottom, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::bottom);
    C2D_DrawImageAt(nameField, 4, 10, 0, nullptr, 1.0f, 1.0f);
    C2D_DrawText(&projectsText[selectedIdx], C2D_WithColor | C2D_AlignLeft, 10, 12, 0.0f, 1.0f, 1.0f, C2D_Color32(0, 0, 0, 255));
    downloadButton->render();
    profileButton->render();
    moreButton->render();
    togglerToScratch->render();
    togglerToBox->render();
}

void CatLoaderList::update(float delta)
{
    if (downloadButton->isClicked() && projects[selectedIdx].id != "0") {
        joinAndFreeWorker();
        if(projects[selectedIdx].title == "") {
            error = "The project name must not be empty";
            tip = "Choose a different project next time if the game has no name after reloading";
            requestSceneChange = true;
        }
        else
            SceneManager::load(new DownloadAnimation(scratchBox, projects[selectedIdx].id, projects[selectedIdx].title));
    }


    if (moreButton->isClicked() || profileButton->isClicked()) {
        error = "Unknown function";
        tip = "sorry that feature does not work yet";
        requestSceneChange = true;
    }

    if (togglerToScratch->isClicked() && scratchBox) {
        togglerToBox->position.y = 228;
        togglerToScratch->position.y = 221;
        scratchBox = false;
        page = 0;
        selectedIdx = 0;
        ApiResult res = getProjects(!scratchBox, getAPIExplore(!scratchBox, studioID, page), projects);
        if (res.success)
        {
            siteEnd = 0;
            for (const auto &p : projects)
                if (p.id != "0")
                    siteEnd++;

            joinAndFreeWorker();
            CatLoaderList::updateText();
        }
        else
        {
            error = res.message;
            tip = res.tip;
            requestSceneChange = true;
        }
    }

    if (togglerToBox->isClicked() && !scratchBox) {
        togglerToBox->position.y = 221;
        togglerToScratch->position.y = 228;
        scratchBox = true;
        page = 0;
        selectedIdx = 0;
        ApiResult res = getProjects(!scratchBox, getAPIExplore(!scratchBox, studioID, page), projects);
        if (res.success)
        {
            siteEnd = 0;
            for (const auto &p : projects)
                if (p.id != "0")
                    siteEnd++;


            joinAndFreeWorker();
            CatLoaderList::updateText();
        }
        else
        {
            error = res.message;
            tip = res.tip;
            requestSceneChange = true;
        }
    }


    if (!splashScreen) {
        if (InputManager::isKeyPressed(Keys::B))
        {
            joinAndFreeWorker();
            requestSceneChange = true;
        }
        if (InputManager::isKeyPressed(Keys::RIGHT))
            selectedIdx++;
        if (InputManager::isKeyPressed(Keys::LEFT))
            selectedIdx--;
        if (InputManager::isKeyPressed(Keys::DOWN))
            selectedIdx += 3;
        if (InputManager::isKeyPressed(Keys::UP))
            selectedIdx -= 3;
    } 
    else {
        if (osGetTime() - splashScreenTimer > 2500) {
            splashScreenTimer = 0;
            splashScreen = false;
             if (SplashSheet)
                C2D_SpriteSheetFree(SplashSheet);
            botSplash = {nullptr, nullptr};
            topSplash = {nullptr, nullptr};
        }
    }



    if (selectedIdx < 0)
    {
        selectedIdx = 0;
        if (page > 0)
        {
            page--;
            ApiResult res = getProjects(!scratchBox, getAPIExplore(!scratchBox, studioID, page), projects);

            if (res.success)
            {
                siteEnd = 0;
                for (const auto &p : projects)
                    if (p.id != "0")
                        siteEnd++;


                joinAndFreeWorker();
                CatLoaderList::updateText();
            }
            else
            {
                error = res.message;
                tip = res.tip;
                requestSceneChange = true;
            }
            selectedIdx = 5;
        }
    }


    if (selectedIdx >= siteEnd)
    {
        selectedIdx = siteEnd - 1;
        if (selectedIdx < 0)
            selectedIdx = 0;

        if (siteEnd == 6)
        {
            page++;
            ApiResult res = getProjects(!scratchBox, getAPIExplore(!scratchBox, studioID, page), projects);

            if (res.success)
            {
                siteEnd = 0;
                for (const auto &p : projects)
                    if (p.id != "0")
                        siteEnd++;


                joinAndFreeWorker();
                CatLoaderList::updateText();
            }
            else
            {
                error = res.message;
                tip = res.tip;
                requestSceneChange = true;
            }
            selectedIdx = 0;
        }
    }


    if (workerDone)
    {
        if (workerRGBA && page == workerPage)
        {
            Project &p = projects[workerIndex];

            size_t bytes = (size_t)workerW * (size_t)workerH * 4;
            C2D_Image img = createC2DImageFromRGBA(workerRGBA, workerW, workerH);
            if (!(bytes > 0) || !(img.tex && img.subtex))
            {
                p.image = C2D_SpriteSheetGetImage(spritesheet, 6);
                img = {nullptr, nullptr};
                p.loaded = true;
            } else {
                
                p.image = img;
                p.loaded = true;
            }
        }
        else
        {

            if (!workerRGBA && page == workerPage) {
                Project &p = projects[workerIndex];
                p.image = C2D_SpriteSheetGetImage(spritesheet, 6);
                p.loaded = true;
            }
            else if (workerIndex >= 0 && workerIndex < 6)
                projects[workerIndex].loaded = false;
        }

        joinAndFreeWorker();
    }

    if (requestSceneChange && !workerThread) {
        if (error != "") SceneManager::load(new ErrorDisplay(error, tip));
        else SceneManager::load(new MainMenu());
        requestSceneChange = false;
        return;
    }

    bool launchedThisFrame = false;


    if (!workerThread && !projects[selectedIdx].loaded)
    {
        workerIndex = selectedIdx;
        int *arg = (int *)malloc(sizeof(int));
        *arg = selectedIdx;

        log(("Trying to launch worker for idx=" + std::to_string(selectedIdx) +
             " page=" + std::to_string(page) +
             " prio=" + std::to_string(mainPrio - 1))
                .c_str());

        Thread t = threadCreate(workerMain, arg, WORKER_STACKSIZE, mainPrio - 1, -2, false);

        if (!t)
        {
            log("threadCreate FAILED");
            free(arg);
        }
        else
        {
            workerThread = t;
            workerBusy = true;
            workerDone = false;
            workerPage = page;
            launchedThisFrame = true;
            log("Worker launched (selected project)");
        }
    }


    if (!launchedThisFrame && !workerThread)
    {
        for (int i = 0; i < siteEnd; ++i)
        {
            if (!projects[i].loaded)
            {
                workerIndex = i;
                int *arg = (int *)malloc(sizeof(int));
                *arg = i;

                log(("Trying fallback worker for idx=" + std::to_string(i)).c_str());

                Thread t = threadCreate(workerMain, arg, WORKER_STACKSIZE, mainPrio - 1, -2, false);

                if (!t)
                {
                    log("fallback threadCreate FAILED");
                    free(arg);
                }
                else
                {
                    workerThread = t;
                    workerBusy = true;
                    workerDone = false;
                    workerPage = page;
                    log("Fallback worker launched");
                }
                break;
            }
        }
    }
}

ApiResult CatLoaderList::updateText()
{
    ApiResult res;
    C2D_TextBufClear(projectsBuf);
    size_t i;
    for (i = 0; i < 6; i++)
    {
        if (projects[i].id == "0")
            break;
        std::string newTitle = projects[i].title;
        newTitle.resize(20);
        C2D_TextFontParse(&projectsText[i], font, projectsBuf, newTitle.c_str());
        C2D_TextOptimize(&projectsText[i]);
    }
    siteEnd = i;
    res.success = true;
    return res;
}



DownloadAnimation::DownloadAnimation(bool NewscratchBox, std::string Newid, std::string name) {
    font = C2D_FontLoad("romfs:/fonts/MyFont.bcfnt");
    textBuf = C2D_TextBufNew(4096);
    C2D_TextFontParse(&msgTexts[0], font, textBuf, "DOWNLOAD");
    C2D_TextFontParse(&msgTexts[1], font, textBuf, "Do not turn off your device");
    C2D_TextOptimize(&msgTexts[0]);
    C2D_TextOptimize(&msgTexts[1]);
    scratchBox = NewscratchBox;
    project_id = Newid;
    project = name;
}

DownloadAnimation::~DownloadAnimation() {
    if (font) {
        C2D_FontFree(font);
        font = nullptr;
    }
    if (textBuf) {
        C2D_TextBufDelete(textBuf);
        textBuf = nullptr;
    }
    exitHttp();
}

void DownloadAnimation::render() {
    C2D_TargetClear(SceneManager::top, C2D_Color32(255, 103, 87, 255));
    C2D_SceneBegin(SceneManager::top);
    C2D_DrawText(&msgTexts[0], C2D_AtBaseline | C2D_WithColor | C2D_AlignCenter, 200.0f, 30.0f, 0, 1.5f, 1.5f, C2D_Color32(0, 153, 51, 255));
    
    C2D_DrawText(&msgTexts[1], C2D_AtBaseline | C2D_WithColor | C2D_AlignCenter | C2D_WordWrap, 200.0f, 235.0f, 0, 0.75f, 0.75f, C2D_Color32(102, 0, 0, 255), 380.0f);

    C2D_TargetClear(SceneManager::bottom, C2D_Color32(255, 103, 87, 255));
}

void DownloadAnimation::update(float delta) {
    if(!scratchBox){
        ApiResult res = download_project_to_folder(project_id, "sdmc:/3ds/GekoStudio", project);
        if (!res.success){
            SceneManager::load(new ErrorDisplay(res.message, res.tip));
            return;
        }
        SceneManager::load(new MainMenu());
    } else {
        ApiResult res = download_ScratchBox_to_folder(project_id, "sdmc:/3ds/GekoStudio", project);
        if (!res.success){
            SceneManager::load(new ErrorDisplay(res.message, res.tip));
            return;
        }
        SceneManager::load(new MainMenu());
    }
}