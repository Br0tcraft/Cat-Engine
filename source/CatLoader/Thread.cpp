#include "CatLoader/Thread.hpp"
#define STB_IMAGE_IMPLEMENTATION
extern "C"
{
#include "CatLoader/stb_image.h"
}

Thread workerThread = nullptr;
bool workerBusy = false;
bool workerDone = false;
int workerIndex = -1;
int workerPage = -1;

u8 *workerRGBA = nullptr;
int workerW = 0, workerH = 0;

void joinAndFreeWorker()
{
    if (workerThread)
    {
        threadJoin(workerThread, U64_MAX);
        threadFree(workerThread);
        workerThread = nullptr;
    }

    // Free worker resources after thread is joined
    if (workerRGBA)
    {
        free(workerRGBA);
        workerRGBA = nullptr;
    }
    
    workerW = 0;
    workerH = 0;
    workerIndex = -1;
    workerPage = -1;
    workerBusy = false;
    workerDone = false;
}

void workerMain(void *arg)
{
    int index = *(int *)arg;
    free(arg);

    Project *proj = &CatLoaderList::projects[index];
    std::vector<u8> png;

    u8 *out = nullptr;
    int outW = 0, outH = 0;

    if (!proj->imagePath.empty())
    {
        ApiResult res = http_download(proj->imagePath, png);
        if (res.success && !png.empty())
        {
            int w, h, ch;
            u8 *data = stbi_load_from_memory(png.data(), (int)png.size(), &w, &h, &ch, 4);
            if (data)
            {
                // resizeImageKeepAspect erwartet (data, w, h, targetH, targetW, outW, outH)
                out = resizeImageKeepAspect(data, w, h, 90, 120, outW, outH);
                stbi_image_free(data);
            }
        }
    }

    // Übergabe an main thread: setze globale Worker-Resultate
    workerRGBA = out;
    workerW = outW;
    workerH = outH;

    workerDone = true;
    workerBusy = false;
}