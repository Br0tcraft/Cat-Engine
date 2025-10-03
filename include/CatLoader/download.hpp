#pragma once
#include <nlohmann/json.hpp>
#include <3ds.h>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <set>
#include "result.hpp"
#include <vector>
#include "../Scenes/CatLoader.hpp"
#include "debugg.hpp"
#include <curl/curl.h>
#include <malloc.h>

std::string getAPIExplore(bool Scratch, std::string &studioID, int &page);

ApiResult initHttp();
void exitHttp();
ApiResult ensure_dir(const std::string& path);
ApiResult save_file_sd(const std::string& path, const u8* data, size_t size);
ApiResult http_download(const std::string &url, std::vector<u8> &out);
ApiResult download_project_to_folder(int project_id, std::string folder_base, std::string project_filename);
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp);

ApiResult http_get_json(const std::string &url, std::string &out);
ApiResult getProjects(const bool isScratch, const std::string &url, std::array<Project, 6> &projects);

u32 next_pow2(u32 n);
u32 rgba_to_abgr(u32 px);
C2D_Image createC2DImageFromRGBA(u8* rgba, int width, int height);
C2D_Image loadC2DImageFromUrl(const std::string &url, int min_width, int min_height);
void freeC2DImage(C2D_Image img);

unsigned char* resizeImage(const unsigned char* src, int width, int height, int new_width, int new_height);
unsigned char* resizeImageKeepAspect(const unsigned char* src, int width, int height, int target_height, int max_width, int& out_width, int& out_height);