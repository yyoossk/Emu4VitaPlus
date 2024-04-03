#pragma once
#include <stdint.h>
#include <unordered_map>

struct Config
{
    Config();
    virtual ~Config();

    std::unordered_map<uint32_t, uint32_t> key_maps;
};