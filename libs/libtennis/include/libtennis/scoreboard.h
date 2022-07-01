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
        int maximumGame = 1;
        int maximumScore = 11;

        int currentGame = 1;
    public:
        scoreboard() = default;
        scoreboard(int maximumGame, int maximumScore);

        void setPlayer1Score(int score);
        int getPlayer1Score();

        void setPlayer2Score(int score);
        int getPlayer2Score();

        void setCurrentGame(int game);
        int getCurrentGame();

        bool checkGameEnd();
        bool checkMatchEnd();
    };
}


#endif //MODERN_CMAKE_TEMPLATE_SCOREBOARD_H
