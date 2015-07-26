#pragma once

#include "base.h"

namespace game {

	class World;
	class ModelEntity;

	class HumanControl {
	public:
		HumanControl(World *world, ModelEntity *human);

		void move(const float3 &move, float rot);
		void update(double time_diff);

	private:
		World *m_world;
		ModelEntity *m_human;
		float2 m_dir;
		float3 m_vector;

		float3 m_move_vector;
		float m_move_rot;
	};

}
