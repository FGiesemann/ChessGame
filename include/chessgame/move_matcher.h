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
 * \brief Match a move list against a SAN move.
 *
 * Finds all moves in the move list that can be described by the SAN move.
 * \param san_move The SAN move.
 * \param moves The move list.
 * \return List of all matching moves.
 */
auto match_san_move(const SANMove &san_move, const chesscore::MoveList &moves) -> chesscore::MoveList;

} // namespace chessgame

#endif
