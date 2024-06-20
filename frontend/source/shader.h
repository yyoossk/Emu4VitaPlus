#pragma once
#include <string>
#include <vector>
#include <vita2d.h>
#include <vita2d_ext.h>
#include <stdint.h>
#include "language_string.h"

class Shader
{
public:
    Shader(const char *name);
    virtual ~Shader();

    vita2d_shader *Get();
    const char *GetName() const;

private:
    std::string _name;
    vita2d_shader *_shader;
    uint8_t *_vertex_buf;
    uint8_t *_fragment_buf;
};

class Shaders : public std::vector<Shader>
{
public:
    Shaders();
    virtual ~Shaders();
    bool Load(const char *path);
    std::vector<LanguageString> GetConfigs();
};

extern Shaders *gShaders;