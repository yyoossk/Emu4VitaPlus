#pragma once
#include "tab_selectable.h"
#include "directory.h"
#include "dialog.h"
#include "rom_name.h"

int32_t GetNameThread(uint32_t args, void *argc);

class TabBrowser : public TabSeletable
{
public:
    TabBrowser();
    virtual ~TabBrowser();
    virtual void SetInputHooks(Input *input) override;
    virtual void UnsetInputHooks(Input *input) override;
    void Show(bool selected);
    virtual void ChangeLanguage(uint32_t language) override;
    bool Visable() override { return _visable; };

    friend int32_t GetNameThread(uint32_t args, void *argc);

private:
    size_t _GetItemCount() override
    {
        return _directory->GetSize();
    };
    void _OnActive(Input *input) override;
    bool _ItemVisable(size_t index) override { return true; };
    void _OnKeyUp(Input *input) override;
    void _OnKeyDown(Input *input) override;
    void _OnKeyLeft(Input *input);
    void _OnKeyRight(Input *input);
    void _OnKeyCross(Input *input);
    void _OnKeyStart(Input *input);
    void _OnKeyTriangle(Input *input);
    void _OnKeySquare(Input *input);
    void _UpdateTexture();
    void _UpdateStatus();
    void _UpdateName();
    void _Update();
    void _Search(const char *s);
    const std::string _GetCurrentFullPath(bool *is_dir = nullptr);

    Directory *_directory;
    vita2d_texture *_texture;
    float _texture_width;
    float _texture_height;
    float _texture_max_width;
    float _texture_max_height;

    My_Imgui_SpinText _spin_text;
    bool _in_refreshing;
    InputTextDialog *_text_dialog;
    Input *_input;
    RomNameMap _name_map;
    const char *_name;
    TextMovingStatus _name_moving_status;
};