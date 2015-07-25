/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "game/animator.h"

namespace game {

Animator::Animator(PModel model) : m_model(std::move(model)) {
	DASSERT(m_model);
	setAnimation(AnimationId::idle, true);
}

bool Animator::hasAnimation(AnimationId::Type anim_type) const {
	return animIndex(anim_type) != -1;
}

void Animator::setAnimation(AnimationId::Type anim_id, bool loop) {
	m_anim_id = anim_id;
	m_anim_pos = 0.0f;
	int idx = animIndex(anim_id);
	m_anim_length = idx == -1 ? 1.0f : m_model->anim(idx).length();
	m_anim_is_looped = loop;
	m_anim_is_finished = false;
}

AnimationId::Type Animator::currentAnimation() const { return m_anim_id; }

void Animator::tick(double time_diff) {
	m_anim_pos += time_diff;
	if(m_anim_pos >= m_anim_length) {
		if(m_anim_is_looped)
			m_anim_pos -= m_anim_length;
		else {
			m_anim_pos = m_anim_length - 0.0001f;
			m_anim_is_finished = true;
		}
	}
}

Pose Animator::modelPose() const { return m_model->animatePose(animIndex(m_anim_id), m_anim_pos); }

int Animator::animIndex(AnimationId::Type anim_id) const {
	const char *name = toString(anim_id);
	for(int a = 0; a < m_model->animCount(); a++) {
		const auto &anim = m_model->anim(a);
		if(caseEqual(anim.name(), name))
			return a;
	}
	return -1;
}
}
