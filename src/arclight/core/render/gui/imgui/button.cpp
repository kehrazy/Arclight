/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 button.cpp
 */

#include "button.hpp"

GUI_BEGIN

Button::Button() {}

Button::Button(const std::string& text, const Vec2f& size) {
	create(text, size);
}



void Button::create(const std::string& text, const Vec2f& size) {
	m_Text = text;
	m_Size = size;
}



std::string Button::getText() const {
	return m_Text;
}

void Button::setText(const std::string& text) {
	m_Text = text;
}



Vec2f Button::getSize() const {
	return m_Size;
}

void Button::setSize(const Vec2f& size) {
	m_Size = size;
}



void Button::update() {

	IImGuiControl::applyRenderModifiers();

	if (ImGui::Button(m_Text.c_str(), { m_Size.x, m_Size.y })) {
		m_OnClicked();
	}

}

GUI_END