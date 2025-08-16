/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_H
#define CHESSGAME_GAME_H

#include "chessgame/cursor.h"
#include "chessgame/metadata.h"
#include "chessgame/tree.h"

#include "chesscore/fen.h"

namespace chessgame {

/**
 * \brief A game of chess.
 *
 * Represents a game of chess. Stores meta information about the game, like the
 * names of the players, the event, etc.
 * Manages the list of moves of the game. Moves can be annotated and the class
 * can also store variations of the game (alternative sequences of moves).
 */
class Game {
public:
    /**
     * \brief Create a new Game.
     *
     * The new game starts with the default starting position.
     */
    Game();

    Game(const GameMetadata &metadata);

    /**
     * \brief Read-only access to the meta data of the game.
     *
     * \return The game meta data.
     */
    [[nodiscard]] auto metadata() const -> const GameMetadata & { return m_metadata; }

    /**
     * \brief Access to the meta data of the game.
     *
     * \return The game meta data.
     */
    auto metadata() -> GameMetadata & { return m_metadata; }

    /**
     * \brief Add a new node to the game tree.
     *
     * The node is appended as a new child to the given parent node.
     * \param parent The parent node of the new node.
     * \param move The move that leads from the parent to the new node.
     * \return The new node.
     */
    auto add_node(std::shared_ptr<GameNode> parent, const chesscore::Move &move) -> std::shared_ptr<GameNode>;

    auto edit() -> Cursor { return {this, m_root}; }
    auto cursor() -> Cursor { return edit(); }
    [[nodiscard]] auto cursor() const -> ConstCursor { return {this, m_root}; }
    [[nodiscard]] auto const_cursor() const -> ConstCursor { return {this, m_root}; }
private:
    GameMetadata m_metadata{};        ///< Meta data for the game.
    std::shared_ptr<GameNode> m_root; ///< Root node of the game tree.
    NodeId m_next_id{2U};             ///< Next available node id.
};

} // namespace chessgame

#endif
