/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef GAME_IMPACT_H
#define GAME_IMPACT_H

#include "game/entity.h"

class Sprite;

namespace game {

DECLARE_ENUM(ImpactType, melee, explosion);

class Impact : public Entity {
  public:
	Impact(ImpactType::Type, EntityRef source, const FBox &area);
	~Impact();

	void draw(Renderer &out) const override;
	void tick() override;
	FBox localBBox() const override;

  protected:
	void applyDamage();

	ImpactType::Type m_type;
	unique_ptr<Sprite> m_sprite;
	EntityRef m_source;
	FBox m_area;
	float m_anim_time;
	bool m_applied_damage;
};
}

#endif
