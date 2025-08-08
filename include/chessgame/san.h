/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_SAN_H
#define CHESSGAME_SAN_H

#include <expected>
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
 * \brief The possible suffix annotations for a move.
 *
 * Suffix annotation can be appended to a SAN move to indicate the quality of
 * the move.
 */
enum class SuffixAnnotation {
    GoodMove,        ///< !
    PoorMove,        ///< ?
    VeryGoodMove,    ///< !!
    VeryPoorMove,    ///< ??
    SpeculativeMove, ///< !?
    QuestionableMove ///< ?!
};

/**
 * \brief Representation of a move in SAN.
 *
 * A move can be described in Standard Algebraic Notation (SAN). This class
 * represents such a move and holds the information that can be extracted from
 * the SAN string.
 */
struct SANMove {
    std::string san_string;                                           ///< The original string representation of the move.
    chesscore::Piece moving_piece;                                    ///< The moving piece.
    chesscore::Square target_square;                                  ///< Target square of the move.
    bool capturing{false};                                            ///< If the move is capturing.
    std::optional<chesscore::Piece> promotion{std::nullopt};          ///< Promotion piece of the move.
    CheckState check_state{CheckState::None};                         ///< Check state of the move.
    std::optional<chesscore::File> disambiguation_file{std::nullopt}; ///< Disambiguation file information.
    std::optional<chesscore::Rank> disambiguation_rank{std::nullopt}; ///< Disambiguation rank information.
    std::optional<SuffixAnnotation> suffix_annotation{std::nullopt};  ///< Suffix annotation of the move.

    chesscore::File target_file{'a'};
    chesscore::Rank target_rank{1};
    bool possible_disambiguation{false};

    /**
     * \brief Equality comparison of two SANMoves.
     *
     * The SANMoves are considered equal, if all pieces of information are equal.
     * \param rhs The move to compare with.
     * \return If the moves are equal.
     */
    auto operator==(const SANMove &rhs) const -> bool = default;
};

enum class SANParserErrorType {
    UnexpectedToken,
    UnexpectedCharsAtEnd,
    InvalidSuffixAnnotation,
    CheckAndCheckmate,
    MissingPieceType,
    MissingRank,
    MissingFile,
};

struct SANParserError {
    SANParserErrorType error_type;
    std::string san;
};

/**
 * \brief Parse a SAN string into a SANMove.
 *
 * Extracts all pieces of information from the SAN string.
 * \param san The SAN string.
 * \param side_to_move The side to move.
 * \return The parsed SANMove.
 */
auto parse_san(const std::string &san, chesscore::Color side_to_move) -> std::expected<SANMove, SANParserError>;

} // namespace chessgame

#endif
