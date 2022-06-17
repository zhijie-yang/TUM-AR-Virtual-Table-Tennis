#ifndef LIBRENDERING_MANAGER
#define LIBRENDERING_MANAGER

#include "librendering/rendering_settings.hpp"

#include <memory>
#include <functional>

namespace librendering {
	class rendering_manager {
	private:
		class impl;
		std::unique_ptr<impl> _impl;
	public:
		rendering_manager();
		~rendering_manager();

		bool quit_get() const;
		void quit_set();

		bool paused_get() const;
		void paused_set(bool value);

		std::function<int(void*, int)> capture_deserialize();

		int init(const rendering_settings& settings);
		int run_tick();
		int term();
	};
}

#endif // !LIBRENDERING_MANAGER
