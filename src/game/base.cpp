/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "game/base.h"

namespace game {

DEFINE_ENUM(EntityId, "landmark", "container", "actor", "item", "projectile", "impact", "trigger");

DEFINE_ENUM(ActionTypeId, "idle", "move", "attack", "get_hit", "die");

DEFINE_ENUM(AnimationId, "idle", "walk", "attack_melee", "attack_ranged", "attack_explosive", "hit",
			"die");

DEFINE_ENUM(DamageType, "slashing", "piercing", "explosion");

DEFINE_ENUM(ProjectileType, "arrow", "bomb");

DEFINE_ENUM(AttackMode, "melee", "ranged", "explosive", "best");


}
