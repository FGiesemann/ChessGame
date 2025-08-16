/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/metadata.h"

namespace chessgame {

auto GameMetadata::set_tag(const std::string &name, const std::string &value) -> void {
    // TODO: Handle all the tags (most are currently ignored)
    if (name == "FEN") {
        initial_position = chesscore::FenString{value};
    }
}

} // namespace chessgame
