#include "CatLoader/download.hpp"
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

u32* SOC_buffer = NULL;

std::string getAPIExplore(bool Scratch, std::string &studioID, int &page) {
    if(Scratch) return "https://api.scratch.mit.edu/studios/" + studioID + "/projects?limit=6&offset=" + std::to_string(page * 6);
    return "https://scratchbox.grady.link/api/projects?sort=likes&ps=6&p=" + std::to_string(page + 1);
}

ApiResult ensure_dir(const std::string& path) {
    ApiResult result;
    result.success = true;
    if (mkdir(path.c_str(), 0777) != 0) {
        if (errno != EEXIST) {
            result.success = false;
            result.message = "Warning: failed to create directory: \n" + path;
            result.tip = "Check SD card and restart 3DS";
        }
    }
    return result;
}

// --- Save data to SD card ---
ApiResult save_file_sd(const std::string& path, const u8* data, size_t size) {
    ApiResult result;
    result.success = false;
    

    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos) ensure_dir(path.substr(0, lastSlash));

    FILE* f = fopen(path.c_str(), "wb");
    if (!f) {
        result.success = false;
        result.message = "Failed to open file for writing: " + path;
        result.tip = "Check SD card and restart 3DS";
        return result;
    }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    if (written != size) {
        result.success = false;
        result.message = "Incomplete write to file: " + path;
        result.tip = "Check SD card and restart 3DS";
        return result;
    }

    result.success = true;
    return result;
}

// callback: schreibt Daten in std::string
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* out = (std::string*)userp;
    out->append((char*)contents, totalSize);
    return totalSize;
}

ApiResult initHttp()
{
    ApiResult result;
    result.success = true;
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if (!SOC_buffer) {
        result.success = false;
        result.message = "SOC_buffer alloc failed!";
        result.tip = "Restart 3ds";
        return result;
    }

    if (socInit(SOC_buffer, SOC_BUFFERSIZE) != 0) {
        result.message = "socInit failed!";
        result.tip = "Restart 3ds";
        result.success = false;
        return result;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    return result;
}

void exitHttp() {
    curl_global_cleanup();
    socExit();
    if(SOC_buffer) {
        free(SOC_buffer);
        SOC_buffer = NULL;
    }
}

// --- Download HTTP resource into vector ---
ApiResult http_download(const std::string &urlin, std::vector<u8> &out) {
    ApiResult result;
    result.success = false;
    result.tip = "Check your internet connection and DNS settings";
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        result.message = "Failed to initialize CURL";
        return result;
    }

    std::string response;
    const char* url = urlin.c_str();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);  // 30 second timeout
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);  // 10 second connect timeout
    curl_easy_setopt(curl, CURLOPT_CAINFO, "romfs:/certificate/certs.pem");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        result.message = std::string("Network error: ") + curl_easy_strerror(res);
        
        switch(res) {
            case CURLE_COULDNT_RESOLVE_HOST:
                result.tip = "Check internet connection and DNS settings on your 3DS";
                break;
            case CURLE_OPERATION_TIMEDOUT:
                result.tip = "Connection timed out. Try again or check internet speed";
                break;
            case CURLE_SSL_CONNECT_ERROR:
                result.tip = "SSL/TLS connection failed. Update certificates";
                break;
            default:
                result.tip = "Check internet connection and restart 3DS";
                break;
        }
        result.success = false;
    } else {
        result.success = true;
        out.assign(response.begin(), response.end());
    }

    curl_easy_cleanup(curl);
    return result;
}
ApiResult download_ScratchBox_to_folder(std::string& project_id, std::string folder_base, const std::string& project_filename) {
    ApiResult result;
    result.success = false;
    result.tip = "Check SD card and restart 3DS";
    // Create base folder
    ensure_dir(folder_base);
    std::string url = "https://scratchbox.grady.link/api/project/" + project_id + "/download";
    std::vector<u8> project_buf;
    result = http_download(url, project_buf);
    if (!result.success) return result;
    std::string path = folder_base + "/" + project_filename + ".sb3";
    result = save_file_sd(path, project_buf.data(), project_buf.size());
    if (!result.success) return result;
    result.success = true;
    result.message = "Project downloaded successfully: " + project_id;
    return result;
}

