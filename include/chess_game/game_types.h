/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_TYPES_H
#define CHESSGAME_GAME_TYPES_H

#include <exception>

#include "chesscore/bitboard.h"
#include "chesscore/position.h"

namespace chessgame {

using Position = chesscore::Position<chesscore::Bitboard>;

class ChessGameError : public std::exception {
public:
    ChessGameError(std::string message) : m_message{std::move(message)} {}

    [[nodiscard]] auto what() const noexcept -> const char * override { return m_message.c_str(); }
private:
    std::string m_message;
};

} // namespace chessgame

#endif
