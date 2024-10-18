#include <string>
#include <string.h>
#include "misc.h"
#include "defines.h"
#include "file.h"
#include "state_manager.h"
#include "log.h"

vita2d_texture *GetRomPreviewImage(const char *path, const char *name)
{
    LogFunctionName;
    LogDebug("%s %s", path, name);
    vita2d_texture *texture = nullptr;

    std::string stem = File::GetStem(name);
    std::string img_path = std::string(path) + "/" PREVIEW_DIR_NAME "/" + stem;

    texture = vita2d_load_PNG_file((img_path + ".png").c_str());
    if (texture)
        goto END;

    texture = vita2d_load_JPEG_file((img_path + ".jpg").c_str());
    if (texture)
        goto END;

    img_path = std::string(CORE_SAVESTATES_DIR) + "/" + stem + "/";
    time_t newest;
    for (int i = 0; i < MAX_STATES; i++)
    {
        char tmp[8];
        if (i == 0)
        {
            strcpy(tmp, "auto");
        }
        else
        {
            snprintf(tmp, 8, "%02d", i);
        }
        std::string jpg_path = img_path + "state_" + tmp + ".jpg";
        if (File::Exist(jpg_path.c_str()))
        {
            if (texture == nullptr)
            {
                texture = vita2d_load_JPEG_file(jpg_path.c_str());
                File::GetCreateTime(jpg_path.c_str(), &newest);
            }
            else
            {
                time_t time;
                File::GetCreateTime(jpg_path.c_str(), &time);
                if (time > newest)
                {
                    vita2d_free_texture(texture);
                    texture = vita2d_load_JPEG_file(jpg_path.c_str());
                    newest = time;
                }
            }
        }
    }

END:
    return texture;
}

void CalcFitSize(float width, float height, float max_width, float max_height, float *out_width, float *out_height)
{
    *out_height = height * max_width / width;
    if (*out_height > max_height)
    {
        *out_height = max_height;
        *out_width = width * max_height / height;
    }
    else
    {
        *out_width = max_width;
    }
}