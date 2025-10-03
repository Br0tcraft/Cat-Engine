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
#include "download.hpp"
#include "debugg.hpp"
#include <curl/curl.h>
#include <malloc.h>

using json = nlohmann::json;

extern Thread workerThread;
extern bool workerBusy;
extern bool workerDone;
extern int workerIndex;
extern int workerPage;

extern u8* workerRGBA;
extern int workerW, workerH;

void joinAndFreeWorker();


void workerMain(void* arg);