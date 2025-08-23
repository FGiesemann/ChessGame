/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/game.h"

namespace chessgame {

Game::Game(const GameMetadata &metadata) : m_metadata{metadata} {
    m_root = std::make_shared<GameNode>(NodeId{1});
    const auto fen_tag = std::ranges::find_if(metadata, [](const auto &tag) { return tag.name == "FEN"; });
    const auto initial_fen = fen_tag == metadata.end() ? chesscore::FenString::starting_position() : chesscore::FenString{fen_tag->value};
    m_root->set_position(Position{initial_fen});
}

Game::Game() : Game{GameMetadata{}} {}

auto Game::add_node(std::shared_ptr<GameNode> parent, const chesscore::Move &move) -> std::shared_ptr<GameNode> {
    auto child = std::make_shared<GameNode>(m_next_id++, move, parent);
    parent->append_child(child);
    return child;
}

} // namespace chessgame
