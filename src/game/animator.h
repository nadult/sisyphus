/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#ifndef GAME_ANIMATOR_H
#define GAME_ANIMATOR_H

#include "game/base.h"

namespace game {

class Animator {
  public:
	Animator(PModel);

	void setAnimation(const string &name, bool loop);
	AnimationId::Type currentAnimation() const;
	bool isAnimationFinished() const { return m_anim_is_finished; }

	void tick(double time_diff);
	PPose modelPose() const;

	int animId() const { return m_anim_id; }
	double currentAnimPos() const { return m_anim_pos; }

  private:
	int animIndex(AnimationId::Type) const;

	PModel m_model;
	int m_anim_id;
	double m_anim_pos;
	double m_anim_length;
	bool m_anim_is_looped;
	bool m_anim_is_finished;
};
}

#endif
