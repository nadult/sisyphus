/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef GAME_BASE_H
#define GAME_BASE_H

#include "../base.h"

namespace game {

class World;
class Entity;
class EntityMap;
class Action;
class Order;
class Actor;
class Animator;
class Mind;

DECLARE_ENUM(SoundType, normal, explosion, shooting, looping, background);

using PEntity = unique_ptr<Entity>;
using PAction = unique_ptr<Action>;
using POrder = unique_ptr<Order>;

DECLARE_ENUM(EntityId, landmark, container, actor, item, projectile, impact, trigger);

DECLARE_ENUM(ActionTypeId, idle = 0, move, attack, get_hit, die);

DECLARE_ENUM(EntityEvent, init_action, tick, anim_finished, hit, impact);

DECLARE_ENUM(AnimationId, idle, walk, attack_melee, attack_ranged, attack_explosive, get_hit, die);

DECLARE_ENUM(DamageType, slashing, piercing, explosion);

DECLARE_ENUM(ProjectileType, arrow, bomb);

DECLARE_ENUM(AttackMode, melee, ranged, explosive, best);

namespace Flags {
	enum Type : unsigned;
};

inline constexpr Flags::Type entityIdToFlag(EntityId::Type id) {
	return (Flags::Type)(1u << (4 + id));
}

static_assert(EntityId::count <= 12, "Flag limit reached");

namespace Flags {
	enum Type : unsigned {
		// Generic flags: when testing, at least one flag must match
		object_mask = 0x00ffffff,

		static_entity = 0x0001,
		dynamic_entity = 0x0002, // can change position and/or bounding box

		entity = 0xffff,
		landmark = entityIdToFlag(EntityId::landmark),
		container = entityIdToFlag(EntityId::container),
		actor = entityIdToFlag(EntityId::actor),
		item = entityIdToFlag(EntityId::item),
		projectile = entityIdToFlag(EntityId::projectile),
		impact = entityIdToFlag(EntityId::impact),
		trigger = entityIdToFlag(EntityId::trigger),

		all = 0xffffff,
		height_map = 0x010000,

		// Functional flags: when testing, all of the selected flags must match
		visible = 0x01000000,
		colliding = 0x02000000,
		functional_mask = 0xff000000,
	};

	inline constexpr Type operator|(Type a, Type b) { return (Type)((unsigned)a | (unsigned)b); }
	inline constexpr Type operator&(Type a, Type b) { return (Type)((unsigned)a & (unsigned)b); }
	inline constexpr Type operator~(Type a) { return (Type)(~(unsigned)a); }

	inline constexpr bool test(unsigned tested_object_flags, Type test) {
		return (tested_object_flags & test & object_mask) &&
			   ((tested_object_flags & (test & functional_mask)) == (test & functional_mask));
	}
}

using EntityRef = Entity *;

class Intersection {
  public:
	Intersection(const Entity *ref = nullptr, float distance = constant::inf)
		: m_ref(ref), m_distance(distance) {}
	explicit operator bool() const { return !isEmpty(); }
	bool operator==(const Intersection &rhs) const {
		return std::tie(m_ref, m_distance) == std::tie(rhs.m_ref, rhs.m_distance);
	}
	bool operator<(const Intersection &rhs) const {
		if(m_distance == rhs.m_distance)
			return m_ref < rhs.m_ref;
		return m_distance < rhs.m_distance;
	}

	const Entity *ref() const { return m_ref; }
	bool isEmpty() const { return m_distance == constant::inf; }
	float distance() const { return m_distance; }

  private:
	const Entity *m_ref;
	float m_distance;
};
}

#endif
