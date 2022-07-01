#ifndef LIBRENDERING_MANAGER
#define LIBRENDERING_MANAGER

#include "librendering/rendering_settings.hpp"

#include <memory>
#include <functional>

#include <glm/glm.hpp>

namespace librendering {
	class rendering_manager {
	public:
		class impl;

		rendering_manager();
		~rendering_manager();

		bool quit_get() const;
		void quit_set();

		bool paused_get() const;
		void paused_set(bool value);

		std::function<int(void*, int, int, int)> capture_deserialize();

		std::function<int(const glm::mat4&)> proj_deserialize();
		std::function<int(const glm::mat4&)> view_deserialize();
		std::function<int(const glm::mat4&)> racket1_deserialize();
		std::function<int(const glm::mat4&)> racket2_deserialize();
		std::function<int(const glm::mat4&)> ball_deserialize();

		int init(const rendering_settings& settings);
		int run_tick();
		int term();
	private:
		std::unique_ptr<impl> _impl;
	};
}

#endif // !LIBRENDERING_MANAGER
