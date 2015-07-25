#pragma once

#include "game/base.h"
#include "game/model_entity.h"

namespace game {

	class World {
	public:
		World();
		static World *instance();

		void draw(Renderer&) const;
		void simulate(double time_diff);
		double timeDiff() const { return m_time_diff; }

	private:
		double m_time_diff;
		PModel m_level;
		unique_ptr<ModelEntity> m_human;
		unique_ptr<MaterialSet> m_level_materials;
	};

}
