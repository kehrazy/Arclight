#pragma once
#include "common.h"

class Window;


GUI_BEGIN

void initialize(::Window& window);

void prepareRender();
void render();

GUI_END