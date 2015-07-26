#include "physics.h"
#include <memory>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <LinearMath/btIDebugDraw.h>

namespace physics {

namespace {

	static Renderer *s_renderer = nullptr;

	static void beginFrame(Renderer &renderer) { s_renderer = &renderer; }

	static void finishFrame() { s_renderer = nullptr; }

	static void drawLine(const float3 &color, const float3 &from, const float3 &to) {
		float3 verts[2] = {from, to};
		if(s_renderer)
			s_renderer->addLines(verts, Color(color), Matrix4::identity());
	}

	class Drawer : public btIDebugDraw {
		void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
			physics::drawLine(float3(color.x(), color.y(), color.z()),
							  float3(from.x(), from.y(), from.z()), float3(to.x(), to.y(), to.z()));
		}

		void drawContactPoint(const btVector3 &pointOnB, const btVector3 &normalOnB,
							  btScalar distance, int lifeTime, const btVector3 &) {}
		void reportErrorWarning(const char *warningString) {}

		void draw3dText(const btVector3 &location, const char *textString) {}

		void setDebugMode(int debugMode) { mode = debugMode; }
		int getDebugMode() const { return mode; }

		int mode;
	};
}

PhysWorld::PhysWorld(PModel level, float scale)
	: broadphase(new btDbvtBroadphase), dispatcher(new btCollisionDispatcher(&colConf)),
	  solver(new btSequentialImpulseConstraintSolver),
	  world(dispatcher, broadphase, solver, &colConf), m_scale(scale) {

	world.setDebugDrawer(new Drawer());
	world.setGravity({0, -1, 0});
	using std::auto_ptr;

	auto mesh = level->toMesh(level->defaultPose());
	auto tris_indices = mesh.trisIndices();
	const auto &positions = mesh.positions();

	btTriangleIndexVertexArray *triArray;
	{
		btVector3 *verts = new btVector3[positions.size()];
		for(int n = 0; n < (int)positions.size(); n++)
			verts[n] = btVector3(positions[n].x, positions[n].y, positions[n].z) * m_scale;

		int *indices = new int[tris_indices.size() * 3];
		for(int n = 0; n < (int)tris_indices.size(); n++)
			for(int i = 0; i < 3; i++)
				indices[n * 3 + i] = tris_indices[n][i];

		triArray = new btTriangleIndexVertexArray(tris_indices.size(), indices, sizeof(int) * 3,
												  positions.size(), (btScalar *)&verts[0].x(),
												  sizeof(btVector3));
	}
	auto trimeshShape = new btBvhTriangleMeshShape(triArray, true);
	m_ground = trimeshShape;

	btDefaultMotionState *groundMotionState =
		new btDefaultMotionState(btTransform({0, 0, 0, 1}, {0, 0, 0}));
	btRigidBody *ground = new btRigidBody(
		btRigidBody::btRigidBodyConstructionInfo(0, groundMotionState, trimeshShape, {0, 0, 0}));
	ground->setCollisionFlags(ground->getCollisionFlags() |
							  btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	world.addRigidBody(ground);
}

namespace {

	btVector3 toBt(const float3 &v) { return btVector3(v.x, v.y, v.z); }
	float3 fromBt(const btVector3 &v) { return float3(v.x(), v.y(), v.z()); }

	struct Callback : public btTriangleCallback {
		Callback(const Segment &segment) : seg(segment), min_dist(constant::inf) {}
		void processTriangle(btVector3 *triangle, int partId, int triangleIndex) override {
			Triangle tri1(fromBt(triangle[0]), fromBt(triangle[1]), fromBt(triangle[2]));
			Triangle tri2(fromBt(triangle[0]), fromBt(triangle[2]), fromBt(triangle[1]));

			float3 mid = (tri1.a() + tri1.b() + tri1.c()) / 3.0f;
			float dist = distance(seg.origin(), mid);

			min_dist = min(min_dist, intersection(seg, tri1));
			min_dist = min(min_dist, intersection(seg, tri2));
		}

		Segment seg;
		float min_dist;
	};
}

float PhysWorld::rayCast(const Segment &seg) const {
	Segment s2(seg.origin() * m_scale, seg.at(seg.max()) * m_scale);
	Callback cb(s2);
	m_ground->performRaycast(&cb, toBt(s2.origin()), toBt(s2.at(seg.max())));
	return cb.min_dist / m_scale;
}

float PhysWorld::getHeight(const float2 &pos) const {
	Segment seg(float3(pos[0], 10000.0f, pos[1]), float3(pos[0], -10000.0f, pos[1]));
	float dist = rayCast(seg);
	return dist < constant::inf ? seg.at(dist).y : 0.0f;
}

RigidBody PhysWorld::addSphere(const float3 &pos, float radius, float mass) {
	btCollisionShape *shape = new btSphereShape(radius * m_scale);
	btDefaultMotionState *motionState = new btDefaultMotionState(
		btTransform({0, 0, 0, 1}, btVector3{pos[0], pos[1], pos[2]} * m_scale));

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);

	btRigidBody *body = new btRigidBody(
		btRigidBody::btRigidBodyConstructionInfo(mass, motionState, shape, inertia));
	world.addRigidBody(body);
	// TODO: usunac body przy destrukcji swiata

	return RigidBody(this, body);
}

