/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_MOVETREE_H
#define CHESSGAME_MOVETREE_H

#include <memory>
#include <vector>

#include "chessgame/types.h"

#include "chesscore/move.h"

namespace chessgame {

/**
 * \brief Identifier for a move in a game.
 *
 * A NodeId identifies a node in a game tree. It is not a global identifier.
 */
struct NodeId {
    uint32_t value; ///< The id value.

    /**
     * \brief Construct a new NodeId object
     *
     * \param val The id value.
     */
    explicit constexpr NodeId(uint32_t val = 0) : value(val) {}

    /**
     * \brief Equality comparison for node ids.
     *
     * \param other The other node id.
     * \return If the node ids are queal.
     */
    auto operator==(const NodeId &other) const -> bool { return value == other.value; }

    /**
     * \brief Inequality comparison for node ids.
     *
     * \param other The other node id.
     * \return If the node ids are not equal.
     */
    auto operator!=(const NodeId &other) const -> bool { return value != other.value; }

    auto operator++() -> NodeId & {
        ++value;
        return *this;
    }

    auto operator++(int) -> NodeId { return NodeId(value++); }

    static const NodeId Invalid; ///< Constant representing an invalid node id.
};

/**
 * \brief Representation of a state in a game.
 *
 * For managing games with variations and analysis of games or positions, a
 * game tree is stored. A GameNode represents a position in the game tree after
 * a move has been made. Multiple children represent continuations of the game
 * with alternative moves.
 */
class GameNode {
public:
    /**
     * \brief Construct a new GameNode object
     *
     * If a prent node is given, the move should also be valid. It is
     * interpreted as the move that lead from the parent node to this node.
     *
     * \param node_id The id of the game node.
     * \param move The move that led to this node.
     * \param parent The parent node.
     */
    explicit GameNode(NodeId node_id, chesscore::Move move = {}, const std::shared_ptr<GameNode> &parent = nullptr) : m_id(node_id), m_move(move), m_parent(parent) {}

    /**
     * \brief Get the id of the node.
     *
     * \return Id of the node.
     */
    [[nodiscard]] auto id() const -> const NodeId & { return m_id; }

    /**
     * \brief Get the move.
     *
     * Returns the move that lead to this game node.
     * \return The move.
     */
    [[nodiscard]] auto move() const -> const chesscore::Move & { return m_move; }

    /**
     * \brief Get the parent node.
     *
     * If the parent no longer exists, nullptr is returned.
     * \return The parent node.
     */
    [[nodiscard]] auto parent() const -> std::shared_ptr<GameNode> { return m_parent.lock(); }

    /**
     * \brief The number of children.
     *
     * Returns the number of children of this node. If this is not a leaf-node,
     * at least 1 should be returned, for the child node representing the main
     * line of the game.
     * \return Number of child nodes.
     */
    [[nodiscard]] auto child_count() const -> size_t { return m_children.size(); }

    /**
     * \brief Get the child node.
     *
     * Returns the child node at the given index. Index 0 represents the main
     * line. If the index is too big, nullptr is returned.
     * \param index Index of the child.
     * \return The child with the given index.
     */
    auto get_child(size_t index) -> std::shared_ptr<GameNode> { return index >= m_children.size() ? nullptr : m_children[index]; }

    /**
     * \brief Get the child node.
     *
     * Returns the child node at the given index. Index 0 represents the main
     * line. If the index is too big, nullptr is returned.
     * \param index Index of the child.
     * \return The child with the given index.
     */
    [[nodiscard]] auto get_child(size_t index) const -> std::shared_ptr<const GameNode> { return index >= m_children.size() ? nullptr : m_children[index]; }

    /**
     * \brief Append a new child node.
     *
     * The given node is appended to the list of child nodes.
     * \param child The child node.
     */
    auto append_child(const std::shared_ptr<GameNode> &child) -> void { m_children.push_back(child); }

    /**
     * \brief Return the comment of the game node.
     *
     * This is a comment of the game position or the move that lead to it.
     * \return The comment.
     */
    [[nodiscard]] auto comment() const -> const std::string & { return m_comment; }

    /**
     * \brief Return the pre-move comment of the game node.
     *
     * This is a comment of the game position or the move that lead to it.
     * \return The comment.
     */
    [[nodiscard]] auto premove_comment() const -> const std::string & { return m_premove_comment; }

    /**
     * \brief Set the comment of the game node.
     *
     * This is a comment of the game position or the move that lead to it.
     * \param comment The comment.
     */
    auto set_comment(const std::string &comment) -> void { m_comment = comment; }

    /**
     * \brief Append to the comment of the game node.
     *
     * This is a comment of the game position or the move that lead to it.
     * \param comment The comment.
     */
    auto append_comment(const std::string &comment) -> void { m_comment += comment; }

    /**
     * \brief Set the pre-move comment of the game node.
     *
     * This is a comment of the game position.
     * \param comment The comment.
     */
    auto set_premove_comment(const std::string &comment) -> void { m_premove_comment = comment; }

    /**
     * \brief Append to the pre-move comment of the game node.
     *
     * This is a comment of the game position.
     * \param comment The comment.
     */
    auto append_premove_comment(const std::string &comment) -> void { m_premove_comment += comment; }

    /**
     * \brief Get the position of the game node.
     *
     * \return The position.
     */
    [[nodiscard]] auto position() const -> const std::optional<Position> & { return m_position; }

    /**
     * \brief Set the position of the game node.
     *
     * \param position The position.
     */
    auto set_position(const Position &position) -> void { m_position = position; }

    /**
     * \brief Calculate the position of this game node.
     *
     * The position is calculated from an ancestor node that has a position and
     * the sequence of moves that lead to this node from that ancestor. The
     * computed position is not stored in the node. Use setPosition to do that.
     * \return The position represented by this node.
     */
    [[nodiscard]] auto calculate_position() const -> Position;

    auto nags() const -> const std::vector<int> & { return m_nags; }
    auto nags() -> std::vector<int> & { return m_nags; }

    auto add_nag(int num) -> void { m_nags.push_back(num); }
private:
    NodeId m_id;                                       ///< The id of this node.
    chesscore::Move m_move;                            ///< The move that led to this node (from the parent node).
    std::weak_ptr<GameNode> m_parent;                  ///< Pointer to the parent node.
    std::vector<std::shared_ptr<GameNode>> m_children; ///< List of child nodes. The first entry represetns the "main line".
    std::string m_comment;                             ///< A comment of the position or move.
    std::string m_premove_comment;                     ///< A comment on this game line, given before the move.
    std::vector<int> m_nags;                           ///< Numeric annotation glyphs describing the move or position.
    std::optional<Position> m_position;                ///< The position described by this node.
};

} // namespace chessgame

#endif
