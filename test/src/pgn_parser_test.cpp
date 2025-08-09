/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessgame/pgn.h"

#include <sstream>
#include <string>

using namespace chessgame;

auto count_ply_on_mainline(const chessgame::Game &game) {
    const auto cursor = game.cursor();
    int child_count{0};
    auto next_cursor = cursor.child(0);
    while (next_cursor.has_value()) {
        ++child_count;
        next_cursor = next_cursor.value().child(0);
    }
    return child_count;
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
    auto game = parser.read_game();
    CHECK(game.has_value());
    CHECK(count_ply_on_mainline(game.value()) == 7);
}
