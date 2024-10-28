#include "SimpleIni.h"
#include "config.h"
#include "file.h"
#include "log.h"

#define MAIN_SECTION "MAIN"

Config *gConfig = nullptr;

Config::Config()
{
    Load();
}

Config::~Config()
{
    Save();
}

bool Config::Load(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);
    CSimpleIniA ini;
    if (ini.LoadFile(path) != SI_OK)
    {
        return false;
    }

    const char *tmp = ini.GetValue(MAIN_SECTION, "last_core");
    if (tmp)
    {
        last_core = tmp;
    }

    return true;
}

bool Config::Save(const char *path)
{
    LogFunctionName;
    LogDebug("path: %s", path);

    CSimpleIniA ini;
    ini.SetValue(MAIN_SECTION, "last_core", last_core.c_str());
    File::Remove(path);
    return ini.SaveFile(path, false) == SI_OK;
}