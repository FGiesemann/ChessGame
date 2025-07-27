/* ************************************************************************** *
 * Chess Game Model                                                           *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAMEMODEL_GAME_METADATA_H
#define CHESSGAMEMODEL_GAME_METADATA_H

#include <optional>
#include <string>

namespace chessgamemodel {

/**
 * \brief Description of a player.
 *
 * Stores metadata about a player. Can include a "snapshot" of the player's
 * changing data at the time of the game (e.g., ELO rating).
 */
struct Player {
    std::string full_name;  ///< Name as it appears in sources (like PGN-Files).
    std::string title;      ///< Offizial title at the time of the game.
    std::optional<int> elo; ///< ELO rating at the time of the game.

    std::string sort_name;                 ///< Name used for sorting.
    std::optional<std::string> fide_id;    ///< FIDE ID of the player, if known.
    std::optional<std::string> federation; ///< Federation of the player, if known.
};

/**
 * \brief Description of an event.
 */
struct Event {
    std::string original_name;        ///< Name as it appears in sources (like PGN-Files).
    std::string name;                 ///< Name used for sorting.
    std::optional<int> edition;       ///< Edition number of the event.
    std::optional<int> year;          ///< Year of the event.
    std::optional<std::string> round; ///< Round of the game during the event.

    std::string site; ///< Location of the event.
    std::string date; ///< Date of the event.
};

/**
 * \brief Stores meta data for a game of chess.
 *
 * Handles meta data like the names of players, the event, etc.
 */
struct GameMetadata {
    Player white_player; ///< Player of white pieces.
    Player black_player; ///< Player of black pieces.
    Event event;         ///< Event of the game.
};

} // namespace chessgamemodel

#endif
