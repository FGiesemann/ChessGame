/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>
#include <chesscore/piece.h>

#include "chessgame/san.h"

using namespace chessgame;
using namespace chesscore;

auto check_move(
    const SANMove &move, Piece piece, Square target_square, bool capturing, std::optional<chesscore::Piece> promotion, CheckState check_state,
    std::optional<SuffixAnnotation> suffix_annotation
) -> bool {
    return move.moving_piece == piece && move.target_square == target_square && move.capturing == capturing && move.check_state == check_state && move.promotion == promotion &&
           move.suffix_annotation == suffix_annotation;
}

auto check_move(
    const SANMove &move, Piece piece, Square target_square, bool capturing, std::optional<chesscore::Piece> promotion, CheckState check_state, File from_file,
    std::optional<SuffixAnnotation> suffix_annotation
) -> bool {
    return check_move(move, piece, target_square, capturing, promotion, check_state, suffix_annotation) && move.disambiguation_file == from_file;
}

auto check_move(
    const SANMove &move, Piece piece, Square target_square, bool capturing, std::optional<chesscore::Piece> promotion, CheckState check_state, Rank from_rank,
    std::optional<SuffixAnnotation> suffix_annotation
) -> bool {
    return check_move(move, piece, target_square, capturing, promotion, check_state, suffix_annotation) && move.disambiguation_rank == from_rank;
}

TEST_CASE("SAN Parser.Castling", "[san]") {
    CHECK(check_move(parse_san("O-O", Color::White), Piece::WhiteKing, Square::G1, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("O-O", Color::Black), Piece::BlackKing, Square::G8, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("O-O-O", Color::White), Piece::WhiteKing, Square::C1, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("O-O-O", Color::Black), Piece::BlackKing, Square::C8, false, std::nullopt, CheckState::None, std::nullopt));
}

TEST_CASE("SAN Parser.Pieces", "[san]") {
    CHECK(check_move(parse_san("e4", Color::White), Piece::WhitePawn, Square::E4, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("c6", Color::Black), Piece::BlackPawn, Square::C6, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("Nf3", Color::White), Piece::WhiteKnight, Square::F3, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("Rc1", Color::Black), Piece::BlackRook, Square::C1, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("Ba3", Color::White), Piece::WhiteBishop, Square::A3, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("Qd4", Color::Black), Piece::BlackQueen, Square::D4, false, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("Kg8", Color::White), Piece::WhiteKing, Square::G8, false, std::nullopt, CheckState::None, std::nullopt));
}

TEST_CASE("SAN Parser.Captures", "[san]") {
    CHECK(check_move(parse_san("Bxe5", Color::White), Piece::WhiteBishop, Square::E5, true, std::nullopt, CheckState::None, std::nullopt));
    CHECK(check_move(parse_san("Qxd4", Color::Black), Piece::BlackQueen, Square::D4, true, std::nullopt, CheckState::None, std::nullopt));
}

TEST_CASE("SAN Parser.Disambiguation", "[san]") {
    CHECK(check_move(parse_san("Ncd4", Color::White), Piece::WhiteKnight, Square::D4, false, std::nullopt, CheckState::None, File{'c'}, std::nullopt));
    CHECK(check_move(parse_san("Ned4", Color::White), Piece::WhiteKnight, Square::D4, false, std::nullopt, CheckState::None, File{'e'}, std::nullopt));
    CHECK(check_move(parse_san("N3d4", Color::White), Piece::WhiteKnight, Square::D4, false, std::nullopt, CheckState::None, Rank{3}, std::nullopt));
    CHECK(check_move(parse_san("Rbd6", Color::Black), Piece::BlackRook, Square::D6, false, std::nullopt, CheckState::None, File{'b'}, std::nullopt));
    CHECK(check_move(parse_san("Rfd6", Color::Black), Piece::BlackRook, Square::D6, false, std::nullopt, CheckState::None, File{'f'}, std::nullopt));
    CHECK(check_move(parse_san("dxe5", Color::White), Piece::WhitePawn, Square::E5, true, std::nullopt, CheckState::None, File{'d'}, std::nullopt));
    CHECK(check_move(parse_san("bxa6", Color::Black), Piece::BlackPawn, Square::A6, true, std::nullopt, CheckState::None, File{'b'}, std::nullopt));
    CHECK(check_move(parse_san("R5xe2", Color::Black), Piece::BlackRook, Square::E2, true, std::nullopt, CheckState::None, Rank{5}, std::nullopt));
}

TEST_CASE("SAN Parser.Check", "[san]") {
    CHECK(check_move(parse_san("d7+", Color::White), Piece::WhitePawn, Square::D7, false, std::nullopt, CheckState::Check, std::nullopt));
    CHECK(check_move(parse_san("dxe3#", Color::Black), Piece::BlackPawn, Square::E3, true, std::nullopt, CheckState::Checkmate, std::nullopt));
    CHECK(check_move(parse_san("Qe3+", Color::White), Piece::WhiteQueen, Square::E3, false, std::nullopt, CheckState::Check, std::nullopt));
    CHECK(check_move(parse_san("Qxe3#", Color::Black), Piece::BlackQueen, Square::E3, true, std::nullopt, CheckState::Checkmate, std::nullopt));
}

TEST_CASE("SAN Parser.Pawn Promotion", "[san]") {
    CHECK(check_move(parse_san("exd8=Q#", Color::White), Piece::WhitePawn, Square::D8, true, Piece::WhiteQueen, CheckState::Checkmate, std::nullopt));
}

TEST_CASE("SAN Parser.Suffix Annotations", "[san]") {
    CHECK(check_move(parse_san("Qxe2!", Color::White), Piece::WhiteQueen, Square::E2, true, std::nullopt, CheckState::None, SuffixAnnotation::GoodMove));
    CHECK(check_move(parse_san("Bec3!?", Color::Black), Piece::BlackBishop, Square::C3, false, std::nullopt, CheckState::None, SuffixAnnotation::SpeculativeMove));
    CHECK(check_move(parse_san("O-O??", Color::White), Piece::WhiteKing, Square::G1, false, std::nullopt, CheckState::None, SuffixAnnotation::VeryPoorMove));
    CHECK(check_move(parse_san("O-O?", Color::Black), Piece::BlackKing, Square::G8, false, std::nullopt, CheckState::None, SuffixAnnotation::PoorMove));
    CHECK(check_move(parse_san("exd8=Q+!!", Color::White), Piece::WhitePawn, Square::D8, true, Piece::WhiteQueen, CheckState::Check, SuffixAnnotation::VeryGoodMove));
    CHECK(check_move(parse_san("dxe2?!", Color::Black), Piece::BlackPawn, Square::E2, true, std::nullopt, CheckState::None, SuffixAnnotation::QuestionableMove));
}
