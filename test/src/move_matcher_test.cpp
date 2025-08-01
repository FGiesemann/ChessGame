/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include <chesscore/move.h>
#include <chessgame/move_matcher.h>

using namespace chessgame;
using namespace chesscore;

TEST_CASE("Move Matcher.Unambiguous Moves", "[move_matcher]") {
    MoveList moves{
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
