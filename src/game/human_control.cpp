#include "game/human_control.h"
#include "game/world.h"
#include "game/animator.h"

namespace game {

HumanControl::HumanControl(World *world, ModelEntity *human, physics::Character *phys)
	: m_world(world), m_human(human), m_phys(phys), m_dir(0, 1), m_walk_animator(m_human->model()), m_push_animator(m_human->model()) {
	m_dir = normalize(m_world->rock()->pos() - m_world->human()->pos()).xz();
	m_pose = m_walk_animator.modelPose();
	m_push_animator.setAnimation("Pushing", true);
	m_move_push_blend = 0.0f;

}

void HumanControl::move(const float3 &in, float rot) {
	float movefw = clamp(in.z, -0.5f, 1.0f);

//	float src_height = m_world->getHeight(m_human->pos().xz());
//	float dst_height = m_world->getHeight((m_human->pos() + float3(m_dir[0], 0, m_dir[1]) * movefw).xz());

	if(fabsf(rot) > 0.01f && fabsf(movefw) < 0.01f)
		movefw = 0.25f;

	if(fabsf(movefw) > 0.01f) {
		float inc = clamp((20.0f - fabsf(m_move_speed)) * 0.1f, 0.1f, 1.0f) * movefw;
		m_move_speed += inc;
	}
	if((movefw > 0) != (m_move_speed > 0))
		m_move_speed *= 0.9f;

	m_move_speed = clamp(m_move_speed, -20.0f * fabsf(movefw), 20.0f * fabsf(movefw));
	m_move_rot = -rot * 3.0f;

}

static Pose blendPose(Pose a, Pose b, float t) {
	vector<Matrix4> out;
	for(int n = 0; n < (int)a.size(); n++) {
		AffineTrans atrans(a.transforms[n]);
		AffineTrans btrans(b.transforms[n]);
		out.emplace_back(lerp(atrans, btrans, t));
	}
	return Pose(std::move(out), a.name_mapping);
}

void HumanControl::update(double time_diff) {
	m_dir = rotateVector(m_dir, m_move_rot * time_diff);
	float2 mv = m_dir * m_move_speed;

	float2 new_pos_xz = m_human->pos().xz() + mv * time_diff;
	float3 new_pos = float3(new_pos_xz[0], m_world->getHeight(new_pos_xz), new_pos_xz[1]);
//	printf("new_pos: %f %f %f\n", new_pos.x, new_pos.y, new_pos.z);
	m_human->setPos(new_pos);

	Quat rot = rotationBetween(float3(0, 0, 1), float3(m_dir[0], 0, m_dir[1]));
	m_human->setRotation(rot);

	auto *rock = m_world->rock();
	float rock_dist = distance(rock->pos().xz(), m_human->pos().xz()) - 8.0f;
	float rock_angle; {
		float2 rock_vec = (rock->pos() - m_human->pos()).xz();
		rock_angle = angleDistance(vectorToAngle(normalize(rock_vec)), vectorToAngle(normalize(m_dir)));
		rock_angle = clamp(rock_angle - constant::pi * 0.1f, 0.0f, 1.0f);

	}
	m_move_push_blend = clamp(1.0f - rock_dist * 0.25f, 0.0f, 1.0f) * clamp(constant::pi * 0.25f - rock_angle, 0.0f, 1.0f);

	m_walk_animator.tick(m_move_speed / 12.0f * time_diff);
	m_push_animator.tick(m_move_speed / 12.0f * time_diff * (2.33333 / 1.375));
	
	m_pose = blendPose(m_walk_animator.modelPose(), m_push_animator.modelPose(), m_move_push_blend);

	m_human->setPose(m_pose);
}
}
