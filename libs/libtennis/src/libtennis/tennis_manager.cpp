//
// Created by Ryou on 2022/6/29.
//

#include "libtennis/tennis_manager.h"

using namespace libtennis;

void tennis_manager::setAttacker(int attacker) {
    this->attacker = attacker;
}

int tennis_manager::getAttacker() {
    return this->attacker;
}

void tennis_manager::setNumHitTable(int numHitTable) {
    this->numHitTable = numHitTable;
}

int tennis_manager::getNumHitTable() {
    return numHitTable;
}

void tennis_manager::setTurnOwner(int turnOwner) {
    this->turnOwner = turnOwner;
}

int tennis_manager::getTurnOwner() {
    return this->getTurnOwner();
}

tennis_manager::tennis_manager(scoreboard board){
    ball tennis_ball;
    tennis_ball.set_position(this->init_ball_pos);
    tennis_ball.set_velocity(glm::vec3(0, 0, 0));

    this->attacker = 0; // host serve first
    this->numHitTable = 0;
    this->board = board;
}

glm::vec3 tennis_manager::externalForcesCalculation() {
    glm::vec3 force = glm::vec3(0, 0, 0);
    if (this->isServed) {
        // Apply gravity
        force += glm::vec3(0, -9.8 * this->tennis_ball.get_mass(), 0); // TODO: meters as unit?
        // Apply air resistance
        if (this->airResistance) {
            glm::vec3 velocity = this->tennis_ball.get_velocity();
            force -= glm::normalize(velocity) * static_cast<float >(pow(glm::length(velocity), 2));
        }
    }
    return force;
}

void tennis_manager::endCurrentTurn(bool win) {
    this->isServed = false; // reset serving status
    this->setTurnOwner(1 - this->getTurnOwner()); // change who should serve
    this->setNumHitTable(0);
    if ((this->getAttacker() == 0 && win) || (this->getAttacker() == 1 && !win)) {
        this->board.setPlayer1Score(this->board.getPlayer1Score() + 1);
    } else {
        this->board.setPlayer2Score(this->board.getPlayer2Score() + 1);
    }

    bool gameEnd = this->board.checkGameEnd();
    if (gameEnd) {
        // reset score
        this->board.setPlayer1Score(0);
        this->board.setPlayer2Score(0);
        this->board.setCurrentGame(this->board.getCurrentGame()+1);

        bool matchEnd = this->board.checkMatchEnd();
        if (matchEnd) {
            // TODO: signal to show a match end
        } else {
            // TODO: signal to show a game end
        }
    } else {
        // TODO: signal to show a turn end
    }
}

void tennis_manager::simulate(float timeStep) {
    if (!this->isServed) {
        // To make it easy, before serving, let the ball fix at the position
        tennis_ball.set_position(this->init_ball_pos);
        tennis_ball.set_velocity(glm::vec3(0, 0, 0));
    }

    // TODO: get transform of rackets, position and size of table and net, depends on libvision
    glm::mat4 racketA2World; // racket A belongs to the host
    glm::mat4 racketB2World;
    glm::mat4 table2World;
    glm::mat4 net2World;

    // compute external forces and accelerations for ball
    glm::vec3 force = this->externalForcesCalculation();

    // leap frog integration to update position and velocity of ball
    glm::vec3 acc = force / this->tennis_ball.get_mass();
    this->tennis_ball.set_velocity(this->tennis_ball.get_velocity() + timeStep * acc);
    this->tennis_ball.set_position(this->tennis_ball.get_position() + timeStep * this->tennis_ball.get_velocity());

    glm::mat4 ball2World = this->tennis_ball.get_model();
    CollisionInfo ball_coll_racketA = checkCollisionSAT(ball2World, racketA2World);
    CollisionInfo ball_coll_racketB = checkCollisionSAT(ball2World, racketB2World);
    CollisionInfo ball_coll_table = checkCollisionSAT(ball2World, table2World);
    CollisionInfo ball_coll_net = checkCollisionSAT(ball2World, net2World);
    if (ball_coll_racketA.isValid || ball_coll_racketB.isValid) {
        int player = ball_coll_racketA.isValid? 0: 1;
        this->setAttacker(player); // change current attacker
        CollisionInfo validColl = ball_coll_racketA.isValid? ball_coll_racketA: ball_coll_racketB;
        // is ball collide with racket?
        if (!this->isServed && (player == turnOwner)) {
            // serving
            this->isServed = true;
        }
        // to make it easy, apply a certain velocity to ball on the collision normal direction
        glm::vec3 direction = validColl.normalWorld;
        this->tennis_ball.set_velocity(direction * 10.0f);
    } else if (ball_coll_table.isValid) {
        int numHitTable = this->getNumHitTable();
        glm::vec3 collisionPoint = ball_coll_table.collisionPointWorld;
        if (numHitTable == 0) {
            // TODO: first hit should on the current player's own side, the side definition depends on libvision
            this->setNumHitTable(numHitTable+1);
        } else if (numHitTable == 1) {
            // TODO: second hit should on the enemy's side
            this->setNumHitTable(numHitTable+1);
        } else {
            // TODO: if third hit is on the enemy's side, win
            this->endCurrentTurn(true);
        }
    } else if (ball_coll_net.isValid) {
        // to make it easy, if ball collide with net, fail
        this->endCurrentTurn(false);
    } else {
        // is ball flying out of boundary?
        float pos_x = this->tennis_ball.get_position()[0];
        // TODO: to get the boundary
        if (pos_x > 100 || pos_x < -100) {
            this->endCurrentTurn(false);
        }
    }
}