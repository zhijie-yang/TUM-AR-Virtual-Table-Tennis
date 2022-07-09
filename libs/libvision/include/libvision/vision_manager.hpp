#ifndef LIBVISION_MANAGER
#define LIBVISION_MANAGER

#include "vision_settings.hpp"

#include <memory>
#include <functional>

namespace libvision {
	class vision_manager {
	private:
		class impl;
		std::unique_ptr<impl> _impl;
	public:
		vision_manager();
		~vision_manager();

		int capture_width_get() const;
		int capture_height_get() const;
		int capture_serialize(const std::function<int(void*, int)>& processor);

		int init(const vision_settings& settings);
		int run_tick();
		int term();
	};
}

#endif // !LIBVISION_MANAGER

#pragma once
