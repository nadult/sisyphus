/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "base.h"
#include "game_renderer.h"
#include "fwk_opengl.h"
#include "fwk_profile.h"
#include "game/world.h"
#include "io/game_controller.h"
#include <algorithm>
#include <random>

using namespace game;

class Game {
  public:
	Game(const IRect &viewport)
		: m_viewport(viewport) {
		m_controller = make_unique<io::GameController>(&m_world, viewport);
	}

	void handleInput(GfxDevice &device, float time_diff) {
		m_controller->handleInput(device, time_diff);
	}

	void tick(float time_diff) {
		FWK_PROFILE("Game::tick");
		m_controller->tick(time_diff);
		m_world.simulate(time_diff);

		m_frame_logs.clear();
		for(auto &log : getLogs()) {
			if(log.second == LogType::single)
				m_logs.push_back(log.first);
			else if(log.second == LogType::per_frame)
				m_frame_logs.push_back(log.first);
		}
		while(m_logs.size() > 6)
			m_logs.erase(m_logs.begin());
	}

	void draw(Renderer &out3d, GameRenderer &out2d) const {
		FWK_PROFILE("Game::draw");
		m_controller->drawView(out3d, out2d);

		TextFormatter log;
		for(auto text : m_frame_logs)
			log("%s\n", text.c_str());
		for(auto text : m_logs)
			log("%s\n", text.c_str());
		auto &font = out2d.font();

		m_max_extents = sum(m_max_extents, FRect(font.font().evalExtents(log)));
		m_max_extents = sum(m_max_extents, FRect({100.0f, 150.0f}));
		FRect extents = m_max_extents;
		float2 text_pos = float2(m_viewport.max) - extents.size();
		out2d.addFilledRect(extents + text_pos, Color(0, 0, 0, 120));
		font.draw(extents + text_pos, {Color::yellow, Color::black, HAlign::left}, log);
	}

	const IRect &viewport() const { return m_viewport; }

  private:
	IRect m_viewport;
	mutable FRect m_max_extents;
	unique_ptr<io::Controller> m_controller;
	vector<string> m_frame_logs;
	vector<string> m_logs;
	game::World m_world;
};

static Game *s_game = nullptr;

bool main_loop(GfxDevice &device) {
	DASSERT(s_game);

	Color nice_background(200, 200, 255);
	GfxDevice::clearColor(nice_background);
	GfxDevice::clearDepth(1.0f);

	float time_diff = 1.0f / 60.0f;
	s_game->handleInput(device, time_diff);
	s_game->tick(time_diff);

	Renderer renderer_3d;
	GameRenderer renderer_2d(s_game->viewport());
	s_game->draw(renderer_3d, renderer_2d);

	/*
	for(const auto &event : device.inputEvents())
			if(event.keyDown(InputKey::tab)) {
				if(m_editor_mode) {
				}

				m_editor_mode ^= 1;
			}*/

	renderer_3d.render();
	renderer_2d.render();

	logText(getProfilerStats(), LogType::per_frame);
	profilerNextFrame();

	return true;
}

int safe_main(int argc, char **argv) {
	double time = getTime();
	int2 resolution(1200, 700);
	bool full = false;

	for(int n = 1; n < argc; n++) {
		if(string("--res") == argv[n]) {
			resolution.x = atoi(argv[n + 1]);
			resolution.y = atoi(argv[n + 2]);
			ASSERT(n + 3 <= argc);
			n += 2;
		}
		else if(string("--fullscreen") == argv[n])
			full = true;
	}
	
	auto &gfx_device = GfxDevice::instance();
	gfx_device.createWindow("Sisyphus", resolution, full);

	Game game((IRect(resolution)));
	s_game = &game;

	gfx_device.runMainLoop(main_loop);

	return 0;
}

int main(int argc, char **argv) {
	try {
		return safe_main(argc, argv);
	} catch(const Exception &ex) {
		printf("%s\n\nBacktrace:\n%s\n", ex.what(), cppFilterBacktrace(ex.backtrace()).c_str());
		return 1;
	}
}
