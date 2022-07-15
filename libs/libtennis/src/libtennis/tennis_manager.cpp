//
// Created by Ryou on 2022/6/29.
//

#include <functional>
#include <memory>
#include "libtennis/tennis_manager.h"
#include <iostream>
#include "libframework/include/constant.h"
#include "libframework/include/ball_status.h"

using namespace libtennis;

static tennis_manager::impl* _instance;

class tennis_manager::impl {
private:
    BallStatus _ball = BallStatus(0, Transform(glm::mat4(1.0f)),
                                  Velocity(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)), FlyingStatus::WAITING_START);
    const glm::vec3 _player1_init_ball_pos = glm::vec3(0.0f, 0.05f, CONST_TABLE_SCALE.z/3);
    const glm::vec3 _player2_init_ball_pos = glm::vec3(0, 0.2f, -1.37f);

    int _server; // the one who should serve
    int _turnOwner; // 0 or 1, 0 is the host, once the ball is hit by the racket, the one who hits the ball becomes the attacker
    int _numHitTable; // 0 to 3

    // deserialization
    glm::mat4 _racket1Model;
    glm::mat4 _racket2Model;
    glm::mat4 _tableModel;
    glm::mat4 _netModel;
    float _timestep;

    bool _isServed = false;
    bool _isServingTurn = false;
    bool _gravity = false;
    bool _airResistance= false;
    bool _gameEnd = false;

    scoreboard _board;

    glm::vec3 _externalForcesCalculation() {
        glm::vec3 force = glm::vec3(0, 0, 0);
        if (_isServed) {
            // Apply gravity
            if (_gravity) {
                force += glm::vec3(0, -9.8 * CONST_BALL_MASS, 0);
            }
            // Apply air resistance
            if (_airResistance) {
                glm::vec3 velocity = _ball.get_velocity();
                force -= glm::normalize(velocity) * static_cast<float >(pow(glm::length(velocity), 2));
            }
        }
        return force;
    }

    void _endCurrentTurn(bool win) {
        _isServed = false; // reset serving status
        _isServingTurn = false;
        _numHitTable = 0;

        _server = 1 - _server;

        _ball.set_velocity(glm::vec3(0, 0, 0));
        //TODO: change init position by server
        _ball.set_position(_player1_init_ball_pos);

        if ((_turnOwner == 0 && win) || (_turnOwner == 1 && !win)) {
            _board.setPlayer1Score(_board.getPlayer1Score() + 1);
        } else {
            _board.setPlayer2Score(_board.getPlayer2Score() + 1);
        }

        bool gameEnd = _board.checkGameEnd();
        if (gameEnd) {
            // reset score
            _board.setPlayer1Score(0);
            _board.setPlayer2Score(0);

            // TODO: redirect to main scene
            _gameEnd = true;
        } else {
            return;
        }
    }

    glm::mat4 _obj2World(glm::mat4 model, int objClass) {
        glm::vec3 table_scale = CONST_TABLE_SCALE;
        glm::vec3 ball_scale = CONST_BALL_SCALE;
        switch (objClass) {
            case 0: // racket
                model = model * glm::scale(CONST_RACKET_SCALE);
                break;
            case 1: // ball
                model *= glm::scale(CONST_BALL_SCALE);
                break;
            case 2: // table
                table_scale.y = 0.01f;
                model *= glm::scale(table_scale);
                break;
            case 3: // net
                table_scale.y *= 2;
                table_scale.z = 0.01f;
                model = model * glm::scale(table_scale);
                break;
            default:
                break;
        }
        // column major to row major
        return glm::transpose(model);
        //return model;
    }

