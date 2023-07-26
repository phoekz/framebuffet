#pragma once

#include <imgui.h>

namespace fb {

struct Window;
struct Dx;

struct Gui;
Gui* gui_create(Window* window, Dx* dx);
void gui_destroy(Gui* gui);
void gui_update(Gui* gui);
void gui_render(Gui* gui, Dx* dx);

}  // namespace fb
