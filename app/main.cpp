#include <iostream>
#include <librendering/rendering_manager.hpp>
#include <libvision/vision_manager.hpp>
#include "libtennis/tennis_manager.h"

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
	rsettings.window_height = 600;
	rsettings.frame_width = vision.capture_width_get();
	rsettings.frame_height = vision.capture_height_get();
	rsettings.free_cam_enabled = true;

    // default maximum score is 11. For simplicity, assume each match contains only one game
    tennis_manager tennis(11);

	if (rendering.init(rsettings)) {
		cerr << "Failed to init rendering.\n";
		return 1;
	}

	// TODO: For debugging, remove later
	rendering.scene_set(rendering_manager::scene::main_menu);
	rendering.player1_deserialize()("John");
	rendering.player2_deserialize()("Sam");

	while (!rendering.quit_get()) {
		if (!rendering.paused_get()) {
            // TODO: receive from server
            // get enemy's racket status
            // get ball status
            rendering.frametime_serialize(tennis.frametime_deserialize());

			if (vision.run_tick()) {
				cerr << "Failed to run tick vision.\n";
				break;
			}
            // vision serialize racket and table transformation for tennis and rendering
			vision.capture_serialize(rendering.capture_deserialize());
            vision.racket1_serialize(rendering.racket1_deserialize());
            vision.racket2_serialize(rendering.racket2_deserialize());
            vision.table_serialize(rendering.table_deserialize());
            vision.racket1_serialize(tennis.racket1_deserialize());
            vision.racket2_serialize(tennis.racket2_deserialize());
            vision.table_serialize(tennis.table_deserialize());

            if (tennis.run_tick()) {
                cerr << "Failed to run tick tennis.\n";
                break;
            }
            tennis.ball_serialize(rendering.ball_deserialize());
            tennis.score1_serialize(rendering.score1_deserialize());
            tennis.score2_serialize(rendering.score2_deserialize());
            tennis.game_status_serialize(rendering.game_status_deserialize());
		}

        // TODO: only the turn owner send latest ball status to server
        // TODO: each user should send racket status

		if (rendering.run_tick()) {
			cerr << "Failed to run tick rendering.\n";
			break;
		}
        rendering.game_status_serialize(tennis.game_status_deserialize());
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
