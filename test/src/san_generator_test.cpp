/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chesscore/bitboard.h"
#include "chesscore/fen.h"
#include "chesscore/position.h"
#include "chesscore_io/chesscore_io.h"

#include "chessgame/san.h"

using namespace chessgame;
using namespace chesscore;

namespace {

auto check_san_move(const Move &move, const SANMove &san_move, const MoveList &moves) -> void {
    CAPTURE(move);
    CAPTURE(san_move.san_string);
    const auto generated_san_move = generate_san_move(move, moves);
    REQUIRE(generated_san_move.has_value());
    CAPTURE(generated_san_move.value().san_string);
    CHECK(generated_san_move.value() == san_move);
}

} // namespace

TEST_CASE("SAN.Generator.Simple Pawn Moves", "[san]") {
    const Position position{FenString{"r1q1krn1/1p4b1/2pn1p2/pP2p1p1/1N1pPP2/2Q2N2/1pP2PPP/5RK1 w - - 0 1"}};
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
    const Position position{FenString{"r1q1krn1/1p4b1/2pn1p2/pP2p1p1/1N1pPP2/2Q2N2/1pP2PPP/5RK1 b - e3 0 1"}};
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
        Move{.from = Square::D4, .to = Square::E3, .piece = Piece::BlackPawn, .captured = Piece::WhitePawn, .capturing_en_passant = true},
        SANMove{.san_string{"dxe3"}, .moving_piece = Piece::BlackPawn, .target_square = Square::E3, .capturing = true}, moves
    );
}

TEST_CASE("SAN.Generator.Simple Piece Moves", "[san]") {
    const Position position{FenString{"5k2/2b5/4rN2/1n5b/5N2/1q5r/2R2Q2/4B3 w - - 0 1"}};
    const auto moves = position.all_legal_moves();

    check_san_move(
        Move{.from = Square::C2, .to = Square::C4, .piece = Piece::WhiteRook}, SANMove{.san_string{"Rc4"}, .moving_piece = Piece::WhiteRook, .target_square = Square::C4}, moves
    );
    check_san_move(
        Move{.from = Square::F6, .to = Square::H7, .piece = Piece::WhiteKnight}, SANMove{.san_string{"Nh7"}, .moving_piece = Piece::WhiteKnight, .target_square = Square::H7}, moves
    );
    check_san_move(
        Move{.from = Square::E1, .to = Square::B4, .piece = Piece::WhiteBishop}, SANMove{.san_string{"Bb4"}, .moving_piece = Piece::WhiteBishop, .target_square = Square::B4}, moves
    );
    check_san_move(
        Move{.from = Square::F4, .to = Square::E6, .piece = Piece::WhiteKnight, .captured = Piece::BlackRook},
        SANMove{.san_string{"Nxe6"}, .moving_piece = Piece::WhiteKnight, .target_square = Square::E6, .capturing = true}, moves
    );
}

TEST_CASE("SAN.Generator.Disambiguation", "[san]") {
    Position positon{FenString{"4k3/8/2r2n2/4P2q/B7/n7/3nq2q/n4r2 b - - 0 1"}};
    const auto moves = positon.all_legal_moves();

    check_san_move( // no disambiguation necessary, rook on c6 is pinned
        Move{.from = Square::F1, .to = Square::C1, .piece = Piece::BlackRook},
        SANMove{.san_string{"Rc1"}, .moving_piece = Piece::BlackRook, .target_square = Square::C1}, moves
    );
    check_san_move( // file disambiguation
        Move{.from = Square::D2, .to = Square::E4, .piece = Piece::BlackKnight},
        SANMove{.san_string{"Nde4"}, .moving_piece = Piece::BlackKnight, .target_square = Square::E4, .disambiguation_file = File{'d'}}, moves
    );
    check_san_move( // rank disambiguation
        Move{.from = Square::A3, .to = Square::C2, .piece = Piece::BlackKnight},
        SANMove{.san_string{"N3c2"}, .moving_piece = Piece::BlackKnight, .target_square = Square::C2, .disambiguation_rank = Rank{3}}, moves
    );
    check_san_move( // square disambiguation
        Move{.from = Square::H5, .to = Square::E5, .piece = Piece::BlackQueen, .captured = Piece::WhitePawn},
        SANMove{.san_string{"Qh5xe5"}, .moving_piece = Piece::BlackQueen, .target_square = Square::E5, .capturing = true, .disambiguation_file = File{'h'}, .disambiguation_rank = Rank{5}}, moves
    );
}

TEST_CASE("SAN.Generator.Castling Moves", "[san]") {
    const Position position_w{FenString{"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"}};
    const auto white_moves = position_w.all_legal_moves();
    const Position position_b{FenString{"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1"}};
    const auto black_moves = position_b.all_legal_moves();

    check_san_move(
        Move{.from = Square::E1, .to = Square::C1, .piece = Piece::WhiteKing}, SANMove{.san_string{"O-O-O"}, .moving_piece = Piece::WhiteKing, .target_square = Square::C1},
        white_moves
    );
    check_san_move(
        Move{.from = Square::E8, .to = Square::C8, .piece = Piece::BlackKing}, SANMove{.san_string{"O-O-O"}, .moving_piece = Piece::BlackKing, .target_square = Square::C8},
        black_moves
    );
    check_san_move(
        Move{.from = Square::E1, .to = Square::G1, .piece = Piece::WhiteKing}, SANMove{.san_string{"O-O"}, .moving_piece = Piece::WhiteKing, .target_square = Square::G1},
        white_moves
    );
    check_san_move(
        Move{.from = Square::E8, .to = Square::G8, .piece = Piece::BlackKing}, SANMove{.san_string{"O-O"}, .moving_piece = Piece::BlackKing, .target_square = Square::G8},
        black_moves
    );
}

TEST_CASE("SAN.Generator.Invalid Move", "[san]") {
    Position position{FenString{"1k3q2/pp6/2n3n1/8/1B4P1/5rN1/2N5/R1K2Q2 w - - 0 1"}};
    const auto white_moves = position.all_legal_moves();
    position.make_move(Move{.from = Square::A1, .to = Square::B1, .piece = Piece::WhiteRook});
    const auto black_moves = position.all_legal_moves();

    CHECK(  // no piece at start square; no bishop can move there
        generate_san_move(Move{.from = Square::D3, .to = Square::F5, .piece = Piece::WhiteBishop}, white_moves) == std::nullopt
    );
    CHECK(generate_san_move(Move{.from = Square::F6, .to = Square::C3, .piece = Piece::WhiteBishop}, white_moves) == std::nullopt); // no bishop at start square
    CHECK(generate_san_move(Move{.from = Square::B4, .to = Square::D7, .piece = Piece::WhiteBishop}, white_moves) == std::nullopt); // bishop cannot move there
    CHECK(generate_san_move(Move{.from = Square::C2, .to = Square::E4, .piece = Piece::WhiteKnight}, white_moves) == std::nullopt); // only other knight can move there
    CHECK(generate_san_move(Move{.from = Square::G3, .to = Square::E5, .piece = Piece::WhiteKnight}, white_moves) == std::nullopt); // no knight can move there
}
