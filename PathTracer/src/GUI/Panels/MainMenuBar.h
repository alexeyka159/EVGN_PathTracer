#pragma once

#include "GUIPanel.h"

class MainMenuBar : public GUIPanel
{
private:

public:
    MainMenuBar() = default;
    inline ~MainMenuBar() { delete this; };

    void Draw() override;
};

