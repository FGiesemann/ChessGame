/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>
#include <chesscore/piece.h>

#include "chessgame/san.h"

using namespace chessgame;
using namespace chesscore;

bool check_move(const SANMove &move, Piece piece, Square target_square, CheckState check_state) {
    return move.piece() == piece && move.target_square() == target_square && move.check_state() == check_state;
}

TEST_CASE("SAN Parser", "[san]") {
    CHECK(check_move(parse_san("O-O", Color::White), Piece::WhiteKing, Square::G1, CheckState::None));
    CHECK(check_move(parse_san("O-O", Color::Black), Piece::BlackKing, Square::G8, CheckState::None));
    CHECK(check_move(parse_san("O-O-O", Color::White), Piece::WhiteKing, Square::C1, CheckState::None));
    CHECK(check_move(parse_san("O-O-O", Color::Black), Piece::BlackKing, Square::C8, CheckState::None));
}
