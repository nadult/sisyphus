#include "game/world.h"
#include "game/human_control.h"
#include "physics.h"
#include "fwk_audio.h"

namespace game {

auto makeMats(PModel model) {
	Shader vertex_shader(Shader::tVertex), fragment_shader(Shader::tFragment);
	Loader("data/mesh_shader.vsh") >> vertex_shader;
	Loader("data/mesh_shader.fsh") >> fragment_shader;
	auto program = make_immutable<Program>(vertex_shader, fragment_shader);
	std::map<string, PMaterial> mats;
	for(auto mat : model->materialDefs())
		mats.emplace(mat.name, make_immutable<Material>(program, mat.diffuse));
	return make_unique<MaterialSet>(make_immutable<Material>(), mats);
}

World::World() : m_time_diff(0) {
	m_level = res::models()["level2"];
	auto human_pos = m_level->findNode("Spawn")->globalTrans().translation;
	auto rock_pos = m_level->findNode("Rock")->globalTrans().translation;

	m_human = make_unique<ModelEntity>(
		ModelEntityDesc{"human", "human", float3(0, 0, 0), 1.0f, 0.0f, true});
	m_rock = make_unique<ModelEntity>(
		ModelEntityDesc{"rock", "rock", float3(0, 0, 0), 1.0f, 0.0f, false});

	m_rock->setPos(rock_pos);
	m_human->setPos(human_pos);

	m_physics = make_unique<physics::PhysWorld>(m_level, 0.01f);
	m_phys_rock =
		make_unique<physics::RigidBody>(std::move(m_physics->addSphere(m_rock->pos(), 7.5f, 20.0f)));
	m_phys_human = make_unique<physics::RigidBody>(
		std::move(m_physics->addCapsule(m_human->pos(), 1.0f, 1000.0f, 1.0f)));

	m_human_control = make_unique<HumanControl>(this, m_human.get(), m_phys_human.get());

	OggStream roll_stream("data/sounds/staczanie.ogg"), ambient_stream("data/sounds/ambient.ogg");
	auto &audio= AudioDevice::instance();
	audio.playSound(make_shared<DSound>(roll_stream.makeSound()), SoundPos(), SoundConfig(0.25f, 1.0f, true)); 
	audio.playSound(make_shared<DSound>(ambient_stream.makeSound()), SoundPos(), SoundConfig(0.1f, 1.0f, true)); 

	m_level_materials = makeMats(m_level);
}

World::~World() = default;

float World::getHeight(const float2 &pos_xz) const { return m_physics->getHeight(pos_xz); }

void World::simulate(double time_diff) {
	m_time_diff = time_diff;
	m_physics->step();

	m_rock->setPos(m_phys_rock->getPosition());
	m_rock->setRotation(m_phys_rock->getRotation());
	m_human_control->update(time_diff);
}

void World::draw(Renderer &out) const {
	std::map<string, PMaterial> level_mats;
	m_level->draw(out, *m_level_materials);
	m_rock->draw(out);
//	m_physics->debugDraw(out);
	m_human->draw(out);
}
}
