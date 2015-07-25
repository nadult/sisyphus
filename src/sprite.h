#ifndef SPRITE_H
#define SPRITE_H

#include "base.h"

struct SpriteDesc {
	string texture_name;
	int2 grid_size;
	float anim_length;
};

class Sprite{
  public:
	Sprite(const string & sprite_name);

	void draw(Renderer &, const float3 &pos, const float2 &size, float anim_time) const;
	float animLength() const { return m_desc.anim_length; }

  protected:
	PMaterial m_material;
	const SpriteDesc &m_desc;
};

#endif
