/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/move_matcher.h"

#include <ranges>
#include <sstream>

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
    return moves | std::views::filter([&san_move](const chesscore::Move &move) { return san_move_matches(san_move, move); }) | std::ranges::to<chesscore::MoveList>();
}

auto match_san_move_wildcard_piece_type(const SANMove &san_move, const chesscore::MoveList &moves) -> chesscore::MoveList {
    return moves | std::views::filter([&san_move](const chesscore::Move &move) { return san_move_matches_any_piece_type(san_move, move); }) |
           std::ranges::to<chesscore::MoveList>();
}

auto find_piece_moves_to_target(chesscore::Piece piece, chesscore::Square target, const chesscore::MoveList &moves) -> chesscore::MoveList {
    return moves | std::views::filter([&piece, &target](const chesscore::Move &move) { return move.piece == piece && move.to == target; }) | std::ranges::to<chesscore::MoveList>();
}

using Disambiguation = std::pair<std::optional<chesscore::File>, std::optional<chesscore::Rank>>;

auto determine_disambiguation(const chesscore::Move &move, const chesscore::MoveList &moves) -> Disambiguation {
    bool different_file{true};
    bool different_rank{true};

    for (const auto &other_move : moves) {
        if (move == other_move) {
            continue;
        }
        if (other_move.from.file() == move.from.file()) {
            different_file = false;
        }
        if (other_move.from.rank() == move.from.rank()) {
            different_rank = false;
        }
    }
    if (different_file) {
        return std::make_pair(move.from.file(), std::nullopt);
    }
    if (different_rank) {
        return std::make_pair(std::nullopt, move.from.rank());
    }
    return std::make_pair(move.from.file(), move.from.rank());
}

auto generate_san_move(const chesscore::Move &move, const chesscore::MoveList &moves) -> std::optional<SANMove> {
    std::stringstream san_string;
    const auto matching_moves = find_piece_moves_to_target(move.piece, move.to, moves);
    if (matching_moves.empty()) {
        return std::nullopt;
    }
    Disambiguation disambiguation{};

    if (move.piece.type != chesscore::PieceType::Pawn) {
        san_string << move.piece.piece_char_colorless();
    }
    if (move.piece.type == chesscore::PieceType::Pawn) {
        if (move.captured.has_value()) {
            san_string << move.from.file().name();
        }
    } else if (matching_moves.size() > 1) {
        disambiguation = determine_disambiguation(move, matching_moves);
        if (disambiguation.first.has_value()) {
            san_string << disambiguation.first.value().name();
        }
        if (disambiguation.second.has_value()) {
            san_string << std::to_string(disambiguation.second.value().rank);
        }
    }
    if (move.captured.has_value()) {
        san_string << 'x';
    }
    san_string << to_string(move.to);
    if (move.promoted.has_value()) {
        san_string << '=' << move.promoted.value().piece_char_colorless();
    }
    return SANMove{
        .san_string = san_string.str(),
        .moving_piece = move.piece,
        .target_square = move.to,
        .capturing = move.captured.has_value(),
        .promotion = move.promoted,
        .disambiguation_file = disambiguation.first,
        .disambiguation_rank = disambiguation.second
    };
}

} // namespace chessgame
