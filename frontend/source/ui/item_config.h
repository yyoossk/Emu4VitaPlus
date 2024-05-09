#pragma once
#include <vector>
#include "language_string.h"
#include "item_base.h"

template <typename T>
class ItemConfig : public virtual ItemBase
{
public:
    ItemConfig(LanguageString text,
               LanguageString info,
               T *config,
               std::vector<LanguageString> texts,
               CallbackFunc active_callback = nullptr,
               CallbackFunc option_callback = nullptr);

    ItemConfig(LanguageString text,
               LanguageString info,
               T *config,
               TEXT_ENUM start,
               size_t count);

    virtual ~ItemConfig();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);
    virtual void OnActive(Input *input);
    T GetConfig() { return *_config; };
    void SetConfig(T value) { *_config = value; };
    void SetConfig(size_t value) { *_config = T(value); };

private:
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);

    T *_config;
    std::vector<LanguageString> _config_texts;
    T _old_config;

    bool _actived;
};

#include "item_config.i"