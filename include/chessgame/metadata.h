/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_METADATA_H
#define CHESSGAME_GAME_METADATA_H

#include <optional>
#include <string>
#include <vector>

#include "chesscore/fen.h"

namespace chessgame {

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
 * \brief Description of an opening.
 */
struct Opening {
    std::string eco;       ///< The ECO code.
    std::string name;      ///< The name of the opening.
    std::string variation; ///< The name of the opening variation.
};

/**
 * \brief Possible ends of a game of chess.
 */
enum class GameResult {
    WhiteWins,   ///< White player winds.
    BlackWins,   ///< Black player wins.
    Draw,        ///< Game ended in a draw.
    InProgress,  ///< Game stil in progress.
    Unknown,     ///< Unknown end of the game.
    Abandoned,   ///< Game was abandoned without result.
    Adjudicaton, ///< Game result was adjudicated.
    NotPlayed    ///< Game was not played.
};

/**
 * \brief A custom metadata tag.
 *
 * This allows adding arbitrary metadata to a game.
 */
struct CustomTag {
    std::string name;  ///< Name of the tag.
    std::string value; ///< Value of the tag.
};

/**
 * \brief Stores meta data for a game of chess.
 *
 * Handles meta data like the names of players, the event, etc.
 */
struct GameMetadata {
    Player white_player;                                  ///< Player of white pieces.
    Player black_player;                                  ///< Player of black pieces.
    Event event;                                          ///< Event of the game.
    Opening opening;                                      ///< Opening of the game.
    GameResult result;                                    ///< Result of the game.
    std::optional<std::string> date;                      ///< Date of the game, if different from event date.
    std::optional<chesscore::FenString> initial_position; ///< Initial position of the game, if different from the starting position.
    int ply_count;                                        ///< Number of (half) moves in the game
    std::string source;                                   ///< Source of the information.
    std::optional<Player> annotator;                      ///< A single annotator of the game, if available.
    std::vector<CustomTag> custom_tags;                   ///< Additional custom tags.
};

} // namespace chessgame

#endif
