//
// Created by Ryou on 2022/6/29.
//

#ifndef TENNIS_MANAGER
#define TENNIS_MANAGER

#include <glm/gtx/transform.hpp>
#include "collisionDetect.h"
#include "libframework/include/ball_status.h"
#include "libframework/include/infos.h"

namespace libtennis {
    class tennis_manager {
    public:
        class impl;

        tennis_manager(int maximumScore);
        ~tennis_manager();

        int run_tick();
        std::function<int(const float&)> frametime_deserialize();
        std::function<int(float*)> racket1_deserialize();
        std::function<int(float*)> table_deserialize();
        std::function<int(const bool&)> game_status_deserialize();
        int ball_serialize(const std::function<int(float*)>& processor);
        int game_status_serialize(const std::function<int(bool)>& processor);

        void simulation_serialize(BallStatus &ball, ScoreBoard &board, bool &isTurnOwner);
        void turnOwner_deserialize(bool isTurnOwner);

    private:
        std::unique_ptr<impl> _impl;
    };
}


#endif //MODERN_CMAKE_TEMPLATE_TENNIS_MANAGER_H
