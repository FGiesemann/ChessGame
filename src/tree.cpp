/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/tree.h"
#include "chessgame/types.h"

namespace chessgame {

const NodeId NodeId::Invalid{0};

auto GameNode::append_child(const std::shared_ptr<GameNode> &child) -> std::shared_ptr<GameNode> {
    const auto iterator = std::ranges::find_if(m_children, [&](const auto &node) { return node->move() == child->move(); });
    if (iterator == m_children.end()) {
        m_children.push_back(child);
        return child;
    }
    return *iterator;
}

auto GameNode::calculate_position() const -> chesscore::Position {
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
