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

		enum scene {
			main_menu,
			connection,
			level,
			ending,
		};

		rendering_manager();
		~rendering_manager();

		bool quit_get() const;
		void quit_set();

		bool paused_get() const;
		void paused_set(bool value);

		scene scene_get() const;
		void scene_set(scene value);

		std::function<int(void*, int, int, int)> capture_deserialize();

		std::function<int(const glm::mat4&)> proj_deserialize();
		std::function<int(const glm::mat4&)> view_deserialize();
        std::function<int(float*)> racket1_deserialize();
        std::function<int(float*)> racket2_deserialize();
        std::function<int(float*)> ball_deserialize();
        std::function<int(float*)> table_deserialize();

		std::function<int(const int&)> score1_deserialize();
		std::function<int(const int&)> score2_deserialize();
		std::function<int(const int&)> match1_deserialize();
		std::function<int(const int&)> match2_deserialize();
		std::function<int(const char*)> player1_deserialize();
		std::function<int(const char*)> player2_deserialize();


		int frametime_serialize(const std::function<int(float)>& processor);

		int init(const rendering_settings& settings);
		int run_tick();
		int term();
	private:
		std::unique_ptr<impl> _impl;
	};
}

#endif // !LIBRENDERING_MANAGER
