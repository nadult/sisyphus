#include "game/human_control.h"
#include "game/world.h"
#include "game/animator.h"

namespace game {

HumanControl::HumanControl(World *world, ModelEntity *human)
	: m_world(world), m_human(human), m_dir(0, 1), m_animator(m_human->model()) {
	m_dir = normalize(m_world->rock()->pos() - m_world->human()->pos()).xz();
}

void HumanControl::move(const float3 &in, float rot) {
	float movefw = clamp(in.z, 0.0f, 1.0f);

	float src_height = m_world->getHeight(m_human->pos().xz());
	float dst_height = m_world->getHeight((m_human->pos() + float3(m_dir[0], 0, m_dir[1]) * movefw).xz());

	if(fabs(rot) > 0.01f && movefw < 0.01f)
		movefw = 0.25f;

	if(movefw > 0.01f) {
		m_move_speed += clamp((20.0f - m_move_speed) * 0.1f, 0.1f, 1.0f) * movefw;
	}
	else
		m_move_speed *= 0.9f;

	m_move_speed = clamp(m_move_speed, 0.0f, 20.0f * movefw);
	m_move_rot = -rot * 3.0f;

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

	m_animator.tick(m_move_speed / 12.0f * time_diff);
	m_human->setPose(m_animator.modelPose());
}
}
