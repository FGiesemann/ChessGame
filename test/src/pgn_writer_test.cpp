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
