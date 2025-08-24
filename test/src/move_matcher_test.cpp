/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/san.h"
#include <catch2/catch_all.hpp>
#include <catch2/internal/catch_string_manip.hpp>

#include "chesscore/move.h"
#include "chessgame/move_matcher.h"
#include "chessgame/types.h"

using namespace chessgame;
using namespace chesscore;

TEST_CASE("Move Matcher.SAN Move.Simple", "[san][move_matcher]") {
    CHECK(san_move_matches(
        SANMove{.san_string = "e4", .moving_piece = Piece::WhitePawn, .target_square = Square::E4}, Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "e4", .moving_piece = Piece::WhitePawn, .target_square = Square::E4}, Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhiteBishop}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "e4", .moving_piece = Piece::WhitePawn, .target_square = Square::E4}, Move{.from = Square::E2, .to = Square::E3, .piece = Piece::WhitePawn}
    ));
    CHECK(san_move_matches(
        SANMove{.san_string = "Qf3", .moving_piece = Piece::WhiteQueen, .target_square = Square::F3}, Move{.from = Square::D1, .to = Square::F3, .piece = Piece::WhiteQueen}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "Qf3", .moving_piece = Piece::WhiteQueen, .target_square = Square::F3}, Move{.from = Square::D1, .to = Square::F3, .piece = Piece::WhiteKnight}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "Qf3", .moving_piece = Piece::WhiteQueen, .target_square = Square::F3}, Move{.from = Square::D1, .to = Square::F4, .piece = Piece::WhiteQueen}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "Qf3", .moving_piece = Piece::WhiteQueen, .target_square = Square::F3}, Move{.from = Square::D1, .to = Square::F3, .piece = Piece::BlackQueen}
    ));
    CHECK(san_move_matches(
        SANMove{.san_string = "Nce5", .moving_piece = Piece::WhiteKnight, .target_square = Square::E5}, Move{.from = Square::C4, .to = Square::E5, .piece = Piece::WhiteKnight}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "Nce5", .moving_piece = Piece::WhiteKnight, .target_square = Square::E5}, Move{.from = Square::D3, .to = Square::E6, .piece = Piece::WhiteKnight}
    ));
    CHECK(san_move_matches(
        SANMove{.san_string = "R2e4", .moving_piece = Piece::WhiteRook, .target_square = Square::E4, .disambiguation_rank = Rank{2}},
        Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhiteRook}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "R2e4", .moving_piece = Piece::WhiteRook, .target_square = Square::E4, .disambiguation_rank = Rank{1}},
        Move{.from = Square::E8, .to = Square::E4, .piece = Piece::WhiteRook}
    ));
}

TEST_CASE("Move Matcher.SAN Move.Capture", "[san][move_matcher]") {
    CHECK(san_move_matches(
        SANMove{.san_string = "Qxg4", .moving_piece = Piece::WhiteQueen, .target_square = Square::G4, .capturing = true},
        Move{.from = Square::D1, .to = Square::G4, .piece = Piece::WhiteQueen, .captured = Piece::BlackQueen}
    ));
    CHECK_FALSE(san_move_matches(
        SANMove{.san_string = "Qxg4", .moving_piece = Piece::WhiteQueen, .target_square = Square::G4, .capturing = true},
        Move{.from = Square::D1, .to = Square::G4, .piece = Piece::WhiteQueen}
    ));
}

TEST_CASE("Move Matcher.SAN Move.Promotion", "[san][move_matcher]") {
    CHECK(san_move_matches(
        SANMove{.san_string = "c8=Q", .moving_piece = Piece::WhitePawn, .target_square = Square::C8, .promotion = Piece::WhiteQueen},
        Move{.from = Square::C7, .to = Square::C8, .piece = Piece::WhitePawn, .promoted = Piece::WhiteQueen}
    ));
}

