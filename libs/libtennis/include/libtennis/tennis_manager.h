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
    private:
        ball tennis_ball;
        const glm::vec3 init_ball_pos = glm::vec3(0, 0, 0);

        int attacker; // 0 or 1, 0 is the host, once the ball is hit by the racket, the one who hits the ball becomes the attacker
        int numHitTable; // 0 to 3
        int turnOwner;

        bool isServed = false;
        bool airResistance=true;

        scoreboard board;

    public:
        tennis_manager(scoreboard board);

        glm::vec3 externalForcesCalculation();

        void setAttacker(int attacker);
        int getAttacker();

        void setTurnOwner(int turnOwner);
        int getTurnOwner();

        void setNumHitTable(int numHitTable);
        int getNumHitTable();

        void endCurrentTurn(bool win);

        void simulate(float timeStep);
    };
}


#endif //MODERN_CMAKE_TEMPLATE_TENNIS_MANAGER_H
