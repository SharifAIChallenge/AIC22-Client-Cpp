#include "ai.h"

using namespace std;

namespace AI {

    int get_thief_starting_node(const GameView &gameView) {
        return 2;
    }

    int thief_move_ai(const GameView &gameView) {
        return 2;
    }

    int police_move_ai(const GameView &gameView) {
        return 1;
    }

    std::string thief_chat_ai(const GameView &gameView) {
        return "Message From Thief!";
    }

    std::string police_chat_ai(const GameView &gameView) {
        return "Message From Police!";
    }
}