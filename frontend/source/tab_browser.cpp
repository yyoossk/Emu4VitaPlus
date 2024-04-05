#include "global.h"
#include "tab_browser.h"
#include "log.h"

TabBrowser::TabBrowser(const char *path) : _index(0)
{
    LogFunctionName;
    _directory = new Directory(path, gEmulator->GetValidExtensions());
}

TabBrowser::~TabBrowser()
{
    LogFunctionName;
    delete _directory;
}

void TabBrowser::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&TabBrowser::_OnKeyUp, this), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&TabBrowser::_OnKeyDown, this), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&TabBrowser::_OnKeyCircle, this));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&TabBrowser::_OnKeyCross, this));
}

void TabBrowser::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void TabBrowser::Show()
{
}

void TabBrowser::_OnKeyUp()
{
    _index += _directory->GetSize() - 1;
    _index %= _directory->GetSize();
}

void TabBrowser::_OnKeyDown()
{
    _index++;
    _index %= _directory->GetSize();
}

void TabBrowser::_OnKeyCircle()
{
    auto item = _directory->GetItem(_index);

    if (item.isDir)
    {
        _directory->SetCurrentPath(_directory->GetCurrentPath() + "/" + item.name);
        _index = 0;
    }
    else
    {
        if (gEmulator->LoadGame((_directory->GetCurrentPath() + "/" + item.name).c_str()))
        {
            gStatus = APP_STATUS_RUN_GAME;
        }
    }
}

void TabBrowser::_OnKeyCross()
{
    auto path = _directory->GetCurrentPath();
    if (path.size() <= 5)
    {
        return;
    }

    size_t pos = path.rfind('/');
    if (pos != std::string::npos)
    {
        LogDebug(path.c_str());
        path = path.substr(0, pos);
        LogDebug(path.c_str());
        _directory->SetCurrentPath(path);
    }
}