/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/san.h"
#include <catch2/catch_all.hpp>

#include <chesscore/move.h>
#include <chessgame/move_matcher.h>

using namespace chessgame;
using namespace chesscore;

TEST_CASE("Move Matcher.Unambiguous Moves", "[san][move_matcher]") {
    const MoveList moves{
        Move{.from = Square::A1, .to = Square::A5, .piece = Piece::WhiteRook},   // Ra5
        Move{.from = Square::C4, .to = Square::E3, .piece = Piece::BlackKnight}, // Ne3
        Move{.from = Square::B7, .to = Square::F3, .piece = Piece::BlackBishop}, // Bf3
        Move{.from = Square::G5, .to = Square::F3, .piece = Piece::BlackKnight}, // Nf3
    };

    const auto moves1 = match_san_move(SANMove{.san_string = "Ra5", .moving_piece = Piece::WhiteRook, .target_square = Square::A5}, moves);
    CHECK(moves1.size() == 1);
    CHECK(move_list_contains(moves1, Move{.from = Square::A1, .to = Square::A5, .piece = Piece::WhiteRook}));

    const auto moves2 = match_san_move(SANMove{.san_string = "Ne3", .moving_piece = Piece::BlackKnight, .target_square = Square::E3}, moves);
    CHECK(moves2.size() == 1);
    CHECK(move_list_contains(moves2, Move{.from = Square::C4, .to = Square::E3, .piece = Piece::BlackKnight}));

    const auto moves3 = match_san_move(SANMove{.san_string = "Bf3", .moving_piece = Piece::BlackBishop, .target_square = Square::F3}, moves);
    CHECK(moves3.size() == 1);
    CHECK(move_list_contains(moves3, Move{.from = Square::B7, .to = Square::F3, .piece = Piece::BlackBishop}));

    const auto moves4 = match_san_move(SANMove{.san_string = "Nf3", .moving_piece = Piece::BlackKnight, .target_square = Square::F3}, moves);
    CHECK(moves4.size() == 1);
    CHECK(move_list_contains(moves4, Move{.from = Square::G5, .to = Square::F3, .piece = Piece::BlackKnight}));
}

TEST_CASE("Move Matcher.Disambiguations", "[san][move_matcher]") {
    const MoveList moves{
        Move{.from = Square::B4, .to = Square::D5, .piece = Piece::WhiteKnight}, // Nbd5
        Move{.from = Square::E3, .to = Square::D5, .piece = Piece::WhiteKnight}, // Ned5
        Move{.from = Square::F2, .to = Square::F4, .piece = Piece::BlackRook},   // R2f4
        Move{.from = Square::F6, .to = Square::F4, .piece = Piece::BlackRook},   // R6f4
    };

    const auto moves1_1 = match_san_move(SANMove{.san_string = "Nd5", .moving_piece = Piece::WhiteKnight, .target_square = Square::D5}, moves);
    CHECK(moves1_1.size() == 2);
    CHECK(move_list_contains(moves1_1, Move{.from = Square::B4, .to = Square::D5, .piece = Piece::WhiteKnight}));
    CHECK(move_list_contains(moves1_1, Move{.from = Square::E3, .to = Square::D5, .piece = Piece::WhiteKnight}));

    const auto moves1_2 = match_san_move(SANMove{.san_string = "Nbd5", .moving_piece = Piece::WhiteKnight, .target_square = Square::D5, .disambiguation_file = File{'b'}}, moves);
    CHECK(moves1_2.size() == 1);
    CHECK(move_list_contains(moves1_2, Move{.from = Square::B4, .to = Square::D5, .piece = Piece::WhiteKnight}));

    const auto moves1_3 = match_san_move(SANMove{.san_string = "Ned5", .moving_piece = Piece::WhiteKnight, .target_square = Square::D5, .disambiguation_file = File{'e'}}, moves);
    CHECK(moves1_3.size() == 1);
    CHECK(move_list_contains(moves1_3, Move{.from = Square::E3, .to = Square::D5, .piece = Piece::WhiteKnight}));

    const auto moves2_1 = match_san_move(SANMove{.san_string = "Rf4", .moving_piece = Piece::BlackRook, .target_square = Square::F4}, moves);
    CHECK(moves2_1.size() == 2);
    CHECK(move_list_contains(moves2_1, Move{.from = Square::F2, .to = Square::F4, .piece = Piece::BlackRook}));
    CHECK(move_list_contains(moves2_1, Move{.from = Square::F6, .to = Square::F4, .piece = Piece::BlackRook}));

    const auto moves2_2 = match_san_move(SANMove{.san_string = "R2f4", .moving_piece = Piece::BlackRook, .target_square = Square::F4, .disambiguation_rank = Rank{2}}, moves);
    CHECK(moves2_2.size() == 1);
    CHECK(move_list_contains(moves2_2, Move{.from = Square::F2, .to = Square::F4, .piece = Piece::BlackRook}));

    const auto moves2_3 = match_san_move(SANMove{.san_string = "R6f4", .moving_piece = Piece::BlackRook, .target_square = Square::F4, .disambiguation_rank = Rank{6}}, moves);
    CHECK(moves2_3.size() == 1);
    CHECK(move_list_contains(moves2_3, Move{.from = Square::F6, .to = Square::F4, .piece = Piece::BlackRook}));
}

TEST_CASE("Move Matcher.Capture", "[san][move_matcher]") {
    const MoveList moves{
        Move{.from = Square::C1, .to = Square::G5, .piece = Piece::WhiteBishop, .captured = Piece::BlackQueen}, // Bxg5
        Move{.from = Square::E5, .to = Square::C4, .piece = Piece::BlackKnight, .captured = Piece::WhitePawn},  // Nxc4
        Move{.from = Square::E3, .to = Square::C4, .piece = Piece::BlackKnight},                                // Nc4
    };

    const auto moves1 = match_san_move(SANMove{.san_string = "Bxg5", .moving_piece = Piece::WhiteBishop, .target_square = Square::G5, .capturing = true}, moves);
    CHECK(moves1.size() == 1);
    CHECK(move_list_contains(moves1, Move{.from = Square::C1, .to = Square::G5, .piece = Piece::WhiteBishop, .captured = Piece::BlackQueen}));

    const auto moves2 = match_san_move(SANMove{.san_string = "Nxc4", .moving_piece = Piece::BlackKnight, .target_square = Square::C4, .capturing = true}, moves);
    CHECK(moves2.size() == 1);
    CHECK(move_list_contains(moves2, Move{.from = Square::E5, .to = Square::C4, .piece = Piece::BlackKnight, .captured = Piece::WhitePawn}));
}
