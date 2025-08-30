/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chesscore_io/chesscore_io.h"
#include "chessgame/pgn.h"

#include <sstream>

using namespace chessgame;

TEST_CASE("PGN.Writer.Metadata", "[pgn]") {
    GameMetadata metadata{};
    metadata.add("BlackFideId", "345377");
    metadata.add("Event", "Test Event");
    metadata.add("Result", "1-0");
    metadata.add("Site", "Test Site");
    metadata.add("Black", "Black Player");
    metadata.add("White", "White Player");
    metadata.add("Round", "1");
    metadata.add("Date", "2022-01-01");
    metadata.add("WhiteELO", "2000");
    metadata.add("Termination", "Normal");

    std::ostringstream sstr;
    PGNWriter writer{&sstr};
    writer.write_metadata(metadata);

    CHECK(sstr.str() == R"([Event "Test Event"]
[Site "Test Site"]
[Date "2022-01-01"]
[Round "1"]
[White "White Player"]
[Black "Black Player"]
[Result "1-0"]
[BlackFideId "345377"]
[Termination "Normal"]
[WhiteELO "2000"]

)");
}

TEST_CASE("PGN.Writer.GameTree", "[pgn]") {
    const std::string pgn_data = R"([Event "Test Event"]
[Site "Test Site"]
[Date "2025.08.30"]
[Round "1"]
[White "White Player"]
[Black "BlackPlayer"]
[Result "*"]
[Termination "unterminated"]
[PlyCount "10"]

1. e4 e5 2. Nf3 Nf6 3. Nc3 (3. Nxe5 Qe7 4. d4 d6) 3. .. Bb4 4. Bb5 (4. a3 Bxc3
5. dxc3 (5. bxc3 5. .. O-O 6. d3 d5 7. Be2 dxe4 8. Nxe5 Qe7 9. Bf4 Nd5 10. Qd2
Nd7 11. Bg4 Nxf4) (5. Bc4 Ba5 6. O-O Nxe4 7. Qe2 d5 8. d3 Nf6 9. Qxe5+ Be6
10. Bg5 O-O) 5. .. O-O 6. Bc4 Nxe4 7. Nxe5 Nd6 8. O-O Re8 9. Nxf7 Nxf7 10.
Qh5 Qe7 11. Bf4 g6) 4. .. c6 5. Ba4 d6 *
)";
    std::istringstream pgn_stream{pgn_data};
    const auto opt_game = PGNParser{pgn_stream}.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    std::ostringstream sstr;
    PGNWriter writer{&sstr};
    writer.write_game_lines(game.cursor());

    CHECK(
        sstr.str() ==
        R"(e2e4 e7e5 g1f3 g8f6 b1c3 (f3e5 d8e7 d2d4 d7d6 ) f8b4 f1b5 (a2a3 b4c3 d2c3 (b2c3 e8g8 d2d3 d7d5 f1e2 d5e4 f3e5 d8e7 c1f4 f6d5 d1d2 b8d7 e2g4 d5f4 ) (f1c4 c3a5 e1g1 f6e4 d1e2 d7d5 d2d3 e4f6 e2e5 c8e6 c1g5 e8g8 ) e8g8 f1c4 f6e4 f3e5 e4d6 e1g1 f8e8 e5f7 d6f7 d1h5 d8e7 c1f4 g7g6 ) c7c6 b5a4 d7d6 )"
    );
}
