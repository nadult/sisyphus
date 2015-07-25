/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "io/game_controller.h"
#include "game_renderer.h"
#include "game/world.h"
#include <algorithm>

using namespace game;

namespace io {

GameController::GameController(const IRect &viewport)
	: m_viewport(viewport), m_camera_controller(World::instance(), viewport) {
		float3 source_pos(-50, 30, -50);
		float3 target_pos(0, 20, 0);
	m_camera_controller.setCamera(source_pos, target_pos);
}

void GameController::handleInput(const GfxDevice &device, float time_diff) {
	// TODO: events should be filtered
	m_camera_controller.handleInput(device, time_diff);
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
	arrow_mesh->draw(out, make_cow<Material>(Color::red), x_rot * scaling(0.03f));
	arrow_mesh->draw(out, make_cow<Material>(Color::green), y_rot * scaling(0.03f));
	arrow_mesh->draw(out, make_cow<Material>(Color::blue), z_rot * scaling(0.03f));
	out.popViewMatrix();
}

void GameController::drawView(Renderer &out, GameRenderer &out2d) const {
	out.setProjectionMatrix(camera().projectionMatrix());
	out.setViewMatrix(camera().viewMatrix());
	World::instance()->draw(out);
	drawAxes(out);

	TextFormatter main_text, log;

	auto &font = out2d.font();

/*	if(m_cursor)
		font.draw(FRect(0, 20, 300, 40), {Color::white, Color::black},
				  format("Cursor: (%.2f %.2f %.2f)", m_cursor->x, m_cursor->y, m_cursor->z));*/
}

}