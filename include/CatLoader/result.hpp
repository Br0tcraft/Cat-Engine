#pragma once
#include <string.h>
#include <citro2d.h>

struct ApiResult {
    bool success = false;
    std::string message; // description of error if success == false
    std::string tip;     // optional tip or hint
};

struct Project {
    std::string id;
    std::string imagePath;
    C2D_Image image;
    std::string title;
    std::string iconPath;
    C2D_Image icon;
    std::string username;
    std::string description;
    bool loaded = false;

    std::vector<u8> rgbaCache;
    int w = 0, h = 0;
};