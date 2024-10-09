#include <imgui_vita2d/imgui_vita.h>
#include <psp2/power.h>
#include <algorithm>
#include <stdio.h>
#include "my_imgui.h"
#include "utils.h"
#include "log.h"
#include "icons.h"

#define TIME_X (VITA_WIDTH - 320)
#define BATTERY_X (VITA_WIDTH - 100)
#define BATTERY_PERCENT_X (BATTERY_X + 30)
#define TOP_RIGHT_Y 13

const char *BATTERY_ICONS[] = {ICON_BATTERY_25, ICON_BATTERY_50, ICON_BATTERY_75, ICON_BATTERY_100};

namespace Utils
{
    void ShowTimePower()
    {
        int percent = scePowerGetBatteryLifePercent();
        ImU32 color = percent <= 25 ? IM_COL32_RED : IM_COL32_GREEN;
        const char *battery;
        if (scePowerIsBatteryCharging())
        {
            battery = ICON_BATTERY_CHARGE;
        }
        else
        {
            battery = BATTERY_ICONS[(percent + 24) / 25 - 1];
        }

        char percent_str[64];
        snprintf(percent_str, 64, "%02d%%", scePowerGetBatteryLifePercent());

        SceDateTime time;
        sceRtcGetCurrentClock(&time, 0);
        SceDateTime time_local;
        SceRtcTick tick;
        sceRtcGetTick(&time, &tick);
        sceRtcConvertUtcToLocalTime(&tick, &tick);
        sceRtcSetTick(&time_local, &tick);

        char time_str[64];
        snprintf(time_str, 64, "%04d/%02d/%02d %02d:%02d:%02d",
                 time_local.year, time_local.month, time_local.day,
                 time_local.hour, time_local.minute, time_local.second);

        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        draw_list->PushClipRectFullScreen();
        draw_list->AddText({TIME_X, TOP_RIGHT_Y}, IM_COL32_WHITE, time_str);
        draw_list->AddText({BATTERY_X, TOP_RIGHT_Y}, IM_COL32_GREEN, battery);
        draw_list->AddText({BATTERY_PERCENT_X, TOP_RIGHT_Y}, color, percent_str);

        draw_list->PopClipRect();
    }

    void Lower(std::string *s)
    {
        std::transform(s->begin(), s->end(), s->begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
    }

    void StripQuotes(std::string *s)
    {
        if (!s->empty() && s->front() == '"')
        {
            s->erase(s->begin());
        }

        if (!s->empty() && s->back() == '"')
        {
            s->erase(s->end() - 1);
        }
    }

    int Utf16leToUtf8(uint16_t utf16_char, char *utf8_char)
    {
        if (utf16_char < 0x80)
        {
            utf8_char[0] = (char)utf16_char;
            return 1;
        }
        else if (utf16_char < 0x800)
        {
            utf8_char[0] = 0xC0 | (utf16_char >> 6);
            utf8_char[1] = 0x80 | (utf16_char & 0x3F);
            return 2;
        }
        else
        {
            utf8_char[0] = 0xE0 | (utf16_char >> 12);
            utf8_char[1] = 0x80 | ((utf16_char >> 6) & 0x3F);
            utf8_char[2] = 0x80 | (utf16_char & 0x3F);
            return 3;
        }
    }

    std::string Utf16leToUtf8(uint16_t *utf16le_str)
    {
        std::string utf8_str;
        char utf8_char[4]; // Buffer to hold UTF-8 characters (max 4 bytes)

        while (*utf16le_str)
        {
            int bytes = Utf16leToUtf8(*utf16le_str, utf8_char);
            utf8_str.append(utf8_char, bytes);
            utf16le_str++;
        }

        return utf8_str;
    }
};