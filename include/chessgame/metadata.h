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

namespace chessgame {

/**
 * \brief A metadata tag.
 *
 * A metadata tag is a key-value pair and describes meta data of a chess game.
 */
struct metadata_tag {
    std::string name;  ///< Name of the metadata tag.
    std::string value; ///< Value of the metadata tag.
};

/**
 * \brief A collection of metadata tags.
 */
using GameMetadata = std::vector<metadata_tag>;

} // namespace chessgame

#endif
