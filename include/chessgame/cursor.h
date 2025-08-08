/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_CURSOR_H
#define CHESSGAME_CURSOR_H

#include <cstddef>
#include <optional>

#include "chessgame/tree.h"

namespace chessgame {

class Game;

/**
 * \brief Represents a navigation and editing point in a chess game.
 *
 * The cursor represents a specific point on the main line or a variation of the
 * chess game. It allows applying moves or adding new variations to the game.
 */
class Cursor {
public:
    /**
     * \brief Create a cursor for a game and a specific node.
     *
     * Neither the game nor the node may be null pointers.
     * \param game The game.
     * \param node The node.
     */
    Cursor(Game *game, const std::shared_ptr<GameNode> &node) : m_game(game), m_node{node} {
        if ((m_game == nullptr) || !node) {
            throw ChessGameError("Invalid game or node provided to Cursor constructor.");
        }
    }

    /**
     * \brief Play a move at the current cursor position.
     *
     * Appends the given move to the current position of the game.
     * \param move The move to apply.
     * \return A cursor pointing to the new position.
     */
    auto play_move(const chesscore::Move &move) const -> Cursor;

    /**
     * \brief Adds a variation of the position.
     *
     * Adds a new variation to the position of the game to which this cursor is
     * pointing. This is done by adding a new child node to the parent of this
     * cursor's node.
     * \param move The move that starts the variation.
     * \return A cursor pointing to the new line.
     */
    auto add_variation(const chesscore::Move &move) const -> std::optional<Cursor>;

    /**
     * \brief Sets the comment for the current node.
     *
     * \param comment The comment.
     */
    auto set_comment(const std::string &comment) const -> void;

    /**
     * \brief Get the parent of the current node.
     *
     * \return Cursor to the parent node, if it exists.
     */
    auto parent() const -> std::optional<Cursor>;

    /**
     * \brief Get a child node of the current node.
     *
     * \param index Index of the child node.
     * \return Cursor to the child node, if it exists.
     */
    auto child(size_t index) const -> std::optional<Cursor>;

    /**
     * \brief Get the position object represented by this game node.
     *
     * Gets or calculates the position for the game node. The position might be
     * stored in the node already, in which case it is simply returned.
     * Otherwise, the position is calculated by following the moves from an
     * ancestor that stores a position.
     * \return The position of this game node.
     */
    auto position() const -> Position;
private:
    Game *m_game{};
    std::weak_ptr<GameNode> m_node;
};

} // namespace chessgame

#endif
