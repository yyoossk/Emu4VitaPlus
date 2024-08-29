#pragma once
#include <vector>
#include <stdint.h>
#include "language_string.h"
#include "item_base.h"

class ItemConfig : public virtual ItemBase
{
public:
    ItemConfig(LanguageString text,
               LanguageString info,
               uint32_t *config,
               std::vector<LanguageString> config_texts,
               CallbackFunc active_callback = nullptr,
               CallbackFunc option_callback = nullptr);

    ItemConfig(LanguageString text,
               LanguageString info,
               uint32_t *config,
               TEXT_ENUM start,
               size_t count,
               CallbackFunc active_callback = nullptr,
               CallbackFunc option_callback = nullptr);

    virtual ~ItemConfig();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);
    virtual void OnActive(Input *input);
    virtual uint32_t GetConfig() const { return *_config; };
    virtual void SetConfig(uint32_t value) { *_config = value; };

protected:
    std::vector<LanguageString> _config_texts;
    uint32_t *_config;

private:
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);

    uint32_t _old_config;
    bool _actived;
};

class ItemIntConfig : public ItemConfig
{
public:
    ItemIntConfig(LanguageString text,
                  LanguageString info,
                  uint32_t *value,
                  size_t start,
                  size_t end,
                  size_t step = 1,
                  CallbackFunc active_callback = nullptr,
                  CallbackFunc option_callback = nullptr);

    virtual void SetConfig(uint32_t value);

private:
    size_t _step;
    uint32_t *_value;
    uint32_t _index;
};