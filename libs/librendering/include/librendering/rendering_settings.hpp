#ifndef LIBRENDERING_SETTINGS
#define LIBRENDERING_SETTINGS

namespace librendering {
	class rendering_settings {
	public:
		int frame_width;
		int frame_height;

		int window_width;
		int window_height;

		bool free_cam_enabled;
	};
}

#endif // !LIBRENDERING_SETTINGS
