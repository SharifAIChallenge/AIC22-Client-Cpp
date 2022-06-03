#ifndef HIDEANDSEEK_AI_H
#define HIDEANDSEEK_AI_H

#include "types.h"

namespace AI {
    using namespace Types;

    int get_thief_starting_node(const GameView &gameView); // returns node_id

    int thief_move_ai(const GameView &gameView); // returns to_node_id
    int police_move_ai(const GameView &gameView); // returns to_node_id

    std::string thief_chat_ai(const GameView &gameView);
    std::string police_chat_ai(const GameView &gameView);
}

#endif
