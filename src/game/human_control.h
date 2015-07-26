#pragma once

#include "base.h"
#include "game/animator.h"
#include "physics.h"

namespace physics { class RigidBody; }

namespace game {

	class World;
	class ModelEntity;
	class Animator;

	class HumanControl {
	public:
		HumanControl(World *world, ModelEntity *human, physics::RigidBody*);

		void move(const float3 &move, float rot);
		void update(double time_diff);

	private:
		World *m_world;
		ModelEntity *m_human;
		physics::RigidBody *m_phys;
		Animator m_animator;
		float2 m_dir;
		float3 m_vector;

		float m_move_rot;
		float m_move_speed;
	};

}
