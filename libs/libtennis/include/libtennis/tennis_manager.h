//
// Created by Ryou on 2022/6/29.
//

#ifndef TENNIS_MANAGER
#define TENNIS_MANAGER

#include "ball.hpp"
#include <glm/gtx/transform.hpp>
#include "collisionDetect.h"
#include "scoreboard.h"

namespace libtennis {
    class tennis_manager {
    public:
        class impl;

        tennis_manager(int maximumScore);
        ~tennis_manager();

        int run_tick();
        std::function<int(const float&)> frametime_deserialize();
        std::function<int(float*)> racket1_deserialize();
        std::function<int(float*)> racket2_deserialize();
        std::function<int(float*)> table_deserialize();
        int ball_serialize(const std::function<int(float*)>& processor);
        int score1_serialize(const std::function<int(int)>& processor);
        int score2_serialize(const std::function<int(int)>& processor);

    private:
        std::unique_ptr<impl> _impl;
    };
}


#endif //MODERN_CMAKE_TEMPLATE_TENNIS_MANAGER_H
