#ifndef LIBRENDERING_SETTINGS
#define LIBRENDERING_SETTINGS

namespace librendering {
	class rendering_settings {
	public:
		int frame_width = 640; /**< Rendered frame width */
		int frame_height = 480; /**< Rendered frame height */

		int window_width = 640; /**< Desktop window width */
		int window_height = 480; /**< Desktop window height */

		bool free_cam_enabled = true; /**< Is free flying camera enabled */

		float popup_timeout = 5.f; /**< How long should popups stay on screen, seconds */
	};
}

#endif // !LIBRENDERING_SETTINGS
