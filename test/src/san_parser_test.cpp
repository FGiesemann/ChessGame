/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>
#include <chesscore/piece.h>

#include "chessgame/san.h"

using namespace chessgame;
using namespace chesscore;

auto check_move(const SANMove &move, Piece piece, Square target_square, bool capturing, CheckState check_state) -> bool {
    return move.piece() == piece && move.target_square() == target_square && move.capturing() == capturing && move.check_state() == check_state;
}

auto check_move(const SANMove &move, Piece piece, Square target_square, bool capturing, CheckState check_state, File from_file) -> bool {
    return check_move(move, piece, target_square, capturing, check_state) && move.from_file() == from_file;
}

auto check_move(const SANMove &move, Piece piece, Square target_square, bool capturing, CheckState check_state, Rank from_rank) -> bool {
    return check_move(move, piece, target_square, capturing, check_state) && move.from_rank() == from_rank;
}

TEST_CASE("SAN Parser.Castling", "[san]") {
    CHECK(check_move(parse_san("O-O", Color::White), Piece::WhiteKing, Square::G1, false, CheckState::None));
    CHECK(check_move(parse_san("O-O", Color::Black), Piece::BlackKing, Square::G8, false, CheckState::None));
    CHECK(check_move(parse_san("O-O-O", Color::White), Piece::WhiteKing, Square::C1, false, CheckState::None));
    CHECK(check_move(parse_san("O-O-O", Color::Black), Piece::BlackKing, Square::C8, false, CheckState::None));
}

TEST_CASE("SAN Parser.Pawn", "[san]") {
    CHECK(check_move(parse_san("e4", Color::White), Piece::WhitePawn, Square::E4, false, CheckState::None));
    CHECK(check_move(parse_san("c6", Color::Black), Piece::BlackPawn, Square::C6, false, CheckState::None));
    CHECK(check_move(parse_san("dxe5", Color::White), Piece::WhitePawn, Square::E5, true, CheckState::None, File{'d'}));
    CHECK(check_move(parse_san("bxa6", Color::Black), Piece::BlackPawn, Square::A6, true, CheckState::None, File{'b'}));
    CHECK(check_move(parse_san("d7+", Color::White), Piece::WhitePawn, Square::D7, false, CheckState::Check));
    CHECK(check_move(parse_san("dxe3#", Color::Black), Piece::BlackPawn, Square::E3, true, CheckState::Checkmate));
}
