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