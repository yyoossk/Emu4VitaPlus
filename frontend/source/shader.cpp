#include <stdlib.h>
#include "shader.h"
#include "file.h"
#include "log.h"
#include "SimpleIni.h"

#define SHADER_PATH "app0:assets/shaders/"
#define SHADER_SECTION "SHADERS"

Shaders *gShaders;

Shader::Shader(const char *name)
    : _name(name),
      _shader(nullptr),
      _vertex_buf(nullptr),
      _fragment_buf(nullptr)
{
}

Shader::~Shader()
{
    LogFunctionName;
    if (_shader)
    {
        vita2d_free_shader(_shader);
    }

    if (_vertex_buf)
    {
        delete[] _vertex_buf;
    }

    if (_fragment_buf)
    {
        delete[] _fragment_buf;
    }
}

vita2d_shader *Shader::Get()
{
    if (!_shader)
    {
        LogDebug("  try to load shader: %s", _name.c_str());
        std::string name = std::string(SHADER_PATH) + _name + "_v.gxp";
        size_t size = File::GetSize(name.c_str());
        if (size == 0)
            goto FAILED;

        _vertex_buf = new uint8_t[size];
        if (!File::ReadFile(name.c_str(), _vertex_buf, size))
            goto FAILED;

        name = std::string(SHADER_PATH) + _name + "_f.gxp";
        size = File::GetSize(name.c_str());
        if (size == 0)
            goto FAILED;

        _fragment_buf = new uint8_t[size];
        if (!File::ReadFile(name.c_str(), _fragment_buf, size))
            goto FAILED;

        _shader = vita2d_create_shader((const SceGxmProgram *)_vertex_buf, (const SceGxmProgram *)_fragment_buf);
        if (_shader)
        {
            LogDebug("  shader %s loaded: %08x", _name.c_str(), _shader);
        }
        else
        {
        FAILED:
            LogError("  failed to load shader: %s", _name.c_str());
            if (_vertex_buf)
            {
                delete[] _vertex_buf;
                _vertex_buf = nullptr;
            }

            if (_fragment_buf)
            {
                delete[] _fragment_buf;
                _fragment_buf = nullptr;
            }
        }
    }
    return _shader;
}

const char *Shader::GetName() const
{
    return _name.c_str();
}

Shaders::Shaders()
{
    Load(SHADER_PATH "shaders.ini");
}

Shaders::~Shaders()
{
}

bool Shaders::Load(const char *path)
{
    LogFunctionName;

    CSimpleIniA ini;
    if (ini.LoadFile(path) != SI_OK)
    {
        LogError("failed to load %s", path);
        return false;
    }

    size_t count = 0;
    char key[8];
    while (true)
    {
        snprintf(key, 8, "%03u", count++);
        const char *value = ini.GetValue(SHADER_SECTION, key, "NULL");
        if (strcmp(value, "NULL") == 0)
        {
            break;
        }
        else
        {
            this->emplace_back(Shader(value));
        }
    }

    return true;
}

std::vector<LanguageString> Shaders::GetConfigs()
{
    std::vector<LanguageString> configs;
    configs.emplace_back(LanguageString(NONE));
    for (auto const &shader : *this)
    {
        configs.emplace_back(shader.GetName());
    }
    return configs;
}