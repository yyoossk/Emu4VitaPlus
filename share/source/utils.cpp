#include <imgui_vita2d/imgui_vita.h>
#include <psp2/power.h>
#include <algorithm>
#include <stdio.h>
#include "my_imgui.h"
#include "utils.h"
#include "log.h"

namespace Utils
{
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

    void TrimString(std::string *s)
    {
        s->erase(s->begin(), std::find_if(s->begin(), s->end(), [](unsigned char ch)
                                          { return !std::isspace(ch); }));
        s->erase(std::find_if(s->rbegin(), s->rend(), [](unsigned char ch)
                              { return !std::isspace(ch); })
                     .base(),
                 s->end());
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

    std::string Utf16leToUtf8(uint16_t *utf16_str)
    {
        std::string utf8_str;
        char utf8_char[4]; // Buffer to hold UTF-8 characters (max 4 bytes)

        while (*utf16_str)
        {
            int bytes = Utf16leToUtf8(*utf16_str, utf8_char);
            utf8_str.append(utf8_char, bytes);
            utf16_str++;
        }

        return utf8_str;
    }

    int Utf16leToUtf8(uint16_t *utf16_str, char *utf8_str, size_t utf8_size)
    {
        char *p = utf8_str;
        char utf8_char[4];

        while (*utf16_str && p - utf8_str < utf8_size)
        {
            char utf8_char[4];
            int bytes = Utf16leToUtf8(*utf16_str++, utf8_char);
            memcpy(p, utf8_char, bytes);
            p += bytes;
        }
        *p = 0;
        return p - utf8_str;
    }
};
