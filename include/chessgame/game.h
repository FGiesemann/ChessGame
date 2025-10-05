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
     * The node is appended as a new child to the given parent node. If the
     * parent node already conatins a child with the given move, no new node is
     * added and that child is returned.
     * \param parent The parent node of the new node.
     * \param move The move that leads from the parent to the new node.
     * \return The new node.
     */
    auto add_node(const std::shared_ptr<GameNode> &parent, const chesscore::Move &move) -> std::shared_ptr<GameNode>;

    /**
     * \brief Get a cursor to the beginning of the game.
     *
     * \return Cursor to the beginning of the game.
     */
    auto edit() -> Cursor { return {this, m_root}; }

    /**
     * \brief Get a const cursor to the beginning of the game.
     *
     * This is an alias for Game::edit().
     * \return Cursor to the beginning of the game.
     */
    auto cursor() -> Cursor { return edit(); }

    /**
     * \brief Get a read-only cursor to the beginning of the game.
     *
     * \return Read-only cursor to the beginning of the game.
     */
    [[nodiscard]] auto cursor() const -> ConstCursor { return {this, m_root}; }

    /**
     * \brief Get a read-only cursor to the beginning of the game.
     *
     * \return Read-only cursor to the beginning of the game.
     */

    [[nodiscard]] auto const_cursor() const -> ConstCursor { return {this, m_root}; }

    /**
     * \brief Get a cursor to the current position on the main line.
     *
     * Return a cursor to the last position on the main line of the game.
     * \return Cursor to the current position on the main line.
     */
    auto current_mainline() -> Cursor { return follow_mainline<Cursor>(edit()); }

    /**
     * \brief Get a cursor to the current position on the main line.
     *
     * Return a cursor to the last position on the main line of the game.
     * \return Cursor to the current position on the main line.
     */
    auto current_mainline() const -> ConstCursor { return follow_mainline<ConstCursor>(const_cursor()); }
private:
    GameMetadata m_metadata{};        ///< Meta data for the game.
    std::shared_ptr<GameNode> m_root; ///< Root node of the game tree.
    NodeId m_next_id{2U};             ///< Next available node id.

    template<typename T>
    static auto follow_mainline(T cursor) -> T {
        auto child_cursor = cursor.child(0);
        while (child_cursor.has_value()) {
            cursor = child_cursor.value();
            child_cursor = cursor.child(0);
        }
        return cursor;
    }
};

} // namespace chessgame

#endif
