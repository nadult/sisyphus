/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "io/game_controller.h"
#include "game_renderer.h"
#include "game/world.h"
#include "game/human_control.h"
#include <algorithm>

using namespace game;

namespace io {

GameController::GameController(game::World *world, const IRect &viewport)
	: m_viewport(viewport), m_world(world), m_camera_controller(world, viewport) {
}

void GameController::handleInput(const GfxDevice &device, float time_diff) {
	// TODO: events should be filtered
	m_camera_controller.handleInput(device, time_diff);

	float rot = 0.0f;
	float3 move;
	bool run = false;

	for(const auto &event : device.inputEvents()) {
		if(event.keyPressed(InputKey::left))
			rot += 1.0f;
		if(event.keyPressed(InputKey::right))
			rot -= 1.0f;
		if(event.keyPressed(InputKey::up))
			move += float3(0, 0, 1);
		if(event.keyPressed(InputKey::down))
			move -= float3(0, 0, 1);
		if(event.keyPressed(InputKey::lshift))
			run = true;
	}

	m_world->humanControl()->move(move, rot, run);
}

void GameController::tick(double time_diff) { m_camera_controller.tick(time_diff); }

void GameController::drawAxes(Renderer &out) const {
	auto arrow_mesh = res::models()["helpers/axis_arrow"];

	auto trans = AffineTrans(camera().viewMatrix());

	Matrix4 x_rot = Matrix4(rotationBetween(float3(0, 1, 0), float3(1, 0, 0)));
	Matrix4 y_rot = Matrix4::identity();
	Matrix4 z_rot = Matrix4(rotationBetween(float3(0, 1, 0), float3(0, 0, 1)));

	out.pushViewMatrix();
	out.setViewMatrix(translation(1.6, -0.9, -2) * Matrix4(trans.rotation));
	arrow_mesh->draw(out, make_immutable<Material>(Color::red), x_rot * scaling(0.03f));
	arrow_mesh->draw(out, make_immutable<Material>(Color::green), y_rot * scaling(0.03f));
	arrow_mesh->draw(out, make_immutable<Material>(Color::blue), z_rot * scaling(0.03f));
	out.popViewMatrix();
}

void GameController::drawView(Renderer &out, GameRenderer &out2d) const {
	out.setProjectionMatrix(camera().projectionMatrix());
	out.setViewMatrix(camera().viewMatrix());
	m_world->draw(out);
	drawAxes(out);

	TextFormatter main_text, log;

	auto &font = out2d.font();

	/*	if(m_cursor)
			font.draw(FRect(0, 20, 300, 40), {Color::white, Color::black},
					  format("Cursor: (%.2f %.2f %.2f)", m_cursor->x, m_cursor->y, m_cursor->z));*/
}
}
