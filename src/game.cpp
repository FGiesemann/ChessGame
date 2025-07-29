/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chess_game/game.h"

namespace chessgame {

Game::Game(const chesscore::FenString &fen) {
    m_root.set_position(Position{fen});
}

Game::Game() : Game{chesscore::FenString::starting_position()} {}

auto Game::add_node(GameNode *parent, const chesscore::Move &move) -> GameNode * {
    auto child = std::make_shared<GameNode>(m_next_id++, move, std::shared_ptr<GameNode>(parent));
    parent->append_child(child);
    return child.get();
}

auto Game::play_move(const chesscore::Move &move) -> GameNode * {
    auto *new_node = add_node(m_current_node, move);
    m_current_node = new_node;
    m_metadata.ply_count++;
    return new_node;
}

} // namespace chessgame
