#include "chessgame/pgn.h"

#include "pgn_database.h"

#include <iostream>
#include <sstream>

auto main() -> int {
    try {
        auto pgn_data = std::istringstream{pgn_database};
        auto parser = chessgame::PGNParser{pgn_data};
        parser.read_game();
    } catch (chessgame::PGNError &e) {
        std::cout << "PGNError:\n" << "  type: " << to_string(e.type()) << '\n' << "  line: " << e.line() << '\n' << "  message: " << e.what() << '\n';
    }
    return 0;
}
