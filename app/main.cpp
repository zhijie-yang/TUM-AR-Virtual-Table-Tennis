#include <iostream>
#include <sstream>
#include <librendering/rendering_manager.hpp>
#include <libvision/vision_manager.hpp>
#include "libtennis/tennis_manager.h"
#include "libframework/include/ball_status.h"
#include "libframework/include/racket_status.h"
#include "libframework/include/infos.h"
#include "libnetwork/include/client.h"

#define RECONNECT_TICKS 50

using namespace std;
using namespace libtennis;
using namespace librendering;
using namespace libvision;

VirtualTennisNetworkClient *g_tennis_client = nullptr;
PlayerInfo *g_player_info = nullptr;
unsigned reconnect_tick_cnt = 0;
librendering::rendering_manager::scene curr_scene = librendering::rendering_manager::scene::main_menu;

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
    rsettings.free_cam_enabled = false;

    // default maximum score is 11. For simplicity, assume each match contains only one game
    tennis_manager tennis(11);

	if (rendering.init(rsettings)) {
		cerr << "Failed to init rendering.\n";
		return 1;
	}

	rendering.scene_set(rendering_manager::scene::main_menu);
	rendering.player1_deserialize()("John");
	rendering.player2_deserialize()("Sam");
    vision.proj_serialize(rendering.proj_deserialize());

	while (!rendering.quit_get()) {
		// creat the client instance
		if (rendering.ready_to_register_get() && g_tennis_client == nullptr) {
			g_tennis_client = new VirtualTennisNetworkClient(std::string(rendering.input_ip_get()), rendering.input_port_get(),
														std::string(rendering.input_player_name_get()));
			rendering.player1_deserialize()(rendering.input_player_name_get());
		}

		// connect to the server after the client instance has been created
		if (g_tennis_client && !g_tennis_client->is_connected()) {
			ClientConnectionResponse connection_res = g_tennis_client->connectServer();
			std::cout << connection_res.get_detail() << std::endl;
			if (connection_res.get_result()) {
				g_player_info = new PlayerInfo(connection_res.get_player_info());
			}
		}

		if (rendering.scene_get() == librendering::rendering_manager::scene::connection) {
				if (g_tennis_client) {
					g_tennis_client->disconnectServer();
					delete g_tennis_client;
					g_tennis_client = nullptr;
				}
			}

		// get the name of the other player
		if (g_tennis_client && g_tennis_client->is_connected() && !rendering.player_2_is_set()) {
			rendering.paused_set(true);
			reconnect_tick_cnt++;
			if (reconnect_tick_cnt > RECONNECT_TICKS) {
				reconnect_tick_cnt = 0;
				std::string opp_name = g_tennis_client->getOpponentName();
				if (opp_name != "_undefined") {
					rendering.player_2_name_set(opp_name);
					rendering.paused_set(false);
				}
			}
		}


		if (!rendering.paused_get()) {
            // TODO: receive from server
            // get enemy's racket status
            // get ball status
			// RacketStatus(,Transform(vision.racket2table()),);
            rendering.frametime_serialize(tennis.frametime_deserialize());

			if (vision.run_tick()) {
				cerr << "Failed to run tick vision.\n";
				break;
			}

			// send racket status of current player to the server
			if (g_tennis_client && g_tennis_client->is_connected()) {
				RacketStatus rs(g_player_info->get_player_id(),
												  Transform(vision.racket2table()), Velocity());
				bool res = g_tennis_client->onVisionTickEnd(rs);
				if (!res) cerr << "Error sending racket status error.\n";
			}

            // vision serialize racket and table transformation for tennis and rendering
            vision.capture_serialize(rendering.capture_deserialize());
            vision.view_serialize(rendering.view_deserialize());
            vision.table_serialize(rendering.table_deserialize());
            vision.racket1_2cam_serialize(rendering.racket1_deserialize());
            // vision.racket1_serialize(rendering.racket1_deserialize());
            //vision.racket2_serialize(rendering.racket2_deserialize());

            vision.racket1_serialize(tennis.racket1_deserialize());
            vision.table_serialize(tennis.table_deserialize());

            if (tennis.run_tick()) {
                cerr << "Failed to run tick tennis.\n";
                break;
            }

			if (g_tennis_client && g_tennis_client->is_connected()) {
				BallStatus bs;
				ScoreBoard sb;
				bool is_turn_owner;
				tennis.simulation_serialize(bs, sb, is_turn_owner);
				// TODO when to change turn owner?
				// consider not only hit with racket
				if (bs.get_status() == FlyingStatus::HIT_WITH_RACKET) {
					g_tennis_client->changeTurn();
				}
				g_tennis_client->onTennisTickEnd(bs, sb, is_turn_owner);
			}

            tennis.ball_serialize(rendering.ball_deserialize());
            tennis.game_status_serialize(rendering.game_status_deserialize());
		}

        // TODO: only the turn owner send latest ball status to server
        // TODO: each user should send racket status
        if (g_tennis_client && g_tennis_client->is_connected()) {
            BallStatus bs;
            ScoreBoard sb;
            RacketStatus opponent_rs;
            unsigned isTurnOwner;
            g_tennis_client->RenderingTickBegin(g_tennis_client->get_player_id(), opponent_rs, bs, sb, isTurnOwner);
            rendering.racket2_deserialize()((float*) (opponent_rs.get_pose().get_value_ptr()));
            rendering.ball_deserialize()((float*) (bs.get_pose().get_value_ptr()));
            rendering.score1_deserialize()(sb.get_player_1_score());
            rendering.score2_deserialize()(sb.get_player_2_score());
        }
		if (rendering.run_tick()) {
			cerr << "Failed to run tick rendering.\n";
			break;
		}
        rendering.game_status_serialize(tennis.game_status_deserialize());
		curr_scene = rendering.scene_get();
	}

	if (rendering.term()) {
		cerr << "Failed to term rendering.\n";
		return 1;
	}

	if (vision.term()) {
		cerr << "Failed to term vision.\n";
		return 1;
	}

	if (g_tennis_client && g_tennis_client->is_connected()) {
		g_tennis_client->disconnectServer();
	}

	if (g_tennis_client != nullptr) delete g_tennis_client;
	if (g_player_info != nullptr) delete g_player_info;

	return 0;
}
