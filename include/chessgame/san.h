/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_SAN_H
#define CHESSGAME_SAN_H

#include <optional>
#include <string>

#include "chesscore/piece.h"
#include "chesscore/square.h"

namespace chessgame {

/**
 * \brief Possible check states of a SAN move.
 */
enum class CheckState {
    None,     ///< No check.
    Check,    ///< Check.
    Checkmate ///< Checkmate.
};

/**
 * \brief Representation of a move in SAN.
 *
 * A move can be described in Standard Algebraic Notation (SAN). This class
 * represents such a move and holds the information that can be extracted from
 * the SAN string.
 */
class SANMove {
public:
    /**
     * \brief Create a new SANMove.
     *
     * Creates a new SANMove with the given information.
     * \param san The original SAN representation of the move, e.g., from a PGN file.
     * \param piece The moving piece.
     * \param target_square The target square of the move.
     * \param check_state Check state of the move.
     */
    SANMove(std::string san, chesscore::Piece piece, chesscore::Square target_square, CheckState check_state)
        : m_san{std::move(san)}, m_piece{piece}, m_target_square{target_square}, m_check_state{check_state}, m_from_file{std::nullopt}, m_from_rank{std::nullopt} {}

    /**
     * \brief Create a new SANMove.
     *
     * Creates a new SANMove with the given information.
     * \param san The original SAN representation of the move, e.g., from a PGN file.
     * \param piece The moving piece.
     * \param target_square The target square of the move.
     * \param check_state Check state of the move.
     * \param from_file The file-disambiguation character of the move.
     */
    SANMove(std::string san, chesscore::Piece piece, chesscore::Square target_square, CheckState check_state, chesscore::File from_file)
        : m_san{std::move(san)}, m_piece{piece}, m_target_square{target_square}, m_check_state{check_state}, m_from_file{from_file}, m_from_rank{std::nullopt} {}

    /**
     * \brief Create a new SANMove.
     *
     * Creates a new SANMove with the given information.
     * \param san The original SAN representation of the move, e.g., from a PGN file.
     * \param piece_type The type of the moving piece.
     * \param target_square The target square of the move.
     * \param check_state Check state of the move.
     * \param from_rank The rank-disambiguation character of the move.
     */
    SANMove(std::string san, chesscore::Piece piece, chesscore::Square target_square, CheckState check_state, chesscore::Rank from_rank)
        : m_san{std::move(san)}, m_piece{piece}, m_target_square{target_square}, m_check_state(check_state), m_from_file{std::nullopt}, m_from_rank{from_rank} {}

    /**
     * \brief Get the original SAN representation of the move.
     *
     * \return The original SAN representation of the move.
     */
    auto san() const -> std::string { return m_san; }

    /**
     * \brief Get the moving piece.
     *
     * \return The moving piece.
     */
    auto piece() const -> chesscore::Piece { return m_piece; }

    /**
     * \brief Get the target square of the move.
     *
     * \return The target square of the move.
     */
    auto target_square() const -> chesscore::Square { return m_target_square; }

    /**
     * \brief Get the check state of the move.
     *
     * \return The check state of the move.
     */
    auto check_state() const -> CheckState { return m_check_state; }

    /**
     * \brief Get the file-disambiguation character of the move.
     *
     * \return The file-disambiguation character of the move.
     */
    auto from_file() const -> std::optional<chesscore::File> { return m_from_file; }

    /**
     * \brief Get the rank-disambiguation character of the move.
     *
     * \return The rank-disambiguation character of the move.
     */
    auto from_rank() const -> std::optional<chesscore::Rank> { return m_from_rank; }

    /**
     * \brief Equality comparison of two SANMoves.
     *
     * The SANMoves are considered equal, if all pieces of information are equal.
     * \param rhs The move to compare with.
     * \return If the moves are equal.
     */
    auto operator==(const SANMove &rhs) const -> bool = default;
private:
    std::string m_san;                          ///< The original string representation of the move.
    chesscore::Piece m_piece;                   ///< The moving piece.
    chesscore::Square m_target_square;          ///< Target square of the move.
    CheckState m_check_state{CheckState::None}; ///< Check state of the move.
    std::optional<chesscore::File> m_from_file; ///< File-distsinguishing character of the move.
    std::optional<chesscore::Rank> m_from_rank; ///< Rank-distsinguishing character of the move.
};

/**
 * \brief Parse a SAN string into a SANMove.
 *
 * Extracts all pieces of information from the SAN string.
 * \param san The SAN string.
 * \param side_to_move The side to move.
 * \return The parsed SANMove.
 */
auto parse_san(const std::string &san, chesscore::Color side_to_move) -> SANMove;

} // namespace chessgame

#endif
