/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/game.h"

namespace chessgame {

Game::Game(const chesscore::FenString &fen) {
    m_root = std::make_shared<GameNode>(NodeId{1});
    m_root->set_position(Position{fen});
}

Game::Game() : Game{chesscore::FenString::starting_position()} {}

auto Game::add_node(std::shared_ptr<GameNode> parent, const chesscore::Move &move) -> std::shared_ptr<GameNode> {
    auto child = std::make_shared<GameNode>(m_next_id++, move, parent);
    parent->append_child(child);
    return child;
}

auto Game::set_tag([[maybe_unused]] const std::string &tag_name, [[maybe_unused]] const std::string &tag_value) -> void {
    // TODO
}

} // namespace chessgame
