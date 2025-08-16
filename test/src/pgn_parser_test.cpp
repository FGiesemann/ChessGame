/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chesscore_io/chesscore_io.h"
#include "chessgame/pgn.h"

#include <algorithm>
#include <sstream>
#include <string>

using namespace chessgame;
using namespace chesscore;

auto count_ply_on_mainline(const chessgame::Game &game) -> int {
    const auto cursor = game.cursor();
    int child_count{0};
    auto next_cursor = cursor.child(0);
    while (next_cursor.has_value()) {
        ++child_count;
        next_cursor = next_cursor.value().child(0);
    }
    return child_count;
}

using GamePath = std::vector<int>;

auto mainline(size_t depth) -> GamePath {
    GamePath path(depth);
    std::fill(path.begin(), path.end(), 0);
    return path;
}

auto get_node(const chessgame::Game &game, const GamePath &path) -> std::shared_ptr<const chessgame::GameNode> {
    auto cursor = game.cursor();
    for (size_t index = 0U; index < path.size(); ++index) {
        const auto child_cursor = cursor.child(path[index]);
        CAPTURE(index);
        REQUIRE(child_cursor.has_value());
        cursor = child_cursor.value();
    }
    return cursor.node();
}

auto get_move(const chessgame::Game &game, const GamePath &path) -> Move {
    return get_node(game, path)->move();
}

auto check_move(const chessgame::Game &game, const GamePath &path, const Move &expected_move) -> void {
    CAPTURE(path);
    const auto &move = get_move(game, path);
    CAPTURE(move);
    CHECK(move == expected_move);
}

auto has_no_following_move(const chessgame::Game &game, const GamePath &path) -> bool {
    CAPTURE(path);
    const auto &node = get_node(game, path);
    return node->child_count() == 0;
}

TEST_CASE("PGN.Parser.Simple Linear Game", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 1-0)";
    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 7);
    check_move(game, mainline(1), Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});
    check_move(game, mainline(2), Move{.from = Square::E7, .to = Square::E5, .piece = Piece::BlackPawn});
    check_move(game, mainline(3), Move{.from = Square::G1, .to = Square::F3, .piece = Piece::WhiteKnight});
    check_move(game, mainline(4), Move{.from = Square::B8, .to = Square::C6, .piece = Piece::BlackKnight});
    check_move(game, mainline(7), Move{.from = Square::B5, .to = Square::A4, .piece = Piece::WhiteBishop});
    CHECK(has_no_following_move(game, mainline(7)));
}

TEST_CASE("PGN.Parser.Game with Comments", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 {It was Pillsbury who first demonstrated the
strength of the this move, which today is routine} 4..Be7 5. Nf3 Nbd7 6. Rc1 O-O
7. e3 b6 {In order to develop the Queen Bishop on Bb7. This was the most popular
way of defending the Queen's gambit declined at the time.} 8. cxd5 {Depriving
Black of the opportunity to play dxc4 when the diagonal b7-g2 would be open for
his Queen Bishop.} 8...exd5 9. Bd3 Bb7 10. O-O c5 1-0)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 20);
    check_move(game, mainline(1), Move{.from = Square::D2, .to = Square::D4, .piece = Piece::WhitePawn});
    check_move(game, mainline(2), Move{.from = Square::D7, .to = Square::D5, .piece = Piece::BlackPawn});
    check_move(game, mainline(7), Move{.from = Square::C1, .to = Square::G5, .piece = Piece::WhiteBishop});
    check_move(game, mainline(8), Move{.from = Square::F8, .to = Square::E7, .piece = Piece::BlackBishop});
    check_move(game, mainline(12), Move{.from = Square::E8, .to = Square::G8, .piece = Piece::BlackKing});
    check_move(game, mainline(14), Move{.from = Square::B7, .to = Square::B6, .piece = Piece::BlackPawn});
    check_move(game, mainline(15), Move{.from = Square::C4, .to = Square::D5, .piece = Piece::WhitePawn, .captured = Piece::BlackPawn});
    check_move(game, mainline(16), Move{.from = Square::E6, .to = Square::D5, .piece = Piece::BlackPawn, .captured = Piece::WhitePawn});
    check_move(game, mainline(20), Move{.from = Square::C7, .to = Square::C5, .piece = Piece::BlackPawn});
}

TEST_CASE("PGN.Parser.Game with NAG", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

{The active Bishop puts White in a position to start a Kingside attack} 1. e4
e5 2. Nf3 $1 Nc6 3. Bb5 a6 4. Ba4 Nf6 $2 5. O-O Be7 $1 $32 6. Qe2 b5 7. Bb3 O-O 8. c3 8...
d5 9. d3 $1 {An excellent reply, avoiding the complications arising from 9.
exd5 and ensuring White a positional advantage since the opening of the d-file
is in his favour (as he can immediately occupy it) - Alekhine} 1-0
)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 17);
    check_move(game, mainline(3), Move{.from = Square::G1, .to = Square::F3, .piece = Piece::WhiteKnight});
    // TODO: check NAG for previous half-move
    check_move(game, mainline(4), Move{.from = Square::B8, .to = Square::C6, .piece = Piece::BlackKnight});
    check_move(game, mainline(8), Move{.from = Square::G8, .to = Square::F6, .piece = Piece::BlackKnight});
    // TODO: check NAG for previous half-move
    check_move(game, mainline(9), Move{.from = Square::E1, .to = Square::G1, .piece = Piece::WhiteKing});
    check_move(game, mainline(10), Move{.from = Square::F8, .to = Square::E7, .piece = Piece::BlackBishop});
    // TODO: check NAGs for previous half-move
    check_move(game, mainline(11), Move{.from = Square::D1, .to = Square::E2, .piece = Piece::WhiteQueen});
}

TEST_CASE("PGN.Parser.Alternative Start", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[SetUp "1"]
[FEN "r4rk1/pp3ppp/2n1q3/8/8/P7/1P3PPP/R1BQ1RK1 w - - 0 1"]
[Result "1-0"]

1. Re1 Rfd8 2. Bd2 Qf5 3. Rc1 Ne5 $1 4. Qc2 Nd3 5. Rf1 1-0)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 9);
    check_move(game, mainline(1), Move{.from = Square::F1, .to = Square::E1, .piece = Piece::WhiteRook});
    check_move(game, mainline(2), Move{.from = Square::F8, .to = Square::D8, .piece = Piece::BlackRook});
    check_move(game, mainline(7), Move{.from = Square::D1, .to = Square::C2, .piece = Piece::WhiteQueen});
    check_move(game, mainline(8), Move{.from = Square::E5, .to = Square::D3, .piece = Piece::BlackKnight});
    check_move(game, mainline(9), Move{.from = Square::E1, .to = Square::F1, .piece = Piece::WhiteRook});
}
