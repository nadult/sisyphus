#pragma once

#include <btBulletDynamicsCommon.h>
#include "base.h"

class btCharacterControllerInterface;
class btDynamicCharacterController;
class btKinematicCharacterController;
class btPairCachingGhostObject;

namespace source {
class Level;
};

namespace physics {

class RigidBody;
class Character;

class PhysWorld {
  public:
	PhysWorld(PModel level, float scale);
	RigidBody addSphere(const float3 &pos, float radius, float mass);
	RigidBody addCapsule(const float3 &pos, float radius, float height, float mass);
	Character addCharacter(const float3 &pos, float radius, float height, float mass);
	void step();
	void debugDraw(Renderer &);

	float scale() const { return m_scale; }

  private:
	btDbvtBroadphase broadphase;
	btDefaultCollisionConfiguration colConf;
	btCollisionDispatcher dispatcher;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld world;
	float m_scale;
};

class Character {
  public:
	float3 getPosition() const;
	Matrix4 getTrans() const;

	void move(const float3 &);
	void rotate(float angle);

  private:
	Character(const PhysWorld *, btKinematicCharacterController *, btPairCachingGhostObject *);
	friend class PhysWorld;

	const PhysWorld *m_world;
	btKinematicCharacterController *m_character;
	btPairCachingGhostObject *m_ghost_obj;
};

class RigidBody {
  public:
	Quat getRotation() const;
	float3 getPosition() const;
	void move(const float3 &);

  private:
	RigidBody(const PhysWorld *, btRigidBody *ptr);
	friend class PhysWorld;

	const PhysWorld *m_world;
	btRigidBody *m_ptr;
};
}