// --- Download a Scratch project to a folder with optional filename for project.json ---
ApiResult download_project_to_folder(std::string& project_id, std::string folder_base, std::string& project_filename) {
    ApiResult result;
    result.tip = "Check SD card and restart 3DS";
    // Create base folder
    ensure_dir(folder_base);

    // 1) Load project meta (to get token)
    std::string meta_url = "https://api.scratch.mit.edu/projects/" + project_id;
    std::vector<u8> meta_buf;
    result = http_download(meta_url, meta_buf);
    if (!result.success) return result;

    json meta = json::parse(meta_buf.begin(), meta_buf.end(), nullptr, false);
    if (meta.is_discarded()) {
        result.success = false;
        result.message = "Failed to parse meta JSON";
        result.tip = "Download update and/or report this bug";
        return result;
    }

    std::string token = meta.value("project_token", "");
    if (token.empty()) {
        result.success = false;
        result.message = "Project token missing (private or non-existent project)";
        result.tip = "use another project";
        return result;
    }

    // 2) Download project.json
    std::string project_url = "https://projects.scratch.mit.edu/" + project_id + "?token=" + token;
    std::vector<u8> proj_buf;
    result = http_download(project_url, proj_buf);
    if (!result.success) return result;

    json project = json::parse(proj_buf.begin(), proj_buf.end(), nullptr, false);
    if (project.is_discarded()) {
        result.success = false;
        result.message = "Failed to parse project JSON";
        return result;
    }

    // 3) Create project folder
    std::string folder = folder_base + "/" + project_filename;
    ensure_dir(folder);

    // 4) Save project.json
    std::string project_json_path = folder + "/" + "project.json";
    result = save_file_sd(project_json_path, proj_buf.data(), proj_buf.size());
    if (!result.success) return result;

    // 5) Collect assets
    std::set<std::string> assets;
    if (project.contains("targets")) {
        for (auto &target : project["targets"]) {
            if (target.contains("costumes")) {
                for (auto &costume : target["costumes"]) {
                    std::string md5ext = costume.value("md5ext", "");
                    if (md5ext.empty() && costume.contains("assetId") && costume.contains("dataFormat"))
                        md5ext = costume["assetId"].get<std::string>() + "." + costume["dataFormat"].get<std::string>();
                    if (!md5ext.empty()) assets.insert(md5ext);
                }
            }
            if (target.contains("sounds")) {
                for (auto &sound : target["sounds"]) {
                    std::string md5ext = sound.value("md5ext", "");
                    if (md5ext.empty() && sound.contains("assetId") && sound.contains("dataFormat"))
                        md5ext = sound["assetId"].get<std::string>() + "." + sound["dataFormat"].get<std::string>();
                    if (!md5ext.empty()) assets.insert(md5ext);
                }
            }
        }
    }

    // 6) Download and save assets
    for (const auto &md5ext : assets) {
        std::string asset_url = "https://assets.scratch.mit.edu/internalapi/asset/" + md5ext + "/get/";
        std::vector<u8> asset_buf;
        result = http_download(asset_url, asset_buf);
        if (!result.success) {
            result.tip = "Failed to download asset: " + md5ext;
            return result;
        }
        std::string asset_path = folder + "/" + md5ext;
        result = save_file_sd(asset_path, asset_buf.data(), asset_buf.size());
        if (!result.success) {
            result.tip = "Failed to save asset: " + md5ext;
            return result;
        }
    }

    result.success = true;
    result.message = "Project downloaded successfully: " + project_id + " (Assets: " + std::to_string(assets.size()) + ")";
    return result;
}

