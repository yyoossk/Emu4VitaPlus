#include <SimpleIni.h>
#include "favourite.h"
#include "log.h"

bool Favourites::Load(const char *path)
{
    LogFunctionName;
    return true;
}

bool Favourites::Save(const char *path)
{
    LogFunctionName;
    CSimpleIniA ini;
    return ini.SaveFile(path) == SI_OK;
}