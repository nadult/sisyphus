/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "game/animator.h"

namespace game {

Animator::Animator(PModel model) : m_model(std::move(model)) {
	DASSERT(m_model);
	setAnimation("Walk", true);
}

void Animator::setAnimation(const string &name, bool loop) {
	int anim_id = -1;
	for(int n = 0; n < (int)m_model->anims().size(); n++)
		if(m_model->anims()[n].name() == name) {
			anim_id = n;
			break;
		}
	ASSERT(anim_id != -1);


	m_anim_id = anim_id;
	m_anim_pos = 0.0f;
	m_anim_length = m_model->anim(m_anim_id).length();
	m_anim_is_looped = loop;
	m_anim_is_finished = false;
}

void Animator::tick(double time_diff) {
	m_anim_pos += time_diff;
	while(m_anim_pos < 0) {
		DASSERT(m_anim_is_looped);
		m_anim_pos += m_anim_length;
	}

	if(m_anim_pos >= m_anim_length) {
		if(m_anim_is_looped)
			m_anim_pos -= m_anim_length;
		else {
			m_anim_pos = m_anim_length - 0.0001f;
			m_anim_is_finished = true;
		}
	}
}

Pose Animator::modelPose() const { return m_model->animatePose(m_anim_id, m_anim_pos); }

}
