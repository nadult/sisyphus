#include "game/human_control.h"
#include "game/world.h"

namespace game {

HumanControl::HumanControl(World *world, ModelEntity *human)
	: m_world(world), m_human(human), m_dir(0, 1) {
	m_dir = normalize(m_world->rock()->pos() - m_world->human()->pos()).xz();
}

void HumanControl::move(const float3 &in, float rot) {
	float3 vector = in;

	float src_height = m_world->getHeight(m_human->pos().xz());
	float dst_height = m_world->getHeight((m_human->pos() + vector).xz());

	float speed = 15.0f;
	m_move_vector = vector * speed;
	m_move_rot = -rot * 2.0f;

}

void HumanControl::update(double time_diff) {
	m_dir = rotateVector(m_dir, m_move_rot * time_diff);
	float2 mv = m_dir * m_move_vector.z + rotateVector(m_dir, constant::pi * 0.5f) * m_move_vector.x;

	float2 new_pos_xz = m_human->pos().xz() + mv * time_diff;
	float3 new_pos = float3(new_pos_xz[0], m_world->getHeight(new_pos_xz), new_pos_xz[1]);
//	printf("new_pos: %f %f %f\n", new_pos.x, new_pos.y, new_pos.z);
	m_human->setPos(new_pos);

	Quat rot = rotationBetween(float3(0, 0, 1), float3(m_dir[0], 0, m_dir[1]));
	m_human->setRotation(rot);
}
}
