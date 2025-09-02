/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_MOVE_MATCHER_H
#define CHESSGAME_GAME_MOVE_MATCHER_H

#include "chessgame/san.h"

#include "chesscore/move.h"

namespace chessgame {

/**
 * \brief Check, if a SAN move matches a move.
 *
 * Checks if the given move can be described by the SAN move.
 * \param san_move The SAN move.
 * \param move The move.
 * \return If the move can be described by the SAN move.
 */
auto san_move_matches(const SANMove &san_move, const chesscore::Move &move) -> bool;

/**
 * \brief Match a move list against a SAN move.
 *
 * Finds all moves in the move list that can be described by the SAN move.
 * \param san_move The SAN move.
 * \param moves The move list.
 * \return List of all matching moves.
 */
auto match_san_move(const SANMove &san_move, const chesscore::MoveList &moves) -> chesscore::MoveList;

/**
 * \brief Match a move list against a SAN move, ignoring the piece type.
 *
 * Finds all moves in the move list that can be described by the SAN move, but allows the moves to have any piece type.
 * \param san_move The SAN move.
 * \param moves The move list.
 * \return List of all matching moves.
 */
auto match_san_move_wildcard_piece_type(const SANMove &san_move, const chesscore::MoveList &moves) -> chesscore::MoveList;

/**
 * \brief Convert a move to a SAN move.
 *
 * This generates a SANMove that describes the given move. It determines the necessary
 * disambiguation information in the context of the given list of moves.
 * This does not determine the check state for the SANMove.
 * \param move The move to be converted.
 * \param moves A list of possible moves (where move should be included).
 * \return A SANMove describing the move.
 */
auto generate_san_move(const chesscore::Move &move, const chesscore::MoveList &moves) -> std::optional<SANMove>;

} // namespace chessgame

#endif
