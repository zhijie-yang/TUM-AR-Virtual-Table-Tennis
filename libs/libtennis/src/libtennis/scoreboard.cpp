//
// Created by Ryou on 2022/7/1.
//

#include "libtennis/scoreboard.h"
using namespace libtennis;

void scoreboard::setPlayer1Score(int score) {
    this->player1Score = score;
}

int scoreboard::getPlayer1Score() {
    return this->player1Score;
}

void scoreboard::setPlayer2Score(int score) {
    this->player2Score = score;
}

int scoreboard::getPlayer2Score() {
    return this->player2Score;
}

void scoreboard::setMaximumScore(int maximumScore) {
    this->maximumScore = maximumScore;
}

int scoreboard::getMaximumScore() {
    return this->maximumScore;
}

bool scoreboard::checkGameEnd() {
    return this->player1Score >= this->maximumScore || this->player2Score >= this->maximumScore;
}