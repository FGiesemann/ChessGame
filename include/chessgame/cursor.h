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

    /**
     * \brief Number of child positions of this position.
     *
     * Returns the number of positions that originate at this position. The
     * first child is the main line of the game while further children are
     * variations.
     * \return Number of children.
     */
    [[nodiscard]] auto child_count() const -> size_t { return m_node.lock()->child_count(); }

    /**
     * \brief Check if the current node has variations.
     *
     * \return If the current node has variations.
     */
    [[nodiscard]] auto has_variations() const -> bool { return child_count() > 1; }

    /**
     * \brief Check if the current node is the start of a variation.
     *
     * It is a start of a variation, when it is a child other than the first of
     * its parent.
     * \return If the current node is the start of a variation.
     */
    [[nodiscard]] auto starts_variation() const -> bool {
        const auto parent_node = m_node.lock()->parent();
        return parent_node && (m_node.lock() != parent_node->get_child(0));
    }

    /**
     * \brief Returns the index of the variation that this node starts.
     *
     * The index is 0 for main line positions. An index i of 1 or greater means,
     * that this node starts the i-th variation in its parent.
     * \return The number of the variation.
     */
    [[nodiscard]] auto variation_number() const -> int {
        const auto parent_node = m_node.lock()->parent();
        return parent_node ? parent_node->get_child_number(m_node.lock()) : 0;
    }

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
     * \brief Return the id of the referenced node.
     *
     * \return Id of the referenced node.
     */
    [[nodiscard]] auto node_id() const -> NodeId { return m_node.lock()->id(); }

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
     * \brief Get the comment for the current node.
     *
     * \return The comment.
     */
    auto comment() const -> std::string { return m_node.lock()->comment(); }

    /**
     * \brief Get the pre-move comment for the current node.
     *
     * \return The pre-move comment.
     */
    auto premove_comment() const -> std::string { return m_node.lock()->premove_comment(); }

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

    /**
     * \brief Access the GameNode references by this cursor.
     *
     * Get access to the GameNode referenced by this cursor.
     */
    auto node() -> std::shared_ptr<NodeType>
    requires(!std::is_const_v<GameType>)
    {
        return m_node.lock();
    }

    /**
     * \brief Access the GameNode references by this cursor.
     *
     * Get access to the GameNode referenced by this cursor.
     */
    auto node() const -> std::shared_ptr<const NodeType> { return m_node.lock(); }

    /**
     * \brief Returns the lit of NAGs for this node.
     *
     * The list of Numeric Annotation Glyphs (NAG) for this node are returned.
     * \return List of NAGs.
     */
    auto nags() const -> const std::vector<int> & { return m_node.lock()->nags(); }

    /**
     * \brief Returns the lit of NAGs for this node.
     *
     * The list of Numeric Annotation Glyphs (NAG) for this node are returned.
     * \return List of NAGs.
     */

    auto nags() -> std::vector<int> &
    requires(!std::is_const_v<GameType>)
    {
        return m_node.lock()->nags();
    }

    /**
     * \brief Returns the move that lead to this position.
     *
     * The GameNode referenced by this cursor represents a position in the game.
     * The move that lead to this position is returned.
     * \return Move that lead to this position.
     */
    auto move() const -> const chesscore::Move & { return m_node.lock()->move(); }

    /**
     * \brief The player that is to move in this position.
     *
     * Returns the color of the player to move next from the position referenced
     * by this cursor.
     * \return Color of the player to move next.
     */
    auto side_to_move() const -> chesscore::Color { return other_color(move().piece.color); }

    /**
     * \brief The player that created this position.
     *
     * Returns the color of the player whose move lead to the position
     * referenced by this cursor.
     * \return Color of the player that created this position.
     */
    auto player_color() const -> chesscore::Color { return move().piece.color; }

    /**
     * \brief Equality comparison of cursors.
     *
     * Two cursors are considered equal, if they reference the same node from
     * the same game.
     * \param other The other cursor.
     * \return If the cursors are equal or not.
     */
    auto operator==(const BaseCursor &other) const -> bool { return m_game == other.m_game && m_node.lock() == other.m_node.lock(); }
private:
    GameType *m_game{};
    std::weak_ptr<NodeType> m_node;
};

/**
 * \brief A cursor that references a position in a chess game.
 *
 * The cursor can be used to edit the game, e.g., add moves and comments.
 */
using Cursor = BaseCursor<Game, GameNode>;

/**
 * \brief A const cursor that references a position in a chess game.
 *
 * This cursor provides read-only access to the game.
 */
using ConstCursor = BaseCursor<const Game, const GameNode>;

} // namespace chessgame

#endif
