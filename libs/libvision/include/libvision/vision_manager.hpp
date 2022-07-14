#ifndef LIBVISION_MANAGER
#define LIBVISION_MANAGER

#include "vision_settings.hpp"

#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

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

		int capture_serialize(const std::function<int(void*, int, int, int)>& processor);
        int racket1_serialize(const std::function<int(float*)>& processor);
        int racket2_serialize(const std::function<int(float*)>& processor);
        int table_serialize(const std::function<int(float*)>& processor);

        int view_serialize(const std::function<int(float*)>& processor);
        int proj_serialize(const std::function<int(float*)>& processor);

		int init(const vision_settings& settings);
		int run_tick();
		int term();
		glm::mat4 racket2table();
	};
}

#endif // !LIBVISION_MANAGER

#pragma once
