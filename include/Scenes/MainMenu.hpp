#pragma once
#include <3ds.h>
#include "../Scene.hpp"
#include "../Object.hpp"
#include "CatLoader.hpp"
#include "../debugg.hpp"
#include "../SceneManager.hpp"

class ProjectList : public Scene
{
private:

    Object *openButton = nullptr;
    Object *editButton = nullptr;
    Object *inSeButton = nullptr;

    Object *Toggler = nullptr;

    C2D_Image nameField;
    C2D_Image ImageBackground;
    
public:
    ProjectList();
    ~ProjectList();

    void render() override;
    void update(float delta) override;
};


class MainMenu : public Scene
{
private:
    C2D_Image logo;

    Object *projectButton = nullptr;
    Object *webButton = nullptr;

    float easing;

    int LogoY;
    
public:
    MainMenu();
    ~MainMenu();

    void render() override;
    void update(float delta) override;
};



class SplashScreen : public Scene
{
public:
    C2D_Image topImg;
    C2D_Image botImg;

    u64 timer;

    SplashScreen();
    ~SplashScreen();

    void render() override;
    void update(float delta) override;
};

class ErrorDisplay : public Scene
{
private:
    C2D_TextBuf textBuf;
    C2D_Text errorTexts[3];
    C2D_Font font = nullptr;
public:
    Object *closeButton = nullptr;
    
    std::string error = "Unknown Error";
    std::string tip = "Idk what happened";

    ErrorDisplay(const std::string& err, const std::string& t);
    ~ErrorDisplay();

    void render() override;
    void update(float delta) override;
};