public:
    impl(int maximumScore) :
            _server{0}, _turnOwner{0}, _numHitTable{0}
    {
        scoreboard board;
        BallStatus ball = BallStatus(0, Transform(glm::mat4(1.0f)),
                                     Velocity(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)), FlyingStatus::WAITING_START);
        ball.set_position(_player1_init_ball_pos);

        board.setMaximumScore(maximumScore);

        _ball = ball;
        _board = board;
        _instance = this;
    }
    ~impl() = default;

    int run_tick() {
        float timestep = _timestep / 4;
        for (int i = 0; i < 4; i++) {
            if (!_isServed) {
                //TODO: change init position by server
                _ball.set_position(_player1_init_ball_pos);
                _ball.set_velocity(glm::vec3(0, 0, 0));
            }

            glm::mat4 racketA2World = _obj2World(_racket1Model, 0);
            glm::mat4 racketB2World = _obj2World(_racket2Model, 0);
            glm::mat4 table2World = _obj2World(_tableModel, 2);
            glm::mat4 net2World = _obj2World(_netModel, 3);

            // compute external forces and accelerations for ball
            glm::vec3 force = _externalForcesCalculation();

            // leap frog integration to update position and velocity of ball
            glm::vec3 acc = force / float(CONST_BALL_MASS);
            _ball.set_velocity(_ball.get_velocity() + timestep * acc);
            _ball.set_position(_ball.get_position() + timestep * _ball.get_velocity());

            glm::mat4 ball2World = _obj2World(_ball.get_pose(), 1);
            CollisionInfo ball_coll_racketA = checkCollisionSAT(ball2World, racketA2World);
            CollisionInfo ball_coll_racketB = checkCollisionSAT(ball2World, racketB2World);
            CollisionInfo ball_coll_table = checkCollisionSAT(ball2World, table2World);
            CollisionInfo ball_coll_net = checkCollisionSAT(ball2World, net2World);
            if (ball_coll_racketA.isValid || ball_coll_racketB.isValid) {
                std::cout << "Hit racket!" << std::endl;
                int player = ball_coll_racketA.isValid? 0: 1;
                _turnOwner = player; // change current turn owner
                CollisionInfo validColl = ball_coll_racketA.isValid? ball_coll_racketA: ball_coll_racketB;

                if (!_isServed && (player == _server)) {
                    // serving
                    _isServed = true;
                    _isServingTurn = true;
                    _gravity = true;
                }
                // TODO: racket ball collision handling
                _ball.set_status(FlyingStatus::HIT_WITH_RACKET);
                glm::vec3 direction = validColl.normalWorld;
                _ball.set_velocity(glm::vec3(direction.x, 2, -0.5));
            } else if (ball_coll_table.isValid) {
                std::cout<< "Hit table" << std::endl;
                glm::vec3 collisionPoint = ball_coll_table.collisionPointWorld;
                _ball.set_status(FlyingStatus::HIT_WITH_TABLE);
                if (_numHitTable == 1) {
                    if (_isServingTurn) {
                        // Case 1: is serving turn, the second hit should be on the enemy's side. Player 1's z > 0, and player2's z < 0
                        if ((_turnOwner == 0 && collisionPoint.z < 0) || (_turnOwner == 1 && collisionPoint.z > 0)) {
                            glm::vec3 v = _ball.get_velocity();
                            _ball.set_velocity(glm::vec3(v.x, -v.y, v.z));
                        } else {
                            // If not on the enemy's side, then lose
                            _endCurrentTurn(false);
                        }
                    } else {
                        // Case 2: is not serving turn, win
                        _endCurrentTurn(true);
                    }

                } else if (_numHitTable == 0) {
                    // Case 1: is serving turn, the first hit should be on the turn owner's side, or lose
                    // Case 2: is not serving turn, the first hit should be on the enemy's side, or lose
                    if ((_isServingTurn && ((_turnOwner == 0 && collisionPoint.z < 0) || (_turnOwner == 1 && collisionPoint.z > 0)))
                        || (!_isServingTurn && ((_turnOwner == 0 && collisionPoint.z > 0) || (_turnOwner == 1 && collisionPoint.z < 0)))) {
                        _endCurrentTurn(false);
                    } else {
                        glm::vec3 v = _ball.get_velocity();
                        _ball.set_velocity(glm::vec3(v.x, -v.y, v.z));
                    }
                } else {
                    _endCurrentTurn(true);
                }
                _numHitTable++;
            } else if (ball_coll_net.isValid) {
                // to make it easy, if ball collide with net, fail
                std::cout<< "Hit net" << std::endl ;
                _ball.set_status(FlyingStatus::HIT_WITH_NET);
                _endCurrentTurn(false);
            } else {
                // is ball flying out of boundary?
                glm::vec3 table_scale = CONST_TABLE_SCALE;
                float offset = 0.5;
                glm::vec3 pos = _ball.get_position();
                if ( (pos.x > table_scale.x / 2 + offset || pos.x < - (table_scale.x / 2 + offset)) ||
                     (pos.z > table_scale.z / 2 + offset || pos.z < - (table_scale.z / 2 + offset)) ||
                     pos.y < -offset) {
                    // fly out of the boundary
                    std::cout<< "Out of boundry" << std::endl;
                    _ball.set_status(FlyingStatus::OUT_OF_BOUND);
                    if (_numHitTable == 0 || (_numHitTable == 1 && _isServingTurn)) {
                        _endCurrentTurn(false);
                    } else {
                        _endCurrentTurn(true);
                    }
                }
            }
        }

        return 0;
    }

    std::function<int(float*)> racket2_deserialize()
    {
        return [&](float* data) -> int
        {
            _racket2Model = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));
            return 0;
        };
    }

    std::function<int(float*)> racket1_deserialize()
    {
        return [&](float* data) -> int
        {
            _racket1Model = glm::mat4(
                    glm::vec4(data[0], data[1], data[2], data[3]),
                    glm::vec4(data[4], data[5], data[6], data[7]),
                    glm::vec4(data[8], data[9], data[10], data[11]),
                    glm::vec4(data[12], data[13], data[14], data[15]));

             _racket1Model = glm::mat4(glm::vec4(1, 0, 0, 0),glm::vec4(0, 0, 1, 0),glm::vec4(0, -1, 0, 0),glm::vec4(0, 0, 0, 1))
                                  * _racket1Model;
            return 0;
        };
    }

    std::function<int(float*)> table_deserialize()
    {
        return [&](float* data) -> int
        {
            _tableModel = _netModel = glm::mat4(1.0f);
            return 0;
        };
    }

    std::function<int(const float&)> frametime_deserialize()
    {
        return [&](const float& value) -> int
        {
            _timestep = value;
            return 0;
        };
    }

    std::function<int(const bool&)> game_status_deserialize()
    {
        return [&](const float& value) -> int
        {
            _gameEnd = value;
            return 0;
        };
    }

    int ball_serialize(const std::function<int(float*)>& processor)
    {
        glm::mat4 model = _ball.get_pose();
        float arr_model[16] = {
                model[0][0], model[0][1], model[0][2], model[0][3],
                model[1][0], model[1][1], model[1][2], model[1][3],
                model[2][0], model[2][1], model[2][2], model[2][3],
                model[3][0], model[3][1], model[3][2], model[3][3]};
        return processor(arr_model);
    }

    int score1_serialize(const std::function<int(int)>& processor) {
        return processor(_board.getPlayer1Score());
    }

    int score2_serialize(const std::function<int(int)>& processor) {
        return processor(_board.getPlayer2Score());
    }

    int game_status_serialize(const std::function<int(bool)>& processor) {
        return processor(_gameEnd);
    }
};

