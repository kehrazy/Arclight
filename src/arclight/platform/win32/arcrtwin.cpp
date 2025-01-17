/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcrtwin.cpp
 */

#include "util/log.hpp"
#include "os/os.hpp"



namespace ArcRuntime {

	bool platformInit() {
		return OS::init();
	}

}
