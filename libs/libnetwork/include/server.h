#ifndef LIBNETWORK_SERVER_H
#define LIBNETWORK_SERVER_H

#include "libframework/include/ball_status.h"
#include "libframework/include/racket_status.h"
#include "libframework/include/infos.h"
#include <utility>

template<class T>
T get_pair_elm(const std::pair<T,T> &p, int const& pos) {
    assert((pos == 0 || pos == 1) && "Invalid pos");
    if (pos == 1) { //pos==1
        return p.second;
    } else {
        return p.first;
    }
}

template<class T>
void set_pair_elm(std::pair<T,T> &p, int const& pos, T const& v) {
    assert((pos == 0 || pos == 1) && "Invalid pos");
    if (pos == 1) {
        p.second = v;
    } else if (pos == 0) {
        p.first = v;
    }
}


class TennisServerManager {
public:
    ~TennisServerManager();

private:
    static TennisServerManager* instance;
    TennisServerManager(){}

    std::pair<bool, bool> client_inited = {false, false};
    std::pair<std::string, std::string> client_ip;
    // std::pair<unsigned, unsigned> client_port;
    std::pair<unsigned, unsigned> client_id;
    std::pair<std::string, std::string> client_name;
    unsigned next_player_id = 1; // 0 for reg_player error

    // game logics
    bool turn_owner = false; // 0/false for player A, 1/true for player B
    BallStatus ball_status;
    std::pair<RacketStatus, RacketStatus> racket_status;
    ScoreBoard score_board = ScoreBoard(0, 0, 11);
    bool game_running = false;

public:
    inline static TennisServerManager* get_instance();

    // getters
    inline std::pair<std::string, std::string> get_client_ip() {return this->client_ip;}
    // inline std::pair<unsigned, unsigned> getClientPort() {return this->client_port;}
    inline std::pair<unsigned, unsigned> get_client_id() {return this->client_id;}
    inline std::pair<std::string, std::string> get_client_name() {return this->client_name;}
    inline unsigned get_turn_owner() {
        return (!this->turn_owner) ? this->client_id.first : this->client_id.second;
        }
    inline BallStatus get_ball_status() {return this->ball_status;}
    // always get racket status of the other player since it's unknown
    inline RacketStatus get_racket_status(int pos) {return get_pair_elm(this->racket_status, !pos);}
    inline ScoreBoard get_score_board() {return this->score_board;}
    inline std::pair<bool, bool> get_client_inited() {return this->client_inited;}
    inline int get_player_pos(unsigned const& player_id) {
        if (player_id == get_pair_elm(this->client_id, 0)) {
            return 0;
        } else if (player_id == get_pair_elm(this->client_id, 1)) {
            return 1;
        } else {
            return -1;
        }
    }

    // setters
    inline void set_ball_status(BallStatus const& ball_status) {
        this->ball_status = ball_status;
    }
    inline void set_racket_status(RacketStatus const &racket_status, int const& pos) {
        set_pair_elm(this->racket_status, pos, racket_status);
    }
    inline void set_score_board(ScoreBoard const &score_board) {
        this->score_board = score_board;
    }

    // game locigs
    bool start_game() {
        if (this->game_running || !this->client_inited.first || !this->client_inited.second) {return false;}
        this->game_running = true;
        return true;
    }
    unsigned reg_player(std::string client_ip, unsigned client_port,
                std::string client_player_name);
    // getters
    inline void change_turn() {this->turn_owner = !this->turn_owner;}
    inline void dereg(bool pos) {if (!pos) this->client_inited.first = false; else this->client_inited.second = false;}
    inline void dereg() {this->client_inited = {false, false};}
};

void RunServer(std::string server_address);

#endif // !LIBNETWORK_SERVER_H
