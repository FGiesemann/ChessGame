/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/game.h"

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

} // namespace chessgame
