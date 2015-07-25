/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef IO_CONTROLLER_H
#define IO_CONTROLLER_H

#include "base.h"

class GameRenderer;

namespace io {

class Controller {
  public:
	virtual ~Controller() = default;
	virtual void handleInput(const GfxDevice &device, float time_diff) = 0;
	virtual void tick(double time_diff) = 0;
	virtual void drawView(Renderer &, GameRenderer &) const = 0;
};
}

#endif