tennis_manager::tennis_manager(int maximumScore)
        : _impl{ std::make_unique<impl>(maximumScore) }
{}

tennis_manager::tennis_manager::~tennis_manager()
{}

int tennis_manager::run_tick()
{
    return _impl->run_tick();
}

std::function<int(float*)> tennis_manager::racket1_deserialize()
{
    return _impl->racket1_deserialize();
}

std::function<int(float*)> tennis_manager::racket2_deserialize()
{
    return _impl->racket2_deserialize();
}

std::function<int(float*)> tennis_manager::table_deserialize()
{
    return _impl->table_deserialize();
}

std::function<int(const float &)> tennis_manager::frametime_deserialize()
{
    return _impl->frametime_deserialize();
}

std::function<int(const bool&)> tennis_manager::game_status_deserialize()
{
    return _impl->game_status_deserialize();
}

int tennis_manager::ball_serialize(const std::function<int(float*)>& processor) {
    return _impl->ball_serialize(processor);
}

int tennis_manager::score1_serialize(const std::function<int(int)>& processor) {
    return _impl->score1_serialize(processor);
}

int tennis_manager::score2_serialize(const std::function<int(int)>& processor) {
    return _impl->score2_serialize(processor);
}

int tennis_manager::game_status_serialize(const std::function<int(bool)>& processor) {
    return _impl->game_status_serialize(processor);
}