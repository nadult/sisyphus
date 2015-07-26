#pragma once

#include "game/base.h"
#include "game/model_entity.h"

namespace physics {
	class PhysWorld;
	class RigidBody;
	class Character;
}

namespace game {

	class HumanControl;

	class World {
	public:
		World();
		~World();

		void draw(Renderer&) const;
		void simulate(double time_diff);
		double timeDiff() const { return m_time_diff; }

		ModelEntity *human() { return m_human.get(); }
		ModelEntity *rock() { return m_rock.get(); }
		HumanControl *humanControl() { return m_human_control.get(); }
		PModel level() { return m_level; }

		float getHeight(const float2 &pos_xz) const;

	private:
		double m_time_diff;
		PModel m_level;
		unique_ptr<physics::PhysWorld> m_physics;
		unique_ptr<ModelEntity> m_rock;
		unique_ptr<physics::RigidBody> m_phys_rock;
		unique_ptr<physics::Character> m_phys_human;
		unique_ptr<MaterialSet> m_level_materials;
		
		unique_ptr<ModelEntity> m_human;
		unique_ptr<HumanControl> m_human_control;
	};


}
