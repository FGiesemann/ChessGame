/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/move_matcher.h"

#include <ranges>

namespace chessgame {

auto san_move_matches(const SANMove &san_move, const chesscore::Move &move) -> bool {
    if (san_move.moving_piece != move.piece || san_move.target_square != move.to) {
        return false;
    }
    if ((san_move.disambiguation_file.has_value() && san_move.disambiguation_file.value() != move.from.file()) ||
        (san_move.disambiguation_rank.has_value() && san_move.disambiguation_rank.value() != move.from.rank())) {
        return false;
    }
    if ((san_move.capturing && move.captured == std::nullopt) || (!san_move.capturing && move.captured != std::nullopt)) {
        return false;
    }
    if (move.promoted != san_move.promotion) {
        return false;
    }
    return true;
}

auto match_san_move([[maybe_unused]] const SANMove &san_move, [[maybe_unused]] const chesscore::MoveList &moves) -> chesscore::MoveList {
    return moves | std::views::filter([&san_move](const chesscore::Move &move) { return san_move_matches(san_move, move); }) | std::ranges::to<std::vector>();
}

} // namespace chessgame
