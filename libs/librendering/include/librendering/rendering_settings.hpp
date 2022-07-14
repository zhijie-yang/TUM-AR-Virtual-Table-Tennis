#ifndef LIBRENDERING_SETTINGS
#define LIBRENDERING_SETTINGS

namespace librendering {
	class rendering_settings {
	public:
		int frame_width = 640;
		int frame_height = 480;

		int window_width = 640;
		int window_height = 480;

		bool free_cam_enabled = true;

		float popup_timeout = 5.f;
	};
}

#endif // !LIBRENDERING_SETTINGS