ApiResult http_get_json(const std::string &url, std::string &out) {
    out.clear();
    httpcContext context;
    u32 statuscode = 0, readsize = 0, size = 0;
    u8 *buf = (u8 *)std::malloc(0x1000);
    if (!buf) {
        ApiResult r;
        r.success = false;
        r.message = "Failed to allocate buffer";
        r.tip = "Check memory availability";
        return r;
    }

    if (httpcOpenContext(&context, HTTPC_METHOD_GET, url.c_str(), 1)) {
        std::free(buf);
        ApiResult r;
        r.success = false;
        r.message = "Failed to open HTTP context";
        return r;
    }

    httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
    httpcAddRequestHeaderField(&context, "User-Agent", "3ds-scratch-client");

    if (httpcBeginRequest(&context)) {
        httpcCloseContext(&context);
        std::free(buf);
        ApiResult r;
        r.success = false;
        r.message = "Failed to begin HTTP request";
        return r;
    }

    Result rett = httpcGetResponseStatusCode(&context, &statuscode);
    if (R_FAILED(rett)) {
        char tmp[64];
        httpcCloseContext(&context);
        std::free(buf);
        ApiResult r;
        r.success = false;
        r.message = tmp;
        r.tip = "Check internet connection / TLS support";
        return r;
    }
    if (statuscode != 200) {
        httpcCloseContext(&context);
        std::free(buf);
        ApiResult r;
        r.success = false;
        r.message = "HTTP error " + std::to_string(statuscode);
        r.tip = "Expected 200 OK";
        return r;
    }

    int ret;
    do {
        ret = httpcDownloadData(&context, buf + size, 0x1000, &readsize);
        size += readsize;

        if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING) {
            u8 *newbuf = (u8 *)std::realloc(buf, size + 0x1000);
            if (!newbuf) {
                httpcCloseContext(&context);
                std::free(buf);
                ApiResult r;
                r.success = false;
                r.message = "Failed to realloc buffer";
                r.tip = "Check memory usage";
                return r;
            }
            buf = newbuf;
        }

    } while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

    out.assign(reinterpret_cast<char*>(buf), size);
    std::free(buf);
    httpcCloseContext(&context);

    ApiResult r;
    r.success = true;
    return r;
}

// Fetches projects from a given URL
ApiResult getProjects(const bool isScratch, const std::string &url, std::array<Project, 6> &projects) {
    ApiResult r;
    r.success = true;
    std::vector<u8> meta_buf;
    ApiResult httpResult = http_download(url, meta_buf);
    std::string response(meta_buf.begin(), meta_buf.end());
    if (!httpResult.success) return httpResult;

    auto data = nlohmann::json::parse(response, nullptr, false);
    if (data.is_discarded() || !data.is_array()) {
        r.success = false;
        r.message = "Failed to parse JSON";
        r.tip = "Check API response format";
        return r;
    }

    for (int i = 0; i < 6; i++) {
        Project pr;
        if (i >= data.size()) {
            pr.id = "0";
            projects[i] = pr;
            break;
        }
        nlohmann::basic_json<>::value_type p = data[i];
        if(isScratch) {
            log("in Scratch");
            pr.id = std::to_string(p.value("id", 0));
            pr.title = p.value("title", "Not Found");
            pr.username = p.value("username", "Unknown");
            pr.imagePath = p.value("image", "");
            if (p.contains("avatar") && p["avatar"].is_object()) {
                pr.iconPath = p["avatar"].value("50x50", "");
            } else {
                pr.iconPath = "";
                r.success = false;
                r.message = "Failed to get ProfilImage";
                r.tip = "Check API response format";
            }
            pr.loaded = false;
            projects[i] = pr;
        } 
        else {
            log("Data");
            pr.id = p.value("id", "0");
            log(pr.id.c_str());
            pr.title = p.value("name", "Not Found");
            log(pr.title.c_str());
            pr.description = p.value("description", "Unknown");
            pr.imagePath = "http://scratchbox.grady.link/api/project/" + pr.id + "/thumbnail";
            log(pr.imagePath.c_str());
            pr.iconPath = "";
            pr.loaded = false;
            projects[i] = pr;
        }
        
    }

    return r;
}


