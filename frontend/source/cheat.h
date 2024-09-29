#pragma once
#include <string>
#include <vector>
#include <SimpleIni.h>

enum cheat_handler_type
{
    CHEAT_HANDLER_TYPE_EMU = 0,
    CHEAT_HANDLER_TYPE_RETRO,
    CHEAT_HANDLER_TYPE_END
};

struct Cheat
{
    bool enable;
    int cheat_type;
    std::string desc;
    std::string code;
    bool big_endian;
    int address;
    int value;
    int address_bit_position;
    int repeat_count;
    int repeat_add_to_address;
    int repeat_add_to_value;
    int memory_search_size;

    bool Load(CSimpleIniA *ini, size_t index);
};

class Cheats : public std::vector<Cheat>
{
public:
    Cheats();
    virtual ~Cheats();

    bool Load(const char *path);

private:
};
