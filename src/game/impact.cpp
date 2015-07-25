/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "game/impact.h"
#include "game/world.h"
#include "sprite.h"

namespace game {

DEFINE_ENUM(ImpactType, "melee", "explosion");

Impact::Impact(ImpactType::Type type, EntityRef source, const FBox &area)
	: m_type(type), m_source(source), m_area(area), m_anim_time(0.0f), m_applied_damage(false) {
	if(type == ImpactType::explosion)
		m_sprite = make_unique<Sprite>("explosion_01");
}
Impact::~Impact() = default;

FBox Impact::localBBox() const { return m_area; }

void Impact::draw(Renderer &out) const {
	if(m_sprite) {
		m_sprite->draw(out, pos() + float3(0, m_area.height() * 0.5f, 0),
					   float2(length(m_area.size().xz()), m_area.height()), m_anim_time);
	}
	out.addWireBox(localBBox(), Color::red, trans());
}

void Impact::tick() {
	if(!m_applied_damage) {
		applyDamage();
		m_applied_damage = true;
	}

	m_anim_time += World::instance()->timeDiff();
	if(!m_sprite || m_anim_time > m_sprite->animLength())
		remove();
}

void Impact::applyDamage() {
/*	Entity *source = refEntity(m_source);

	auto refs = findAll(m_area + pos(), {Flags::actor, m_source});
	for(auto ref : refs) {
		if(auto *actor = refEntity<Actor>(ref)) {
			Damage damage{m_type == ImpactType::explosion ? DamageType::explosion
														  : DamageType::slashing,
						  m_type == ImpactType::explosion ? 30.0f : 20.0f,
						  normalize(pos() - actor->pos()), m_source};

			actor->onImpact(damage);
		}
	}*/
}
}