u32 next_pow2(u32 n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

// Hilfsfunktion: RGBA nach ABGR swappen (3DS benötigt ABGR)
u32 rgba_to_abgr(u32 px)
{
    u8 r = (px & 0xff000000) >> 24;
    u8 g = (px & 0x00ff0000) >> 16;
    u8 b = (px & 0x0000ff00) >> 8;
    u8 a = px & 0x000000ff;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

C2D_Image createC2DImageFromRGBA(u8* rgba, int width, int height) {

   int final_width = width;
    int final_height = height;

    // Neues Bild mit Mindestgröße und Transparenz anlegen, falls nötig
    unsigned char* final_data = nullptr;
    if (final_width != width || final_height != height) {
        final_data = (unsigned char*)calloc(final_width * final_height * 4, 1); // alles transparent
        if (!final_data) return {nullptr, nullptr};
        int x_offset = (final_width - width) / 2;
        int y_offset = (final_height - height) / 2;
        for (int y = 0; y < height; ++y) {
            memcpy(
                final_data + ((y + y_offset) * final_width + x_offset) * 4,
                rgba + (y * width) * 4,
                width * 4
            );
        }
        rgba = final_data;
        width = final_width;
        height = final_height;
    }

    // Texturgröße auf nächste Potenz von 2 runden
    u32 texWidth = next_pow2(width);
    u32 texHeight = next_pow2(height);

    // C3D_Tex anlegen
    C3D_Tex* tex = new C3D_Tex();
    if (!C3D_TexInit(tex, texWidth, texHeight, GPU_RGBA8)) {
        delete tex;
        if (final_data) free(final_data);
        return {nullptr, nullptr};
    }
    C3D_TexSetFilter(tex, GPU_LINEAR, GPU_LINEAR);

    // Textur mit 0 initialisieren
    memset(tex->data, 0, texWidth * texHeight * 4);

    // RGBA → ABGR konvertieren und ins Tiled-Format schreiben
    const u32* src = (const u32*)rgba;
    for (u32 j = 0; j < (u32)height; j++) {
        for (u32 i = 0; i < (u32)width; i++) {
            u32 src_idx = j * width + i;
            u32 abgr_px = rgba_to_abgr(src[src_idx]);

            // 3DS-Swizzle (Tiled Layout)
            u32 dst_ptr_offset = (
                (((j >> 3) * (texWidth >> 3) + (i >> 3)) << 6) +
                ((i & 1) |
                ((j & 1) << 1) |
                ((i & 2) << 1) |
                ((j & 2) << 2) |
                ((i & 4) << 2) |
                ((j & 4) << 3))
            );
            ((u32*)tex->data)[dst_ptr_offset] = abgr_px;
        }
    }

    if (final_data) free(final_data);

    // Subtexture anlegen
    Tex3DS_SubTexture* subtex = new Tex3DS_SubTexture();
    subtex->width = width;
    subtex->height = height;
    subtex->left = 0.0f;
    subtex->top = 1.0f;
    subtex->right = (float)width / (float)texWidth;
    subtex->bottom = 1.0f - ((float)height / (float)texHeight);

    // Ergebnis zurückgeben
    C2D_Image img;
    img.tex = tex;
    img.subtex = subtex;
    return img;
}


// Optional: Freigabe-Funktion
void freeC2DImage(C2D_Image img)
{
    if (img.tex)
    {
        C3D_TexDelete(img.tex);
        delete img.tex;
    }
    if (img.subtex)
        delete img.subtex;
}

unsigned char* resizeImage(const unsigned char* src, int width, int height, int new_width, int new_height) {
    unsigned char* dst = (unsigned char*)malloc(new_width * new_height * 4);
    if (!dst) return nullptr;

    for (int j = 0; j < new_height; j++) {
        for (int i = 0; i < new_width; i++) {
            // Nearest-neighbor Skalierung
            int src_x = i * width / new_width;
            int src_y = j * height / new_height;
            for (int c = 0; c < 4; c++) {
                dst[(j * new_width + i) * 4 + c] = src[(src_y * width + src_x) * 4 + c];
            }
        }
    }
    return dst;
}

// Optional: proportional skalieren auf bestimmte Höhe, max Breite
unsigned char* resizeImageKeepAspect(const unsigned char* src, int width, int height, int target_height, int max_width, int& out_width, int& out_height) {
    int new_height = target_height;
    int new_width = width * target_height / height;
    if (new_width > max_width) new_width = max_width;

    out_width = new_width;
    out_height = new_height;

    return resizeImage(src, width, height, new_width, new_height);
}