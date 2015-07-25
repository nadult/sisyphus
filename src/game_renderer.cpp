/* Copyright (C) 2015 Krzysztof Jakubowski <nadult@fastmail.fm>
 */

#include "game_renderer.h"

GameRenderer::GameRenderer(const IRect &viewport) : Renderer2D(viewport) {
	m_font = res::fonts()["liberation_16"];
	m_font_texture = res::textures()[m_font->textureName()];
	m_font_renderer = make_unique<FontRenderer>(m_font, m_font_texture, *this);
}
GameRenderer::~GameRenderer() = default;

void GameRenderer::addRect(const FRect &rect, const RectStyle &style) {
	if(style.fill_color != Color::transparent)
		Renderer2D::addFilledRect(rect, style.fill_color);
	if(style.border_color != Color::transparent)
		Renderer2D::addRect(rect, style.border_color);
}
