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

/**
 * \brief A general exception type for errors relating to chess game.
 */
class ChessGameError : public std::exception {
public:
    /**
     * \brief Create a new exception.
     *
     * Creates a new exception with the gven message.
     * \param message The message.
     */
    ChessGameError(std::string message) : m_message{std::move(message)} {}

    /**
     * \brief Return the stored message.
     *
     * Returns the message stored in the exception.
     * \return The message.
     */
    [[nodiscard]] auto what() const noexcept -> const char * override { return m_message.c_str(); }
private:
    std::string m_message;
};

} // namespace chessgame

#endif
