#include "game/world.h"

namespace game {

	auto makeMats(PModel model) {
		Shader vertex_shader(Shader::tVertex), fragment_shader(Shader::tFragment);
		Loader("data/mesh_shader.vsh") >> vertex_shader;
		Loader("data/mesh_shader.fsh") >> fragment_shader;
		auto program = make_cow<Program>(vertex_shader, fragment_shader);
		std::map<string, PMaterial> mats;
		for(auto mat : model->materialDefs())
			mats.emplace(mat.name, make_cow<Material>(program, mat.diffuse));
		return make_unique<MaterialSet>(make_cow<Material>(), mats);
	}

	World::World() :m_time_diff(0) {
		m_level = res::models()["level1"];
		m_human = make_unique<ModelEntity>(ModelEntityDesc{"human", "human", float3(0, 0, 0), 1.0f, 0.0f, false});
		m_rock = make_unique<ModelEntity>(ModelEntityDesc{"rock", "rock", float3(0, 0, 0), 1.0f, 0.0f, false});

		m_level_materials = makeMats(m_level);
		m_human->setPos(float3(0, 70.0f, 0));
		m_rock->setPos(float3(10, 75.0f, 10.0f));
	}
		
	void World::simulate(double time_diff) {
		m_time_diff = time_diff;
	}
		
	void World::draw(Renderer &out) const {
		std::map<string, PMaterial> level_mats;
		m_level->draw(out, *m_level_materials);
		m_human->draw(out);
		m_rock->draw(out);
	}

	World *World::instance() {
		static World s_world;
		return &s_world;
	}



}
