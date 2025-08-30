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
    metadata.emplace_back("BlackFideId", "345377");
    metadata.emplace_back("Event", "Test Event");
    metadata.emplace_back("Result", "1-0");
    metadata.emplace_back("Site", "Test Site");
    metadata.emplace_back("Black", "Black Player");
    metadata.emplace_back("White", "White Player");
    metadata.emplace_back("Round", "1");
    metadata.emplace_back("Date", "2022-01-01");
    metadata.emplace_back("WhiteELO", "2000");
    metadata.emplace_back("Termination", "Normal");

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
