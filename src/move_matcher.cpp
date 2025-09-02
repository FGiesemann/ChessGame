/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/move_matcher.h"

#include <ranges>

namespace chessgame {

auto san_move_matches_any_piece_type(const SANMove &san_move, const chesscore::Move &move) -> bool {
    if (san_move.target_square != move.to) {
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

auto san_move_matches(const SANMove &san_move, const chesscore::Move &move) -> bool {
    if (san_move.moving_piece != move.piece) {
        return false;
    }
    return san_move_matches_any_piece_type(san_move, move);
}

auto match_san_move(const SANMove &san_move, const chesscore::MoveList &moves) -> chesscore::MoveList {
    return moves | std::views::filter([&san_move](const chesscore::Move &move) { return san_move_matches(san_move, move); }) | std::ranges::to<std::vector>();
}

auto match_san_move_wildcard_piece_type(const SANMove &san_move, const chesscore::MoveList &moves) -> chesscore::MoveList {
    return moves | std::views::filter([&san_move](const chesscore::Move &move) { return san_move_matches_any_piece_type(san_move, move); }) | std::ranges::to<std::vector>();
}

auto generate_san_move(const chesscore::Move &move, [[maybe_unused]] const chesscore::MoveList &moves) -> std::optional<SANMove> {
    using namespace std::string_literals;

    std::string san_string{to_string(move.to)};
    if (move.captured.has_value()) {
        san_string = "x" + san_string;
    }
    if (move.piece.type != chesscore::PieceType::Pawn) {
        san_string = move.piece.piece_char_colorless() + san_string;
    } else {
        if (move.captured.has_value()) {
            san_string = move.from.file().name() + san_string;
        }
    }
    if (move.promoted.has_value()) {
        san_string += "="s + move.promoted.value().piece_char_colorless();
    }
    return SANMove{.san_string = san_string, .moving_piece = move.piece, .target_square = move.to, .capturing = move.captured.has_value(), .promotion = move.promoted};
}

} // namespace chessgame
