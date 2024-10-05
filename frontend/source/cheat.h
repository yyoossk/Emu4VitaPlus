#pragma once
#include <string>
#include <vector>
#include <stdint.h>
#include <SimpleIni.h>
#include "thread_base.h"
#include "delay.h"

enum cheat_handler_type
{
    CHEAT_HANDLER_TYPE_EMU = 0,
    CHEAT_HANDLER_TYPE_RETRO,
    CHEAT_HANDLER_TYPE_END
};

struct Cheat
{
    uint32_t enable;
    uint32_t cheat_type;
    uint32_t handler;
    std::string desc;
    std::string code;
    uint32_t big_endian;
    uint32_t address;
    uint32_t value;
    uint32_t address_bit_position;
    uint32_t repeat_count;
    uint32_t repeat_add_to_address;
    uint32_t repeat_add_to_value;
    uint32_t memory_search_size;

    bool Load(CSimpleIniA *ini, size_t index);
    void Apply(int index) const;
};

class Cheats : public std::vector<Cheat>, public ThreadBase
{
public:
    Cheats();
    virtual ~Cheats();

    bool Load(const char *path);

private:
    static int _CheatThread(SceSize args, void *argp);

    Delay _delay;
};
