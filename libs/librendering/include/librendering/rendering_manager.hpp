#ifndef LIBRENDERING_MANAGER
#define LIBRENDERING_MANAGER

#include "librendering/rendering_settings.hpp"

#include <memory>
#include <functional>

#include <glm/glm.hpp>

namespace librendering {
	/**
	* The main class responsible for rendering and communication with other libraries.
	*/
	class rendering_manager {
	public:
		class impl;
		/**
		* Scene type for proper UI display.
		*/
		enum scene {
			main_menu, /**< Main menu where you can exit or start the play session */
			connection, /**< Play session setup screen, where you can join a server */
			level, /**< Level screen, where you actually play the game */
			ending, /**< Ending screen, showing the final game score */
			waiting_p2,
		};

		rendering_manager();
		~rendering_manager();

		/**
		* Get if the app should quit.
		*/
		bool quit_get() const;
		/**
		* Tell the app to quit ASAP.
		*/
		void quit_set();

		/**
		* Get if the game should be paused.
		*/
		bool paused_get() const;
		/**
		* Set if the game should be paused.
		* 
		* @param value New paused value.
		*/
		void paused_set(bool value);

		/**
		* Get current UI scene.
		*/
		scene scene_get() const;
		/**
		* Set current UI scene.
		* 
		* @param value New scene value.
		*/
		void scene_set(scene value);

		/**
		* Get a function which deserializes a captured video frame.
		* Accepts frame buffer, width, height, channels.
		*/
		char* input_ip_get() const;
		int input_port_get() const;
		bool ready_to_register_get() const;
		char* input_player_name_get() const;
		void player_2_name_set(std::string const& name);
		bool player_2_is_set() const;

		std::function<int(void*, int, int, int)> capture_deserialize();

		/**
		* Get a function which deserializes the camera projection matrix.
		* Accepts matrix array pointer.
		*/
		std::function<int(float*)> proj_deserialize();
		/**
		* Get a function which deserializes the camera view matrix.
		* Accepts matrix array pointer.
		*/
		std::function<int(float*)> view_deserialize();
		/**
		* Get a function which deserializes the racket transformation matrix of player 1.
		* Accepts matrix array pointer.
		*/
        std::function<int(float*)> racket1_deserialize();
		/**
		* Get a function which deserializes the racket transformation matrix of player 2.
		* Accepts matrix array pointer.
		*/
        std::function<int(float*)> racket2_deserialize();
		/**
		* Get a function which deserializes the ball transformation matrix.
		* Accepts matrix array pointer.
		*/
        std::function<int(float*)> ball_deserialize(bool isTurnOwner);
		/**
		* Get a function which deserializes the table scale/transformation matrix.
		* Accepts matrix array pointer.
		*/
        std::function<int(float*)> table_deserialize();

		/**
		* Get a function which deserializes the player 1 current in-game score.
		* Accepts new in-game score.
		*/
		std::function<int(const int&)> score1_deserialize();
		/**
		* Get a function which deserializes the player 2 current in-game score.
		* Accepts new in-game score.
		*/
		std::function<int(const int&)> score2_deserialize();
		/**
		* Get a function which deserializes the player 1 won matches score.
		* Accepts new match score.
		*/
		std::function<int(const int&)> match1_deserialize();
		/**
		* Get a function which deserializes the player 2 won matches score.
		* Accepts new match score.
		*/
		std::function<int(const int&)> match2_deserialize();
		/**
		* Get a function which deserializes the player 1 name.
		* Accepts new name.
		*/
		std::function<int(const char*)> player1_deserialize();
		/**
		* Get a function which deserializes the player 2 name.
		* Accepts new name.
		*/
		std::function<int(const char*)> player2_deserialize();
		/**
		* Get a function which deserializes the game status.
		* Accepts new game status.
		*/
        std::function<int(const bool&)> game_status_deserialize();

		/**
		* Serializes current frame time (time between frames / time of executing one frame update for all libraries).
		* 
		* @param processor Deserialization function for current frame time.
		*/
		int frametime_serialize(const std::function<int(float)>& processor);
		/**
		* Serializes current game status.
		* 
		* @param processor Deserialization function for current game status.
		*/
        int game_status_serialize(const std::function<int(bool)>& processor);

		/**
		* Serializes current frame time (time between frames / time of executing one frame update for all libraries).
		* 
		* @param settings rendering settings, like width, height, startup UI scene, etc.
		*/
		int init(const rendering_settings& settings);
		/**
		* Run the rendering update. This renders a single frame on GPU, updates UI and handles events.
		*/
		int run_tick();
		/**
		* Terminate the rendering manager. Clears up all allocated data.
		*/
		int term();
	private:
		std::unique_ptr<impl> _impl;
	};
}

#endif // !LIBRENDERING_MANAGER
