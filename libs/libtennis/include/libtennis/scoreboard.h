//
// Created by Ryou on 2022/7/1.
//

#ifndef MODERN_CMAKE_TEMPLATE_SCOREBOARD_H
#define MODERN_CMAKE_TEMPLATE_SCOREBOARD_H

namespace libtennis {
    class scoreboard {
    private:
        int player1Score = 0;
        int player2Score = 0;
        int maximumScore = 11;

    public:
        scoreboard() = default;

        void setPlayer1Score(int score);
        int getPlayer1Score();

        void setPlayer2Score(int score);
        int getPlayer2Score();

        void setMaximumScore(int maximumScore);
        int getMaximumScore();

        bool checkGameEnd();
    };
}


#endif //MODERN_CMAKE_TEMPLATE_SCOREBOARD_H
