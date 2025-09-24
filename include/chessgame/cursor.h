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
template<typename GameType, typename NodeType>
class BaseCursor {
public:
    /**
     * \brief Create a cursor for a game and a specific node.
     *
     * Neither the game nor the node may be null pointers.
     * \param game The game.
     * \param node The node.
     */
    BaseCursor(GameType *game, const std::shared_ptr<NodeType> &node) : m_game(game), m_node{node} {
        if ((m_game == nullptr) || !node) {
            throw ChessGameError("Invalid game or node provided to Cursor constructor.");
        }
    }

    /**
     * \brief Implicit conversion operator.
     *
     * Allows implicit conversion from Cursor to ConstCursor.
     * \return A ConstCursor.
     */
    operator BaseCursor<const Game, const GameNode>() const { return BaseCursor<const Game, const GameNode>{m_game, m_node}; }

    /**
     * \brief Get the parent of the current node.
     *
     * \return Cursor to the parent node, if it exists.
     */
    [[nodiscard]] auto parent() const -> std::optional<BaseCursor> {
        const auto parent_node = m_node.lock()->parent();
        if (parent_node) {
            return BaseCursor{m_game, parent_node};
        }
        return {};
    }

    [[nodiscard]] auto child_count() const -> size_t { return m_node.lock()->child_count(); }

    [[nodiscard]] auto has_variations() const -> bool { return child_count() > 1; }

    /**
     * \brief Get a child node of the current node.
     *
     * \param index Index of the child node.
     * \return Cursor to the child node, if it exists.
     */
    [[nodiscard]] auto child(size_t index) const -> std::optional<BaseCursor> {
        const auto child_node = m_node.lock()->get_child(index);
        if (child_node) {
            return BaseCursor{m_game, child_node};
        }
        return {};
    }

    /**
     * \brief Get the position object represented by this game node.
     *
     * Gets or calculates the position for the game node. The position might be
     * stored in the node already, in which case it is simply returned.
     * Otherwise, the position is calculated by following the moves from an
     * ancestor that stores a position.
     * \return The position of this game node.
     */
    [[nodiscard]] auto position() const -> chesscore::Position { return m_node.lock()->calculate_position(); }

    /**
     * \brief Play a move at the current cursor position.
     *
     * Appends the given move to the current position of the game.
     * \param move The move to apply.
     * \return A cursor pointing to the new position.
     */
    [[nodiscard]] auto play_move(const chesscore::Move &move) -> BaseCursor
    requires(!std::is_const_v<GameType>)
    {
        const auto node_ptr = m_game->add_node(m_node.lock(), move);
        return {m_game, node_ptr};
    }

    /**
     * \brief Adds a variation of the position.
     *
     * Adds a new variation to the position of the game to which this cursor is
     * pointing. This is done by adding a new child node to the parent of this
     * cursor's node.
     * \param move The move that starts the variation.
     * \return A cursor pointing to the new line.
     */
    [[nodiscard]] auto add_variation(const chesscore::Move &move) -> std::optional<BaseCursor>
    requires(!std::is_const_v<GameType>)
    {
        const auto parent_cursor = parent();
        if (parent_cursor) {
            const auto parent_node = parent_cursor->m_node;
            const auto node_ptr = m_game->add_node(parent_node.lock(), move);
            return BaseCursor{m_game, node_ptr};
        }
        return {};
    }

    /**
     * \brief Sets the comment for the current node.
     *
     * \param comment The comment.
     */
    auto set_comment(const std::string &comment) -> void
    requires(!std::is_const_v<GameType>)
    {
        m_node.lock()->set_comment(comment);
    }

    /**
     * \brief Appends to the comment for the current node.
     *
     * \param comment The comment.
     */
    auto append_comment(const std::string &comment) -> void
    requires(!std::is_const_v<GameType>)
    {
        m_node.lock()->append_comment(comment);
    }

    /**
     * \brief Sets the pre-move comment for the current node.
     *
     * \param comment The comment.
     */
    auto set_premove_comment(const std::string &comment) -> void
    requires(!std::is_const_v<GameType>)
    {
        m_node.lock()->set_premove_comment(comment);
    }

    /**
     * \brief Appends to the pre-move comment for the current node.
     *
     * \param comment The comment.
     */
    auto append_premove_comment(const std::string &comment) -> void
    requires(!std::is_const_v<GameType>)
    {
        m_node.lock()->append_premove_comment(comment);
    }

    auto node() -> std::shared_ptr<NodeType>
    requires(!std::is_const_v<GameType>)
    {
        return m_node.lock();
    }

    auto node() const -> std::shared_ptr<const NodeType> { return m_node.lock(); }
private:
    GameType *m_game{};
    std::weak_ptr<NodeType> m_node;
};

using Cursor = BaseCursor<Game, GameNode>;
using ConstCursor = BaseCursor<const Game, const GameNode>;

} // namespace chessgame

#endif
