/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chess_game/game_tree.h"
#include "chess_game/game_types.h"

namespace chessgame {

const NodeId NodeId::Invalid{0};

auto GameNode::calculate_position() const -> Position {
    if (m_position.has_value()) {
        return *m_position;
    }

    if (const auto parent = m_parent.lock(); parent) {
        auto position = parent->calculate_position();
        position.make_move(m_move);
        return position;
    }
    throw ChessGameError{"No ancestor with position information found"};
}

} // namespace chessgame
