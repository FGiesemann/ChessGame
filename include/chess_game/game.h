/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_H
#define CHESSGAME_GAME_H

#include "chess_game/game_metadata.h"

namespace chessgame {

/**
 * \brief A game of chess.
 *
 * Represents a game of chess. Stores meta information about the game, like the
 * names of the players, the event, etc.
 * Manages the list of moves of the game. Moves can be annotated and the class
 * can also store variations of the game (alternative sequences of moves).
 */
class Game {
public:
    /**
     * \brief Read-only access to the meta data of the game.
     *
     * \return The game meta data.
     */
    auto metadata() const -> const GameMetadata & { return m_metadata; }

    /**
     * \brief Access to the meta data of the game.
     *
     * \return The game meta data.
     */
    auto metadata() -> GameMetadata & { return m_metadata; }
private:
    GameMetadata m_metadata{}; ///< Meta data for the game.
    GameNode m_root{};         ///< Root node of the game tree.
};

} // namespace chessgame

#endif
