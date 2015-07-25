#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include "base.h"

class GameRenderer : public Renderer2D {
  public:
	GameRenderer(const IRect &viewport);
	~GameRenderer();

	void addRect(const FRect &rect, const RectStyle &style);

	FontRenderer &font() { return *m_font_renderer; }

  private:
	PFont m_font;
	PTexture m_font_texture;
	unique_ptr<FontRenderer> m_font_renderer;
};

#endif
