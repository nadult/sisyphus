/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef IO_GAME_CONTROLLER_H
#define IO_GAME_CONTROLLER_H

#include "game/entity.h"
#include "game/world.h"
#include "io/camera_controller.h"

namespace io {

class GameController {
  public:
	GameController(game::World *, const IRect &viewport);
	void handleInput(const GfxDevice &device, float time_diff);
	void tick(double time_diff);
	void drawView(Renderer &, GameRenderer &) const;

	const Camera &camera() const { return m_camera_controller.camera(); }

  private:
	void drawAxes(Renderer &) const;

	IRect m_viewport;
	game::World *m_world;
	Segment m_cursor;
	game::EntityRef m_pointed_entity;
	CameraController m_camera_controller;
};
}

#endif
