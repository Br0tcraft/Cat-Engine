#include "../Scenes/MainMenu.hpp"



SplashScreen::SplashScreen() {
    name = "SplashScreen";
    spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/SplashScreen.t3x");

    topImg = C2D_SpriteSheetGetImage(spritesheet, 0);
    botImg = C2D_SpriteSheetGetImage(spritesheet, 1);

    timer = osGetTime();
    
    log(("Load Scene " + name + " success").c_str());
}

SplashScreen::~SplashScreen() {
    if (spritesheet)
        C2D_SpriteSheetFree(spritesheet);
    topImg = {nullptr, nullptr};
    botImg = {nullptr, nullptr};
}

void SplashScreen::render() {
    C2D_TargetClear(SceneManager::top, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::top);
    C2D_DrawImageAt(topImg, 0, 0, 0, nullptr, 1.0f, 1.0f);

    C2D_TargetClear(SceneManager::bottom, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::bottom);
    C2D_DrawImageAt(botImg, 0, 0, 0, nullptr, 1.0f, 1.0f);
}

void SplashScreen::update(float delta) {
    if(osGetTime() - timer > 3000) {
    // Wechsel zur nächsten Szene
    SceneManager::load(new MainMenu());
    }
}

MainMenu::MainMenu() {
    name = "MainMenu";
    spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/MainMenu.t3x");
    easing = 0.36f;
    logo = C2D_SpriteSheetGetImage(spritesheet, 0);

    projectButton = new Object(this);
    projectButton->image = C2D_SpriteSheetGetImage(spritesheet, 1);
    projectButton->position = vec2i(26, 63);
    projectButton->scale = vec2(1.0f, 1.0f);
    projectButton->size = vec2i(114, 114);

    webButton = new Object(this);
    webButton->image = C2D_SpriteSheetGetImage(spritesheet, 2);
    webButton->position = vec2i(183, 63);
    webButton->size = vec2i(114, 114);
    webButton->scale = vec2(1.0f, 1.0f);
}

MainMenu::~MainMenu() {
    if (spritesheet)
        C2D_SpriteSheetFree(spritesheet);
    logo = {nullptr, nullptr};
}

void MainMenu::update(float delta) {
    //Logo movement
    const float speed = 0.23f;
    const float amplitude = 17.0f;

    easing += speed * delta;
    easing = fmodf(easing, 1.0f);

    float eased = 0.5f * (1.0f - cosf(easing * 2.0f * 3.14159265f));

    LogoY = static_cast<int>(eased * amplitude) - 15;

    //button click
    if(projectButton->isClicked())
    {
        SceneManager::load(new ProjectList());
    }
    else if(webButton->isClicked())
    {
        SceneManager::load(new CatLoaderList());
    }
}

void MainMenu::render() {
    C2D_TargetClear(SceneManager::top, C2D_Color32(255, 103, 87, 255));
    C2D_SceneBegin(SceneManager::top);
    C2D_DrawImageAt(logo, 0, LogoY, 0, nullptr, 1.0f, 1.0f);

    C2D_TargetClear(SceneManager::bottom, C2D_Color32(255, 103, 87, 255));
    C2D_SceneBegin(SceneManager::bottom);
    projectButton->render();
    webButton->render();
}


ProjectList::ProjectList() {
    name = "ProjectList";
    spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/ProjectList.t3x");

    ImageBackground = C2D_SpriteSheetGetImage(spritesheet, 4);
    nameField = C2D_SpriteSheetGetImage(spritesheet, 0);

    openButton = new Object(this);
    openButton->image = C2D_SpriteSheetGetImage(spritesheet, 1);
    openButton->position = vec2i(5, 45);
    openButton->size = vec2i(308, 52);
    openButton->scale = vec2(1.0f, 1.0f);

    editButton = new Object(this);
    editButton->image = C2D_SpriteSheetGetImage(spritesheet, 2);
    editButton->position = vec2i(5, 103);
    editButton->size = vec2i(308, 52);
    editButton->scale = vec2(1.0f, 1.0f);

    inSeButton = new Object(this);
    inSeButton->image = C2D_SpriteSheetGetImage(spritesheet, 3);
    inSeButton->position = vec2i(5, 161);
    inSeButton->size = vec2i(308, 52);
    inSeButton->scale = vec2(1.0f, 1.0f);

    Toggler = new Object(this);
    Toggler->image = C2D_SpriteSheetGetImage(spritesheet, 5);
    Toggler->position = vec2i(-3, 217);
    Toggler->size = vec2i(326, 44);
    Toggler->scale = vec2(1.0f, 1.0f);

    
    log(("Load Scene " + name + " success").c_str());
}

