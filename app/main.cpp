#include <iostream>
#include <libtennis/ball.hpp>
#include <librendering/rendering_manager.hpp>
#include <libvision/vision_manager.hpp>

using namespace std;
using namespace libtennis;
using namespace librendering;
using namespace libvision;

int main(int, char **) {
	vision_manager vision;
	vision_settings vsettings;
	vsettings.camera_id = 0;

	if (vision.init(vsettings)) {
		cerr << "Failed to init vision.\n";
		return -1;
	}

	rendering_manager rendering;
	rendering_settings rsettings;
	rsettings.window_width = 800;
	rsettings.window_height = 480;
	rsettings.frame_width = vision.capture_width_get();
	rsettings.frame_height = vision.capture_height_get();

	if (rendering.init(rsettings)) {
		cerr << "Failed to init rendering.\n";
		return 1;
	}

	while (!rendering.quit_get()) {
		if (!rendering.paused_get()) {
			if (vision.run_tick()) {
				cerr << "Failed to run tick vision.\n";
				break;
			}

			vision.capture_serialize(rendering.capture_deserialize());
		}

		if (rendering.run_tick()) {
			cerr << "Failed to run tick rendering.\n";
			break;
		}
	}

	if (rendering.term()) {
		cerr << "Failed to term rendering.\n";
		return 1;
	}

	if (vision.term()) {
		cerr << "Failed to term vision.\n";
		return 1;
	}

	return 0;
}
