/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/cursor.h"

namespace chessgame {

auto Cursor::play_move(const chesscore::Move &move) const -> Cursor {
    const auto node_ptr = std::shared_ptr<GameNode>(m_game->add_node(m_node.get(), move));
    return {m_game, node_ptr};
}

auto Cursor::add_variation(const chesscore::Move &move) const -> std::optional<Cursor> {
    const auto parent_cursor = parent();
    if (parent_cursor) {
        const auto parent_node = parent_cursor->m_node;
        const auto node_ptr = std::shared_ptr<GameNode>(m_game->add_node(parent_node.get(), move));
        return Cursor{m_game, node_ptr};
    }
    return {};
}

auto Cursor::set_comment(const std::string &comment) const -> void {
    m_node->set_comment(comment);
}

auto Cursor::parent() const -> std::optional<Cursor> {
    const auto parent_node = m_node->parent();
    if (parent_node) {
        return Cursor{m_game, parent_node};
    }
    return {};
}

auto Cursor::child(size_t index) const -> std::optional<Cursor> {
    const auto child_node = m_node->get_child(index);
    if (child_node) {
        return Cursor{m_game, child_node};
    }
    return {};
}

auto Cursor::position() const -> Position {
    return m_node->calculate_position();
}

} // namespace chessgame
