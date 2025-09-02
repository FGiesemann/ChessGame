/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include <chesscore/bitboard.h>
#include <chesscore/fen.h>
#include <chesscore/position.h>
#include <chesscore_io/chesscore_io.h>

#include "chessgame/move_matcher.h"
#include "chessgame/san.h"

using namespace chessgame;
using namespace chesscore;

auto check_san_move(const Move &move, const SANMove &san_move, const MoveList &moves) -> void {
    CAPTURE(move);
    CAPTURE(san_move.san_string);
    const auto generated_san_move = generate_san_move(move, moves);
    REQUIRE(generated_san_move.has_value());
    CAPTURE(generated_san_move.value().san_string);
    REQUIRE(generated_san_move.value() == san_move);
}

TEST_CASE("SAN.Generator.Simple Pawn Moves", "[san]") {
    const Position<Bitboard> position{FenString{"r1q1krn1/1p4b1/2pn1p2/pP2p1p1/1N1pPP2/2Q2N2/1pP2PPP/5RK1 w - - 0 1"}};
    const auto moves = position.all_legal_moves();

    check_san_move(
        Move{.from = Square::F4, .to = Square::F5, .piece = Piece::WhitePawn}, SANMove{.san_string{"f5"}, .moving_piece = Piece::WhitePawn, .target_square = Square::F5}, moves
    );
    check_san_move(
        Move{.from = Square::H2, .to = Square::H4, .piece = Piece::WhitePawn}, SANMove{.san_string{"h4"}, .moving_piece = Piece::WhitePawn, .target_square = Square::H4}, moves
    );
    check_san_move(
        Move{.from = Square::B5, .to = Square::C6, .piece = Piece::WhitePawn, .captured = Piece::BlackPawn},
        SANMove{.san_string{"bxc6"}, .moving_piece = Piece::WhitePawn, .target_square = Square::C6, .capturing = true}, moves
    );
}

TEST_CASE("SAN.Generator.Pawn Moves", "[san]") {
    const Position<Bitboard> position{FenString{"r1q1krn1/1p4b1/2pn1p2/pP2p1p1/1N1pPP2/2Q2N2/1pP2PPP/5RK1 b - e3 0 1"}};
    const auto moves = position.all_legal_moves();

    check_san_move(
        Move{.from = Square::B2, .to = Square::B1, .piece = Piece::BlackPawn, .promoted = Piece::BlackQueen},
        SANMove{.san_string{"b1=Q"}, .moving_piece = Piece::BlackPawn, .target_square = Square::B1, .promotion = Piece::BlackQueen}, moves
    );
    check_san_move(
        Move{.from = Square::A5, .to = Square::B4, .piece = Piece::BlackPawn, .captured = Piece::WhiteKnight},
        SANMove{.san_string{"axb4"}, .moving_piece = Piece::BlackPawn, .target_square = Square::B4, .capturing = true}, moves
    );
    check_san_move(
        Move{.from = Square::E5, .to = Square::F4, .piece = Piece::BlackPawn, .captured = Piece::WhitePawn},
        SANMove{.san_string{"exf4"}, .moving_piece = Piece::BlackPawn, .target_square = Square::F4, .capturing = true}, moves
    );
    check_san_move(
        Move{.from = Square::D4, .to = Square::E3, .piece = Piece::BlackPawn, .captured = Piece::WhiteQueen, .capturing_en_passant = true},
        SANMove{.san_string{"dxe3"}, .moving_piece = Piece::BlackPawn, .target_square = Square::E3, .capturing = true}, moves
    );
}
