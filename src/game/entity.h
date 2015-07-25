/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include "game/base.h"

namespace game {

class Entity {
  public:
	Entity() : m_is_dead(false) {}
	virtual ~Entity() = default;
	void operator=(const Entity &) = delete;
	Entity(const Entity &) = delete;

	const float3 &pos() const { return m_pos; }
	const Quat &rotation() const { return m_rotation; }

	virtual void setPos(const float3 &new_pos);
	virtual void setRotation(const Quat &new_rotation);

	Matrix4 trans() const;
	Matrix4 invTrans() const;

	virtual FBox localBBox() const = 0;
	virtual FBox bbox() const;

	virtual Intersection intersect(const Segment &segment) const;
	virtual bool isIntersecting(const Frustum &) const;

	virtual bool isDead() const { return m_is_dead; }
	void kill() { m_is_dead = true; }

	virtual void tick() {}
	virtual void draw(Renderer &) const {}

	void remove() {
		m_is_dead = true;
	}

  protected:
	float3 m_pos;
	Quat m_rotation;
	bool m_is_dead;
};

}

#endif