TEST_CASE("Move Matcher.List.Unambiguous Moves", "[san][move_matcher]") {
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

TEST_CASE("Move Matcher.List.Disambiguations", "[san][move_matcher]") {
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

TEST_CASE("Move Matcher.List.Capture", "[san][move_matcher]") {
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

TEST_CASE("Move Matcher.List.Promotion", "[san][move_matcher]") {
    const MoveList moves{
        Move{.from = Square::E7, .to = Square::E8, .piece = Piece::WhitePawn, .promoted = Piece::WhiteQueen},                                // e8=Q
        Move{.from = Square::C2, .to = Square::C1, .piece = Piece::BlackPawn, .promoted = Piece::BlackKnight},                               // c1=N
        Move{.from = Square::E2, .to = Square::D1, .piece = Piece::BlackPawn, .captured = Piece::BlackRook, .promoted = Piece::WhiteBishop}, // exd1=B
        Move{.from = Square::A7, .to = Square::B8, .piece = Piece::WhitePawn, .captured = Piece::BlackQueen, .promoted = Piece::WhiteRook},  // axb8=R
        Move{.from = Square::C7, .to = Square::C8, .piece = Piece::WhitePawn, .promoted = Piece::WhiteQueen},                                // c8=Q
    };

    const auto moves1 = match_san_move(SANMove{.san_string = "e8=Q", .moving_piece = Piece::WhitePawn, .target_square = Square::E8, .promotion = Piece::WhiteQueen}, moves);
    CHECK(moves1.size() == 1);
    CHECK(move_list_contains(moves1, Move{.from = Square::E7, .to = Square::E8, .piece = Piece::WhitePawn, .promoted = Piece::WhiteQueen}));

    const auto moves2 = match_san_move(SANMove{.san_string = "e8=Q", .moving_piece = Piece::WhitePawn, .target_square = Square::E8, .promotion = Piece::WhiteBishop}, moves);
    CHECK(moves2.size() == 0);

    const auto moves3 = match_san_move(SANMove{.san_string = "c1=N", .moving_piece = Piece::BlackPawn, .target_square = Square::C1, .promotion = Piece::BlackKnight}, moves);
    CHECK(moves3.size() == 1);
    CHECK(move_list_contains(moves3, Move{.from = Square::C2, .to = Square::C1, .piece = Piece::BlackPawn, .promoted = Piece::BlackKnight}));

    const auto moves4 = match_san_move(
        SANMove{
            .san_string = "exd1=B",
            .moving_piece = Piece::BlackPawn,
            .target_square = Square::D1,
            .capturing = true,
            .promotion = Piece::WhiteBishop,
            .disambiguation_file = File{'e'}
        },
        moves
    );
    CHECK(moves4.size() == 1);
    CHECK(move_list_contains(moves4, Move{.from = Square::E2, .to = Square::D1, .piece = Piece::BlackPawn, .captured = Piece::BlackRook, .promoted = Piece::WhiteBishop}));

    const auto moves5 = match_san_move(
        SANMove{
            .san_string = "axb8=R",
            .moving_piece = Piece::WhitePawn,
            .target_square = Square::B8,
            .capturing = true,
            .promotion = Piece::WhiteRook,
            .disambiguation_file = File{'a'}
        },
        moves
    );
    CHECK(moves5.size() == 1);
    CHECK(move_list_contains(moves5, Move{.from = Square::A7, .to = Square::B8, .piece = Piece::WhitePawn, .captured = Piece::BlackQueen, .promoted = Piece::WhiteRook}));

    const auto moves6 = match_san_move(SANMove{.san_string = "axb8", .moving_piece = Piece::WhitePawn, .target_square = Square::B8, .capturing = true}, moves);
    CHECK(moves6.size() == 0);

    const auto moves7 = match_san_move(SANMove{.san_string = "c8=Q", .moving_piece = Piece::WhitePawn, .target_square = Square::C8, .promotion = Piece::WhiteQueen}, moves);
    CHECK(moves7.size() == 1);
    CHECK(move_list_contains(moves7, Move{.from = Square::C7, .to = Square::C8, .piece = Piece::WhitePawn, .promoted = Piece::WhiteQueen}));
}

auto check_single_move(const std::string &san_str, Color side_to_move, const MoveList &moves, const Move &expected_move) -> void {
    const auto expected_san_move = parse_san(san_str, side_to_move);
    CAPTURE(san_str);
    REQUIRE(expected_san_move.has_value());
    const auto matched_moves = match_san_move(*expected_san_move, moves);
    CHECK(matched_moves.size() == 1);
    CHECK(move_list_contains(matched_moves, expected_move));
}

TEST_CASE("Move Matcher.Position.White", "[san][move_matcher]") {
    const chessgame::Position position_w{FenString{"3r4/ppP4k/1q2Np1r/b4Np1/1RP1B1nP/3n3P/P4B2/1KR5 w - - 0 1"}};

    const auto white_moves = position_w.all_legal_moves();
    CHECK(white_moves.size() == 55);
    check_single_move("a3", Color::White, white_moves, Move{.from = Square::A2, .to = Square::A3, .piece = Piece::WhitePawn});
    check_single_move("a4", Color::White, white_moves, Move{.from = Square::A2, .to = Square::A4, .piece = Piece::WhitePawn});
    check_single_move("Rxb6", Color::White, white_moves, Move{.from = Square::B4, .to = Square::B6, .piece = Piece::WhiteRook, .captured = Piece::BlackQueen});
    check_single_move("Rb5", Color::White, white_moves, Move{.from = Square::B4, .to = Square::B5, .piece = Piece::WhiteRook});
    check_single_move("Rb3", Color::White, white_moves, Move{.from = Square::B4, .to = Square::B3, .piece = Piece::WhiteRook});
    check_single_move("Rb2", Color::White, white_moves, Move{.from = Square::B4, .to = Square::B2, .piece = Piece::WhiteRook});
    check_single_move("Ka1", Color::White, white_moves, Move{.from = Square::B1, .to = Square::A1, .piece = Piece::WhiteKing});
    check_single_move("Kc2", Color::White, white_moves, Move{.from = Square::B1, .to = Square::C2, .piece = Piece::WhiteKing});
    check_single_move("c8=Q", Color::White, white_moves, Move{.from = Square::C7, .to = Square::C8, .piece = Piece::WhitePawn, .promoted = Piece::WhiteQueen});
    check_single_move(
        "cxd8=B", Color::White, white_moves, Move{.from = Square::C7, .to = Square::D8, .piece = Piece::WhitePawn, .captured = Piece::BlackRook, .promoted = Piece::WhiteBishop}
    );
    check_single_move("c5", Color::White, white_moves, Move{.from = Square::C4, .to = Square::C5, .piece = Piece::WhitePawn});
    check_single_move("Rd1", Color::White, white_moves, Move{.from = Square::C1, .to = Square::D1, .piece = Piece::WhiteRook});
    check_single_move("Re1", Color::White, white_moves, Move{.from = Square::C1, .to = Square::E1, .piece = Piece::WhiteRook});
    check_single_move("Rf1", Color::White, white_moves, Move{.from = Square::C1, .to = Square::F1, .piece = Piece::WhiteRook});
    check_single_move("Rg1", Color::White, white_moves, Move{.from = Square::C1, .to = Square::G1, .piece = Piece::WhiteRook});
    check_single_move("Rh1", Color::White, white_moves, Move{.from = Square::C1, .to = Square::H1, .piece = Piece::WhiteRook});
    check_single_move("Rc2", Color::White, white_moves, Move{.from = Square::C1, .to = Square::C2, .piece = Piece::WhiteRook});
    check_single_move("Rc3", Color::White, white_moves, Move{.from = Square::C1, .to = Square::C3, .piece = Piece::WhiteRook});
    check_single_move("Nxd8", Color::White, white_moves, Move{.from = Square::E6, .to = Square::D8, .piece = Piece::WhiteKnight, .captured = Piece::BlackRook});
    check_single_move("Nc5", Color::White, white_moves, Move{.from = Square::E6, .to = Square::C5, .piece = Piece::WhiteKnight});
    check_single_move("Ned4", Color::White, white_moves, Move{.from = Square::E6, .to = Square::D4, .piece = Piece::WhiteKnight});
    check_single_move("Nf4", Color::White, white_moves, Move{.from = Square::E6, .to = Square::F4, .piece = Piece::WhiteKnight});
    check_single_move("Nxg5+", Color::White, white_moves, Move{.from = Square::E6, .to = Square::G5, .piece = Piece::WhiteKnight, .captured = Piece::BlackPawn});
    check_single_move("Neg7", Color::White, white_moves, Move{.from = Square::E6, .to = Square::G7, .piece = Piece::WhiteKnight});
    check_single_move("Nf8+", Color::White, white_moves, Move{.from = Square::E6, .to = Square::F8, .piece = Piece::WhiteKnight});
    check_single_move("Bxb7", Color::White, white_moves, Move{.from = Square::E4, .to = Square::B7, .piece = Piece::WhiteBishop, .captured = Piece::BlackPawn});
    check_single_move("Bc6", Color::White, white_moves, Move{.from = Square::E4, .to = Square::C6, .piece = Piece::WhiteBishop});
    check_single_move("Bd5", Color::White, white_moves, Move{.from = Square::E4, .to = Square::D5, .piece = Piece::WhiteBishop});
    check_single_move("Bf3", Color::White, white_moves, Move{.from = Square::E4, .to = Square::F3, .piece = Piece::WhiteBishop});
    check_single_move("Bg2", Color::White, white_moves, Move{.from = Square::E4, .to = Square::G2, .piece = Piece::WhiteBishop});
    check_single_move("Bh1", Color::White, white_moves, Move{.from = Square::E4, .to = Square::H1, .piece = Piece::WhiteBishop});
    check_single_move("Bxd3", Color::White, white_moves, Move{.from = Square::E4, .to = Square::D3, .piece = Piece::WhiteBishop, .captured = Piece::BlackKnight});
    check_single_move("Ne7", Color::White, white_moves, Move{.from = Square::F5, .to = Square::E7, .piece = Piece::WhiteKnight});
    check_single_move("Nd6", Color::White, white_moves, Move{.from = Square::F5, .to = Square::D6, .piece = Piece::WhiteKnight});
    check_single_move("Nfd4", Color::White, white_moves, Move{.from = Square::F5, .to = Square::D4, .piece = Piece::WhiteKnight});
    check_single_move("Ne3", Color::White, white_moves, Move{.from = Square::F5, .to = Square::E3, .piece = Piece::WhiteKnight});
    check_single_move("Ng3", Color::White, white_moves, Move{.from = Square::F5, .to = Square::G3, .piece = Piece::WhiteKnight});
    check_single_move("Nxh6", Color::White, white_moves, Move{.from = Square::F5, .to = Square::H6, .piece = Piece::WhiteKnight, .captured = Piece::BlackRook});
    check_single_move("Nfg7", Color::White, white_moves, Move{.from = Square::F5, .to = Square::G7, .piece = Piece::WhiteKnight});
    check_single_move("Bxb6", Color::White, white_moves, Move{.from = Square::F2, .to = Square::B6, .piece = Piece::WhiteBishop, .captured = Piece::BlackQueen});
    check_single_move("Bc5", Color::White, white_moves, Move{.from = Square::F2, .to = Square::C5, .piece = Piece::WhiteBishop});
    check_single_move("Bd4", Color::White, white_moves, Move{.from = Square::F2, .to = Square::D4, .piece = Piece::WhiteBishop});
    check_single_move("Be3", Color::White, white_moves, Move{.from = Square::F2, .to = Square::E3, .piece = Piece::WhiteBishop});
    check_single_move("Bg1", Color::White, white_moves, Move{.from = Square::F2, .to = Square::G1, .piece = Piece::WhiteBishop});
    check_single_move("Be1", Color::White, white_moves, Move{.from = Square::F2, .to = Square::E1, .piece = Piece::WhiteBishop});
    check_single_move("Bg3", Color::White, white_moves, Move{.from = Square::F2, .to = Square::G3, .piece = Piece::WhiteBishop});
    check_single_move("hxg5", Color::White, white_moves, Move{.from = Square::H4, .to = Square::G5, .piece = Piece::WhitePawn, .captured = Piece::BlackPawn});
    check_single_move("h5", Color::White, white_moves, Move{.from = Square::H4, .to = Square::H5, .piece = Piece::WhitePawn});
    check_single_move("hxg4", Color::White, white_moves, Move{.from = Square::H3, .to = Square::G4, .piece = Piece::WhitePawn, .captured = Piece::BlackKnight});
}

TEST_CASE("Move Matcher.Position.Black", "[san][move_matcher]") {
    const chessgame::Position position_b{FenString{"3r4/ppP4k/1q2Np1r/b4Np1/1RP1B1nP/3n3P/P4B2/1KR5 b - - 0 1"}};
    const auto black_moves = position_b.all_legal_moves();
    CHECK(black_moves.size() == 43);
    check_single_move("Bxb4", Color::Black, black_moves, Move{.from = Square::A5, .to = Square::B4, .piece = Piece::BlackBishop, .captured = Piece::WhiteRook});
    check_single_move("a6", Color::Black, black_moves, Move{.from = Square::A7, .to = Square::A6, .piece = Piece::BlackPawn});
    check_single_move("Qxb4", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::B4, .piece = Piece::BlackQueen, .captured = Piece::WhiteRook});
    check_single_move("Qb5", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::B5, .piece = Piece::BlackQueen});
    check_single_move("Qa6", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::A6, .piece = Piece::BlackQueen});
    check_single_move("Qxc7", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::C7, .piece = Piece::BlackQueen, .captured = Piece::WhitePawn});
    check_single_move("Qc5", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::C5, .piece = Piece::BlackQueen});
    check_single_move("Qd4", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::D4, .piece = Piece::BlackQueen});
    check_single_move("Qe3", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::E3, .piece = Piece::BlackQueen});
    check_single_move("Qxf2", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::F2, .piece = Piece::BlackQueen, .captured = Piece::WhiteBishop});
    check_single_move("Qc6", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::C6, .piece = Piece::BlackQueen});
    check_single_move("Qd6", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::D6, .piece = Piece::BlackQueen});
    check_single_move("Qxe6", Color::Black, black_moves, Move{.from = Square::B6, .to = Square::E6, .piece = Piece::BlackQueen, .captured = Piece::WhiteKnight});
    check_single_move("Nxc1", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::C1, .piece = Piece::BlackKnight, .captured = Piece::WhiteRook});
    check_single_move("Nxb4", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::B4, .piece = Piece::BlackKnight, .captured = Piece::WhiteRook});
    check_single_move("Nc5", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::C5, .piece = Piece::BlackKnight});
    check_single_move("Nde5", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::E5, .piece = Piece::BlackKnight});
    check_single_move("Nf4", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::F4, .piece = Piece::BlackKnight});
    check_single_move("Ndxf2", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::F2, .piece = Piece::BlackKnight, .captured = Piece::WhiteBishop});
    check_single_move("Ne1", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::E1, .piece = Piece::BlackKnight});
    check_single_move("Nb2", Color::Black, black_moves, Move{.from = Square::D3, .to = Square::B2, .piece = Piece::BlackKnight});
    check_single_move("Ra8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::A8, .piece = Piece::BlackRook});
    check_single_move("Rb8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::B8, .piece = Piece::BlackRook});
    check_single_move("Rc8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::C8, .piece = Piece::BlackRook});
    check_single_move("Re8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::E8, .piece = Piece::BlackRook});
    check_single_move("Rf8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::F8, .piece = Piece::BlackRook});
    check_single_move("Rg8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::G8, .piece = Piece::BlackRook});
    check_single_move("Rh8", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::H8, .piece = Piece::BlackRook});
    check_single_move("Rd7", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::D7, .piece = Piece::BlackRook});
    check_single_move("Rd6", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::D6, .piece = Piece::BlackRook});
    check_single_move("Rd5", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::D5, .piece = Piece::BlackRook});
    check_single_move("Rd4", Color::Black, black_moves, Move{.from = Square::D8, .to = Square::D4, .piece = Piece::BlackRook});
    check_single_move("Nge5", Color::Black, black_moves, Move{.from = Square::G4, .to = Square::E5, .piece = Piece::BlackKnight});
    check_single_move("Ne3", Color::Black, black_moves, Move{.from = Square::G4, .to = Square::E3, .piece = Piece::BlackKnight});
    check_single_move("Nh2", Color::Black, black_moves, Move{.from = Square::G4, .to = Square::H2, .piece = Piece::BlackKnight});
    check_single_move("Ngxf2", Color::Black, black_moves, Move{.from = Square::G4, .to = Square::F2, .piece = Piece::BlackKnight, .captured = Piece::WhiteBishop});
    check_single_move("gxh4", Color::Black, black_moves, Move{.from = Square::G5, .to = Square::H4, .piece = Piece::BlackPawn, .captured = Piece::WhitePawn});
    check_single_move("Rg6", Color::Black, black_moves, Move{.from = Square::H6, .to = Square::G6, .piece = Piece::BlackRook});
    check_single_move("Rxh4", Color::Black, black_moves, Move{.from = Square::H6, .to = Square::H4, .piece = Piece::BlackRook, .captured = Piece::WhitePawn});
    check_single_move("Rh5", Color::Black, black_moves, Move{.from = Square::H6, .to = Square::H5, .piece = Piece::BlackRook});
    check_single_move("Kg6", Color::Black, black_moves, Move{.from = Square::H7, .to = Square::G6, .piece = Piece::BlackKing});
    check_single_move("Kg8", Color::Black, black_moves, Move{.from = Square::H7, .to = Square::G8, .piece = Piece::BlackKing});
    check_single_move("Kh8", Color::Black, black_moves, Move{.from = Square::H7, .to = Square::H8, .piece = Piece::BlackKing});
}

TEST_CASE("Move Matcher.Position.Disambiguation", "[san][move_matcher]") {
    const chessgame::Position position{FenString{"3r1rk1/1p1bqp2/p1pR1p1p/8/4P3/P4B2/1PP1QPP1/3R3K w - - 0 22"}};
    const auto white_moves = position.all_legal_moves();
    check_single_move("Rd1d2", Color::White, white_moves, Move{.from = Square::D1, .to = Square::D2, .piece = Piece::WhiteRook});
}
