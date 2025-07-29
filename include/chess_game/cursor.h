/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_CURSOR_H
#define CHESSGAME_CURSOR_H

#include <cstddef>
#include <optional>

#include "chess_game/game.h"
#include "chess_game/tree.h"

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
        if ((m_game == nullptr) || !m_node) {
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
    auto play_move(const chesscore::Move &move) -> Cursor;

    /**
     * \brief Adds a variation beginning at the cursor position.
     *
     * Branches the line at the cursor and starts a new variation.
     * \param move The move that starts the variation.
     * \return A cursor pointing to the new line.
     */
    auto add_variation(const chesscore::Move &move) -> Cursor;

    /**
     * \brief Sets the comment for the current node.
     *
     * \param comment The comment.
     */
    auto set_comment(const std::string &comment) -> void;

    /**
     * \brief Get the parent of the current node.
     *
     * \return Cursor to the parent node, if it exists.
     */
    auto parent() -> std::optional<Cursor>;

    /**
     * \brief Get a child node of the current node.
     *
     * \param index Index of the child node.
     * \return Cursor to the child node, if it exists.
     */
    auto child(size_t index) -> std::optional<Cursor>;
private:
    Game *m_game{};
    std::shared_ptr<GameNode> m_node;
};

} // namespace chessgame

#endif
