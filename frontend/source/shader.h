#pragma once
#include <string>
#include <vita2d.h>
#include <vita2d_ext.h>

class Shader
{
public:
    Shader(const char *name);
    virtual ~Shader();

    vita2d_shader *Get();

private:
    std::string _name;
    vita2d_shader *_shader;
};