ProjectList::~ProjectList() {
    if (spritesheet)
        C2D_SpriteSheetFree(spritesheet);
    ImageBackground = {nullptr, nullptr};
    nameField = {nullptr, nullptr};
    delete openButton;
    delete editButton;
    delete inSeButton;
    delete Toggler;
}
void ProjectList::render() {
    C2D_TargetClear(SceneManager::top, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::top);
    C2D_DrawImageAt(ImageBackground, 0, 0, 0, nullptr, 1.0f, 1.0f);

    C2D_TargetClear(SceneManager::bottom, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::bottom);
    C2D_DrawImageAt(nameField, 4, 10, 0, nullptr, 1.0f, 1.0f);
    openButton->render();
    editButton->render();
    inSeButton->render();
    Toggler->render();
}

void ProjectList::update(float delta) {
    if (InputManager::isKeyPressed(Keys::B)) SceneManager::load(new MainMenu());
}

ErrorDisplay::ErrorDisplay(const std::string& err, const std::string& t) {
    error = err;
    tip = t;
    name = "Error Scene";
    spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/Error.t3x");

    closeButton = new Object(this);
    closeButton->image = C2D_SpriteSheetGetImage(spritesheet, 0);
    closeButton->position = vec2i(5, 161);
    closeButton->size = vec2i(308, 52);
    closeButton->scale = vec2(1.0f, 1.0f);

    font = C2D_FontLoad("romfs:/fonts/MyFont.bcfnt");
    textBuf = C2D_TextBufNew(4096);
    C2D_TextFontParse(&errorTexts[0], font, textBuf, "ERROR OCCURRED");
    C2D_TextFontParse(&errorTexts[1], font, textBuf, (error + "\n\n" + tip).c_str());
    //C2D_TextFontParse(&errorTexts[2], font, textBuf, );
    C2D_TextFontParse(&errorTexts[2], font, textBuf, "close the App to continue");
    C2D_TextOptimize(&errorTexts[0]);
    C2D_TextOptimize(&errorTexts[1]);
    C2D_TextOptimize(&errorTexts[2]);
    //C2D_TextOptimize(&errorTexts[3]);
}

ErrorDisplay::~ErrorDisplay() {
    if (spritesheet)
        C2D_SpriteSheetFree(spritesheet);
    delete closeButton;
    if (font) {
        C2D_FontFree(font);
        font = nullptr;
    }
    if (textBuf) {
        C2D_TextBufDelete(textBuf);
        textBuf = nullptr;
    }
}

void ErrorDisplay::render() {
    C2D_TargetClear(SceneManager::top, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::top);
    C2D_DrawText(&errorTexts[0], C2D_AtBaseline | C2D_WithColor | C2D_AlignCenter, 200.0f, 30.0f, 0, 1.5f, 1.5f, C2D_Color32(179, 0, 0, 255));
    C2D_DrawText(&errorTexts[1], C2D_WithColor | C2D_AlignLeft | C2D_WordWrap, 10.0f, 60.0f, 0, 1.0f, 1.0f, C2D_Color32(255, 255, 255, 255), 380.0f);
    C2D_DrawText(&errorTexts[2], C2D_AtBaseline | C2D_WithColor | C2D_AlignCenter | C2D_WordWrap, 200.0f, 235.0f, 0, 0.75f, 0.75f, C2D_Color32(255, 255, 255, 255), 380.0f);

    C2D_TargetClear(SceneManager::bottom, C2D_Color32(87, 145, 184, 255));
    C2D_SceneBegin(SceneManager::bottom);
    closeButton->render();
}

void ErrorDisplay::update(float delta) {
    if(closeButton->isClicked()) {
        SceneManager::CloseApp = true;
    }
    return;
}