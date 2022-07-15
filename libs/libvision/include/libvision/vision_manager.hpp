#ifndef LIBVISION_MANAGER
#define LIBVISION_MANAGER

#include "vision_settings.hpp"

#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#define CENTER_MARKER_ID 23
#define RACKET_MARKER_ID 40
#define MARKERS_NUM 3           //The number of markers on the board
#define VIEW_POINTS 8           // Viewpoints used to calibrate the camera
#define MARKER_SIZE 0.035       // the side length of the marker unit:meter
#define MARKER_PIXEL 199        // the pixel of one side of the marker
#define THRESHOLD 5e-3          //threshold used to stablize the board detection
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
	};
}

#endif // !LIBVISION_MANAGER

#pragma once
