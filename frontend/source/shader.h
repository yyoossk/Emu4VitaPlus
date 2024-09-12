#pragma once
#include <string>
#include <vector>
#include <vita2d.h>
#include <vita2d_ext.h>
#include <stdint.h>
#include "language_string.h"

struct Params
{
    const SceGxmProgramParameter *texture_size;
    const SceGxmProgramParameter *video_size;
    const SceGxmProgramParameter *output_size;
    bool Init(const SceGxmProgram *program);
};

class Shader
{
public:
    Shader(const char *name);
    virtual ~Shader();

    vita2d_shader *Get() { return _shader; };
    bool Valid() { return _shader != nullptr; };
    const char *GetName() const;
    void SetUniformData(const float *texture_size, const float *output_size);

private:
    void _Load();
    void _Clean();

    std::string _name;
    vita2d_shader *_shader;
    uint8_t *_vertex_buf;
    uint8_t *_fragment_buf;
    Params _vertex_parms;
    Params _fragment_parms;
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