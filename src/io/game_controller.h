/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef IO_GAME_CONTROLLER_H
#define IO_GAME_CONTROLLER_H

#include "game/entity.h"
#include "game/world.h"
#include "io/controller.h"
#include "io/camera_controller.h"

namespace io {

class GameController : public Controller {
  public:
	GameController(const IRect &viewport);
	void handleInput(const GfxDevice &device, float time_diff) override;
	void tick(double time_diff) override;
	void drawView(Renderer &, GameRenderer &) const override;

	const Camera &camera() const { return m_camera_controller.camera(); }

  private:
	void drawAxes(Renderer &) const;

	IRect m_viewport;
	Segment m_cursor;
	game::EntityRef m_pointed_entity;
	CameraController m_camera_controller;
};
}

#endif
