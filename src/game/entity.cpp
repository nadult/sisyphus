/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "game/entity.h"
#include "game/world.h"

namespace game {

FBox Entity::bbox() const { return trans() * localBBox(); }

Intersection Entity::intersect(const Segment &segment) const {
	float dist = intersection(invTrans() * segment, localBBox());
	return dist < constant::inf ? Intersection(this, dist) : Intersection();
}

bool Entity::isIntersecting(const Frustum &frustum) const {
	return (invTrans() * frustum).isIntersecting(localBBox());
}

void Entity::setPos(const float3 &new_pos) { m_pos = new_pos; }
void Entity::setRotation(const Quat &new_rotation) { m_rotation = normalize(new_rotation); }

Matrix4 Entity::trans() const { return translation(m_pos) * Matrix4(m_rotation); }
Matrix4 Entity::invTrans() const { return Matrix4(inverse(m_rotation)) * translation(-m_pos); }

}
