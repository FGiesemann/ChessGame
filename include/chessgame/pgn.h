/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_PGN_H
#define CHESSGAME_GAME_PGN_H

#include <expected>
#include <iosfwd>

#include "chessgame/game.h"

namespace chessgame {

/**
 * \brief Type of a PGN error.
 *
 * These errors can appear when parsing PGN data.
 */
enum class PGNErrorType {

};

/**
 * \brief Error when parsing PGN data.
 *
 * Describes an error that occured while reading PGN data.
 */
struct PGNError {
    PGNErrorType type;   ///< Error type.
    std::string message; ///< Additional description of the error.
    int line;            ///< Line number where the error occured.
};

/**
 * \brief Parse PGN data.
 *
 * Reads PGN data from an input stream. Extracts the game information and
 * returns it as a Game object.
 * \param in_stream The input stream.
 * \return The parsed game or an error description.
 */
auto read_pgn(std::istream &in_stream) -> std::expected<Game, PGNError>;

} // namespace chessgame

#endif
