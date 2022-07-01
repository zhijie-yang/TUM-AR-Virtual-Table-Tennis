//
// Created by Ryou on 2022/7/1.
//

#include "libtennis/scoreboard.h"
using namespace libtennis;

scoreboard::scoreboard(int maximumGame, int maximumScore) {
    this->maximumScore = maximumScore;
    this->maximumGame = maximumGame;
}

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

void scoreboard::setCurrentGame(int game) {
    this->currentGame = game;
}

int scoreboard::getCurrentGame() {
    return this->currentGame;
}

bool scoreboard::checkGameEnd() {
    return this->player1Score >= this->maximumScore || this->player2Score >= this->maximumScore;
}

bool scoreboard::checkMatchEnd() {
    return this->currentGame > this->maximumGame;
}