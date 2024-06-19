#include "shader.h"

Shader::Shader(const char *name)
    : _name(name),
      _shader(nullptr)
{
}

Shader::~Shader()
{
    if (_shader)
    {
        vita2d_free_shader(_shader);
    }
}

vita2d_shader *Shader::Get()
{
    return _shader;
}