/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/move_matcher.h"

namespace chessgame {

auto match_san_move([[maybe_unused]] const SANMove &san_move, [[maybe_unused]] const chesscore::MoveList &moves) -> chesscore::MoveList {
    return {};
}

} // namespace chessgame