void PhysWorld::debugDraw(Renderer &out) {
	world.getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	out.pushViewMatrix();
	out.mulViewMatrix(scaling(1.0f / m_scale));
	beginFrame(out);
	world.debugDrawWorld();
	finishFrame();
	out.popViewMatrix();
}

RigidBody PhysWorld::addCapsule(const float3 &pos, float radius, float height, float mass) {
	btCollisionShape *shape = new btCapsuleShape(radius * m_scale, height * m_scale);
	btDefaultMotionState *motionState = new btDefaultMotionState(
		btTransform({0, 0, 0, 1}, btVector3{pos[0], pos[1], pos[2]} * m_scale));

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
	rbInfo.m_friction = 1.0f;
	rbInfo.m_linearSleepingThreshold = 0.0f;
	rbInfo.m_angularSleepingThreshold = 0.0f;

	btRigidBody *body = new btRigidBody(rbInfo);
	body->setAngularFactor(0);
	world.addRigidBody(body);
	// TODO: usunac body przy destrukcji swiata

	return RigidBody(this, body);
}

Character PhysWorld::addCharacter(const float3 &pos, float radius, float height, float mass) {

	btPairCachingGhostObject *ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(
		btTransform({0, 0, 0, 1}, btVector3{pos[0], pos[1], pos[2]} * m_scale));

	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	btConvexShape *capsule = new btCapsuleShape(radius * m_scale, height * m_scale);
	ghostObject->setCollisionShape(capsule);
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.05);
	btKinematicCharacterController *character =
		new btKinematicCharacterController(ghostObject, capsule, stepHeight);

	world.addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter,
							 btBroadphaseProxy::DefaultFilter);
	world.addAction(character);

	return Character(this, character, ghostObject);
}

void PhysWorld::step() { world.stepSimulation(1.0f / 60.0f, 10.0f); }

RigidBody::RigidBody(const PhysWorld *world, btRigidBody *ptr) : m_world(world), m_ptr(ptr) {
	DASSERT(m_ptr);
}

float3 RigidBody::getPosition() const {
	btTransform trans;
	m_ptr->getMotionState()->getWorldTransform(trans);
	return float3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()) /
		   m_world->scale();
}

Quat RigidBody::getRotation() const {
	btTransform trans;
	m_ptr->getMotionState()->getWorldTransform(trans);
	auto rot = trans.getRotation();
	return Quat(rot.x(), rot.y(), rot.z(), rot.w());
}

void RigidBody::move(const float3 &v) { m_ptr->applyCentralImpulse(btVector3(v[0], v[1], v[2])); }

Character::Character(const PhysWorld *world, btKinematicCharacterController *character,
					 btPairCachingGhostObject *ghost_obj)
	: m_world(world), m_character(character), m_ghost_obj(ghost_obj) {
	// m_character->setGravity(0.0f);
}

void Character::fix() {}

float3 Character::getPosition() const {
	btTransform trans = m_ghost_obj->getWorldTransform();
	return float3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()) /
		   m_world->scale();
}

Matrix4 Character::getTrans() const {
	btTransform trans = m_ghost_obj->getWorldTransform();
	Matrix4 temp;
	DASSERT(sizeof(temp[0][0]) == sizeof(btScalar));
	trans.getOpenGLMatrix(&temp[0][0]);
	return scaling(100, 100, 100) * temp * scaling(0.01, 0.01, 0.01);
}

void Character::move(const float3 &vec) {
	btMatrix3x3 xform = m_ghost_obj->getWorldTransform().getBasis();

	btVector3 strafeDir = xform[0];
	btVector3 upDir = xform[1];
	btVector3 forwardDir = xform[2];
	forwardDir.normalize();
	upDir.normalize();
	strafeDir.normalize();
	swap(strafeDir[1], upDir[0]);
	swap(strafeDir[2], forwardDir[0]);
	swap(forwardDir[1], upDir[2]);

	m_character->setWalkDirection(-strafeDir * vec[0] + upDir * vec[1] - forwardDir * vec[2]);
}

void Character::rotate(float ang) {
	btMatrix3x3 orn = m_ghost_obj->getWorldTransform().getBasis();
	orn *= btMatrix3x3(btQuaternion(btVector3(0, 1, 0), ang));
	m_ghost_obj->getWorldTransform().setBasis(orn);
}
}
