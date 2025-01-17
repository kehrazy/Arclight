/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 notificationicon.cpp
 */

#include "notification.hpp"

#include "image/image.hpp"



namespace OS {

	void Notification::setIcon(const Image<Pixel::RGBA8>& icon) {
		setIconInternal(*handle, icon.getWidth(), icon.getHeight(), icon.getImageData(), true);
	}



	void Notification::setImage(const Image<Pixel::RGBA8>& icon) {
		setIconInternal(*handle, icon.getWidth(), icon.getHeight(), icon.getImageData(), false);
	}

}
