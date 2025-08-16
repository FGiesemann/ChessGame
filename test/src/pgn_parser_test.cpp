/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessgame/pgn.h"

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

    check_move(game, {0}, Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});
    check_move(game, {0, 0}, Move{.from = Square::E7, .to = Square::E5, .piece = Piece::BlackPawn});
    check_move(game, {0, 0, 0}, Move{.from = Square::G1, .to = Square::F3, .piece = Piece::WhiteKnight});
    check_move(game, {0, 0, 0, 0}, Move{.from = Square::B8, .to = Square::C6, .piece = Piece::BlackKnight});
    check_move(game, {0, 0, 0, 0, 0, 0, 0}, Move{.from = Square::B5, .to = Square::A4, .piece = Piece::WhiteBishop});
    CHECK(has_no_following_move(game, {0, 0, 0, 0, 0, 0, 0}));
}
