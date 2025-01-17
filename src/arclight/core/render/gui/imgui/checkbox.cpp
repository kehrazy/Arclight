/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 checkbox.cpp
 */

#include "checkbox.hpp"

GUI_BEGIN

CheckBox::CheckBox() {}

CheckBox::CheckBox(const std::string& text, bool checked) {
	create(text, checked);
}



void CheckBox::create(const std::string& text, bool checked) {
	m_Text = text;
	m_Checked = checked;
}



bool CheckBox::isChecked() const {
	return m_Checked;
}

void CheckBox::setChecked(bool checked) {
	m_Checked = checked;
}



std::string CheckBox::getText() const {
	return m_Text;
}

void CheckBox::setText(const std::string& text) {
	m_Text = text;
}



void CheckBox::update() {

	IImGuiControl::applyRenderModifiers();

	if (ImGui::Checkbox(m_Text.c_str(), &m_Checked)) {
		m_OnClicked();
	}

}

GUI_END